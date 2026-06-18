import * as SkeletonUtils from 'three/addons/utils/SkeletonUtils.js';
import { markRaw } from 'vue';
import { Alpaca } from './entities/Alpaca.js';
import { Collectable } from './entities/Collectable.js';
import { Item } from './entities/Item.js';
import { getModel } from './modelCache.js';
import { registerEntity } from './registerEntity.js';
import { attachCollider } from './useCollider.js';

export async function createAlpaca(
  name = "Alpaca",
  color = null,
  position = [0, 0, 0],
  rotation = 0,
  scale = [1, 1, 1]
) {
  const path = '/models/alpaca.glb';
  const { model, animations } = await getModel(path);
  const clone = SkeletonUtils.clone(model);
  attachCollider(clone);

  const alpaca = new Alpaca(clone, animations, {
    name, color, position, rotation, scale,
  });

  registerEntity(alpaca, 'alpaca');
  return markRaw(alpaca);
}

export async function createItem(
  path,
  position = [0, 0, 0],
  rotation = 0,
  scale = [1, 1, 1],
) {
  const { model, animations } = await getModel(path);
  const clone = SkeletonUtils.clone(model);
  attachCollider(clone);

  const item = new Item(clone, animations, { position, rotation, scale });
  item.path = path;
  item.type = 'item' // used to distingush between deco for colliders

  registerEntity(item, 'item');
  return markRaw(item);
}

export async function createDecoration(
  path,
  position = [0, 0, 0],
  rotation = 0,
  scale = [1, 1, 1],
) {
  const { model, animations } = await getModel(path);
  const clone = SkeletonUtils.clone(model);
  attachCollider(clone);

  const deco = new Item(clone, animations, { position, rotation, scale });
  deco.path = path;
  deco.type = 'deco' // used to distingush between item for colliders

  registerEntity(deco, 'decoration');
  return markRaw(deco);
}

export async function createCollectable(
  path,
  position = [0, 0, 0],
  rotation = 0,
  scale = [1, 1, 1],
) {
  const { model, animations } = await getModel(path);
  const clone = SkeletonUtils.clone(model);
  attachCollider(clone);

  const collectable = new Collectable(clone, animations, { position, rotation, scale });
  registerEntity(collectable, 'collectable');
  return markRaw(collectable);
}
