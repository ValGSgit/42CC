import * as THREE from 'three';
import { CONST } from '../config/constants.js';
import { collectables } from '../core/entities/Collectable.js';
import { gMinigame, gScene, gUser } from '../core/globals.js';
import { getRandomTimer } from '../utils/randomValues.js';
import { spawnObjectRandomly } from '../utils/spawnRandomly.js';

const coinsGroup = new THREE.Group();
let isGroupAdded = false;
let timer = 1
const coin = collectables.find(item => item.name === 'Coin');

async function spawnCoin() {
  timer = getRandomTimer();

  if (!isGroupAdded && gScene.value) {
    gScene.value.add(coinsGroup);
    isGroupAdded = true;
  }
  const tempGroup = await spawnObjectRandomly(coin, 1);
  if (tempGroup && tempGroup.children.length > 0) {
    coinsGroup.add(tempGroup.children[0]);
  }
}

export async function updateCoins(delta) {
  if (gMinigame.value.mode !== 0) return;

  const spinSpeed = 1.5;
  coinsGroup.children.forEach((coin) => {
    coin.rotation.y += spinSpeed * delta;
  });

  if (coinsGroup.children.length <= CONST.MAX_COINS) {
    timer -= delta;
    if (timer <= 0) {
      await spawnCoin();
    }
  }
}

export function spendCoins(cost) {
  gUser.value.coins -= cost
}

export function clearCoins() {
  coinsGroup.children.length = 0
  isGroupAdded = false
}

export function addCoins(amount) {
  gUser.value.coins += amount;
}

export function useCoinUI() {

  const spawnFlyingCoin = (index, targetX, targetY) => {
    const coin = document.createElement('div');
    coin.innerHTML = '<img src="/icons/ui/coin.svg" alt="" width="42" height="42" />';
    coin.className = 'flying-coin';

    const isUp = Math.random() > 0.5;
    const randomArc = isUp ? 0.3 + Math.random() * 0.5 : - 0.1 - Math.random() * 0.5
    coin.style.setProperty('--arc-factor', randomArc);

    const startX = window.innerWidth / 2 + (Math.random() * 100 - 50);
    const startY = window.innerHeight / 2 + (Math.random() * 100 - 50);
    coin.style.left = `${startX}px`;
    coin.style.top = `${startY}px`;

    document.body.appendChild(coin);

    requestAnimationFrame(() => {
      coin.style.transform = 'translate(-50%, -50%) scale(1)';
    });

    setTimeout(() => {
      coin.style.left = `${targetX}px`;
      coin.style.top = `${targetY}px`;
      coin.classList.add('is-flying');
    }, 50 + index * 4);

    coin.addEventListener('transitionend', (e) => {
      if (e.propertyName !== 'left') return;

      gUser.value.coins += 1;
      coin.remove();

      const hudElement = document.getElementById('coin-hud');
      if (hudElement) {
        hudElement.classList.remove('animate-pulse');
        void hudElement.offsetWidth;
        hudElement.classList.add('animate-pulse');
      }
    });
  };

  const collectRewards = (amount) => {
    const hudElement = document.getElementById('coin-hud');
    let targetX = 100; // Fallback
    let targetY = 100; // Fallback

    if (hudElement) {
      const rect = hudElement.getBoundingClientRect();
      targetX = rect.left + rect.width / 2;
      targetY = rect.top + rect.height / 2;
    }

    for (let i = 0; i < amount; i++) {
      setTimeout(() => {
        spawnFlyingCoin(i, targetX, targetY);
      }, 10 + i * 10);
    }
  };

  return { collectRewards };
}

