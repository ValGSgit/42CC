import { CONST } from "../config/constants";

export function getHearts(hp) {
  const validHp = Math.max(0, Math.min(hp, CONST.HP));
  return '❤️'.repeat(validHp) + '💔'.repeat(CONST.HP - validHp);
}