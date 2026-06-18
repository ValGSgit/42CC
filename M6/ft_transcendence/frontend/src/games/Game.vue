<template>
  <div v-if="showLoginWarning" class="modal-overlay">
    <div class="shop-title">Welcome to Alpaca Party!
      <button class="shop-btn" @click="warningOff" title="Close">Try</button>
      <router-link to="/login" class="shop-btn">Login</router-link>
      <div class="features">
      <div class="feature-card">
        <h3><AppIcon name="alpaca" :size="36" /> Raise Alpacas</h3>
        <p>Buy, name and customise alpacas with unique colours and speeds.</p>
      </div>
      <div class="feature-card">
        <h3><AppIcon name="barn" :size="36" /> Build Your Farm</h3>
        <p>Plant trees, expand land and decorate your world.</p>
      </div>
      <div class="feature-card">
        <h3><AppIcon name="coin" :size="36" /> Earn Coins</h3>
        <p>Collect coins to unlock upgrades and grow your herd.</p>
      </div>
        <div class="feature-card">
        <h3><AppIcon name="joystick-full" :size="36" /> Mini Games</h3>
        <p>Play Mini games with your friends, online and offline.</p>
      </div>
    </div>
    </div>
  </div>

  <div ref="gameContainer" class="scene-container"></div>
  <div v-if="!gameIsReady" class="modal-overlay">Loading...</div>
  <div v-if="gameIsReady">

  <div v-if="gMinigame.mode && gMinigame.isGameOver" class="modal-overlay">
    <div class="shop-title">
      <div>Final Score:</div>
      <button v-if="gMinigame.mode === 1 || gMinigame.mode === 2" class="shop-btn">Score: {{ gUser.point }} <AppIcon name="alpaca" :size="36" /></button>
      <template v-if="gMinigame.mode === 3 || gMinigame.mode === 4">
        <button v-for="player in gMinigame.players" :key="'end-' + player.id" class="shop-btn">
          {{ player.name || `P${player.id}` }} Score: {{ player.point }} <AppIcon name="score" :size="24" />
        </button>
      </template>
      <div class="action-container">
        <button v-if="gMinigame.mode === 1 || gMinigame.mode === 3" class="shop-btn" @click="changeGame(gMinigame.mode, playerCount)">
          Play Again <AppIcon name="refresh-box" :size="24" />
        </button>
        <button class="shop-btn" @click="changeGame()" title="Return to Farm">
          Return to Farm <AppIcon name="tractor" :size="24" />
        </button>
      </div>
    </div>
  </div>

