import * as THREE from 'three';
import { devWarn } from '../../services/logger.js';
import { gMinigame, gPlayer } from '../core/globals.js';
import { checkWithinBounds, usePhysics } from '../core/usePhysics.js';
import { getRandomPos, getRandomTimer } from '../utils/randomValues.js';

const HUNT_RANGE = 35;   // distance at which AI switches to hunting the player
const SPIT_RANGE = 20;   // distance at which AI starts spitting during hunt
const SPIT_COOLDOWN = 1.8; // seconds between aimed spits

const dummy = new THREE.Object3D();

export function alpacaAI() {
  const { checkCollision } = usePhysics();

  const checkHunting = (alpaca) => {
    if (gMinigame.value.mode === 0)
      return false;

    const ai = alpaca.ai
    const player = gPlayer.value;

    if (player && !player.isDead) {
      const distToPlayer = alpaca.model.position.distanceTo(player.model.position);
      // 50% chance to hunt the player if they're in range
      if (distToPlayer < HUNT_RANGE && Math.random() < 0.5) {
        ai.state = 'hunting';
        ai.spitTimer = SPIT_COOLDOWN * Math.random(); // stagger initial spits
        return true;
      }
    }
    return false;
  }

  const handleIdle = (alpaca, delta) => {
    const ai = alpaca.ai;
    const target = alpaca.target;

    ai.timer -= delta;
    if (ai.timer <= 0) {
      if (checkHunting(alpaca))
        return;
      target.copy(getRandomPos());
      ai.state = 'moving';
      alpaca.spit();
    }
  };

  const handleMoving = (alpaca, delta) => {
    const ai = alpaca.ai;
    const model = alpaca.model;
    const target = alpaca.target;

    const distance = model.position.distanceTo(target);

    if (distance < 0.5) {
      ai.state = 'idle';
      ai.timer = getRandomTimer();
      alpaca.isAutoMoving = false
    } else {
      const direction = new THREE.Vector3().subVectors(target, model.position).normalize();

      dummy.position.copy(model.position);
      dummy.lookAt(target);

      const speed = alpaca.speed * delta;
      const nextX = model.position.x + (direction.x * speed);
      const nextZ = model.position.z + (direction.z * speed);
      const isWithinBounds = checkWithinBounds(nextX, nextZ);
      const isColliding = checkCollision(model, nextX, nextZ);
      const bounceDistance = 0.5;

      if (!isWithinBounds || isColliding) {
        const backwardVector = new THREE.Vector3(0, 0, -1);
        backwardVector.applyQuaternion(model.quaternion);
        if (isWithinBounds)
          model.position.addScaledVector(backwardVector, bounceDistance);
        ai.state = 'idle';
        ai.timer = 1;
        alpaca.isAutoMoving = false
      } else {
        model.position.x = nextX;
        model.position.z = nextZ;
        model.quaternion.slerp(dummy.quaternion, 5 * delta);
      }
    }
  };

  const handleHunting = (alpaca, delta) => {
    const player = gPlayer.value;

    // Abandon hunt if player is gone, dead, or out of range
    if (!player || player.isDead) {
      alpaca.ai.state = 'idle';
      alpaca.ai.timer = getRandomTimer();
      alpaca.isAutoMoving = false;
      return;
    }

    const ai = alpaca.ai;
    const model = alpaca.model;
    const distance = model.position.distanceTo(player.model.position);

    if (distance > HUNT_RANGE * 1.5) {
      ai.state = 'idle';
      ai.timer = getRandomTimer();
      alpaca.isAutoMoving = false;
      return;
    }

    // Move towards player unless already close
    if (distance > 5) {
      const direction = new THREE.Vector3().subVectors(player.model.position, model.position).normalize();
      dummy.position.copy(model.position);
      dummy.lookAt(player.model.position);

      const speed = alpaca.speed * delta;
      const nextX = model.position.x + direction.x * speed;
      const nextZ = model.position.z + direction.z * speed;

      const isWithinBounds = checkWithinBounds(nextX, nextZ);
      const isColliding = checkCollision(model, nextX, nextZ);

      if (!isWithinBounds || isColliding) {
        ai.state = 'idle';
        ai.timer = 1;
        alpaca.isAutoMoving = false;
      } else {
        model.position.x = nextX;
        model.position.z = nextZ;
        model.quaternion.slerp(dummy.quaternion, 6 * delta);
        alpaca.isAutoMoving = true;
      }
    } else {
      // Close enough — face the player directly
      dummy.position.copy(model.position);
      dummy.lookAt(player.model.position);
      model.quaternion.slerp(dummy.quaternion, 8 * delta);
      alpaca.isAutoMoving = false;
    }

    // Spit at the player on cooldown when in range
    ai.spitTimer = (ai.spitTimer || 0) - delta;
    if (ai.spitTimer <= 0 && distance < SPIT_RANGE) {
      dummy.position.copy(model.position);
      dummy.lookAt(player.model.position);
      model.quaternion.copy(dummy.quaternion);
      alpaca.spit();
      ai.spitTimer = SPIT_COOLDOWN;
    }
  };

  const updateAI = (alpaca, delta) => {
    if (alpaca.isDead) return

    if (gMinigame.value.mode > 1 && gMinigame.value.mode < 5) // no AI update in multiplayer and alpacaRoad
      return

    switch (alpaca.ai.state) {
      case 'idle':
        handleIdle(alpaca, delta);
        break;
      case 'moving':
        handleMoving(alpaca, delta);
        break;
      case 'hunting':
        handleHunting(alpaca, delta);
        break;
      default:
        devWarn(`Unknown AI state: ${alpaca.ai.state}`);
        alpaca.ai.state = 'idle';
        break;
    }
    alpaca.isMoving = (alpaca.ai.state === 'moving' || alpaca.ai.state === 'hunting');
  };

  return { updateAI, handleMoving };
}