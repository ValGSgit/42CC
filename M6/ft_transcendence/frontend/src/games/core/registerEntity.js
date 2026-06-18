import { gAlpacas, gCollectables, gCollidables, gDecorations, gEditables, gItems} from './globals.js';
import { devWarn } from '../../services/logger.js';

export function registerEntity(entity, type) {

  const classInstance = entity;
  const model = entity.model ? entity.model : entity;
  model.userData.entityType = type;

  switch (type) {
    case 'alpaca':
      model.userData.isEditable = true;
      gAlpacas.push(classInstance);
      gCollidables.push(model);
      gEditables.push(model);
      break;

    case 'item':
      model.userData.isEditable = true;
      model.userData.isCollider = true;
      gItems.push(classInstance);
      gCollidables.push(model);
      gEditables.push(model);
      break;

    case 'decoration':
      model.userData.isEditable = true;
      model.userData.isCollider = false;
      gDecorations.push(classInstance);
      gEditables.push(model);
      break;

    case 'collectable':
      model.userData.isEditable = false;
      model.userData.isCollider = false;
      gCollectables.push(classInstance);
      break;

    default:
      devWarn(`Registry Warning: Unknown entity type '${type}'`);
  }
}