<div class="hud-container hud-left">
    <div
      id="coin-hud"
      v-if="gMinigame.mode === 0 || gMinigame.isGameOver"
      class="stat" :class="{ 'overlay': gMinigame.isGameOver }">
      <span class="hud-coins">
        <AppIcon name="coin" :size="36" />
        <span class="hud-coins-val">{{ gUser.coins }}</span>
      </span>
    </div>
      <template v-if="gMinigame.mode === 1 || gMinigame.mode === 2">
        <div v-for="player in gMinigame.players" :key="'ui-' + player.id" class="stat multiplayer-row">
          <span class="p-name">{{ player.name || `P${player.id}`}}:</span>
          <span class="p-hp">{{ getHearts(player.hp) }}</span>
          <span class="p-point"><AppIcon name="alpaca" :size="36" /> {{ player.point }}</span>
        </div>
      </template>
      <template v-if="gMinigame.mode > 2 &&!gMinigame.isGameOver">
        <div v-for="player in gMinigame.players" :key="'ui-' + player.id" class="stat multiplayer-row">
          <span class="p-name">{{ player.name || `P${player.id}`}}:</span>
          <span class="p-hp">{{ getHearts(player.hp) }}</span>
          <span class="p-point"><AppIcon name="score" :size="24" /> {{ player.point }}</span>
        </div>
      </template>
  </div>

    <div class="floating-text-container">
      <div 
        v-for="popup in floatingTexts" 
        :key="popup.id" 
        :class="['floating-text', `floating-text-${popup.type}`]"
        :style="{ left: popup.x + 'px', top: popup.y + 'px' }"
      >
        {{ popup.text }}
      </div>
    </div>

    <div class="hud-container hud-right">
      <button class="hud-btn" @click="openGameMenu()" :title="gMinigame.mode ? 'Exit Game' : 'Minigames'"><AppIcon :name="gMinigame.mode ? 'exit' : 'joystick-full'" :size="28" /></button>
      <button v-if="isAuthenticated && !gMinigame.mode" class="hud-btn" @click="openLobbyMenu(5)" title="Visit Farm"><AppIcon name="barn" :size="28" /></button>
      <button v-if="!gMinigame.mode" class="hud-btn" @click="openShopMenu()" title="Shop"><AppIcon name="shopping-bags" :size="28" /></button>
      <button v-if="!gMinigame.mode" class="hud-btn" @click="openEditMode()" title="Edit Scene"><AppIcon name="pencil-ruler" :size="28" /></button>
      <button v-if="!gMinigame.mode" class="hud-btn" @click="openLightMenu()" title="Edit Light"><AppIcon name="sun-full" :size="30" /></button>
      <button v-if="!gMinigame.mode" class="hud-btn" @click="changeCamera()" title="Change Camera"><AppIcon name="camera-farm" :size="28" /></button>
      <button v-if="!gMinigame.mode && CONST.DEBUG" class="hud-btn" @click="addDebugCoins()" title="DEBUG: Add Coins" style="background: #ffd700; color: #000;"><AppIcon name="debug-coin" :size="28" /></button>
    </div>
    
    <div v-if="gUI.shopMenu" class="modal-overlay">
      <div class="shop-title">Mini Shop
        <button class="shop-btn" @click="openFarmMenu()" title="Upgrade Farm"><AppIcon name="tractor" :size="32" /> Upgrade Farm</button>
        <button class="shop-btn" @click="openAlpacaShop()" title="Buy Alpaca"><AppIcon name="alpaca-buy" :size="32" /> Buy Alpaca</button>
        <button class="shop-btn" @click="openItemShop()" title="Buy Item"><AppIcon name="barn" :size="32" /> Buy Item</button>
        <button class="close-btn" @click="closeShopMenu()" title="Close"><AppIcon name="close" :size="18" /></button>
      </div>
    </div>

    <div v-if="gUI.gameMenu" class="modal-overlay">
      <div class="shop-title">Select Game
        <div class="color-grid">
        <button class="shop-btn" @click="changeGame(1, playerCount)" title="Spit Royale with AI">Spit Royale</button>
        <button v-if="isAuthenticated" class="shop-btn" @click="openLobbyMenu(2)" title="Spit Royale Online">Online Lobby</button>
        <button class="shop-btn" @click="changeGame(3, playerCount)" title="Alpaca Road">Alpaca Road</button>
        <button v-if="isAuthenticated" class="shop-btn" @click="openLobbyMenu(4)" title="Alpaca Road Online">Online Lobby</button>
      </div>
      <select v-model="playerCount" class="player-selector" title="Number of Players">
        <option :value="1">1 Player</option>
        <option :value="2">2 Players</option>
        <option :value="3">3 Players</option>
        <option :value="4">4 Players</option>
      </select>
        <button class="close-btn" @click="closeGameMenu()" title="Close"><AppIcon name="close" :size="22" /></button>
      </div>
    </div>
    <MultiplayerLobby v-if="gUI.lobbyMenu && (gMinigame.mode === 2 || gMinigame.mode === 4)" />
    <div v-if="gUI.lobbyMenu && gMinigame.mode === 5" class="modal-overlay">
      <div class="shop-title">Visit A Friend's Farm
        <div v-for="friend in gMinigame.lobby" :key="friend.id">
          <button class="shop-btn" @click="visitFarm(friend.id, friend.username)">
            {{ friend.username }}
          </button>
        </div>
        <button class="close-btn" @click="changeGame(0)" title="Close"><AppIcon name="close" :size="22" /></button>
      </div>
    </div>
    <div v-if="gMinigame.mode === 5 && gMinigame.isVisiting" class="edit-mode">
      <div class="shop-title"> Visiting {{ friendName }}'s Farm
      </div>
    </div>
    <div v-if="gUI.farmMenu" class="modal-overlay">
      <div class="shop-title">
        Upgrade Farm
        <div class="stats-content">
          <div class="stat-row"> Current Farm Size: {{ gUser.upgrades }}/{{ CONST.MAX_UPGRADES }} </div>
          <div class="stat-row"> Current Herd Size: {{ gAlpacas.length}}/{{getHerdSize(gUser.herdsize)}} </div>
        </div>

        <div class="itemshop-grid">
        <button class="itemshop-card" @click="increaseFarmSize(gUser.upgrades)">
          <span class="item-name">Increase Farm Size</span>
          <div class="icon-container">
            <span style="position: relative; bottom: 10px;"><AppIcon name="tractor" :size="84" /></span>
            <span class="item-cost"> {{ getUpgradeCost(gUser.upgrades)}} <AppIcon name="coin" :size="36" /> </span>
          </div>
        </button>

        <button class="itemshop-card" @click="increaseHerdSize(gUser.herdsize)">
          <span class="item-name">Increase Herd Size</span>
          <div class="icon-container">
            <span style="position: relative; bottom: 5px;"><AppIcon name="alpaca" :size="72" /></span>
            <span class="item-cost"> {{ getHerdSizeCost(gUser.herdsize) }} <AppIcon name="coin" :size="36" /> </span>
          </div>
        </button>
      </div>

        <button class="close-btn" @click="gUI.farmMenu = false" title="Close"><AppIcon name="close" :size="22" /></button>
      </div>
    </div>

    <div v-if="gUI.itemShop" class="modal-overlay">
      <div class="shop-title"> 
        Buy Item 
        
        <div class="itemshop-grid">
          <button v-for="item in shopItems" :key="item.name" class="itemshop-card" @click="buyItem(item)">
            <span class="item-name">{{ item.name }}</span>
            <div class="icon-container">
              <img :src="item.icon" :alt="item.name" class="item-icon" />
              <span class="item-cost">{{ item.cost }} <AppIcon name="coin" :size="36" /></span>
            </div>
          </button>
        </div>
        
        <button class="close-btn" @click="closeItemShop()" title="Close"><AppIcon name="close" :size="22" /></button>
      </div>
    </div>
    
    <div v-if="gUI.alpacaShop" class="modal-overlay">
      <div class="shop-title">Buy Alpaca
        <div class="input-group">
          <label>Name</label>
          <input type="text" v-model="alpacaConfig.name" placeholder="Name your alpaca..." />
        </div>
        <div class="input-group">
          <label>Size </label>
          <input type="range" v-model.number="alpacaConfig.scale" min="0.75" max="1.25" step="0.05" />
        </div>
        <div class="color-grid">
          <button class="shop-btn" @click="alpacaConfig.color = '#795740'; buyAlpaca()" title="Brown">Brown</button>
          <button class="shop-btn" @click="alpacaConfig.color = '#111111'; buyAlpaca()" title="Black">Black</button>
          <button class="shop-btn" @click="alpacaConfig.color = '#555555'; buyAlpaca()" title="Grey">Grey</button>
          <button class="shop-btn" @click="alpacaConfig.color = '#ffffff'; buyAlpaca()" title="White">White</button>
        </div>
        <div class="custom-color-row">
          <input type="color" v-model="alpacaConfig.color" class="custom-picker" />
          <button class="shop-btn" @click="buyAlpaca()" title="Custom">Custom</button>
        </div>
        <button class="close-btn" @click="closeAlpacaShop()" title="Close"><AppIcon name="close" :size="22" /></button>
      </div>
    </div>
    
    <div v-if="gUI.alpacaStats" class="modal-overlay">
      <div class="shop-title">Alpaca Stats
        <div class="stats-content" :key="updateVue">
          <div class="stat-row">
            <strong>Name:</strong> 
            <span v-if="!gUI.isEditingName" class="editable-text">
              {{ gPlayer.name }}
              <button class="icon-btn" @click="gUI.isEditingName = true" title="Edit Name"><AppIcon name="pencil-ruler" :size="16" /></button>
            </span>
            <span v-else class="editing-mode">
              <input type="text" v-model="gPlayer.name" @keyup.enter="changeName(gPlayer.name); gUI.isEditingName = false" class="name-input" />
            </span>
          </div>
          <div class="stat-row"><strong>Age:</strong> {{ gPlayer.age }}</div>
          <div class="stat-row">
            <strong>Color:</strong> 
            <input type="color" v-model="gPlayer.color" @change="changeColor(gPlayer.color)" class="custom-picker" title="Change Alpaca Color" />
          </div>
          <div class="stat-row">
            <strong>Speed:</strong> 
            <span v-if="gPlayer.speedOffset === 0"> Normal</span>
            <span v-if="gPlayer.speedOffset > 0"> Fast</span>
            <span v-if="gPlayer.speedOffset < 0"> Slow</span>
            <div class="speed-controls">
              <button class="stat-btn" @click="changeSpeed(-1)" title="Decrease Speed">-</button>
              <button class="stat-btn" @click="changeSpeed(1)" title="Increase Speed">+</button>
            </div>
          </div>
        </div>
        <button class="close-btn" @click="closeAlpacaStats()" title="Close"><AppIcon name="close" :size="22" /></button>
      </div>
    </div>
        
    <div v-if="gUI.editMode" class="edit-mode">
      <div class="shop-title"> Edit Mode
          <div v-if="gEditState.selected" class="edit-actions">
            <button class="shop-btn" @click="sellItem()"><AppIcon name="coin" :size="22" /> Sell Item</button>
            <button class="shop-btn" @click="cancelPlacement()">Cancel <AppIcon name="close" :size="22" /></button>
          </div>
      <button class="close-btn" @click="closeEditMode()" title="Close"><AppIcon name="close" :size="22" /></button>
      </div>
        <div class="controls-hint">
        <h4><AppIcon name="tools" :size="16" /> Edit Controls</h4>
        <div class="control-row">
          <span><AppIcon name="mouse" :size="16" /> Click</span>
          <span>Select / Place</span>
        </div>
        <div class="control-row">
          <span><AppIcon name="scroll-icon" :size="16" /> Scroll</span>
          <span>Rotate</span>
        </div>
        <div class="control-row">
          <span><AppIcon name="scroll-icon" :size="16" /> Shift + Scroll</span>
          <span>Scale</span>
        </div>
      </div>
    </div>

    <div v-if="gUI.lightMenu" class="modal-overlay">
      <div class="shop-title">Edit Light
        <div class="toggle-row">
          <span>Day/Night Cycle</span>
          <label class="switch" title="Toggle Day/Night Cycle">
            <input type="checkbox" :checked="gUI.isLightCycling" @change="toggleLightCycle()">
            <span class="slider"></span>
          </label>
        </div>
        <button class="shop-btn" @click="setTimeOfDay('sunrise')"><AppIcon name="sunrise" :size="30" /> Sunrise</button>
        <button class="shop-btn" @click="setTimeOfDay('day')"><AppIcon name="sun-full" :size="30" /> Day</button>
        <button class="shop-btn" @click="setTimeOfDay('sunset')"><AppIcon name="sunset" :size="30" /> Sunset</button>
        <button class="shop-btn" @click="setTimeOfDay('night')"><AppIcon name="night" :size="30" /> Night</button>
        <button class="close-btn" @click="closeLightMenu()" title="Close"><AppIcon name="close" :size="22" /></button>
      </div>
    </div>
  </div>
