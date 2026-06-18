import { gAlpacas, gMinigame, gUser } from '../core/globals.js'

export const CONST = {
  BASE_RADIUS: 25,
  PLAYER_FORWARD_SPEED: 14,
  PLAYER_ROTATION: 4.0,

  JUMPING_SPEED: 9,
  JUMPING_MAX_HEIGHT: 3,

  CALIBRATION: 0.25, // Change this for animation speed

  CAMERA_OFFSET: { x: 0, y: 3, z: 0 },
  CAMERA_LERP: 0.025,
  MIN_ZOOM: 10.0,
  MAX_ZOOM: 200.0,

  DEBUG: 0,
  COLLIDER_SIZE: 0.8,
  AR_ENABLED: false,
  SBS_ENABLED: false,

  MAX_UPGRADES: 5,

  HP: 3,
  SECONDS_PER_INGAME_YEAR: 300,
  SECONDS_PER_DAYPHASE: 60,

  get ALPACA_COST() {
    return gAlpacas.length
  },

  get FLOOR_RADIUS() {
    let offset = 0
    if (gMinigame.value.mode !== 2) // all players will have same size
      offset = gUser.value.upgrades * 5.0 //change upgrade size
    return this.BASE_RADIUS + offset
  },

  get MAX_MOVE_RADIUS() {
    return this.FLOOR_RADIUS - 2
  },

  get MAX_COINS() {
    return 3 + gUser.value.upgrades;
  },
}