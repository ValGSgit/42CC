import * as THREE from 'three';
import { OBB } from 'three/addons/math/OBB.js';
import { CONST } from '../config/constants.js';
import { MATERIALS as MATS } from '../config/materials.js';

export function attachCollider(model) {
  let collider = null;

  model.traverse((child) => {
    if (child.isMesh) {
      if (child.name.startsWith('UCX_')) {
        collider = child;
        collider.name = "UCX_Collider";
        collider.visible = false;
      }
      else {
        child.castShadow = true;
        child.receiveShadow = true;
      }
    }
  });

  if (!collider) {
    collider = generateCollider(model);
  } else {
    collider = generateCollider(model, collider);
    collider.name = "Collider";
  }

  model.userData.collider = collider;
  setupOBB(collider);
}

function setupOBB(colliderMesh) {
  colliderMesh.geometry.computeBoundingBox();
  const boundingBox = colliderMesh.geometry.boundingBox;

  const baseOBB = new OBB()
  boundingBox.getCenter(baseOBB.center)
  boundingBox.getSize(baseOBB.halfSize).multiplyScalar(0.5)

  colliderMesh.userData.baseOBB = baseOBB
}


function generateCollider(model, ucx) {
  const compute = ucx ? ucx : model;
  let size = new THREE.Vector3()
  const center = new THREE.Vector3()
  const boundingBox = new THREE.Box3().setFromObject(compute)

  boundingBox.getSize(size)
  boundingBox.getCenter(center)
  if (!ucx)
    size.multiplyScalar(CONST.COLLIDER_SIZE)

  const geo = new THREE.BoxGeometry(size.x, size.y, size.z)
  const mat = CONST.DEBUG ? MATS.debug : MATS.collider
  const collider = new THREE.Mesh(geo, mat)

  model.add(collider)
  model.worldToLocal(center)
  collider.position.copy(center)
  collider.name = "Collider"

  return collider;
} 
