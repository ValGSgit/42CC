<template>
  <!-- Sidebar mode: static inline card, no overlay -->
  <div v-if="sidebar" class="fake-ad-container fake-ad-sidebar">
    <div class="ad-header">
      <span class="ad-label">Advertisement</span>
    </div>
    <div class="ad-body">
      <img :src="gifUrl" alt="Farm Merge Valley Ad" class="ad-gif" />
      <div class="ad-text">
        <p class="ad-title">{{ title }}</p>
        <p class="ad-subtitle">{{ subtitle }}</p>
      </div>
      <button class="cta-btn" @click="onCtaClick">{{ ctaText }}</button>
    </div>
    <div class="ad-footer">
      <span>Sponsored · alpacagram.hawktwah</span>
    </div>
  </div>

  <!-- Modal/overlay mode (original behavior) -->
  <div v-else-if="visible" class="fake-ad-overlay" @click.self="closeAd">
    <div class="fake-ad-container">
      <!-- Ad Header -->
      <div class="ad-header">
        <span class="ad-label">Advertisement</span>
        <button v-if="canClose" class="close-btn" @click="closeAd"><AppIcon name="close" :size="14" /></button>
        <div v-else class="close-countdown">Close in {{ countdown }}s</div>
      </div>

      <!-- Ad Content -->
      <div class="ad-body">
        <img
          :src="gifUrl"
          alt="Farm Merge Valley Ad"
          class="ad-gif"
        />

        <div class="ad-text">
          <p class="ad-title">{{ title }}</p>
          <p class="ad-subtitle">{{ subtitle }}</p>
        </div>

        <button class="cta-btn" @click="onCtaClick">
          {{ ctaText }}
        </button>
      </div>

      <!-- Ad Footer -->
      <div class="ad-footer">
        <span>Sponsored · alpacagram.hawktwah</span>
      </div>
    </div>
  </div>
</template>

<script lang="ts">

import { defineComponent, ref, onMounted, onUnmounted, watch } from 'vue'
import shrekGif from '../assets/shrek.gif'
import AppIcon from './AppIcon.vue'

export default defineComponent({
  name: 'FakeAd',
  components: { AppIcon },

  props: {
    gifUrl: {
      type: String,
      default: shrekGif,
    },
    title: {
      type: String,
      default: '🌾 Alpaca Farm',
    },
    subtitle: {
      type: String,
      default: 'Enjoy all the alpacas and none of the spit!    Play FREE now.',
    },
    ctaText: {
      type: String,
      default: 'PLAY FREE NOW',
    },
    closableAfter: {
      type: Number,
      default: 3, // seconds before close button appears
    },
    autoClose: {
      type: Number,
      default: 5, // 0 = no auto close; set seconds to enable
    },
    sidebar: {
      type: Boolean,
      default: false, // when true: renders as static inline card, no overlay/timers
    },
  },

  emits: ['close', 'cta-click'],

  setup(props, { emit }) {
    const visible = ref(true)
    const countdown = ref(props.closableAfter)
    const canClose = ref(props.closableAfter === 0)

    let countdownTimer: ReturnType<typeof setInterval> | null = null
    let autoCloseTimer: ReturnType<typeof setTimeout> | null = null

    const closeAd = () => {
      visible.value = false
      emit('close')
    }

    const onCtaClick = () => {
      emit('cta-click')
    }

    onMounted(() => {
      if (props.sidebar) return // static panel — no timers
      if (props.closableAfter > 0) {
        countdownTimer = setInterval(() => {
          countdown.value--
          if (countdown.value <= 0) {
            canClose.value = true
            clearInterval(countdownTimer!)
          }
        }, 1000)
      }

      if (props.autoClose > 0) {
        autoCloseTimer = setTimeout(() => closeAd(), props.autoClose * 1000)
      }
    })

    onUnmounted(() => {
      if (countdownTimer) clearInterval(countdownTimer)
      if (autoCloseTimer) clearTimeout(autoCloseTimer)
    })

    return { visible, countdown, canClose, closeAd, onCtaClick }
  },
})
</script>

<style src="../styles/components/FakeAd.css" scoped></style>
