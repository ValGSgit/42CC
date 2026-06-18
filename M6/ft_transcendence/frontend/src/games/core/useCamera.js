import * as THREE from 'three';
import { CONST } from '../config/constants.js';
import { gEditState, gEngine, gUI } from './globals.js';

const offset = new THREE.Vector3();
const lookAt = new THREE.Vector3();
const currentPosition = new THREE.Vector3();
const savedOrbitOffset = new THREE.Vector3(0, 10, 15);
const fpEyePosition = new THREE.Vector3();
let isTransitioningToOrbit = false;

export function useCamera(camera, controls) {

  const updateCamera = (player) => {
    if (!controls || !player) return
    const playerMesh = player.model ? player.model : player
    const mode = gUI.cameraMode;
    controls.maxPolarAngle = Math.PI / 2 - 0.1; // reset controls changed by first person camera
    controls.minPolarAngle = 0;

    switch (mode) {
      case 0:
        handleOrbit(playerMesh)
        break;
      case 1:
        handleThirdPerson(playerMesh)
        break;
      case 2:
        handleAlpacaRoad()
        break;
      case 3:
        handleFirstPerson(playerMesh);
        break;
    }
  }

  const handleAlpacaRoad = () => {
    const camera = gEngine.value.camera;
    camera.position.set(0, 15, -30);

    if (gEngine.value.controls) {
      gEngine.value.controls.target.set(0, 0, 20);
      gEngine.value.controls.update();
    }
  }

  const handleOrbit = (player) => {
    offset.set(CONST.CAMERA_OFFSET.x, CONST.CAMERA_OFFSET.y, CONST.CAMERA_OFFSET.z)
    if (!gUI.editMode)
      currentPosition.copy(player.position).add(offset)

    const t = 1.0 - Math.pow(CONST.CAMERA_LERP, CONST.CAMERA_LERP)
    controls.target.lerp(currentPosition, t)

    if (isTransitioningToOrbit) {
      controls.enabled = false;
      lookAt.copy(controls.target).add(savedOrbitOffset);
      camera.position.lerp(lookAt, t);
      if (camera.position.distanceTo(lookAt) < 0.5) {
        isTransitioningToOrbit = false;
        controls.enabled = true;
      }
    } else {
      controls.enabled = true;
      savedOrbitOffset.copy(camera.position).sub(controls.target);
    }

    controls.update()
  }

  const handleThirdPerson = (player) => {
    controls.enabled = true;

    offset.set(
      CONST.CAMERA_OFFSET.x - 5,
      CONST.CAMERA_OFFSET.y,
      CONST.CAMERA_OFFSET.z - 20
    )
    offset.applyQuaternion(player.quaternion)
    offset.add(player.position)

    lookAt.set(0, 5, 10)
    lookAt.applyQuaternion(player.quaternion)
    lookAt.add(player.position)

    const t = 1.0 - Math.pow(CONST.CAMERA_LERP, CONST.CAMERA_LERP)
    currentPosition.lerpVectors(camera.position, offset, t)
    lookAt.lerpVectors(controls.target, lookAt, t)

    camera.position.copy(currentPosition)
    controls.target.copy(lookAt)
    controls.update()
  }

  const handleFirstPerson = (player) => {
    controls.minPolarAngle = 0; // unlock looking at the ceiling
    controls.maxPolarAngle = Math.PI;

    fpEyePosition.set(0, 8, -2); // alpaca rider view

    // for AR glasses moving
    if (gEngine.value.spatialOffset) {
      const spatial = gEngine.value.spatialOffset;
      fpEyePosition.x += spatial.x;
      fpEyePosition.y += spatial.y;
      fpEyePosition.z += spatial.z;
    }

    camera.position.copy(fpEyePosition).applyQuaternion(player.quaternion).add(player.position);
    const finalRotation = player.quaternion.clone();

    // for AR glasses rotation
    if (gEngine.value.spatialRotation) {
      const headRot = gEngine.value.spatialRotation.clone();
      headRot.x = -headRot.x;
      finalRotation.multiply(headRot);
    }

    camera.quaternion.copy(finalRotation);

    // update orbit controls so it moves with us but don't update it
    const forward = new THREE.Vector3(0, 0, 10);
    forward.applyQuaternion(camera.quaternion);
    controls.target.copy(camera.position).add(forward);
  }

  return { updateCamera }
}

export function changeCamera() {
  const engine = gEngine.value;
  if (!engine || !engine.camera || !engine.controls) return;

  if (gUI.editMode)
    return;

  if (gUI.cameraMode === 0) {
    gUI.cameraMode = 1;
  } else if (gUI.cameraMode === 1 && gEngine.value.spatialOffset) { // Jump to First Person only if AR data is present
    gUI.cameraMode = 3;
  } else {
    gUI.cameraMode = 0;
    isTransitioningToOrbit = true;
  }
}

export function changeEditModeCamera() {
  const engine = gEngine.value;
  if (!engine || !engine.camera || !engine.controls) return;

  if (gUI.editMode) {
    gEditState.cameraMode = gUI.cameraMode;
    gUI.cameraMode = 0;
    if (gUI.cameraMode !== gEditState.cameraMode)
      isTransitioningToOrbit = true;
  } else {
    gUI.cameraMode = gEditState.cameraMode;
  }
}

export function checkControlsEnabled() {
  return !gUI.lockCamera && !gEditState.selected;
}