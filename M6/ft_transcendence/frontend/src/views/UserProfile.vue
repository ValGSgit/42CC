<template>
  <div class="profile-wrap">
    <div v-if="loading" class="loading-msg">Loading profile…</div>

    <!-- Error (404 / network) -->
    <div v-else-if="error" class="error-msg">{{ error }}</div>

    <!-- Locked profile — private user, not friends -->
    <section v-else-if="isPrivate && profile" class="overview">
      <!-- Hero banner with lock overlay -->
      <header class="hero">
        <div class="hero-bg" aria-hidden="true">
          <span class="hero-blob a"></span>
          <span class="hero-blob b"></span>
          <span class="hero-grid"></span>
        </div>
        <div class="hero-content">
          <div class="avatar-wrap" :class="{ on: profile.isOnline }">
            <div class="hero-avatar">
              <img v-if="profile.avatar" :src="profile.avatar" :alt="profile.username" />
              <span v-else class="avatar-initials">{{ profile.username?.slice(0,2).toUpperCase() }}</span>
            </div>
            <span class="presence-dot" :class="{ on: profile.isOnline }"></span>
          </div>
          <div class="hero-meta">
            <div class="name-row">
              <h1 class="display-name">{{ profile.username }}</h1>
              <span class="online-badge" :class="profile.isOnline ? 'badge-on' : 'badge-off'">
                <span class="badge-dot"></span>
                {{ profile.isOnline ? 'Online' : 'Offline' }}
              </span>
            </div>
            <p class="handle">@{{ profile.username }}</p>
          </div>
        </div>
      </header>

      <!-- Locked banner with friend actions -->
      <div class="locked-banner">
        <span class="lock-icon">🔒</span>
        <p>This profile is private.<br />Add <strong>{{ profile.username }}</strong> as a friend to see their stats and posts.</p>
        <div v-if="authStore.isAuthenticated && profile.id !== authStore.user?.id" class="friend-actions">
          <template v-if="friendStatus?.status === 'pending_sent'">
            <button class="btn btn-pill btn-secondary" disabled>Request Sent</button>
          </template>
          <template v-else-if="friendStatus?.status === 'pending_received'">
            <button class="btn btn-pill btn-primary" @click="acceptFriendRequest">Accept Request</button>
            <button class="btn btn-pill btn-secondary" @click="declineFriendRequest">Decline</button>
          </template>
          <template v-else>
            <button class="btn btn-pill btn-primary" @click="sendFriendRequest">Add Friend</button>
          </template>
        </div>
      </div>

      <div class="locked-preview" aria-hidden="true">
        <div class="locked-row"></div>
        <div class="locked-row short"></div>
        <div class="locked-row"></div>
        <div class="locked-row short"></div>
      </div>
    </section>

    <!-- Full profile -->
    <section v-else-if="profile" class="overview">
      <!-- Hero banner -->
      <header class="hero">
        <div class="hero-bg" aria-hidden="true">
          <span class="hero-blob a"></span>
          <span class="hero-blob b"></span>
          <span class="hero-grid"></span>
        </div>
        <div class="hero-content">
          <div class="avatar-wrap" :class="{ on: profile.is_online }">
            <div class="hero-avatar">
              <img v-if="profile.avatar" :src="profile.avatar" :alt="profile.username" />
              <span v-else class="avatar-initials">{{ profile.username?.slice(0,2).toUpperCase() }}</span>
            </div>
            <span class="presence-dot" :class="{ on: profile.is_online }"></span>
          </div>
          <div class="hero-meta">
            <div class="name-row">
              <h1 class="display-name">{{ profile.username }}</h1>
              <span class="online-badge" :class="profile.is_online ? 'badge-on' : 'badge-off'">
                <span class="badge-dot"></span>
                {{ profile.is_online ? 'Online' : 'Offline' }}
              </span>
            </div>
            <p class="handle">@{{ profile.username }}</p>
            <p class="bio" v-if="profile.bio">{{ profile.bio }}</p>
            <p class="status-line" v-if="profile.status">
              <span class="status-glyph">⌬</span> {{ profile.status }}
            </p>
          </div>
        </div>
      </header>

      <!-- Stats row -->
      <section v-if="stats" class="stats-row">
        <article class="stat-card">
          <span class="stat-icon stat-icon-wins" aria-hidden="true">
            <svg viewBox="0 0 24 24"><path d="M7 4h10v3a5 5 0 0 1-10 0V4Z"/><path d="M5 5H3v2a3 3 0 0 0 3 3"/><path d="M19 5h2v2a3 3 0 0 1-3 3"/><path d="M9 19h6M12 13v6"/></svg>
          </span>
          <span class="stat-val">{{ (stats.wins || 0).toLocaleString() }}</span>
          <span class="stat-label">Wins</span>
        </article>
        <article class="stat-card">
          <span class="stat-icon stat-icon-losses" aria-hidden="true">
            <svg viewBox="0 0 24 24"><path d="M5 11a7 7 0 1 1 14 0v3a2 2 0 0 1-1 1.7V18a2 2 0 0 1-2 2h-1v-2h-2v2h-2v-2H9v2H8a2 2 0 0 1-2-2v-2.3A2 2 0 0 1 5 14v-3Z"/><circle cx="9" cy="12" r="1.2" fill="currentColor"/><circle cx="15" cy="12" r="1.2" fill="currentColor"/></svg>
          </span>
          <span class="stat-val">{{ (stats.losses || 0).toLocaleString() }}</span>
          <span class="stat-label">Losses</span>
        </article>
        <article class="stat-card">
          <span class="stat-icon stat-icon-coins" aria-hidden="true">
            <svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="9"/><path d="M12 6v2m0 8v2M8 12h8"/></svg>
          </span>
          <span class="stat-val">{{ winRate }}</span>
          <span class="stat-label">Win Rate</span>
        </article>
      </section>

      <!-- Friend Actions (if applicable) -->
      <div v-if="authStore.isAuthenticated && profile.id !== authStore.user?.id && actionError" class="action-error">{{ actionError }}</div>
      <div v-if="authStore.isAuthenticated && profile.id !== authStore.user?.id" class="friend-actions-row">
        <template v-if="friendStatus?.status === 'pending_sent'">
          <button class="btn btn-pill btn-secondary" disabled>Request Sent</button>
        </template>
        <template v-else-if="friendStatus?.status === 'pending_received'">
          <button class="btn btn-pill btn-primary" @click="acceptFriendRequest">Accept Request</button>
          <button class="btn btn-pill btn-secondary" @click="declineFriendRequest">Decline</button>
        </template>
        <template v-else-if="friendStatus?.status === 'friends'">
          <button class="btn btn-pill btn-secondary" @click="removeFriend">Remove Friend</button>
        </template>
        <template v-else>
          <button class="btn btn-pill btn-primary" @click="sendFriendRequest">Add Friend</button>
        </template>
      </div>

      <!-- Posts section -->
      <section v-if="userPosts.length" class="posts-section">
        <header class="section-head">
          <h2>Posts</h2>
        </header>
        <div v-for="post in userPosts" :key="post.id" class="mini-post">
          <p class="mini-post-content">{{ post.content }}</p>
          <img v-if="post.image_url" :src="post.image_url" class="mini-post-image" alt="" />
          <div class="mini-post-footer">
            <span>{{ post.likes_count || 0 }} likes · {{ post.comments_count || 0 }} comments</span>
            <span>{{ formatDate(post.created_at) }}</span>
          </div>
        </div>
      </section>
      <div v-else-if="userPosts.length === 0" class="empty-msg">No posts yet.</div>
    </section>
  </div>