</template>

<!---------------------- SCRIPT ------------------------->
<script setup>
import AppIcon from '../components/AppIcon.vue'
import MultiplayerLobby from './components/MultiplayerLobby.vue'

import { storeToRefs } from 'pinia'
import * as THREE from 'three'
import { StereoEffect } from 'three/addons/effects/StereoEffect.js'
import { onMounted, onUnmounted, ref, shallowRef } from 'vue'
import { devError } from '../services/logger.js'
import { useAuthStore } from '../stores/auth.js'
import '../styles/games/game.css'
import { alpacaHandling } from './components/alpacaHandling.js'
import { alpacaConfig, alpacaShop } from './components/alpacaShop.js'
import { alpacaStats } from './components/alpacaStats.js'
import { updateCoins } from './components/coins.js'
import { editLight } from './components/editLight.js'
import { useEditMode } from './components/editMode.js'
import { useFloatingText } from './components/floatingText.js'
import { itemShop } from './components/itemShop.js'
import { getHerdSize, getHerdSizeCost, getUpgradeCost, upgradeFarm } from './components/upgradeFarm.js'
import { CONST } from './config/constants.js'
import { addDebugCoins } from './core/debug.js'
import { updateAlpacas } from './core/entities/Alpaca.js'
import { updateCollectables } from './core/entities/Collectable.js'
import { shopItems } from './core/entities/Item.js'
import { cleanupFPSstats, initFPSstats } from './core/FPSstats.js'
import { gAlpacas, gEditState, gEngine, gMinigame, gPlayer, gScene, gUI, gUser } from './core/globals.js'
import { flushSave } from './core/saveLoadGame.js'
import { changeCamera, checkControlsEnabled, useCamera } from './core/useCamera.js'
import { useGameEngine } from './core/useGameEngine.js'
import { useInput } from './core/useInput.js'
import { init_redot, render_redot } from './core/useSpatialBridge.js'
import { useUIManager } from './core/useUIManager.js'
import { watchChanges } from './core/watchChanges.js'
import { changeGame, friendName, returnFarm, visitFarm } from './mini_games/init.js'
import { updateMinigame } from './mini_games/minigames.js'
import { getHearts } from './utils/uiHelpers.js'
import { initWorld } from './world/initWorld.js'
import { debug } from '../services/logger.js'

