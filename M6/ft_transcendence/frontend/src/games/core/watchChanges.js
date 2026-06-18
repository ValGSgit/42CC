import { watch } from 'vue';
import { gPlayer, gUser } from './globals.js';
import { saveGame } from './saveLoadGame.js';


export function watchChanges() {
  const stopGamePlayWatcher = watch(
    [
      () => gUser.value.coins,
      () => gUser.value.upgrades,
      gPlayer,
    ],
    () => {
      saveGame();
    })

  return () => { stopGamePlayWatcher() };
}

