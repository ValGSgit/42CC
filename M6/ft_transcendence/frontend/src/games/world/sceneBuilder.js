import * as THREE from 'three'
import * as PRIMITIVES from '../assets/primitives.js'
import { CONST } from '../config/constants.js'
import { gScene } from '../core/globals.js'
import * as GRADIENT from "../utils/createGradient.js"

export function setupEnvironment(scene) {
  scene.background = GRADIENT.Linear('#4abdff', '#142191')
  setupLighting(scene)
  createFloor(scene)
}

export function setupLighting(scene) {
  const ambientLight = new THREE.AmbientLight('#ffffff', 0.8)
  const sunLight = new THREE.DirectionalLight('#ffffff', 1.2)
  gScene.value.ambientLight = ambientLight
  gScene.value.sunLight = sunLight

  setSunLight(15, 45, 3);
  sunLight.castShadow = true
  sunLight.shadow.mapSize.width = 4096;
  sunLight.shadow.mapSize.height = 4096;
  sunLight.shadow.bias = -0.001

  scene.add(ambientLight)
  scene.add(sunLight)
  scene.add(sunLight.target);

  adjustSunBox();

  if (CONST.DEBUG) {
    const helper = new THREE.CameraHelper(sunLight.shadow.camera);
    sunLight.helper = helper;
    scene.add(helper);
  }
}

export function adjustSunBox(val = 60, far = 70, near = 10) {
  const sunLight = gScene.value.sunLight;

  sunLight.shadow.camera.left = -val
  sunLight.shadow.camera.right = val
  sunLight.shadow.camera.top = val
  sunLight.shadow.camera.bottom = -val

  sunLight.shadow.camera.far = far;
  sunLight.shadow.camera.near = near;
  sunLight.shadow.camera.updateProjectionMatrix();
  if (sunLight.helper)
    sunLight.helper.update();
}

export function setSunLight(x = 15, y = 45, z = 3, tX = 0, tY = 0, tZ = 0) {
  const sunLight = gScene.value.sunLight;

  sunLight.position.set(x, y, z);
  sunLight.target.position.set(tX, tY, tZ);
  sunLight.target.updateMatrixWorld();
}

function createFloor(scene) {
  const texture = GRADIENT.Radial('#7afc00', '#083f16')
  const matTop = new THREE.MeshStandardMaterial({ map: texture, roughness: 0.8 })
  const matSide = new THREE.MeshStandardMaterial({ color: '#002208', roughness: 0.8 })

  const floor = PRIMITIVES.Cylinder(CONST.FLOOR_RADIUS, 1, 64, [matSide, matTop, matSide])
  floor.position.y = -floor.geometry.parameters.height / 2

  scene.add(floor)
  gScene.value.floor = floor
}