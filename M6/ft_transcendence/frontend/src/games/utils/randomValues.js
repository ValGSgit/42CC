import * as THREE from 'three';
import { CONST } from '../config/constants.js';

export function getRandomPos() {
  const angle = getRandomRot();
  const radius = CONST.MAX_MOVE_RADIUS * Math.sqrt(Math.random());
  const x = Math.cos(angle) * radius;
  const z = Math.sin(angle) * radius;

  return new THREE.Vector3(x, 0, z)
}

export function getRandomVal() {
  const angle = getRandomRot();
  const radius = CONST.MAX_MOVE_RADIUS * Math.sqrt(Math.random());
  const random = Math.cos(angle) * radius;

  return random;
}

export function getRandomRot() {
  const angle = Math.random() * Math.PI * 2;
  return angle;
}

export function getRandomScale() {
  const base = 1.0;
  const offset = 0.25;
  const lower = base - offset;
  const upper = base + offset;
  const scale = Math.random() * (upper - lower) + lower;
  return new THREE.Vector3(scale, scale, scale);
}

export function getRandomTimer() {
  return 2 + Math.random() * 5;
}

export function getRandomSpeed() {
  let randomTime = getRandomTimer()
  randomTime = Math.floor(randomTime / 2)
  if (randomTime % 3 === 0)
    return 1 // fast
  else if (randomTime % 2 === 0)
    return 0.8 // middle
  return 0.6 //slow
}

export function getRandomInt(max) {
  return Math.floor(Math.random() * max);
}