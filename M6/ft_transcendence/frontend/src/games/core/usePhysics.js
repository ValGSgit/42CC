import { OBB } from 'three/addons/math/OBB.js'
import { CONST } from '../config/constants.js'
import { MATERIALS as MATS } from '../config/materials.js'
import { gCollidables } from './globals.js'

const sourceOBB = new OBB()
const obstacleOBB = new OBB()

export function usePhysics() {

  const checkCollisionWith = (sourceObj, objects) => {
    let hasCollision = false

    const sourceCollider = sourceObj.userData.collider
    if (!sourceCollider || !sourceCollider.userData.baseOBB) return false

    sourceCollider.updateMatrixWorld(true)
    sourceOBB.copy(sourceCollider.userData.baseOBB)
    sourceOBB.applyMatrix4(sourceCollider.matrixWorld)

    for (const obstacle of objects) {
      const obstacleModel = obstacle.model || obstacle
      if (!obstacleModel) continue
      if (obstacleModel === sourceObj || obstacleModel.uuid === sourceObj.uuid || obstacleModel.id === sourceObj.id) continue

      const obstacleCollider = obstacleModel.userData.collider
      if (!obstacleCollider || !obstacleCollider.userData.baseOBB) continue

      obstacleCollider.updateMatrixWorld(true)
      obstacleOBB.copy(obstacleCollider.userData.baseOBB)
      obstacleOBB.applyMatrix4(obstacleCollider.matrixWorld)

      if (sourceOBB.intersectsOBB(obstacleOBB)) {
        hasCollision = true
        if (CONST.DEBUG)
          drawDebugBox(obstacleCollider)
        break
      }
    }
    return hasCollision
  }

  const checkCollision = (player, nextX, nextZ, nextRotY) => {
    const oldX = player.position.x
    const oldZ = player.position.z
    const oldRotY = player.rotation.y

    player.position.x = nextX
    player.position.z = nextZ
    if (nextRotY !== undefined) player.rotation.y = nextRotY
    player.updateMatrixWorld(true)

    let hasCollision = checkCollisionWith(player, gCollidables)
    if (hasCollision) {
      player.position.x = oldX
      player.position.z = oldZ
      player.rotation.y = oldRotY
      player.updateMatrixWorld(true)
    }
    return hasCollision
  }

  return { checkCollision, checkCollisionWith }
}

export function checkWithinBounds(x, z) {
  const distance = Math.sqrt(x * x + z * z)
  const withinBounds = distance < CONST.MAX_MOVE_RADIUS
  return withinBounds
}

export function usePos() {

  const storePos = (model) => {
    model.userData.originalPos = model.position.clone()
    model.userData.originalRotY = model.rotation.y
  }

  const restorePos = (model) => {
    if (model.userData.originalPos) {
      model.position.x = model.userData.originalPos.x;
      model.position.z = model.userData.originalPos.z;
      model.rotation.y = model.userData.originalRotY;
    }
  }

  return { storePos, restorePos }
}

function drawDebugBox(hitMesh) {
  hitMesh.material = MATS.collider_hit
  hitMesh.visible = true

  if (hitMesh.userData.debugTimer) {
    clearTimeout(hitMesh.userData.debugTimer)
  }

  hitMesh.userData.debugTimer = setTimeout(() => {
    hitMesh.material = MATS.debug
    hitMesh.userData.debugTimer = null
  }, 1000)
}
