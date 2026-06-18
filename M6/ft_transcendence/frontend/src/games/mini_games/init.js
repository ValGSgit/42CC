import { ref } from 'vue';
import { debug } from '../../services/logger.js';
import { useAuthStore } from '../../stores/auth.js';
import { clearCoins } from '../components/coins.js';
import { CONST } from '../config/constants.js';
import { gAlpacas, gMinigame, gPlayer, gScene, gUI, gUser } from '../core/globals.js';
import { flushSave, pauseSaves, resumeSaves } from '../core/saveLoadGame.js';
import { useGameEngine } from '../core/useGameEngine.js';
import { initWorld } from '../world/initWorld.js';
import { initAlpacaRoad, initAlpacaRoadOnline } from './alpacaRoad.js';
import { clearAnnouncements } from './annoucement.js';
import { activeClient } from './GameClient.js';
import { initSpitRoyalAI, initSpitRoyalOnline } from './spitRoyal.js';

const miniGameContainer = ref(null)
const { clearScene, resetGArrays } = useGameEngine(miniGameContainer)
const tempAlpacas = []
let visitPlayerId = null
export let friendName = null

export async function changeGame(mode = 0, playerCount = 1) {
  gUI.gameMenu = false
  debug("changeGame:", mode);
  if (!gPlayer.value || !gUser.value) return;
  if (gMinigame.value.mode === 0) await flushSave();

  const savedPlayers = gMinigame.value.players;

  resetMinigame();
  gMinigame.value.players = savedPlayers;
  gMinigame.value.mode = mode;
  gMinigame.value.isOnline = (gMinigame.value.mode === 2 || gMinigame.value.mode === 4);
  gUser.value.hp = CONST.HP
  gUser.value.point = 0
  resetAlpaca(gPlayer.value)
  tempAlpacas.length = 0
  for (let i = 1; i < gAlpacas.length; ++i) {
    if (gAlpacas[i] !== gPlayer.value) {
      tempAlpacas.push(gAlpacas[i])
      resetAlpaca(gAlpacas[i])
    }
  }
  clearScene(gScene.value)
  clearCoins()
  resetGArrays()
  initGameMode(mode, playerCount, tempAlpacas);
}

export async function returnFarm() {
  const authStore = useAuthStore()

  activeClient.disconnect();
  clearAnnouncements();
  resetMinigame();
  gMinigame.value.mode = 0;

  gUI.lobbyMenu = false;
  gUI.lockCamera = false;
  gUI.cameraMode = 0
  visitPlayerId = null

  // Block farm-mode autosaves until the world is fully loaded. Without this,
  // `gPlayer.value = null` (and coin/upgrade refreshes inside loadGameData)
  // trigger the watcher → saveGame() at T+300ms. If initWorld takes longer
  // (network + GLB loads commonly do), the debounced save writes empty
  // gAlpacas/gItems back to the server and wipes the user's farm.
  pauseSaves()
  try {
    gPlayer.value = null
    clearScene(gScene.value)
    resetGArrays()
    await initWorld(gScene.value, authStore.isAuthenticated)
  } finally {
    resumeSaves()
  }
  await flushSave()
}

function resetAlpaca(alpaca,) {
  alpaca.hp = CONST.HP
  alpaca.point = 0
  alpaca.isDead = 0
  alpaca.model.position.set(0, 0, 0)
  alpaca.model.rotation.y = 0
}

function resetMinigame() {
  gMinigame.value.isOnline = false;
  gMinigame.value.isReady = false;
  gMinigame.value.isActive = false;
  gMinigame.value.isGameOver = false;
  gMinigame.value.isVisiting = false;
  gMinigame.value.currentRoomName = null;
  gMinigame.value.players = [];
}

async function initGameMode(mode, playerCount, tempAlpacas) {
  debug("initGameMode: ", mode);
  switch (mode) {
    case 1:
      initSpitRoyalAI(10, tempAlpacas);
      break;
    case 2:
      initSpitRoyalOnline();
      break;
    case 3:
      initAlpacaRoad(playerCount, tempAlpacas);
      break;
    case 4:
      initAlpacaRoadOnline(playerCount, tempAlpacas);
      break;
    case 5: { // visit farm
      const authStore = useAuthStore();
      // Visiting someone else's farm — the load also nulls gPlayer and
      // mutates gUser, which would scheduled an autosave on top of someone
      // else's data. Suppress until the load completes.
      pauseSaves();
      try {
        await initWorld(gScene.value, authStore.isAuthenticated, visitPlayerId);
      } finally {
        resumeSaves();
      }
      break;
    }
    default:
      await returnFarm();
  }
}

export async function visitFarm(playerId, username) {
  visitPlayerId = playerId
  friendName = username
  gUI.lobbyMenu = false
  changeGame(5)
  gMinigame.value.isVisiting = true;
}