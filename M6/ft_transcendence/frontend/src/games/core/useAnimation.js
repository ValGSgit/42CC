import * as THREE from 'three';
import { CONST } from '../config/constants.js';
import { gMinigame, gPlayer } from './globals.js';
import { roadSpeed } from '../mini_games/alpacaRoad.js';

export function handleAnimation(player, animDir, speed) {
  if (gMinigame.value.mode === 3 || gMinigame.value.mode === 4 )
    speed = roadSpeed / 2.1 
  if (CONST.SBS_ENABLED)
    speed /= 2 // SBS calibration
  const { mixer, animations } = player
  const idleAction = mixer.clipAction(animations[1])
  const jumpAction = mixer.clipAction(animations[2])
  const deadAction = mixer.clipAction(animations[0])

  const walkAction = speed > 14.0 ? mixer.clipAction(animations[3]) : mixer.clipAction(animations[5])
  const calibration = speed > 14.0 ? 0.125 : CONST.CALIBRATION
  let newAction = idleAction;

  if (!player.currentAction) {
    player.currentAction = idleAction
  }

  if (player.isDead === -1 || player.isDead === 1) {
    newAction = deadAction
    newAction.setLoop(THREE.LoopOnce)
    if (player.isDead === -1)
      player.isDead = 0 // not dead yet
    if (player.isDead)
      newAction.clampWhenFinished = true
  }
  else if (animDir !== 0) { // moving
    if (player.isJumping)
      newAction = jumpAction
    else
      newAction = walkAction
    if (gMinigame.value.mode === 4)
      walkAction.timeScale = speed * 0.125
    else
      walkAction.timeScale = (speed * calibration) * animDir
  } else {
    newAction = idleAction
  }

  if ((gMinigame.value.mode === 3 || gMinigame.value.mode === 4 )&& !player.isJumping && !player.isDead && gMinigame.value.isActive)
    newAction = walkAction // always walking in Alpaca Road mini game

  if (player.currentAction !== newAction) {
    player.currentAction.fadeOut(0.4)
    newAction.reset()
    if (newAction === jumpAction) {
      newAction.time = 0.15;
    }
    newAction.fadeIn(0.4).play()
    player.currentAction = newAction
  }
}