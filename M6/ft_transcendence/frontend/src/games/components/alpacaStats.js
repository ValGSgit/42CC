import { ref, triggerRef } from 'vue';
import { gPlayer } from "../core/globals";

const updateVue = ref(0);

export function alpacaStats() {

  const changeColor = (color) => {
    if (gPlayer.value && gPlayer.value.setColor) {
      gPlayer.value.setColor(color);
      triggerRef(gPlayer);
    }
    updateVue.value++;
  }

  const changeName = (name) => {
    if (gPlayer.value && gPlayer.value.model) {
      gPlayer.value.model.name = name;
      triggerRef(gPlayer);
    }
    updateVue.value++;
  };


  const changeSpeed = (amount) => {
    if (gPlayer.value) {
      gPlayer.value.changeSpeed(amount * 0.1);
      triggerRef(gPlayer);
    }
    updateVue.value++;
  }

  return { changeColor, changeName, changeSpeed, updateVue };
}

