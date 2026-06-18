import { CONST } from '../config/constants.js';
import { shopItems } from '../core/entities/Item.js';
import { gAlpacas, gScene, gUser } from '../core/globals.js';
import { spawnObjectRandomly } from '../utils/spawnRandomly.js';

const UPGRADE_COST = [25, 50, 100, 250, 500];
const HERDSIZE_COST = [15, 25, 50, 100];
const HERDSIZES = [3, 5, 10, 15, 25];

export function upgradeFarm() {

  const increaseFarmSize = async (level) => {
    const cost = getUpgradeCost(level);

    if (cost === "Max" || level >= UPGRADE_COST.length) {
      alert("You reached max upgrades!");
      return;
    }
    if (!checkCoinsPrice(cost)) return;

    const floor = gScene.value.floor
    gUser.value.coins -= cost;
    gUser.value.upgrades++;
    floor.scale.x = CONST.FLOOR_RADIUS / CONST.BASE_RADIUS
    floor.scale.z = CONST.FLOOR_RADIUS / CONST.BASE_RADIUS

    const grass = shopItems.find(item => item.name === 'Grass');
    const newGrassGroup = await spawnObjectRandomly(grass, 10, true);
    gScene.value.add(newGrassGroup);

    const stones = shopItems.find(item => item.name === 'Stones');
    const stonesGroup = await spawnObjectRandomly(stones, 3, true);
    gScene.value.add(stonesGroup);

    const tree = shopItems.find(item => item.name === 'Tree');
    const treeGroup = await spawnObjectRandomly(tree, 2, true);
    gScene.value.add(treeGroup);
  }

  const increaseHerdSize = (level) => {
    const cost = getHerdSizeCost(level);

    if (cost === "Max" || level >= UPGRADE_COST.length) {
      alert("You reached max herdsize!");
      return;
    }
    if (!checkCoinsPrice(cost)) return;

    gUser.value.coins -= cost;
    gUser.value.herdsize++;
  }
  return { increaseFarmSize, increaseHerdSize }
}

export function getUpgradeCost(level) {
  if (level >= UPGRADE_COST.length) return "Max";
  return UPGRADE_COST[level];
}

export function getHerdSizeCost(level) {
  if (level >= HERDSIZE_COST.length) return "Max";
  return HERDSIZE_COST[level];
}

export function getHerdSize(level) {
  if (level < HERDSIZES.length)
    return HERDSIZES[level];
  return 0;
}

export function checkCoinsPrice(cost) {
  if (gUser.value.coins < cost) {
    alert('Not enough coins!');
    return false;
  }
  return true;
}

export function checkHerdSize() {
  if (gAlpacas.length >= getHerdSize(gUser.value.herdsize)) {
    alert('Herdsize limit reached!');
    return false;
  }
  return true;
}