</template>

<script setup>
import { computed, onMounted, ref } from 'vue'
import { useRoute } from 'vue-router'
import api from '../services/api.js'
import { devError } from '../services/logger.js'
import { useAuthStore } from '../stores/auth.js'

const route = useRoute()
const authStore = useAuthStore()

const profile = ref(null)
const stats = ref(null)
const userPosts = ref([])
const loading = ref(true)
const error = ref(null)
const isPrivate = ref(false)
const friendStatus = ref(null) // { status: 'none'|'pending_sent'|'pending_received'|'friends', requestId? }
const actionError = ref(null)

const winRate = computed(() => {
  if (!stats.value) return '0%'
  const total = (stats.value.wins || 0) + (stats.value.losses || 0)
  if (total === 0) return '0%'
  const rate = Math.round(((stats.value.wins || 0) / total) * 100)
  return `${rate}%`
})

function formatDate(ts) {
  if (!ts) return ''
  const d = new Date(ts)
  return `${String(d.getDate()).padStart(2, '0')}-${String(d.getMonth() + 1).padStart(2, '0')}-${d.getFullYear()}`
}

onMounted(async () => {
  const userId = route.params.id
  try {
    const { data } = await api.get(`/users/${userId}`)
    profile.value = data.user
    friendStatus.value = data.friend_status
  } catch (e) {
    if (e.response?.status === 403) {
      isPrivate.value = true
      profile.value = e.response?.data?.user ?? null
      friendStatus.value = e.response?.data?.friend_status ?? null
    } else {
      error.value = e.response?.data?.error?.message || 'User not found'
    }
  } finally {
    loading.value = false
  }

  if (isPrivate.value) return

  // Fetch game stats and posts in parallel only for visible profiles
  try {
    const { data } = await api.get(`/game/stats?userId=${userId}`)
    stats.value = data.stats
  } catch (e) { devError(e) }

  try {
    const { data } = await api.get(`/posts/user/${userId}?limit=10`)
    userPosts.value = data.posts || []
  } catch (e) { devError(e) }
})

async function sendFriendRequest() {
  actionError.value = null
  try {
    const { data } = await api.post('/friends/requests', { userId: profile.value.id })
    friendStatus.value = data.autoAccepted
      ? { status: 'friends' }
      : { status: 'pending_sent' }
  } catch (e) {
    actionError.value = e.response?.data?.error?.message || 'Failed to send request'
  }
}

async function acceptFriendRequest() {
  actionError.value = null
  try {
    await api.put(`/friends/requests/${friendStatus.value.requestId}/accept`)
    friendStatus.value = { status: 'friends' }
  } catch (e) {
    actionError.value = e.response?.data?.error?.message || 'Failed to accept request'
  }
}

async function declineFriendRequest() {
  actionError.value = null
  try {
    await api.put(`/friends/requests/${friendStatus.value.requestId}/decline`)
    friendStatus.value = { status: 'none' }
  } catch (e) {
    actionError.value = e.response?.data?.error?.message || 'Failed to decline request'
  }
}

async function removeFriend() {
  actionError.value = null
  try {
    await api.delete(`/friends/${profile.value.id}`)
    friendStatus.value = { status: 'none' }
  } catch (e) {
    actionError.value = e.response?.data?.error?.message || 'Failed to remove friend'
  }
}
</script>

<style src="../styles/views/UserProfile.css" scoped></style>
