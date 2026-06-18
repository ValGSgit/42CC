<template>
  <!-- Hidden SVG symbol defs -->
  <svg width="0" height="0" style="position:absolute;pointer-events:none" aria-hidden="true">
    <defs>
      <symbol id="i-trophy" viewBox="0 0 16 16" fill="none">
        <path d="M8 11c-2.761 0-5-2.239-5-5V2h10v4c0 2.761-2.239 5-5 5z" stroke="currentColor" stroke-width="1.5"/>
        <path d="M3 4H1.5A1.5 1.5 0 0 0 0 5.5C0 7.433 1.343 9 3 9" stroke="currentColor" stroke-width="1.5" stroke-linecap="round"/>
        <path d="M13 4h1.5A1.5 1.5 0 0 1 16 5.5C16 7.433 14.657 9 13 9" stroke="currentColor" stroke-width="1.5" stroke-linecap="round"/>
        <path d="M8 11v3M5 14h6" stroke="currentColor" stroke-width="1.5" stroke-linecap="round"/>
      </symbol>
      <symbol id="i-skull" viewBox="0 0 16 16" fill="none">
        <path d="M8 1a5 5 0 0 0-5 5c0 1.887 1.036 3.53 2.564 4.416L6 13h4l.436-2.584A5.001 5.001 0 0 0 8 1z" stroke="currentColor" stroke-width="1.5" stroke-linejoin="round"/>
        <path d="M6 13v2h4v-2" stroke="currentColor" stroke-width="1.5" stroke-linejoin="round"/>
        <circle cx="6" cy="7" r="1" fill="currentColor"/><circle cx="10" cy="7" r="1" fill="currentColor"/>
      </symbol>
      <symbol id="i-flag" viewBox="0 0 16 16" fill="none">
        <path d="M3 1v14M3 2l10 3-10 3" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
      </symbol>
      <symbol id="i-coin" viewBox="0 0 16 16" fill="none">
        <circle cx="8" cy="8" r="6.25" stroke="currentColor" stroke-width="1.5"/>
        <path d="M8 5v6M6.5 6.5h2a1 1 0 0 1 0 2h-1a1 1 0 0 0 0 2H9" stroke="currentColor" stroke-width="1.5" stroke-linecap="round"/>
      </symbol>
      <symbol id="i-refresh" viewBox="0 0 16 16" fill="none">
        <path d="M13.5 8A5.5 5.5 0 1 1 8 2.5c1.52 0 2.9.616 3.9 1.613L13.5 5.5" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
        <path d="M14 2v4h-4" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
      </symbol>
    </defs>
  </svg>

  <div class="lb-panel">
    <!-- Header -->
    <div class="lb-header">
      <span class="lb-title">Leaderboard</span>
      <span class="lb-live" :class="{ 'lb-live--on': socketConnected }">
        <span class="lb-dot"></span>LIVE
      </span>
    </div>

    <!-- Segmented tabs -->
    <div class="lb-tabs" :data-tab="board">
      <button
        v-for="tab in tabs"
        :key="tab.value"
        class="lb-tab"
        :data-id="tab.value"
        :class="{ 'lb-tab--active': board === tab.value }"
        @click="switchTab(tab.value)"
      >
        <svg class="tab-icon" width="14" height="14" aria-hidden="true">
          <use :href="`#i-${tab.icon}`" />
        </svg>
        <span class="tab-label">{{ tab.label }}</span>
      </button>
    </div>

    <!-- Loading / empty -->
    <div v-if="loading" class="lb-loading">Loading…</div>
    <div v-else-if="!entries.length" class="lb-empty">No data yet</div>

    <!-- Entries -->
    <div v-else class="lb-list">
      <div
        v-for="(entry, i) in entries"
        :key="entry.userId"
        class="lb-entry"
        :class="{
          'lb-entry--gold':   i === 0,
          'lb-entry--silver': i === 1,
          'lb-entry--bronze': i === 2,
          'lb-entry--you':    isYou(entry),
        }"
      >
        <!-- Rank -->
        <span class="lb-rank">
          <svg v-if="i === 0" class="trophy-icon" width="14" height="14" aria-hidden="true">
            <use href="#i-trophy" />
          </svg>
          <span v-else-if="i < 3" class="medal-pill" :class="i === 1 ? 'medal-pill--silver' : 'medal-pill--bronze'">
            {{ i + 1 }}
          </span>
          <span v-else class="lb-rank-num">#{{ i + 1 }}</span>
        </span>

        <!-- Avatar -->
        <div class="lb-avatar-wrap">
          <img
            v-if="entry.avatar"
            :src="entry.avatar"
            class="lb-avatar"
            alt=""
            @error="e => (e.target.style.display = 'none')"
          />
          <div
            v-else
            class="lb-avatar lb-avatar--initials"
            :style="{ background: avatarColor(entry.userId) }"
          >
            {{ (entry.username?.[0] ?? '?').toUpperCase() }}
          </div>
        </div>

        <!-- Info -->
        <div class="lb-info">
          <span class="lb-username">
            {{ entry.username }}
            <span v-if="isYou(entry)" class="you-tag">you</span>
          </span>
          <span class="lb-record">Lv {{ entry.level ?? 1 }}</span>
        </div>

        <!-- Score -->
        <span
          class="lb-score"
          :class="{
            'lb-score--kills':  board === 'kills',
            'lb-score--stages': board === 'obstacles',
            'lb-score--coins':  board === 'coins',
            'lb-score--fresh':  freshIds.has(entry.userId),
          }"
        >
          <template v-if="board === 'coins'">
            <svg class="coin-glyph" width="11" height="11" aria-hidden="true"><use href="#i-coin" /></svg>
            {{ entry.value ?? 0 }}
          </template>
          <template v-else-if="board === 'kills'">{{ entry.value ?? 0 }} ⚔</template>
          <template v-else>{{ entry.value ?? 0 }} ◄</template>
        </span>
      </div>
    </div>

    <!-- Footer -->
    <div class="lb-footer">
      <span>Updated {{ lastUpdated }}</span>
      <button
        class="lb-refresh"
        :class="{ 'lb-refresh--spinning': loading }"
        :disabled="loading"
        @click="fetchLeaderboard"
        aria-label="Refresh leaderboard"
      >
        <svg width="13" height="13" aria-hidden="true"><use href="#i-refresh" /></svg>
      </button>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, onUnmounted } from 'vue'
