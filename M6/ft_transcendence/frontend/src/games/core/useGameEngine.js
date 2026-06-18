import * as THREE from 'three';
import { BokehPass } from 'three/addons/postprocessing/BokehPass.js';
import { EffectComposer } from 'three/addons/postprocessing/EffectComposer.js';
import { OutputPass } from 'three/addons/postprocessing/OutputPass.js';
import { RenderPass } from 'three/addons/postprocessing/RenderPass.js';
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls.js';
import { shallowRef } from 'vue';
import { CONST } from '../config/constants.js';
import { gAlpacas, gCollectables, gCollidables, gDecorations, gEditables, gEditState, gItems, gPlayer, gScene, gUser } from './globals.js';
import { clearModelCache } from './modelCache.js';

export function useGameEngine(containerRef) {
  // Use shallowRef for Three.js objects (prevents Vue from making them reactive and slow)
  const scene = shallowRef(null)
  const camera = shallowRef(null)
  const renderer = shallowRef(null)
  const controls = shallowRef(null)
  const composer = shallowRef(null)

  let bokehPass = null;
  let renderTarget = null;
  let animationId

  const init = () => {
    if (!containerRef.value) return null
    // memory cleanup
    if (gScene.value) { clearScene(gScene.value) }

    resetGArrays()

    //gUser.value = null
    gPlayer.value = null

    scene.value = new THREE.Scene()
    gScene.value = scene.value
    gScene.value.floor = null

    // lights
    gScene.value.ambientLight = null
    gScene.value.sunLight = null

    // for selection
    gEditState.selected = null
    gEditState.ghost = null

    // camera mode
    gScene.value.cameraMode = 0

    // FOG
    const fogColor = '#56738e';
    gScene.value.fog = new THREE.FogExp2(fogColor, 0.0025);

    // CAMERA
    const w = containerRef.value.clientWidth
    const h = containerRef.value.clientHeight
    camera.value = new THREE.PerspectiveCamera(45, w / h, 0.1, 1000)
    camera.value.position.set(30, 30, 50)
    camera.value.lookAt(0, 0, 0)

    // RENDERER
    renderer.value = new THREE.WebGLRenderer({ antialias: true, alpha: true })
    renderer.value.setSize(w, h)
    renderer.value.setPixelRatio(window.devicePixelRatio)
    renderer.value.shadowMap.enabled = true;
    renderer.value.shadowMap.type = THREE.PCFSoftShadowMap;
    renderer.value.outputColorSpace = THREE.SRGBColorSpace;
    containerRef.value.appendChild(renderer.value.domElement)

    // CONTROLS
    controls.value = new OrbitControls(camera.value, renderer.value.domElement);
    controls.value.enableDamping = true
    controls.value.dampingFactor = 0.05
    controls.value.minDistance = CONST.MIN_ZOOM
    controls.value.maxDistance = CONST.MAX_ZOOM
    //to limit the camera movement to hemisphere instead of full sphere
    controls.value.maxPolarAngle = Math.PI / 2 - 0.1


    const size = renderer.value.getSize(new THREE.Vector2());

    // Stored on the closure so cleanup() can dispose it — EffectComposer
    // doesn't dispose its constructor-supplied render target on its own.
    renderTarget = new THREE.WebGLRenderTarget(size.x, size.y, {
      samples: 4
    });

    composer.value = new EffectComposer(renderer.value, renderTarget);
    composer.value.setPixelRatio(window.devicePixelRatio);

    // 2. Add the basic render pass (draws the actual scene)
    const renderPass = new RenderPass(scene.value, camera.value);
    composer.value.addPass(renderPass);

    // 3. Add the Depth of Field (Bokeh) pass
    bokehPass = new BokehPass(scene.value, camera.value, {
      focus: 50.0,       // Distance in units where things are sharp
      aperture: 0.000001,  // How quickly things get blurry (lower = shallower)
      maxblur: 0.01      // Maximum blur intensity
    });
    composer.value.addPass(bokehPass);
    const outputPass = new OutputPass();
    composer.value.addPass(outputPass)
    bokehPass.enabled = true;

    return {
      scene: scene.value,
      camera: camera.value,
      renderer: renderer.value,
      controls: controls.value,
      composer: composer.value
    }
  }

  const setDoF = (enabled) => {
    if (bokehPass) bokehPass.enabled = enabled
  }

  const clearScene = (scene) => {
    if (!scene) return;

    scene.traverse((object) => {
      if (object.geometry && !object.geometry.userData?.shared) {
        object.geometry.dispose();
      }
      if (object.material) {
        if (Array.isArray(object.material)) {
          object.material.forEach(cleanupMaterial);
        } else {
          cleanupMaterial(object.material);
        }
      }
    });
    while (scene.children.length > 0) {
      const child = scene.children[0];
      scene.remove(child);
    }
  };
  // Skip resources owned by modelCache or the shared MATS table — disposing
  // them here would corrupt the next clone (white meshes, missing textures).
  const cleanupMaterial = (material) => {
    if (material.userData?.shared) return;
    for (const key in material) {
      const value = material[key];
      if (value && value.isTexture && !value.userData?.shared) {
        value.dispose();
      }
    }
    material.dispose();
  };

  const cleanup = () => {
    if (animationId) cancelAnimationFrame(animationId)

    // 1. Walk the graph and dispose geometries / materials / textures.
    clearScene(scene.value)

    if (scene.value) {
      if (scene.value.sunLight) scene.value.sunLight.dispose()
      // scene.background may be a Texture (cubemap, equirect) or a Color —
      // only the former has a dispose() method.
      if (scene.value.background && typeof scene.value.background.dispose === 'function') {
        scene.value.background.dispose()
      }
    }

    // 2. Post-processing chain. EffectComposer doesn't clean these up on
    //    its own; without explicit disposal the GPU keeps the offscreen
    //    framebuffer + samplers around (~1 MB per session).
    if (composer.value) {
      if (typeof composer.value.dispose === 'function') {
        composer.value.dispose()
      }
      composer.value = null
    }
    if (bokehPass && typeof bokehPass.dispose === 'function') {
      bokehPass.dispose()
    }
    bokehPass = null
    if (renderTarget && typeof renderTarget.dispose === 'function') {
      renderTarget.dispose()
    }
    renderTarget = null

    // 3. Reset module-level arrays before the renderer goes — keeps
    //    stray references to Three.js objects from outliving the context.
    resetGArrays()

    // 4. WebGL context + DOM.
    if (renderer.value) {
      renderer.value.dispose()
      renderer.value.forceContextLoss(); // forces WebGL to release the context
      renderer.value.domElement.remove(); // Remove the canvas from the HTML
      renderer.value = null
    }

    // 5. Drop cached GLB resources so a remount starts from a clean slate.
    //    Safe here because cleanup is the unmount path — no live clones remain.
    clearModelCache()
  }

  const onResize = () => {
    if (!containerRef.value || !camera.value || !renderer.value) return
    const w = containerRef.value.clientWidth
    const h = containerRef.value.clientHeight
    camera.value.aspect = w / h
    camera.value.updateProjectionMatrix()
    renderer.value.setSize(w, h)
    if (composer.value) {
      composer.value.setSize(w, h)
    }
  }

  const resetGArrays = () => {
    gAlpacas.length = 0;
    gCollectables.length = 0;
    gCollidables.length = 0;
    gItems.length = 0;
    gDecorations.length = 0;
    gEditables.length = 0;
  }

  return { init, cleanup, onResize, clearScene, resetGArrays, setDoF }
}