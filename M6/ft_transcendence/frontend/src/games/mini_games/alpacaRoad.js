import * as THREE from 'three';
import { debug } from '../../services/logger.js';
import * as PRIMITIVES from '../assets/primitives.js';
import { useCoinUI } from '../components/coins.js';
import { editLight } from '../components/editLight.js';
import { useFloatingText } from '../components/floatingText.js';
import { CONST } from '../config/constants.js';
import { createAlpaca, createDecoration, createItem } from '../core/createObjects.js';
import { gMinigame, gPlayer, gScene, gUI } from '../core/globals.js';
import { registerEntity } from '../core/registerEntity.js';
import { removeObject } from '../core/removeObjects.js';
import { attachCollider } from '../core/useCollider.js';
import { usePhysics } from '../core/usePhysics.js';
import { getRandomInt, getRandomTimer } from '../utils/randomValues.js';
import { adjustSunBox, setSunLight, setupLighting } from '../world/sceneBuilder.js';
import { makeAnnouncement } from './annoucement.js';
import { activeClient } from './GameClient.js';
import { saveGameResult } from './utils.js';

const roadLength = 700;
const roadBack = -25;
const roadOffset = roadLength / 2 + roadBack;
export let roadSpeed;
const startZ = roadLength + roadBack;

let level;
let alivePlayers;
let initalPlayerCount;
let activePlayers = [];
const playerPositions = [2.5, -2.5, -7.5, 7.5];

let obstacleTimer = 2;
let timerMultiplier = 1;

const fullPaths = ['models/lamp.glb', 'models/concreteBarricade.glb']
const singlePaths = ['models/trafficCones.glb', 'models/barricade.glb']
const sceneryPaths = ['models/newyorkBuilding.glb', 'models/tree.glb', 'models/bankBuilding.glb', 'models/brooklynBuilding.glb']

const fullObstacle = [];
const singleObstacle = [];
const activeObstacles = [];

const buildingSelection = [];
const roadScene = [];
let assetsLoaded = false;
let totalPoints = 0;

let hasAwardedRewards = false;
let activeTimer = 1.0;

const { spawnFloatingText } = useFloatingText();
const { collectRewards } = useCoinUI();
const { setTimeOfDay } = editLight();

// =========================================================
// 1. INITIALIZATION
// =========================================================

export async function initAlpacaRoad(playerCount, tempAlpacas) {
  cleanupAlpacaRoad()
  gMinigame.value.mode = 3;

  await setupRoadScene(gScene.value);
  await loadAssets();
  await initPlayers(playerCount, tempAlpacas);
  initScenery();
  initObstacles();
  initGameValues(playerCount);
  gMinigame.value.isActive = true;
}

export async function initAlpacaRoadOnline() {
  cleanupAlpacaRoad();
  gMinigame.value.mode = 4;

  await setupRoadScene(gScene.value);
  await loadAssets();

  activePlayers.length = 0;
  gPlayer.value.socketId = activeClient.sessionId;

  gScene.value.add(gPlayer.value.model);
  activePlayers.push(gPlayer.value);
  registerEntity(gPlayer.value, 'alpaca');

  initScenery();
  initGameValues(1);
}

function initGameValues(playerCount) {
  level = 1;
  totalPoints = 0;
  alivePlayers = playerCount;
  initalPlayerCount = playerCount;
  roadSpeed = 30;
  gUI.cameraMode = 2;
  gUI.lockCamera = true;
  gUI.isLightCycling = false;
  assetsLoaded = true;
  hasAwardedRewards = false;
}

async function setupRoadScene(scene) {
  setupLighting(scene);
  setSunLight(25, 125, roadLength / 4, 0, 0, 200);
  adjustSunBox(200, 150, 1, 0, 0, 0);
  setTimeOfDay('day');

  const road = PRIMITIVES.Box(30, 1, roadLength, '#666666')
  road.position.y = -road.geometry.parameters.height / 2;
  road.position.z += roadOffset;
  scene.add(road)

  const sidewalk = await createDecoration('/models/sidewalk.glb');
  roadScene.push(sidewalk.model)
  scene.add(sidewalk.model);

  initRoadStripes();
}

