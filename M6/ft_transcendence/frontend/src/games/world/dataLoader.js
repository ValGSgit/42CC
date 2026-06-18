import api from '../../services/api.js';
import { debug, devError } from '../../services/logger.js';
import { gUser } from '../core/globals.js';

export async function loadGameData(visitPlayerId) {
  try {
    let res
    if (!visitPlayerId)
      res = await api.get('/game/farm')
    else
      res = await api.get('/game/farm', { params: { userId: visitPlayerId } })
    const farmData = res.data.farm;
    if (!visitPlayerId)
      gUser.value.coins = farmData.coins;
    gUser.value.upgrades = farmData.upgrades;
    gUser.value.herdsize = farmData.herdsize;

    debug("FarmData:", farmData);
    return farmData
  } catch (error) {
    devError('Failed to load user stats:', error)
    return null
  }
}