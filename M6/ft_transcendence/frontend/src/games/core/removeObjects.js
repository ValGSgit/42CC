import { gAlpacas, gCollectables, gCollidables, gDecorations, gEditables, gItems } from "./globals";

export function removeObject(object) {
  const entity = getEntity(object);
  const model = entity.model ? entity.model : entity;

  removeFromArray(entity, gAlpacas);
  removeFromArray(entity, gItems);
  removeFromArray(entity, gDecorations);
  removeFromArray(entity, gCollectables);

  removeFromArray(model, gCollidables);
  removeFromArray(model, gEditables);

  if (model && model.parent) {
    model.removeFromParent()
    removeMatsAndGeo(model);
  }
}

export function removeFromArray(item, array) {
  const index = array.indexOf(item);
  if (index > -1) {
    array.splice(index, 1);
  }
}

function getEntity(object) {
  let entity
  entity = gAlpacas.find(item => item.model === object)
  if (entity) return entity
  entity = gItems.find(item => item.model === object)
  if (entity) return entity
  entity = gDecorations.find(item => item.model === object)
  if (entity) return entity
  return object
}

function removeMatsAndGeo(model) {
  model.traverse((child) => {
    if (!child.isMesh) return;
    // Cache-owned geometry/material is reused by future clones — disposing
    // here would turn the next spawn into a white mesh.
    if (child.geometry && !child.geometry.userData?.shared) {
      child.geometry.dispose();
    }
    const mats = Array.isArray(child.material) ? child.material : [child.material];
    for (const mat of mats) {
      if (!mat || mat.userData?.shared) continue;
      mat.dispose();
    }
  });
}