import * as THREE from 'three'
import { createAlpaca, createDecoration, createItem } from '../core/createObjects.js'
import { devError } from '../../services/logger.js'
import { shopItems } from '../core/entities/Item.js'
import { gPlayer } from '../core/globals.js'
import { spawnObjectRandomly } from '../utils/spawnRandomly.js'
import { loadGameData } from './dataLoader.js'
import { setupEnvironment } from './sceneBuilder.js'

export async function initWorld(scene, isAuthenticated = false, visitPlayerId) {
  let data = null
  if (isAuthenticated) {
    try {
      data = await loadGameData(visitPlayerId)
    } catch (e) {
      devError('Failed to load game data, starting fresh.', e)
    }
  }

  setupEnvironment(scene)
  await initAlpacas(scene, data)
  await initItems(scene, data?.items)
}

export async function initAlpacas(scene, user) {
  const alpacaGroup = new THREE.Group()
  const alpacasToLoad = (user?.alpacas?.length > 0) ? user.alpacas : [null];

  for (const savedData of alpacasToLoad) {
    let newAlpaca;
    if (savedData) {
      newAlpaca = await createAlpaca(
        savedData.name,
        savedData.color,
        savedData.position,
        savedData.rotation,
        savedData.scale,
      );
      newAlpaca.speedOffset = savedData.speedOffset;
      newAlpaca.rotationOffset = savedData.rotationOffset;
      newAlpaca.age = savedData.age;
      newAlpaca.aliveTime = savedData.aliveTime;
      newAlpaca.model.userData.cost = savedData.cost;
      newAlpaca.selected = savedData.selected;
    } else {
      newAlpaca = await createAlpaca();
    }
    if (!gPlayer.value) gPlayer.value = newAlpaca;
    if (newAlpaca.selected) gPlayer.value = newAlpaca;
    alpacaGroup.add(newAlpaca.model)
  }
  scene.add(alpacaGroup);
}

export async function initItems(scene, savedItems) {
  let itemsGroup = new THREE.Group();

  if (savedItems && savedItems.length > 0) {
    itemsGroup = await initSavedItems(savedItems);
  } else {
    const tree = shopItems.find(item => item.name === 'Tree');
    itemsGroup.add(await spawnObjectRandomly(tree, 4));

    const stones = shopItems.find(item => item.name === 'Stones');
    itemsGroup.add(await spawnObjectRandomly(stones, 4));

    const grass = shopItems.find(item => item.name === 'Grass');
    itemsGroup.add(await spawnObjectRandomly(grass, 30));
  }
  scene.add(itemsGroup);
}

async function initSavedItems(savedItems) {
  const loadedItems = new THREE.Group()
  for (const item of savedItems) {
    let loadedItem
    if (item.type === 'item')
      loadedItem = await createItem(item.path, item.position, item.rotation, item.scale);
    else
      loadedItem = await createDecoration(item.path, item.position, item.rotation, item.scale);
    loadedItem.model.name = item.name;
    loadedItem.model.userData.cost = item.cost;
    loadedItems.add(loadedItem.model);
  }
  return loadedItems;
}