const gameContainer = ref(null)
const gameIsReady= shallowRef(false)
const clock = new THREE.Clock()

const { changeColor, changeName, changeSpeed, updateVue } = alpacaStats()
const { initInput, cleanupInput, updateInputState, resetInput} = useInput()
const { setTimeOfDay, updateLighting, toggleLightCycle} = editLight()
const { buyAlpaca } = alpacaShop()
const { openEditMode, closeEditMode, openShopMenu, closeShopMenu, openFarmMenu, openAlpacaShop, closeAlpacaShop, closeAlpacaStats, openItemShop, closeItemShop, openLightMenu, closeLightMenu, openGameMenu, closeGameMenu, openLobbyMenu, closeLobbyMenu } = useUIManager()
const { init, cleanup, onResize } = useGameEngine(gameContainer)
const { increaseFarmSize, increaseHerdSize } = upgradeFarm()
const { buyItem } = itemShop()
const authStore = useAuthStore()
const { isAuthenticated } = storeToRefs(authStore)
const { cancelPlacement, sellItem} =  useEditMode()
const showLoginWarning = ref(false);
const warningOff = () => {showLoginWarning.value = false;};
const { updateSpits } = alpacaHandling();
const { floatingTexts} = useFloatingText();

let animationFrameId
let cameraUpdate = null;
let stopMyWatcher
let stats;
let playerCount = 1
let effect

