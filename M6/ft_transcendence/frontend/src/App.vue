<template>
  <div id="app">
    <!-- Top navbar — hidden on full-screen auth pages -->
    <nav v-if="!isAuthRoute" :class="['navbar', { scrolled }]">
      <div class="nav-bar-glow"></div>
      <div class="nav-container">
        <router-link to="/" class="nav-logo">
          <AppIcon name="alpaca" :size="24" class="logo-icon" />
          <span class="logo-text">AlpacaParty</span>
        </router-link>

        <button :class="['hamburger', { open: mobileOpen }]" @click="mobileOpen = !mobileOpen" aria-label="Toggle menu">
          <span class="bar"></span>
          <span class="bar"></span>
          <span class="bar"></span>
        </button>

        <div :class="['nav-links', { open: mobileOpen }]">
          <router-link to="/game" class="nav-link nav-game" @click="mobileOpen = false">AlpacaFarm</router-link>

          <template v-if="authStore.isAuthenticated">
            <span class="nav-divider"></span>
            <router-link to="/feed" class="nav-link" @click="mobileOpen = false">Feed</router-link>
            <router-link to="/friends" class="nav-link" @click="mobileOpen = false">Friends</router-link>
            <span class="nav-divider"></span>

            <router-link to="/profile" class="nav-link" title="Profile" @click="mobileOpen = false">
              <AppIcon name="user" :size="18" />
            </router-link>

            <div class="notif-wrapper">
              <button class="nav-link nav-btn notification-btn" @click="toggleNotifications" title="Notifications">
                <AppIcon name="bell-full" :size="20" />
                <span v-if="unreadCount" class="notif-badge">{{ unreadCount }}</span>
              </button>
              <div v-if="showNotifPanel" class="notif-panel">
                <div class="notif-panel-header">
                  <span>Notifications</span>
                  <div class="notif-header-actions">
                    <button
                      v-if="notifications.length"
                      class="notif-clear"
                      @click="clearAllNotifications"
                    >
                      Clear all
                    </button>
                    <button class="notif-close" @click="showNotifPanel = false">&times;</button>
                  </div>
                </div>
                <div v-if="!notifications.length" class="notif-empty">No notifications yet</div>
                <div
                  v-for="n in notifications"
                  :key="n.id"
                  :class="['notif-item', { unread: !n.is_read }]"
                  @click="markNotifRead(n)"
                >
                  <span class="notif-msg">{{ n.message }}</span>
                  <span class="notif-time">{{ formatNotifTime(n.created_at) }}</span>
                </div>
              </div>
            </div>

            <button class="nav-link nav-btn logout-btn" @click="handleLogout" title="Logout">
              <AppIcon name="logout" :size="18" />
              <span class="logout-label">Logout</span>
            </button>
          </template>
          <template v-else>
            <span class="nav-divider"></span>
            <router-link to="/login" class="nav-link nav-login" @click="mobileOpen = false">Login</router-link>
            <router-link to="/register" class="nav-link nav-register" @click="mobileOpen = false">Sign Up</router-link>
          </template>
        </div>
      </div>
    </nav>

    <!-- Page content -->
    <main :class="['main-content', { 'game-content': isGameRoute }]">
      <router-view />
    </main>

    <!-- Footer -->
    <footer v-if="hasFooter" class="app-footer">
      <div class="footer-container">
        <span class="footer-copy">&copy; 2026 AlpacaParty</span>
        <div class="footer-links">
          <router-link to="/help">Help</router-link>
          <a href="/api/docs/public">API Docs</a>
          <router-link to="/privacy">Privacy Policy</router-link>
          <router-link to="/terms">Terms of Service</router-link>
        </div>
      </div>
    </footer>

    <!-- Floating message button (positioned fixed via global CSS in style.css) -->
    <div v-if="authStore.isAuthenticated && !isAuthRoute" :class="['bottom-left-nav', { 'with-footer': hasFooter }]">
      <button class="message-btn" @click="showMessagesModal = true" title="Messages">
        <AppIcon name="message" :size="44" />
        <span v-if="unreadMessages" class="msg-badge">{{ unreadMessages }}</span>
      </button>
    </div>

    <!-- Messages modal -->
    <div v-if="showMessagesModal" class="modal-overlay" @click.self="showMessagesModal = false">
      <div class="messages-modal-content">
        <button class="modal-close-top" @click="showMessagesModal = false">&times;</button>
        <Messages @close="showMessagesModal = false" />
      </div>
    </div>

    <!-- AI help desk widget -->
    <HelpDeskChat v-if="authStore.isAuthenticated && !isAuthRoute" :with-footer="hasFooter" />

  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted, watch, computed } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import { useAuthStore } from './stores/auth.js'
import api from './services/api.js'
import { devError } from './services/logger.js'
import { connectSocket, disconnectSocket } from './services/socket.js'
import AppIcon from './components/AppIcon.vue'
import HelpDeskChat from './components/HelpDeskChat.vue'
import Messages from './views/Messages.vue'

const authStore = useAuthStore()
const router = useRouter()
const route = useRoute()

const mobileOpen = ref(false)
const scrolled = ref(false)
const showNotifPanel = ref(false)
const showMessagesModal = ref(false)
const notifications = ref([])
const unreadCount = ref(0)
const unreadMessages = ref(0)

watch(showMessagesModal, (open) => {
  if (open) unreadMessages.value = 0
})

