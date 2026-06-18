import * as THREE from 'three';
import { CONST } from '../config/constants.js';
import { gScene, gUI } from '../core/globals.js';
import { useUIManager } from '../core/useUIManager.js';

const newAmbient = new THREE.Color('#ffffff');
let newAmbientIntensity = 0.8;

const newSun = new THREE.Color('#ffffff');
let newSunIntensity = 1.2;

const currBgTop = new THREE.Color('#4abdff');
const currentBgBottom = new THREE.Color('#142191');

const newBgTop = new THREE.Color('#4abdff');
const newBgBottom = new THREE.Color('#142191');

let waitTimer = 0;
const dayCycle = ['sunrise', 'day', 'sunset', 'night'];
let currentCycleIndex = 1;

let transitionFaster = false;

export function editLight() {
  const { closeMenus } = useUIManager();

  const setLighting = (ambientColor, ambientIntensity, sunColor, sunIntensity, colorTop, colorBottom) => {
    newAmbient.set(ambientColor);
    newAmbientIntensity = ambientIntensity;

    newSun.set(sunColor);
    newSunIntensity = sunIntensity;

    newBgTop.set(colorTop);
    newBgBottom.set(colorBottom);
  };

  const changeTimeOfDay = (mode) => {
    currentCycleIndex = dayCycle.indexOf(mode);
    waitTimer = 0;

    switch (mode) {
      case 'sunrise':
        setLighting('#fda450', 0.6, '#64c6ff', 1.2, '#4ac6ff', '#e46f0f');
        break;
      case 'day':
        setLighting('#ffffff', 1.0, '#ffffff', 1.5, '#4abdff', '#142191');
        break;
      case 'sunset':
        setLighting('#ffd67e', 0.8, '#ffb764', 1.2, '#ffa6cb', '#ffa600');
        break;
      case 'night':
        setLighting('#333355', 1.3, '#5556aa', 0.1, '#1f316b', '#030614');
        break;
      default:
        changeTimeOfDay('day');
        break;
    }
  };

  const toggleLightCycle = () => {
    gUI.isLightCycling = !gUI.isLightCycling;
    if (gUI.isLightCycling) {
      waitTimer = 0;
    }
  };

  // this is when it gets clicked to close the menu afterwards
  const setTimeOfDay = (mode) => {
    transitionFaster = true;
    if (mode) {
      changeTimeOfDay(mode);
    }
    closeMenus();
  }

  const updateBackground = (scene) => {
    const canvas = scene.background.image;
    const context = canvas.getContext('2d');

    const gradient = context.createLinearGradient(0, 0, 0, canvas.height);

    gradient.addColorStop(0, '#' + currBgTop.getHexString());
    gradient.addColorStop(1, '#' + currentBgBottom.getHexString());

    context.fillStyle = gradient;
    context.fillRect(0, 0, canvas.width, canvas.height);

    scene.background.needsUpdate = true;
  }

  const updateColors = (scene, delta) => {
    const ambient = scene.ambientLight;
    const sun = scene.sunLight;
    const lerpSpeed = transitionFaster ? delta * 3 : delta * 0.5;

    if (ambient) {
      ambient.color.lerp(newAmbient, lerpSpeed);
      ambient.intensity = THREE.MathUtils.lerp(ambient.intensity, newAmbientIntensity, lerpSpeed);
    }
    if (sun) {
      sun.color.lerp(newSun, lerpSpeed);
      sun.intensity = THREE.MathUtils.lerp(sun.intensity, newSunIntensity, lerpSpeed);
    }

    currBgTop.lerp(newBgTop, lerpSpeed);
    currentBgBottom.lerp(newBgBottom, lerpSpeed);
  }

  const cycleLighting = (delta) => {
    const topRDiff = Math.abs(currBgTop.r - newBgTop.r);
    const topGDiff = Math.abs(currBgTop.g - newBgTop.g);
    const topBDiff = Math.abs(currBgTop.b - newBgTop.b);
    const topDiff = topRDiff + topGDiff + topBDiff;

    const botRDiff = Math.abs(currentBgBottom.r - newBgBottom.r);
    const botGDiff = Math.abs(currentBgBottom.g - newBgBottom.g);
    const botBDiff = Math.abs(currentBgBottom.b - newBgBottom.b);
    const botDiff = botRDiff + botGDiff + botBDiff;

    const isNewColor = (topDiff < 0.05) && (botDiff < 0.05);

    if (isNewColor) {
      if (transitionFaster) {
        transitionFaster = false;
      }
      if (gUI.isLightCycling) {
        waitTimer += delta;

        if (waitTimer >= (CONST.SECONDS_PER_DAYPHASE / getSunState())) {
          currentCycleIndex = (currentCycleIndex + 1) % dayCycle.length;
          changeTimeOfDay(dayCycle[currentCycleIndex]);
        }
      }
    } else {
      waitTimer = 0;
    }
  }

  // shortens the wait time for sunrise and sunset
  const getSunState = () => {
    if (currentCycleIndex % 2 === 0) {
      return 3;
    }
    return 1;
  }

  const updateLighting = (delta) => {
    if (!gScene.value) return;
    const scene = gScene.value;

    updateColors(scene, delta);
    if (scene.background && scene.background.isTexture && scene.background.image) {
      updateBackground(scene);
    }
    cycleLighting(delta)
  };

  return { setTimeOfDay, updateLighting, toggleLightCycle };
}