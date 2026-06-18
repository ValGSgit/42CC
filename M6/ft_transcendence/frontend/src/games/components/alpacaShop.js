import { reactive } from 'vue';
import { CONST } from '../config/constants';
import { createAlpaca } from '../core/createObjects';
import { useUIManager } from "../core/useUIManager";
import { setupPlacement } from './editMode';
import { checkCoinsPrice, checkHerdSize } from './upgradeFarm';

export function alpacaShop() {
  const { closeMenus } = useUIManager()

  async function buyAlpaca() {
    if (!checkCoinsPrice(CONST.ALPACA_COST) || !checkHerdSize()) {
      return;
    }

    const pos = [0, 0, 0];
    const s = alpacaConfig.scale;
    const scale = [s, s, s];
    const alpaca = await createAlpaca(
      alpacaConfig.name,
      alpacaConfig.color,
      pos,
      0,
      scale);
    alpaca.model.userData.cost = CONST.ALPACA_COST;
    closeMenus();
    resetAlpacaConfig();
    setupPlacement(alpaca.model);
  }
  return { buyAlpaca }
}

const defaultAlpaca = {
  name: 'New Alpaca',
  color: '#634632',
  scale: 1.0
}

export const alpacaConfig = reactive({ ...defaultAlpaca });
export const resetAlpacaConfig = () => {
  Object.assign(alpacaConfig, defaultAlpaca);
}