import api from '../services/api.js'
import { connectSocket, getSocket } from '../services/socket.js'
import { useAuthStore } from '../stores/auth.js'

const authStore = useAuthStore()

// Three independent leaderboards. Each entry is uniformly shaped
// { userId, username, avatar, level, value } — the `value` is whatever
// the active board ranks by (kills, obstacles, coins).
const tabs = [
  { label: 'Kills',     value: 'kills',     icon: 'skull' },
  { label: 'Obstacles', value: 'obstacles', icon: 'flag'  },
  { label: 'Coins',     value: 'coins',     icon: 'coin'  },
]

const AVATAR_PALETTE = [
  '#6c63ff','#ff6b6b','#ffa94d','#a9e34b','#4dabf7',
  '#f783ac','#63e6be','#da77f2','#74c0fc','#fcc419',
]

function avatarColor(userId) {
  return AVATAR_PALETTE[Number(userId) % AVATAR_PALETTE.length]
}

const board         = ref('kills')
const entries       = ref([])
const loading       = ref(false)
const updatedAt     = ref(null)
const socketConnected = ref(false)
const freshIds      = ref(new Set())

let socket      = null
let freshTimer  = null

function isYou(entry) {
  return authStore.isAuthenticated && Number(entry.userId) === Number(authStore.user?.id)
}

const lastUpdated = computed(() => {
  if (!updatedAt.value) return '—'
  const diff = Date.now() - updatedAt.value
  if (diff < 60_000)   return 'just now'
  if (diff < 3_600_000) return `${Math.floor(diff / 60_000)}m ago`
  return `${Math.floor(diff / 3_600_000)}h ago`
})

async function fetchLeaderboard() {
  loading.value = true
  try {
    const { data } = await api.get(`/game/leaderboard?board=${board.value}&limit=10`)
    const next = data.leaderboard || []

    // Mark rows whose score (value) changed since last fetch for the flash.
    const prevMap = new Map(entries.value.map(e => [e.userId, e]))
    const changed = new Set()
    next.forEach(e => {
      const prev = prevMap.get(e.userId)
      if (prev && prev.value !== e.value)
        changed.add(e.userId)
    })
    freshIds.value = changed
    if (freshTimer)
      clearTimeout(freshTimer)
    if (changed.size)
      freshTimer = setTimeout(() => { freshIds.value = new Set() }, 1400)
    entries.value = next
    updatedAt.value = Date.now()
  } catch {
    entries.value = []
  } finally {
    loading.value = false
  }
}

function switchTab(type) {
  board.value = type
  fetchLeaderboard()
}

function handleSocketConnect()    { socketConnected.value = true  }
function handleSocketDisconnect() { socketConnected.value = false }
function handleGameFinish()       { fetchLeaderboard() }

function setupSocket() {
  socket = getSocket() || connectSocket()
  if (!socket) return
  socketConnected.value = socket.connected
  socket.off('connect',    handleSocketConnect)
  socket.off('disconnect', handleSocketDisconnect)
  socket.off('game:finish', handleGameFinish)
  socket.on('connect',    handleSocketConnect)
  socket.on('disconnect', handleSocketDisconnect)
  socket.on('game:finish', handleGameFinish)
}

onMounted(() => {
  fetchLeaderboard()
  setupSocket()
})

onUnmounted(() => {
  if (freshTimer) clearTimeout(freshTimer)
  if (socket) {
    socket.off('connect',    handleSocketConnect)
    socket.off('disconnect', handleSocketDisconnect)
    socket.off('game:finish', handleGameFinish)
  }
})
</script>

<style src="../styles/components/ActiveLeaderboard.css" scoped></style>
