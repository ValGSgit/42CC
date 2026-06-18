import * as THREE from 'three';
import { useFloatingText } from '../components/floatingText.js';
import { debug } from '../../services/logger.js';
import { MATERIALS as MATS } from '../config/materials.js';
import { gAlpacas, gMinigame, gPlayer, gScene, gUser } from "../core/globals.js";
import { useUIManager } from '../core/useUIManager.js';
import { activeClient } from '../mini_games/GameClient.js';

const floorPlane = new THREE.Plane(new THREE.Vector3(0, 1, 0), 0);
const worldPoint = new THREE.Vector3();
const { openAlpacaStats } = useUIManager();
const activeSpits = [];
const { spawnFloatingText } = useFloatingText();

export function alpacaHandling() {

  const setMoveLocation = (raycaster) => {
    if (!raycaster.ray.intersectPlane(floorPlane, worldPoint))
      return;
    if (!gPlayer.value)
      return;
    gPlayer.value.target = worldPoint.clone();
    gPlayer.value.isAutoMoving = true;
  }

  const switchAlpaca = (obj, raycaster) => {
    let alpacaToSwitch = findAlpaca(obj)
    if (!alpacaToSwitch && raycaster) {
      setMoveLocation(raycaster)
    }
    else if (gPlayer.value === alpacaToSwitch)
      openAlpacaStats();
    else
      gPlayer.value = alpacaToSwitch
    return true
  }

  const makeSpit = (alpaca, targetPoint) => {
    if (alpaca.isDead === 1)
      return;

    const origin = new THREE.Vector3().copy(alpaca.model.position);
    let dx = Math.sin(alpaca.model.rotation.y);
    let dz = Math.cos(alpaca.model.rotation.y);

    origin.y += 5;
    origin.x += dx * 3;
    origin.z += dz * 3;

    let direction;
    if (targetPoint) {
      direction = new THREE.Vector3().subVectors(targetPoint, origin).normalize();
    } else {
      direction = new THREE.Vector3(dx, -0.4, dz).normalize();
    }

    if (gMinigame.value.mode === 2 && alpaca === gPlayer.value) {
      activeClient.sendSpit({ x: direction.x, y: direction.y, z: direction.z });
    }

    const beam = createLaserBeam(origin, direction, 1);
    gScene.value.add(beam);

    activeSpits.push({
      owner: alpaca,
      mesh: beam,
      direction: direction,
      currentPos: origin,
      distanceTraveled: 0,
      maxDistance: 15,
      speed: 30
    });
  };

  const updateSpits = (delta) => {
    for (let i = activeSpits.length - 1; i >= 0; i--) {
      const s = activeSpits[i];

      const step = s.direction.clone().multiplyScalar(s.speed * delta);
      s.currentPos.add(step);
      s.mesh.position.copy(s.currentPos);
      s.distanceTraveled += s.speed * delta;

      const targets = gAlpacas.map(a => a.model);
      const hits = new THREE.Raycaster(s.currentPos, s.direction, 0, s.speed * delta).intersectObjects(targets, true);
      if (hits.length > 0 || s.distanceTraveled > s.maxDistance) {

        if (hits.length > 0) {
          const hitAlpaca = findAlpaca(hits[0].object);

          if (hitAlpaca && hitAlpaca !== s.owner && hitAlpaca.isDead !== 1) {
            if (gMinigame.value.mode === 0 || gMinigame.value.mode === 5) {
              // fake hit in farm
              hitAlpaca.beingHit(s.owner);
            }
            else if (gMinigame.value.mode === 1) {
              // --- SINGLE PLAYER LOGIC ---
              if (hitAlpaca.hp > 0)
                spawnFloatingText(hitAlpaca.model, '-💔', 'hearts');
              hitAlpaca.beingHit(s.owner);
              if (gPlayer.value === hitAlpaca)
                gMinigame.value.players[0].hp--;
              gMinigame.value.players[0].point = gUser.value.point;

            } else if (gMinigame.value.mode === 2) {
              // --- MULTIPLAYER LOGIC ---
              if (s.owner === gPlayer.value && hitAlpaca.socketId && hitAlpaca.socketId !== activeClient.socket.id) {
                debug("hit alpaca:", hitAlpaca.health);
                gPlayer.value.point++;
                activeClient.sendSpitHit(hitAlpaca.socketId);
                if (hitAlpaca.hp > 0)
                  spawnFloatingText(hitAlpaca.model, '-💔', 'hearts');
              }
            }
          }
        }

        gScene.value.remove(s.mesh);
        s.mesh.geometry.dispose();
        activeSpits.splice(i, 1);
      }
    }
  };

  return { switchAlpaca, makeSpit, updateSpits };
}

const createLaserBeam = (origin, direction, length) => {
  const geo = new THREE.CylinderGeometry(0.05, 0.05, length, 8);
  geo.translate(0, length / 2, 0);
  const laser = new THREE.Mesh(geo, MATS.spit);
  laser.quaternion.setFromUnitVectors(new THREE.Vector3(0, 1, 0), direction);
  laser.position.copy(origin);
  return laser;
};

const findAlpaca = (alpaca) => {
  while (alpaca) {
    for (let i = 0; i < gAlpacas.length; ++i) {
      if (alpaca.id === gAlpacas[i].model.id) {
        return gAlpacas[i]
      }
    }
    alpaca = alpaca.parent
  }
  return null
}