import { alpacaAI } from '../components/alpacaAI.js';
import { CONST } from '../config/constants.js';
import { gAlpacas, gMinigame, gPlayer } from './globals.js';
import { useInput } from './useInput.js';
import { checkWithinBounds, usePhysics } from './usePhysics.js';

export function usePlayerControls() {
  const { keys } = useInput()
  const { checkCollision } = usePhysics()

  const handleJumping = (player, delta) => {
    const { model } = player;
    let isVerticalMoving = false;
    let keydown = false;

    if (player === gPlayer.value)
      keydown = keys.space
    if (gMinigame.value.mode === 3 && player === gAlpacas[1])
      keydown = keys.l
    if (gMinigame.value.mode === 3 && player === gAlpacas[2])
      keydown = keys.enter
    if (gMinigame.value.mode === 3 && player === gAlpacas[3])
      keydown = keys.q

    // Jump up
    if (keydown && model.position.y <= CONST.JUMPING_MAX_HEIGHT && !player.isFalling) {
      if (CONST.SBS_ENABLED)
        model.position.y += CONST.JUMPING_SPEED * 2;
      else
        model.position.y += CONST.JUMPING_SPEED * delta;
      player.isJumping = true;
      isVerticalMoving = true;
    }
    // Fall down
    if (model.position.y > 0 && (!keydown || player.isFalling)) {
      if (CONST.SBS_ENABLED)
        model.position.y -= CONST.JUMPING_SPEED * 2;
      else
        model.position.y -= CONST.JUMPING_SPEED * delta;
      player.isJumping = true;
    }
    // Hit the ground
    if (model.position.y <= 0) {
      model.position.y = 0;
      player.isJumping = false;
      if (!keydown) player.isFalling = false;
    }
    // Hit the ceiling/max height of jump
    if (model.position.y >= CONST.JUMPING_MAX_HEIGHT) {
      player.isFalling = true;
    }
    return isVerticalMoving;
  }

  const handleSpitting = (player) => {
    if (keys.f) {
      player.spit();
      keys.f = false;
    }
  }

  const handleWalking = (player, delta) => {
    const { model } = player;

    const rotSpeed = player.rotationSpeed * delta;
    let speed = player.speed * delta;
    let dir = 0;
    let nextRotY = model.rotation.y;
    let isWalking = false;
    if (gMinigame.value.mode < 3 || gMinigame.value.mode === 5) // no walking for alpaca road mini game
    {
      if (keys.w) { dir = 1; isWalking = true; player.isAutoMoving = false; }
      if (keys.s) { dir = -1; speed = speed * 0.5; isWalking = true; player.isAutoMoving = false; }
      if (keys.a) { nextRotY += rotSpeed; isWalking = true; player.isAutoMoving = false; }
      if (keys.d) { nextRotY -= rotSpeed; isWalking = true; player.isAutoMoving = false; }
    }

    return { dir, speed, nextRotY, isWalking };
  }

  const checkMovement = (model, dir, speed, nextRotY) => {
    const dx = Math.sin(nextRotY) * speed * dir;
    const dz = Math.cos(nextRotY) * speed * dir;

    const nextX = model.position.x + dx;
    const nextZ = model.position.z + dz;

    if (checkWithinBounds(nextX, nextZ)) {
      const collided = checkCollision(model, nextX, nextZ, nextRotY);
      if (collided) {
        // Apply a small bounce back so the player doesn't get pixel-perfect stuck inside the collision box
        model.position.x -= dx * 0.5;
        model.position.z -= dz * 0.5;
      }
    }
  }

  const updatePlayer = (player, delta) => {
    if (!player || !player.model || player.isDead) return;

    const { model } = player;
    const isJumping = handleJumping(player, delta);
    const { dir, speed, nextRotY, isWalking } = handleWalking(player, delta);
    const { handleMoving } = alpacaAI(); // for double click moving
    handleSpitting(player);

    if (isWalking) {
      checkMovement(model, dir, speed, nextRotY);
    }
    else if (player.isAutoMoving)
      handleMoving(player, delta)
    player.isMoving = isWalking || isJumping || player.isAutoMoving;
    player.animDir = keys.s ? -1 : (player.isMoving ? 1 : 0);
  }

  return { updatePlayer }
}
