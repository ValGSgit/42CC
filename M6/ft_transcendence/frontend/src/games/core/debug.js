import { gAlpacas, gCollectables, gCollidables, gDecorations, gEditables, gItems, gUser } from "./globals";
import { debug } from '../../services/logger.js';

export function printDebug() {
  if (gAlpacas)
    debug("gAlpacas.length = ", gAlpacas.length)
  if (gItems)
    debug("gItems.length = ", gItems.length)
  if (gCollectables)
    debug("gCollectables.length = ", gCollectables.length)
  if (gCollidables)
    debug("gCollidables.length = ", gCollidables.length)
  if (gEditables)
    debug("gEditables.length = ", gEditables.length)
  if (gUser) {
    debug("gUser = ", gUser.value)
  }
  resetValues()
}

//clear all
function resetValues() {
  gAlpacas.length = 1;
  gUser.value.coins = 0;
  gItems.length = 0;
  gDecorations.length = 0;
  gCollectables.length = 0;
  gCollidables.length = 0;
  gEditables.length = 0;

  gUser.value.upgrades = 0
  gUser.value.coins = 10
}

export function addDebugCoins() {
  gUser.value.coins += 1000
}