async function loadAssets() {
  if (fullObstacle.length === 0) {
    const loadedFull = await Promise.all(fullPaths.map(path => createItem(path)));
    loadedFull.forEach(item => fullObstacle.push(item.model));
  }
  if (singleObstacle.length === 0) {
    const loadedSingle = await Promise.all(singlePaths.map(path => createItem(path)));
    loadedSingle.forEach(item => singleObstacle.push(item.model));
  }
  if (buildingSelection.length === 0) {
    const loadedAssets = await Promise.all(sceneryPaths.map(path => createDecoration(path)));
    loadedAssets.forEach(item => buildingSelection.push(item.model));
  }
}

function initScenery() {
  if (buildingSelection.length === 0) return;

  const buildingDepth = 60;
  const buildingOffset = -30;
  const rScaleUp = new THREE.Vector3(1, 1, 1);
  const lScaleUp = new THREE.Vector3(-1, 1, 1);
  const numBuildings = Math.ceil(roadLength / buildingDepth) + 1;

  for (let i = 0; i < numBuildings; i++) {
    const zPos = i * buildingDepth;

    // Right
    let id = getRandomID(buildingSelection);
    const rBuild = buildingSelection[id].clone();
    rBuild.position.set(buildingOffset, 0, zPos);
    gScene.value.add(rBuild);
    roadScene.push(rBuild);
    rBuild.userData.targetScale = rScaleUp;

    // Left
    id = getRandomID(buildingSelection);
    const lBuild = buildingSelection[id].clone();
    lBuild.scale.x = -1;
    lBuild.position.set(-buildingOffset, 0, zPos);
    gScene.value.add(lBuild);
    roadScene.push(lBuild);
    lBuild.userData.targetScale = lScaleUp;
  }
}

function initRoadStripes() {
  const numRows = 8;
  const spacingZ = roadLength / numRows;

  const stripeMat = new THREE.MeshStandardMaterial({ color: '#dddddd' });
  const stripe = PRIMITIVES.Box(1, 0.1, 10, stripeMat);
  const targetScale = new THREE.Vector3(1, 1, 1);

  for (let row = 0; row < numRows; row++) {
    let offsetX = -5;
    for (let lane = 0; lane < 3; lane++) {
      const roadStripe = stripe.clone();
      roadStripe.position.z = startZ - (row * spacingZ);
      roadStripe.position.x = offsetX;
      roadStripe.userData.targetScale = targetScale;
      roadScene.push(roadStripe);
      gScene.value.add(roadStripe);
      offsetX += 5;
    }
  }
}

// =========================================================
// 2. GAME LOOP
// =========================================================

export function updateAlpacaRoad(delta) {
  if (!assetsLoaded) return;

  if (!gMinigame.value.isActive) return;
  spawnObstacles(delta)
  updateObstacles(delta)
  updatePlayers(delta)
  updateRoadScene(delta)
  updateDifficulty()
  if (alivePlayers <= 0) endMinigame();
}

export function updateAlpacaRoadOnline(delta) {
  if (!assetsLoaded) return;

  if (gMinigame.value.isGameOver) {
    endMinigame();
    return;
  }
  syncObstacles();
  syncPlayers(delta);
  if (gMinigame.value.isActive) {
    updateRoadScene(delta);
    checkLocalCollisions();
    checkLocalJump();
    checkActivity(delta);
  }
}

// =========================================================
// 3. ONLINE SYNC LOGIC
// =========================================================

