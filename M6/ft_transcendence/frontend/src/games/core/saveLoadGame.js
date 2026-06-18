import api from '../../services/api.js'
import { debug, devError } from '../../services/logger.js'
import { useAuthStore } from '../../stores/auth.js'
import { gAlpacas, gDecorations, gItems, gMinigame, gPlayer, gUser } from './globals.js'

let _saveTimer = null
// Save-suppression guard. Used during farm-reload (returnFarm → initWorld)
// so the watcher-triggered saveGame() from `gPlayer.value = null` / coin
// refresh doesn't fire with empty gAlpacas/gItems and wipe the DB before
// the load finishes populating the world.
let _suppressed = false
export function pauseSaves() {
  _suppressed = true
  clearTimeout(_saveTimer)
  _saveTimer = null
}
export function resumeSaves() {
  _suppressed = false
}
export function saveGame() {
  if (_suppressed) return
  clearTimeout(_saveTimer)
  _saveTimer = setTimeout(_doSave, 300)
}

export async function flushSave() {
  if (_suppressed) return
  clearTimeout(_saveTimer)
  _saveTimer = null
  await _doSave()
}

async function _doSave() {
  const authStore = useAuthStore()
  if (!authStore.isAuthenticated || !authStore.user) {
    debug("user not logged in, not saving")
    return
  }

  if (!gUser.value || !gPlayer.value) {
    return
  }

  if (gMinigame.value.mode) {
    saveMinigame();
    return
  }


  const saveAlpacas = gAlpacas.filter(a => !a.isAI).map(alpaca => {
    let selected = false
    if (gPlayer.value === alpaca)
      selected = true // save current selected alpaca
    return {
      name: alpaca.name,
      color: alpaca.color,
      position: alpaca.model.position.toArray(),
      rotation: alpaca.model.rotation.y,
      scale: alpaca.model.scale.toArray(),
      speedOffset: alpaca.speedOffset,
      rotationOffset: alpaca.rotationOffset,
      age: alpaca.age,
      aliveTime: alpaca.aliveTime,
      cost: alpaca.model.userData.cost,
      selected
    };
  });

  const getItemsData = () => {
    const items = gItems.map(item => {
      return {
        path: item.path,
        position: item.model.position.toArray(),
        rotation: item.model.rotation.y,
        scale: item.model.scale.toArray(),
        name: item.model.name,
        type: item.type,
        cost: item.model.userData.cost
      };
    });

    const decorations = gDecorations.map(item => {
      return {
        path: item.path,
        position: item.model.position.toArray(),
        rotation: item.model.rotation.y,
        scale: item.model.scale.toArray(),
        name: item.model.name,
        type: item.type,
        cost: item.model.userData.cost
      };
    });
    return [...items, ...decorations];
  };

  try {
    const itemsData = getItemsData();
    debug("gUser:", gUser);
    const res = await api.put('/game/farm', {
      items: itemsData,
      alpacas: saveAlpacas,
      coins: gUser.value.coins,
      upgrades: gUser.value.upgrades,
      herdsize: gUser.value.herdsize
    })
    debug(res);
    debug('✅ Farm stats synced to server')
  } catch (error) {
    devError('Failed to sync farm stats:', error)
  }
}

async function saveMinigame() {
  try {
    await api.put('/game/farm', {
      coins: gUser.value.coins,
    })
    debug('✅ Farm stats synced to server after minigame')
  } catch (error) {
    devError('Failed to sync farm stats:', error)
  }
}