onMounted(async () => {
  document.body.classList.add('lock-screen');
  if (!authStore.isAuthenticated)
    showLoginWarning.value = true
  else
    showLoginWarning.value = false
  gEngine.value = init()
  if (CONST.AR_ENABLED)
    init_redot();
  if (CONST.SBS_ENABLED)
  {
    effect = new StereoEffect(gEngine.value.renderer);
    effect.setSize(window.innerWidth, window.innerHeight);
  }

  if (!gEngine.value) {
    devError('Init failed: Scene not returned from globalEngine.')
  }
  else
  {
    stats = initFPSstats(gameContainer.value);
    initInput()
    const { updateCamera } = useCamera(gEngine.value.camera, gEngine.value.controls)
    cameraUpdate = updateCamera

    if (authStore.isAuthenticated && authStore.user?.username)
      gUser.value.name = authStore.user.username;
    await initWorld(gScene.value, authStore.isAuthenticated)
    gameIsReady.value = true
    stopMyWatcher = watchChanges()
    if(gMinigame.value)
      returnFarm()
    gameLoop()
  }
  window.addEventListener('resize', onResize)
})

const gameLoop = () => {
  if (stats) stats.begin();
  resetInput()
  animationFrameId = requestAnimationFrame(gameLoop)

  const delta = clock.getDelta()
  const player = gPlayer.value

  if (player) {
    if (cameraUpdate) {
      cameraUpdate(player)
    }
  }
  updateInputState()
  updateAlpacas(delta);
  updateCollectables(player, delta);
  updateCoins(delta);
  updateSpits(delta)
  updateLighting(delta);
  if(gMinigame.value.isActive) {
    updateMinigame(delta);
  }

  if (gEngine.value?.controls) {
    gEngine.value.controls.enabled = checkControlsEnabled();
    if (gEngine.value.controls.enabled) {
      gEngine.value.controls.update()
    }
  }
  if (CONST.SBS_ENABLED)
    effect.render(gEngine.value.scene, gEngine.value.camera);
  else if (gEngine.value?.renderer) {
    gEngine.value.renderer.render(
    gEngine.value.scene,
    gEngine.value.camera)
  }

  if (stats) stats.end();
  if (gUI.cameraMode === 3) // render red dot for first person mode
    render_redot()
}

onUnmounted(async () => {
  document.body.classList.remove('lock-screen');
  try {
    await flushSave()
  } catch (e) {
    // best-effort flush; continue teardown even on failure
    // eslint-disable-next-line no-console
    debug('flushSave failed during unmount:', e)
  }
  if (stopMyWatcher) stopMyWatcher()
  cancelAnimationFrame(animationFrameId)
  window.removeEventListener('resize', onResize)
  cleanupFPSstats(stats, gameContainer.value);
  cleanupInput();
  cleanup()
})
</script>

<!---------------------- STYLE ------------------------->
<style src="../styles/games/game.css"></style>
