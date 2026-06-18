import * as THREE from 'three';
import { CONST } from '../config/constants';
import { devWarn } from '../../services/logger.js';
import { createAlpaca, createCollectable, createDecoration, createItem } from "../core/createObjects";
import { gCollidables } from '../core/globals';
import { getModel } from '../core/modelCache';
import { attachCollider } from '../core/useCollider';
import { usePhysics } from "../core/usePhysics";
import { getRandomPos, getRandomRot, getRandomScale } from "./randomValues";

export async function spawnObjectRandomly(object, amount, ring = false) {
  const itemsGroup = new THREE.Group();
  let itemsData;
  if (ring) {
    itemsData = await getRandomPointInRing(object.path, amount);
  } else {
    itemsData = await getValidRandomPos(object.path, amount);
  }

  for (const data of itemsData) {
    let item;
    switch (object.type) {
      case 'alpaca':
        item = await createAlpaca(null, null, data.position, data.rotation, data.scale);
        break;
      case 'item':
        item = await createItem(data.path, data.position, data.rotation, data.scale);
        break;
      case 'decoration':
        item = await createDecoration(data.path, data.position, data.rotation, data.scale);
        break;
      case 'collectable':
        data.scale = [0, 0, 0];
        item = await createCollectable(data.path, data.position, data.rotation, data.scale);
        break;
      default:
        devWarn(`Spawn Object Warning: Unknown entity type '${type}'`);
    }
    if (item && item.model) {
      if (object.type === 'item' || object.type === 'decoration')
        item.model.userData.cost = object.cost;
      itemsGroup.add(item.model)
    }
  }
  return itemsGroup;
}

export async function getValidRandomPos(path, amount) {
  const itemsData = [];
  const { checkCollisionWith } = usePhysics();
  const { model } = await getModel(path);
  const dummy = model.clone();
  attachCollider(dummy);

  for (let i = 0; i < amount; i++) {
    let isColliding = true;
    let attempts = 0;
    let pos = new THREE.Vector3();
    let scale = getRandomScale();
    let rot = getRandomRot();
    dummy.scale.copy(scale);
    dummy.rotation.y = rot;

    while (isColliding && attempts < 100) {
      pos.copy(getRandomPos());
      dummy.position.copy(pos);
      dummy.updateMatrixWorld(true);
      isColliding = checkCollisionWith(dummy, gCollidables);
      attempts++;
    }
    if (!isColliding) {
      itemsData.push({
        path: path,
        position: pos.toArray(),
        rotation: rot,
        scale: scale.toArray()
      })
    }
  }
  return itemsData;
}

async function getRandomPointInRing(path, amount) {
  const oldRadius = CONST.FLOOR_RADIUS - 5;
  const newRadius = CONST.FLOOR_RADIUS;
  const itemsData = [];
  const { checkCollisionWith } = usePhysics();
  const { model } = await getModel(path);
  const dummy = model.clone();
  attachCollider(dummy);

  for (let i = 0; i < amount; i++) {
    let isColliding = true;
    let attempts = 0;
    let pos = new THREE.Vector3();
    let scale = getRandomScale();
    let rot = getRandomRot();
    dummy.scale.copy(scale);
    dummy.rotation.y = rot;

    while (isColliding && attempts < 100) {
      const angle = Math.random() * Math.PI * 2;

      // Calculate the evenly distributed radius using the area math
      const rInnerSq = oldRadius * oldRadius;
      const rOuterSq = newRadius * newRadius;
      const randomRadius = Math.sqrt(Math.random() * (rOuterSq - rInnerSq) + rInnerSq);

      // Convert the angle and radius back into X and Z coordinates
      const x = Math.cos(angle) * randomRadius;
      const z = Math.sin(angle) * randomRadius;
      dummy.position.x = x;
      dummy.position.z = z;
      dummy.updateMatrixWorld(true);
      isColliding = checkCollisionWith(dummy, gCollidables);
      if (!isColliding) {
        pos.set(x, 0, z);
      }
      attempts++;
    }
    if (!isColliding) {
      itemsData.push({
        path: path,
        position: pos.toArray(),
        rotation: rot,
        scale: scale.toArray()
      })
    }
  }
  return itemsData;
}