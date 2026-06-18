import * as THREE from 'three'
import { reactive } from 'vue'
import { debug } from '../../services/logger.js'
import { alpacaHandling } from '../components/alpacaHandling.js'
import { useEditMode } from '../components/editMode.js'
import { CONST } from '../config/constants.js'
import { printDebug } from './debug.js'
import { gEditState, gEngine, gMinigame, gPlayer, gScene, gUI } from './globals.js'
import { handInput, headInput, useSpatialBridge } from './useSpatialBridge.js'
import { useUIManager } from './useUIManager.js'

// move it to outside of the function so it can be used in useEngine and other functions
const keys = reactive({
  w: false, a: false, s: false, d: false, f: false, space: false, pointer: false
})

const heldKeys = new Set();

export function useInput() {
  const { switchAlpaca } = alpacaHandling()
  const { selectItem, highlightItem, moveItem, placeItem, rotateItem, scaleItem, cancelPlacement } = useEditMode()
  const { closeMenus, openAlpacaShop } = useUIManager()

  // Only when AR glasses is connected
  const handleSpatialInput = (data) => {
    if (data.type === 'hand')
      handInput(data);
    if (data.type === 'head' && gEngine.value?.camera)
      headInput(data)
  }

  // Initialize the bridge
  const { initSpatialBridge, closeSpatialBridge } = useSpatialBridge(handleSpatialInput)

  const onKeyDown = (e) => {
    switch (e.code) {
      case 'KeyW': heldKeys.add(e.code); break;// add it to the set so it doesnt fight with the controller
      case 'KeyA': heldKeys.add(e.code); break;
      case 'KeyS': heldKeys.add(e.code); break;
      case 'KeyD': heldKeys.add(e.code); break;
      case 'Space': heldKeys.add(e.code); break;
      case 'KeyF': if (!heldKeys.has(e.code)) { heldKeys.add(heldKeys.add(e.code)); keys.f = true }; break;
      case 'ShiftLeft': keys.shift = true; break;
      case 'KeyP': printDebug(); break;
      case 'Escape': handleEscapeKey(); break;
      case 'KeyQ': keys.q = true; break;
      case 'KeyL': keys.l = true; break;
      case 'Enter': keys.enter = true; break;
    }
  }

  const onKeyUp = (e) => {
    switch (e.code) {
      case 'KeyW': heldKeys.delete(e.code); break
      case 'KeyA': heldKeys.delete(e.code); break
      case 'KeyS': heldKeys.delete(e.code); break
      case 'KeyD': heldKeys.delete(e.code); break
      case 'KeyF': heldKeys.delete(e.code); break;
      case 'Space': heldKeys.delete(e.code); break
      case 'KeyQ': keys.q = false; break
      case 'KeyL': keys.l = false; break
      case 'Enter': keys.enter = false; break
      case 'ShiftLeft': keys.shift = false; break;
    }
  }

  const onWheel = (e) => {
    if (gUI.editMode && gEditState.selected) {
      if (keys.shift) {
        scaleItem(e)
      }
      else {
        rotateItem(e)
      }
    }
  }

  const onDoubleClick = (e) => {
    // disable double click in mini games
    if (gMinigame.value.isActive || (gMinigame.value.mode && gMinigame.value.mode !== 5))
      return

    debug("double Click!");
    const rect = gEngine.value.renderer.domElement.getBoundingClientRect()
    const pointer = new THREE.Vector2()
    pointer.x = ((e.clientX - rect.left) / rect.width) * 2 - 1
    pointer.y = -((e.clientY - rect.top) / rect.height) * 2 + 1

    const raycaster = new THREE.Raycaster()
    raycaster.setFromCamera(pointer, gEngine.value.camera)

    const intersects = raycaster.intersectObjects(gScene.value.children, true)
    if (intersects.length > 0) {
      switchAlpaca(intersects[0].object, raycaster)
    }
  }

  const handleMouseMove = (e) => {
    // item selected to move
    if (!gEngine.value) return;
    if (gUI.editMode || gEditState.selected) {
      if (gEditState.selected) {
        moveItem(e)
      }
      else {
        highlightItem(e)
      }
    }
  }

  const onPointerDown = (e) => {
    if ((gMinigame.value.mode === 1 || gMinigame.value.mode === 1) && gPlayer.value)
      gPlayer.value.spit()
    keys.pointer = true
    // Select item in edit mode
    if (gUI.editMode || gEditState.selected) {
      if (gEditState.selected) {
        placeItem()
      } else {
        selectItem(e)
      }
    }
  }

  const handleEscapeKey = () => {
    closeMenus()
  }

  //reset input so the keys dont stick (addictive)
  function resetInput() {
    keys.w = false;
    keys.a = false;
    keys.s = false;
    keys.d = false;
    keys.space = false;
  }

  // keyboard and controller working together
  function updateInputState() {
    keys.w = heldKeys.has('KeyW');
    keys.s = heldKeys.has('KeyS');
    keys.a = heldKeys.has('KeyA');
    keys.d = heldKeys.has('KeyD');
    keys.space = heldKeys.has('Space');

    const gp = navigator.getGamepads()[0];
    if (gp) {
      if (gp.axes[1] < -0.1) keys.w = true;
      if (gp.axes[1] > 0.1) keys.s = true;
      if (gp.axes[0] < -0.1) keys.a = true;
      if (gp.axes[0] > 0.1) keys.d = true;
      if (gp.buttons[0].pressed) keys.space = true;
      if (gp.buttons[1].pressed && gPlayer.value) keys.f = true;
    }
  }

  const onGamepadConnect = (e) => {
    debug("Gamepad connected at index %d: %s. %d buttons, %d axes.",
      e.gamepad.index, e.gamepad.id,
      e.gamepad.buttons.length, e.gamepad.axes.length);
  };

  const initInput = () => {
    const canvas = gEngine.value.renderer.domElement;

    window.addEventListener('keydown', onKeyDown)
    window.addEventListener('keyup', onKeyUp)
    window.addEventListener('mousemove', handleMouseMove)
    window.addEventListener('wheel', onWheel, { passive: true })
    canvas.addEventListener('dblclick', onDoubleClick)
    canvas.addEventListener('pointerdown', onPointerDown)
    window.addEventListener("gamepadconnected", onGamepadConnect);
    if (CONST.AR_ENABLED)
      initSpatialBridge() // Start the WebSocket
  }

  const cleanupInput = () => {
    const canvas = gEngine.value.renderer.domElement;

    window.removeEventListener('keydown', onKeyDown)
    window.removeEventListener('keyup', onKeyUp)
    window.removeEventListener('mousemove', handleMouseMove);
    window.removeEventListener('wheel', onWheel);
    canvas.removeEventListener('dblclick', onDoubleClick)
    canvas.removeEventListener('pointerdown', onPointerDown)
    window.removeEventListener("gamepadconnected", onGamepadConnect);
    if (CONST.AR_ENABLED)
      closeSpatialBridge() // Stop the WebSocket
  }

  return { keys, initInput, cleanupInput, updateInputState, resetInput }
}