export function syncObstacles() {
  const serverObstacles = activeClient.serverData.obstacles;
  if (!serverObstacles) return;

  roadSpeed = activeClient.serverData.roadSpeed;
  const serverIds = new Set(serverObstacles.map(o => o.id));

  for (let i = activeObstacles.length - 1; i >= 0; i--) {
    const localMesh = activeObstacles[i];
    if (!serverIds.has(localMesh.userData.serverId)) {
      removeObstacle(localMesh, i);
    }
  }

  serverObstacles.forEach(serverObs => {
    let localMesh = activeObstacles.find(m => m.userData.serverId === serverObs.id);
    if (!localMesh) {
      localMesh = createObstacleFromBackend(serverObs);
      localMesh.userData.serverId = serverObs.id;
      activeObstacles.push(localMesh);
      gScene.value.add(localMesh);
    }
    localMesh.position.z = serverObs.z;
    localMesh.updateMatrixWorld(true);
  });
}

function syncPlayers(delta) {
  const serverPlayers = gMinigame.value.players;
  if (!serverPlayers) return;

  // Add new players
  serverPlayers.forEach(sPlayer => {
    let localAlpaca = activePlayers.find(p => p.socketId === sPlayer.id);
    if (!localAlpaca && assetsLoaded) {
      const placeholder = { socketId: sPlayer.id, isSpawning: true };
      activePlayers.push(placeholder);

      createAlpaca().then(newAlpaca => {
        const index = activePlayers.findIndex(p => p.socketId === sPlayer.id);
        if (index !== -1) {
          newAlpaca.socketId = sPlayer.id;
          if (sPlayer.color) {
            newAlpaca.setColor(sPlayer.color);
          }
          gScene.value.add(newAlpaca.model);
          activePlayers[index] = newAlpaca;
        }
      });
    }
  });

  // Remove disconnected players
  for (let i = activePlayers.length - 1; i >= 0; i--) {
    const localAlpaca = activePlayers[i];
    if (!serverPlayers.find(p => p.id === localAlpaca.socketId)) {
      if (localAlpaca.model) gScene.value.remove(localAlpaca.model);
      activePlayers.splice(i, 1);
    }
  }

  // Update logic & Lanes
  for (let i = 0; i < activePlayers.length; i++) {
    const localAlpaca = activePlayers[i];
    if (localAlpaca.isSpawning) continue;

    const serverData = serverPlayers.find(p => p.id === localAlpaca.socketId);

    if (serverData) {
      // SET THE CORRECT LANE POSITION!
      if (serverData.lane !== undefined) {
        localAlpaca.model.position.x = playerPositions[serverData.lane];
      }

      // JUMP TRIGGER
      if (localAlpaca.socketId !== activeClient.sessionId) {
        if (serverData.isJumping && !localAlpaca.isJumping) {
          localAlpaca.isJumping = true;
        }
      }

      // HP && HIT
      if (serverData.hp < localAlpaca.hp && !localAlpaca.isBeingHit) {
        localAlpaca.isBeingHit = true;
        spawnFloatingText(localAlpaca.model, '-💔', 'hearts');
      }
      if (serverData.point > localAlpaca.point) {
        spawnFloatingText(localAlpaca.model, '+1');
      }
      localAlpaca.hp = serverData.hp;
      localAlpaca.point = serverData.point;
      localAlpaca.isDead = serverData.isDead;
    }

    if (localAlpaca.isJumping || localAlpaca.model.position.y > 0) handleJump(localAlpaca, delta);

    // SPIN
    if (localAlpaca.isBeingHit) spinAlpacaUp(localAlpaca, delta);

    // DIE
    if (localAlpaca.isDead && localAlpaca.isBeingHit) {
      if (localAlpaca.model.position.z > roadBack) {
        localAlpaca.model.position.z -= (roadSpeed * delta);
      }
    }
  }
}

function handleJump(alpaca, delta) {
  if (alpaca.socketId === activeClient.sessionId)
    return;
  const { model } = alpaca

  if (model.position.y <= CONST.JUMPING_MAX_HEIGHT && !alpaca.isFalling) {
    model.position.y += CONST.JUMPING_SPEED * delta;
    alpaca.isJumping = false;
  }
  // Fall down
  if (model.position.y > 0 && (alpaca.isFalling)) {
    model.position.y -= CONST.JUMPING_SPEED * delta;
    alpaca.isFalling = true;
  }
  // Hit the ground
  if (model.position.y <= 0) {
    model.position.y = 0;
    alpaca.isJumping = false;
    alpaca.isFalling = false;
  }
  // Hit the ceiling/max height of jump
  if (model.position.y >= CONST.JUMPING_MAX_HEIGHT) {
    alpaca.isFalling = true;
  }
}