const PAGES_WITHOUT_FOOTER = ['Game', 'Home']
const hasFooter = computed(() => !PAGES_WITHOUT_FOOTER.includes(route.name))
const isGameRoute = computed(() => ['Game', 'SpitRoyale'].includes(route.name))
// Used to hide the global nav / footer on fullscreen-only pages. Empty for
// now — kept as a hook so future routes (game-over modal, splash, etc.) can
// opt out of the chrome without touching every consumer.
const isAuthRoute = computed(() => false)

router.afterEach(() => { mobileOpen.value = false })

async function handleLogout() {
  disconnectSocket()
  await authStore.logout()
  router.push('/login')
}

function toggleNotifications() {
  showNotifPanel.value = !showNotifPanel.value
  if (showNotifPanel.value) fetchNotifications()
}

async function fetchNotifications() {
  try {
    const { data } = await api.get('/notifications')
    // Message notifications are surfaced on the floating message bubble (via the
    // unread-messages badge), so keep them out of the notification bell to avoid
    // double-counting the same event.
    notifications.value = (data.notifications || []).filter(n => n.type !== 'message')
    unreadCount.value = notifications.value.filter(n => !n.is_read).length
  } catch (e) { devError(e) }
}

async function clearAllNotifications() {
  try {
    await api.put('/notifications/read-all')
  } catch (e) { devError(e) }
  notifications.value = []
  unreadCount.value = 0
}

async function fetchUnreadMessages() {
  try {
    const { data } = await api.get('/chat/unread')
    unreadMessages.value = data.count || 0
  } catch { /* ignore */ }
}

async function markNotifRead(n) {
  if (!n.is_read) {
    try {
      await api.put(`/notifications/${n.id}/read`)
      n.is_read = true
      unreadCount.value = Math.max(0, unreadCount.value - 1)
    } catch (e) { devError(e) }
  }
  showNotifPanel.value = false
  const targetByType = {
    friend_request: '/friends',
    game_invite:    '/game',
    game_finish:    '/game',
    post_like:      '/feed',
    achievement:    '/profile',
  }
  router.push(targetByType[n.type] || '/')
}

function formatNotifTime(ts) {
  if (!ts) return ''
  const diff = Date.now() - new Date(ts)
  if (diff < 60_000)    return 'just now'
  if (diff < 3_600_000) return `${Math.floor(diff / 60_000)}m ago`
  if (diff < 86_400_000) return `${Math.floor(diff / 3_600_000)}h ago`
  return new Date(ts).toLocaleDateString()
}

function handleOutsideClick(e) {
  const wrapper = document.querySelector('.notif-wrapper')
  if (wrapper && !wrapper.contains(e.target)) showNotifPanel.value = false
}

function handleScroll() { scrolled.value = window.scrollY > 12 }

// Socket listener lifecycle.
//
// The auth-state watcher previously called `sock.on(...)` five times every
// time `isAuthenticated` flipped true. If the watcher re-fired without
// going through `false` first (rapid auth refresh, dev-server HMR keeping
// the singleton alive, etc.) the same handlers got registered N times and
// every notification / presence event fired N times.
//
// Capture the handlers once, track the socket they're attached to, and
// always unbind before binding.
const socketHandlers = {
  // Skip 'message' notifications here — those are reflected on the message
  // bubble's unread badge through the 'dm:message' handler below.
  notification: (n) => { if (n?.type !== 'message') unreadCount.value++ },
  connect:      () => { if (authStore.user) authStore.user.isOnline = true },
  disconnect:   () => { if (authStore.user) authStore.user.isOnline = false },
  presence:     ({ userId, isOnline }) => {
    if (authStore.user && Number(userId) === Number(authStore.user.id)) {
      authStore.user.isOnline = isOnline
    }
  },
  'dm:message': () => {
    if (!showMessagesModal.value) unreadMessages.value++
  },
}
let boundSocket = null

function bindSocketHandlers(sock) {
  if (!sock) return
  // If we're being asked to bind to the same socket twice (which would
  // double-register every handler), unbind first.
  if (boundSocket === sock) unbindSocketHandlers()
  for (const [event, handler] of Object.entries(socketHandlers)) {
    sock.on(event, handler)
  }
  boundSocket = sock
}

function unbindSocketHandlers() {
  if (!boundSocket) return
  for (const [event, handler] of Object.entries(socketHandlers)) {
    boundSocket.off(event, handler)
  }
  boundSocket = null
}

watch(() => authStore.isAuthenticated, (isAuth) => {
  if (isAuth) {
    // Auth is cookie-based (httpOnly + sameSite=strict + secure). Socket.IO
    // is configured with `withCredentials: true` so the JWT cookie is sent on
    // the handshake and validated by socketAuthMiddleware. No client-side token.
    const sock = connectSocket()
    bindSocketHandlers(sock)
    // If already connected when the watcher runs, mark online immediately
    if (sock.connected && authStore.user) authStore.user.isOnline = true

    fetchNotifications()
    fetchUnreadMessages()
  } else {
    unbindSocketHandlers()
    disconnectSocket()
    unreadCount.value = 0
    unreadMessages.value = 0
    notifications.value = []
  }
}, { immediate: true })

onMounted(() => {
  document.addEventListener('click', handleOutsideClick)
  window.addEventListener('scroll', handleScroll, { passive: true })
})
onUnmounted(() => {
  document.removeEventListener('click', handleOutsideClick)
  window.removeEventListener('scroll', handleScroll)
  unbindSocketHandlers()
})
</script>

<style src="./styles/App.css" scoped></style>
