<template>
  <!-- File-based icons (multi-color, can't use currentColor) -->
  <img
    v-if="fileSrc"
    :src="fileSrc"
    :width="size"
    :height="size"
    class="app-icon app-icon--img"
    aria-hidden="true"
    alt=""
  />
  <!-- Sprite-based icons (single-color, themed via currentColor) -->
  <svg
    v-else
    :width="size"
    :height="size"
    class="app-icon"
    aria-hidden="true"
    focusable="false"
  >
    <use :href="`#icon-${name}`" />
  </svg>
</template>

<script setup>
import { computed } from 'vue';

const props = defineProps({
  name: { type: String, required: true },
  size: { type: [Number, String], default: 16 },
})

// Multi-color SVGs that live in /icons/ui/ and can't use currentColor
const FILE_ICONS = {
  'close':          '/icons/ui/cross-close.svg',
  'message':        '/icons/ui/message.svg',
  'logout':         '/icons/ui/logout.svg',
  'camera-full':    '/icons/ui/camera-comment-section.svg',
  'trash-full':     '/icons/ui/trash.svg',
  'refresh-box':    '/icons/ui/refresh.svg',
  'user':           '/icons/ui/alpaca.svg',
  'alpaca':         '/icons/ui/alpaca.svg',
  'alpaca-buy':     '/icons/ui/buy-alpaca.svg',
  'bell-full':      '/icons/ui/bell.svg',
  'sun-full':       '/icons/ui/sun.svg',
  'joystick-full':  '/icons/ui/joystick.svg',
  'coin':           '/icons/ui/coin.svg',
  'pencil-ruler':   '/icons/ui/pencil-ruler-farm.svg',
  'shopping-bags':  '/icons/ui/shopping-bags.svg',
  'camera-farm':    '/icons/ui/camera-farm.svg',
  'barn':           '/icons/ui/barn.svg',
  'tractor':        '/icons/ui/tractor.svg',
  'debug-coin':     '/icons/ui/coin.svg',
  'score':          '/icons/ui/score.svg',
  'sunrise':        '/icons/ui/sunrise.svg',
  'sunset':         '/icons/ui/sunset.svg',
  'night':          '/icons/ui/night.svg',
  'tools':          '/icons/ui/tools.svg',
  'mouse':          '/icons/ui/mouse.svg',
  'scroll-icon':    '/icons/ui/scroll.svg',
  'exit':           '/icons/ui/exit.svg',
}

const fileSrc = computed(() => FILE_ICONS[props.name] ?? null)
</script>

<style src="../styles/components/AppIcon.css" scoped></style>