function checkLocalCollisions() {
  const localAlpaca = activePlayers.find(p => p.socketId === activeClient.sessionId);

  if (!localAlpaca || localAlpaca.isDead || localAlpaca.isBeingHit) return;

  const { checkCollisionWith } = usePhysics();

  if (checkCollisionWith(localAlpaca.model, activeObstacles)) {
    localAlpaca.isBeingHit = true;
    localAlpaca.hp--;
    spawnFloatingText(localAlpaca.model, '-💔', 'hearts');
    activeClient.sendHit();
  }
}

function checkActivity(delta) {
  activeTimer -= delta
  if (activeTimer <= 0) {
    activeClient.sendActive();
    activeTimer = 1.0;
  }
}

function checkLocalJump() {
  const localAlpaca = activePlayers.find(p => p.socketId === activeClient.sessionId);

  if (!localAlpaca || localAlpaca.isDead) return;

  if (localAlpaca.isJumping && !localAlpaca.lastSentJump) {
    activeClient.sendJump();
    localAlpaca.lastSentJump = true;
  } else if (!localAlpaca.isJumping) {
    localAlpaca.lastSentJump = false;
  }
}

function createObstacleFromBackend(serverObs) {
  let obstacle;
  const modelIndex = serverObs.typeId !== undefined ? serverObs.typeId : 0;

  if (!serverObs.isFull) {
    obstacle = singleObstacle[modelIndex].clone();
    obstacle.position.x = playerPositions[serverObs.lane];
    obstacle.userData.isFullWidth = false;
  } else {
    obstacle = fullObstacle[modelIndex].clone();
    obstacle.userData.isFullWidth = true;
  }

  attachCollider(obstacle);
  obstacle.rotation.y = serverObs.rotation || 0;
  obstacle.position.z = serverObs.z;

  obstacle.frustumCulled = false;
  obstacle.traverse(child => { if (child.isMesh) child.frustumCulled = false; });

  return obstacle;
}

// =========================================================
// 4. OFFLINE HELPERS
// =========================================================

export function spawnObstacles(delta) {
  if (fullObstacle.length === 0) return;
  obstacleTimer -= delta;

  if (obstacleTimer <= 0) {
    obstacleTimer = (getRandomTimer() / 2) * timerMultiplier;
    createObstacle();
  }
}

export function createObstacle() {
  let obstacle;
  const isFull = Math.random() > 0.8;

  if (!isFull) {
    obstacle = singleObstacle[getRandomID(singleObstacle)].clone();

    const valid = getValidLanes();
    const targetLane = valid.length > 0 ? valid[getRandomID(valid)] : 0;

    obstacle.position.x = playerPositions[targetLane];
    obstacle.userData.isFullWidth = false;
  } else {
    obstacle = fullObstacle[getRandomID(fullObstacle)].clone();
    obstacle.userData.isFullWidth = true;
  }

  attachCollider(obstacle);
  obstacle.rotation.y = 0;
  obstacle.position.z = roadLength;

  obstacle.pointGiven = false;
  obstacle.frustumCulled = false;
  obstacle.traverse(child => { if (child.isMesh) child.frustumCulled = false; });

  activeObstacles.push(obstacle);
  gScene.value.add(obstacle);
}

