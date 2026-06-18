import { ref } from 'vue';
import { gEngine } from '../core/globals.js';

const floatingTexts = ref([]);
let textIdCounter = 0;

export function useFloatingText() {
  const spawnFloatingText = (object, spawnText, type = 'default') => {
    if (!gEngine.value) return;

    const camera = gEngine.value.camera;
    const pos = object.position.clone();

    pos.project(camera); // -1 to 1

    // conversion to 0 to 1
    const x = (pos.x * 0.5 + 0.5) * window.innerWidth;
    const y = (pos.y * -0.5 + 0.5) * window.innerHeight;

    const id = textIdCounter++;
    floatingTexts.value.push({ id, x, y, text: spawnText, type });

    setTimeout(() => {
      floatingTexts.value = floatingTexts.value.filter(t => t.id !== id);
    }, 2500);
  }
  return { floatingTexts, spawnFloatingText };
};