async function initPlayers(playerCount, tempAlpacas) {
  activePlayers.length = 0;
  activePlayers.push(gPlayer.value);
  registerEntity(gPlayer.value, 'alpaca');
  gMinigame.value.players = [];

  for (let i = 0; i < playerCount - 1; i++) {
    if (tempAlpacas[i]) {
      activePlayers.push(tempAlpacas[i]);
      registerEntity(tempAlpacas[i], 'alpaca');
    } else {
      activePlayers.push(await createAlpaca());
    }
  }
  for (let i = 0; i < activePlayers.length; i++) {
    const alpaca = activePlayers[i];
    gScene.value.add(alpaca.model);
    alpaca.model.position.x = playerPositions[i];
    if (gMinigame.value.mode !== 4) {
      gMinigame.value.players.push({ id: i + 1, name: alpaca.name, hp: CONST.HP, point: 0 });
    }
  }
}

function initObstacles() {
  const amount = 8;
  for (let i = 0; i < amount; ++i) {
    createObstacle();
    activeObstacles[i].position.z = startZ - (roadLength / amount * i);
  }
}

function updatePlayers(delta) {
  for (let i = 0; i < activePlayers.length; i++) {
    const alpaca = activePlayers[i];
    if (gMinigame.value.players[i]) {
      gMinigame.value.players[i].hp = alpaca.hp;
      gMinigame.value.players[i].point = alpaca.point;
    }
    checkAlpaca(alpaca);
    if (alpaca.isBeingHit) {
      spinAlpacaUp(alpaca, delta);
    }
    if (alpaca.isDead && alpaca.isBeingHit) {
      if (alpaca.model.position.z > roadBack) {
        alpaca.model.position.z -= (roadSpeed * delta);
      }
    }
  }
}

function updateDifficulty() {
  const pointsPerLevel = 4 + level;
  const avgPoints = totalPoints / initalPlayerCount;
  const newLevel = Math.floor(avgPoints / pointsPerLevel) + 1;

  if (newLevel > level) {
    const minSpeed = 25;
    const maxSpeed = 100;
    const factor = 0.08;
    const difficultyFactor = 1 - Math.exp(-factor * newLevel);

    const newSpeed = minSpeed + (maxSpeed - minSpeed) * difficultyFactor;
    const newTimerMult = Math.max(0.5, timerMultiplier - 0.05);

    applyLevelUp(newLevel, newSpeed, newTimerMult);
  }
}

function applyLevelUp(newLevel, newSpeed, newTimerMult) {
  level = newLevel;
  roadSpeed = newSpeed;
  timerMultiplier = newTimerMult;
  makeAnnouncement(`LEVEL ${level}`, 2000);
}

function updateObstacles(delta) {
  for (let j = activeObstacles.length - 1; j >= 0; j--) {
    let obstacle = activeObstacles[j];
    if (!obstacle) continue;
    obstacle.updateMatrixWorld(true);
    obstacle.position.z -= roadSpeed * delta;
    if (obstacle.position.z < -0.25) {
      if (obstacle.userData.isCollider === true && !obstacle.pointGiven) {
        awardPoints(obstacle);
      }
      if (obstacle.position.z < -roadLength / 2 + roadOffset) {
        removeObstacle(obstacle, j);
      }
    }
  }
}

function checkAlpaca(alpaca) {
  if (alpaca.isDead || alpaca.isBeingHit) return;

  const { checkCollisionWith } = usePhysics();
  const isColliding = checkCollisionWith(alpaca.model, activeObstacles);

  if (isColliding) {
    alpaca.isBeingHit = true;
    alpaca.hp--;
    spawnFloatingText(alpaca.model, '-💔', 'hearts');
    if (alpaca.hp === 0) {
      alpaca.isDead = true;
      setTimeout(() => {
        alivePlayers--;
      }, 1500);
    }
  }
}

function awardPoints(obstacle) {
  for (let i = 0; i < activePlayers.length; i++) {
    const alpaca = activePlayers[i];
    if (!alpaca.isDead && !alpaca.isBeingHit) {
      if (obstacle.userData.isFullWidth) {
        alpaca.point++;
        totalPoints++;
        spawnFloatingText(alpaca.model, '+1');
      } else {
        const distance = Math.abs(alpaca.model.position.x - obstacle.position.x);
        if (distance < 1) {
          activePlayers[i].point++;
          totalPoints++;
          spawnFloatingText(alpaca.model, '+1');
        }
      }
    }
  }
  obstacle.pointGiven = true;
}

function getRandomID(array) {
  return getRandomInt(array.length);
}

function getValidLanes() {
  const validLanes = [];
  for (let i = 0; i < activePlayers.length; i++) {
    if (!activePlayers[i].isDead) validLanes.push(i);
  }
  return validLanes;
}

// =========================================================
// 5. SHARED UTILS (Online & Offline)
// =========================================================

function endMinigame() {
  gMinigame.value.isGameOver = true;

  if (hasAwardedRewards) return;
  hasAwardedRewards = true;

  gMinigame.value.isActive = false;

  let playerPoints = 0;
  if (gMinigame.value.isOnline) {
    const local = activePlayers.find(p => p.socketId === activeClient.sessionId);
    if (local) playerPoints = local.point || 0;
  } else {
    playerPoints = activePlayers[0].point || 0;
  }

  const earnedCoins = Math.floor(playerPoints / 2);
  debug(`Minigame Over! Points: ${playerPoints}, Coins: ${earnedCoins}`);

  if (earnedCoins > 0) {
    setTimeout(() => { collectRewards(earnedCoins); }, 50);
  }

  // Offline only — server-side persists online matches via _persistOutcome.
  // Backend rejects 'win' here; we only contribute the obstacle count to
  // the leaderboard so a fast run still ranks even if the player died.
  if (!gMinigame.value.isOnline && playerPoints > 0) {
    saveGameResult('alpaca_road', 'loss', { obstacles: playerPoints, stage: level });
  }
}

function removeObstacle(obstacle, index) {
  gScene.value.remove(obstacle);
  activeObstacles.splice(index, 1);
  removeObject(obstacle);
}

function updateRoadScene(delta) {
  const movement = roadSpeed * delta;
  for (let i = 1; i < roadScene.length; i++) {
    const item = roadScene[i];
    item.position.z -= movement;
    if (item.position.z < roadBack - 10) {
      item.position.z = startZ;
      item.scale.set(0.1, 0.1, 0.1);
    }
    const target = item.userData.targetScale;
    if (item.scale.distanceTo(target) < 0.01) {
      item.scale.copy(item.userData.targetScale);
    } else {
      item.scale.lerp(item.userData.targetScale, delta * 5);
    }
  }
}

function spinAlpacaUp(alpaca, delta) {
  const spinSpeed = 6.0 * (delta);
  const liftSpeed = 30 * (delta);

  alpaca.model.rotation.x += spinSpeed;
  if (alpaca.model.rotation.x < Math.PI) {
    alpaca.model.position.y += liftSpeed;
  } else {
    alpaca.model.position.y -= liftSpeed;
  }

  if (alpaca.model.rotation.x > Math.PI * 2) {
    alpaca.model.rotation.x = 0;
    alpaca.model.position.y = 0;
    alpaca.isBeingHit = false;
    if (gMinigame.value.isOnline && alpaca.socketId === activeClient.sessionId) {
      activeClient.sendHitComplete();
    }
  }
}

function cleanupAlpacaRoad() {
  assetsLoaded = false;
  gUI.isLightCycling = true;

  activeObstacles.forEach(obj => {
    gScene.value.remove(obj);
    removeObject(obj);
  });
  activeObstacles.length = 0;

  roadScene.forEach(item => gScene.value.remove(item));
  roadScene.length = 0;

  activePlayers.forEach(alpaca => {
    if (alpaca !== gPlayer.value) {
      gScene.value.remove(alpaca.model);
    } else {
      alpaca.model.position.set(0, 0, 0);
      alpaca.model.rotation.set(0, 0, 0);
      alpaca.isDead = false;
      alpaca.isBeingHit = false;
    }
  });
  activePlayers.length = 0;
  setSunLight();
  adjustSunBox();

  gUI.lockCamera = false;
  gUI.cameraMode = 1;
  gMinigame.value.isGameOver = false;
  debug("🧹 Minigame cleaned up.");
}
