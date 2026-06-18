<template>
  <div class="profile-wrap">

    <!-- ── Tab bar — pill switcher ── -->
    <nav class="tab-pills" role="tablist">
      <button
        class="tab-pill"
        :class="{ active: activeTab === 'overview' }"
        role="tab" :aria-selected="activeTab === 'overview'"
        @click="setActiveTab('overview')"
      >Overview</button>
      <button
        class="tab-pill"
        :class="{ active: activeTab === 'settings' }"
        role="tab" :aria-selected="activeTab === 'settings'"
        @click="setActiveTab('settings')"
      >Settings</button>
    </nav>

    <!-- ══════════════════════ OVERVIEW TAB ══════════════════════ -->
    <section v-if="activeTab === 'overview'" class="overview">

      <!-- 1. Hero banner -->
      <header class="hero">
        <div class="hero-bg" aria-hidden="true">
          <span class="hero-blob a"></span>
          <span class="hero-blob b"></span>
          <span class="hero-grid"></span>
        </div>
        <div class="hero-content">
          <div class="avatar-wrap" :class="{ on: authStore.user?.isOnline }">
            <div class="hero-avatar">
              <img v-if="authStore.user?.avatar" :src="authStore.user.avatar" :alt="authStore.user?.username" />
              <span v-else class="avatar-initials">{{ authStore.user?.username?.slice(0,2).toUpperCase() }}</span>
            </div>
            <span class="presence-dot" :class="{ on: authStore.user?.isOnline }"></span>
          </div>
          <div class="hero-meta">
            <div class="name-row">
              <h1 class="display-name">{{ authStore.user?.username }}</h1>
              <span class="online-badge" :class="authStore.user?.isOnline ? 'badge-on' : 'badge-off'">
                <span class="badge-dot"></span>
                {{ authStore.user?.isOnline ? 'Online' : 'Offline' }}
              </span>
            </div>
            <p class="handle">@{{ authStore.user?.username }}</p>
            <p class="bio" v-if="authStore.user?.bio">{{ authStore.user.bio }}</p>
            <p class="status-line" v-if="authStore.user?.status">
              <span class="status-glyph">⌬</span> {{ authStore.user.status }}
            </p>
          </div>
        </div>
      </header>

      <!-- 2. XP progression -->
      <section class="xp-row">
        <span class="level-pill">Lv.&nbsp;<b>{{ userStats.level }}</b></span>
        <div class="xp-track">
          <div class="xp-label">
            <span><b>{{ intoLevel }}</b> / 100 XP to level {{ userStats.level + 1 }}</span>
            <span class="xp-remaining">{{ xpToNextLevel }} XP to go</span>
          </div>
          <div class="xp-bar">
            <div class="xp-fill" :style="{ width: xpProgress + '%' }">
              <span class="xp-shimmer"></span>
            </div>
          </div>
        </div>
        <span class="level-pill level-pill-next">Lv.&nbsp;<b>{{ userStats.level + 1 }}</b></span>
      </section>

      <!-- 3. Stats row -->
      <section class="stats-row">
        <article class="stat-card">
          <span class="stat-icon stat-icon-wins" aria-hidden="true">
            <svg viewBox="0 0 24 24"><path d="M7 4h10v3a5 5 0 0 1-10 0V4Z"/><path d="M5 5H3v2a3 3 0 0 0 3 3"/><path d="M19 5h2v2a3 3 0 0 1-3 3"/><path d="M9 19h6M12 13v6"/></svg>
          </span>
          <span class="stat-val">{{ userStats.wins.toLocaleString() }}</span>
          <span class="stat-label">Wins</span>
        </article>
        <article class="stat-card">
          <span class="stat-icon stat-icon-losses" aria-hidden="true">
            <svg viewBox="0 0 24 24"><path d="M5 11a7 7 0 1 1 14 0v3a2 2 0 0 1-1 1.7V18a2 2 0 0 1-2 2h-1v-2h-2v2h-2v-2H9v2H8a2 2 0 0 1-2-2v-2.3A2 2 0 0 1 5 14v-3Z"/><circle cx="9" cy="12" r="1.2" fill="currentColor"/><circle cx="15" cy="12" r="1.2" fill="currentColor"/></svg>
          </span>
          <span class="stat-val">{{ userStats.losses.toLocaleString() }}</span>
          <span class="stat-label">Losses</span>
        </article>
        <article class="stat-card">
          <span class="stat-icon stat-icon-level" aria-hidden="true">
            <svg viewBox="0 0 24 24"><path d="M12 3l2.8 5.7 6.2.9-4.5 4.4 1.1 6.2L12 17.9 6.4 20.2l1.1-6.2L3 9.6l6.2-.9L12 3z"/></svg>
          </span>
          <span class="stat-val">{{ userStats.level.toLocaleString() }}</span>
          <span class="stat-label">Level</span>
        </article>
      </section>

      <!-- 4. Achievements -->
      <section class="ach-section">
        <header class="section-head">
          <h2>Achievements</h2>
          <span class="section-count">
            {{ achievements.filter(a => a.unlocked).length }} / {{ allAchievements.length || achievements.length }}
          </span>
        </header>
        <div class="ach-grid" v-if="allAchievements.length">
          <article
            v-for="a in allAchievements"
            :key="a.id"
            class="ach-card"
            :class="{ unlocked: unlockedMap[a.id], locked: !unlockedMap[a.id] }"
            :title="unlockedMap[a.id] ? (a.unlockedAt ? `Unlocked ${new Date(a.unlockedAt).toLocaleDateString()}` : 'Unlocked') : 'Locked — ' + a.description"
          >
            <div class="ach-icon">
              <img v-if="a.icon && a.icon.startsWith('/')" :src="a.icon" class="ach-icon-img" alt="" />
              <span v-else>{{ a.icon || '🏆' }}</span>
            </div>
            <div class="ach-name">{{ a.name }}</div>
            <div class="ach-desc">{{ a.description }}</div>
            <div class="ach-foot">
              <span class="xp-pip">+{{ a.xpReward ?? a.points ?? 0 }} XP</span>
              <span v-if="!unlockedMap[a.id]" class="lock-icon">🔒</span>
            </div>
          </article>
        </div>
        <div class="ach-grid" v-else-if="achievements.length">
          <article
            v-for="a in achievements"
            :key="a.id"
            class="ach-card unlocked"
            :title="a.description"
          >
            <div class="ach-icon">
              <img v-if="a.icon && a.icon.startsWith('/')" :src="a.icon" class="ach-icon-img" alt="" />
              <span v-else>{{ a.icon || '🏆' }}</span>
            </div>
            <div class="ach-name">{{ a.name }}</div>
            <div class="ach-desc">{{ a.description }}</div>
            <div class="ach-foot">
              <span class="xp-pip">+{{ a.xpReward ?? a.points ?? 0 }} XP</span>
            </div>
          </article>
        </div>
        <p v-if="!allAchievements.length && !achievements.length" class="empty-msg">No achievements yet.</p>
      </section>

      <!-- 5. My Posts -->
      <section class="posts-section">
        <header class="section-head">
          <h2>My Posts</h2>
        </header>
        <div v-if="postsLoading" class="empty-msg">Loading posts…</div>
        <div v-else-if="!userPosts.length" class="empty-msg">No posts yet — go spit some facts.</div>
        <div v-for="post in userPosts" :key="post.id" class="mini-post">
          <p class="mini-post-content">{{ post.content }}</p>
          <img v-if="post.image_url" :src="post.image_url" class="mini-post-image" alt="" />
          <div class="mini-post-footer">
            <span>{{ post.likes_count || 0 }} likes · {{ post.comments_count || 0 }} comments</span>
            <span>{{ formatDate(post.created_at) }}</span>
          </div>
        </div>
      </section>
    </section>

    <!-- ══════════════════════ SETTINGS TAB ══════════════════════ -->
    <div v-else-if="activeTab === 'settings'" class="settings" :class="{ mounted: settingsMounted }">

      <!-- Flash banner -->
      <Transition name="flash">
        <div v-if="globalMsg" class="flash" :class="'flash-' + globalMsg.type" role="status">
          <span class="flash-icon" aria-hidden="true">
            <svg v-if="globalMsg.type === 'success'" viewBox="0 0 24 24"><path d="M5 12l4 4 10-10"/></svg>
            <svg v-else viewBox="0 0 24 24"><circle cx="12" cy="12" r="9"/><path d="M12 8v5M12 17v.01"/></svg>
          </span>
          <span class="flash-text">{{ globalMsg.text }}</span>
          <button class="flash-x" @click="globalMsg = null" aria-label="Dismiss">
            <svg viewBox="0 0 24 24"><path d="M6 6l12 12M18 6L6 18"/></svg>
          </button>
        </div>
      </Transition>

      <!-- 1. Edit Profile -->
      <section class="settings-section" style="--i:0">
        <h3>Edit Profile</h3>
        <form @submit.prevent="saveProfile">
          <div class="field">
            <label for="f-username">Username</label>
            <input id="f-username" v-model="profileForm.username" type="text" autocomplete="username" maxlength="32" />
          </div>
          <div class="field">
            <label for="f-email">Email</label>
            <input id="f-email" v-model="profileForm.email" type="email" autocomplete="email" />
          </div>
          <div class="field">
            <label for="f-bio">Bio</label>
            <textarea id="f-bio" v-model="profileForm.bio" rows="3" maxlength="500"></textarea>
            <span class="counter">{{ (profileForm.bio || '').length }}/500</span>
          </div>
          <div class="field">
            <label for="f-status">Status</label>
            <input id="f-status" v-model="profileForm.status" type="text" maxlength="200" />
            <span class="counter">{{ (profileForm.status || '').length }}/200</span>
          </div>
          <div class="field">
            <label>Avatar</label>
            <div class="avatar-row">
              <div class="avatar-preview">
                <img v-if="profileForm.avatar" :src="profileForm.avatar" alt="avatar preview" />
                <span v-else class="avatar-fallback">{{ profileForm.username?.slice(0,2).toUpperCase() }}</span>
              </div>
              <div class="avatar-actions">
                <label class="btn btn-secondary file-btn">
                  Upload Image
                  <input type="file" accept="image/*" @change="uploadAvatar" hidden />
                </label>
              </div>
            </div>
          </div>
          <div class="s-actions">
            <button type="submit" class="btn btn-primary" :disabled="savingProfile">
              {{ savingProfile ? 'Saving…' : 'Save Profile' }}
            </button>
          </div>
        </form>
      </section>

      <!-- 2. Privacy -->
      <section class="settings-section" style="--i:1">
        <h3>Privacy</h3>
        <div class="s-row">
          <div class="s-row-meta">
            <div class="s-row-label">Public Profile</div>
            <p class="s-row-desc">Allow anyone to view your profile via the public API.</p>
          </div>
          <label class="toggle">
            <input type="checkbox" v-model="profileForm.is_public" @change="saveProfile" />
            <span class="toggle-track"><span class="toggle-thumb"></span></span>
          </label>
        </div>
      </section>

      <!-- 3. Change Password -->
      <section class="settings-section" style="--i:2">
        <h3>Change Password</h3>
        <form @submit.prevent="changePassword">
          <!-- Hidden username field for password manager + a11y heuristics; must live inside the form. -->
          <input type="text" :value="profileForm.username" autocomplete="username" class="visually-hidden" tabindex="-1" aria-hidden="true" readonly />
          <div class="field">
            <label for="f-pw1">Current Password</label>
            <div style="display:flex;align-items:center;gap:8px">
              <input id="f-pw1" v-model="pwForm.current" :type="showPw ? 'text' : 'password'" autocomplete="current-password" maxlength="50" />
              <button type="button" class="pwd-toggle" @click="showPw = !showPw" :aria-pressed="showPw" :aria-label="showPw ? 'Hide password' : 'Show password'">{{ showPw ? 'Hide' : 'Show' }}</button>
            </div>
          </div>
          <div class="field">
            <label for="f-pw2">New Password</label>
            <div style="display:flex;align-items:center;gap:8px">
              <input id="f-pw2" v-model="pwForm.newPw" :type="showPw ? 'text' : 'password'" autocomplete="new-password" maxlength="50" />
              <button type="button" class="pwd-toggle" @click="showPw = !showPw" :aria-pressed="showPw" :aria-label="showPw ? 'Hide password' : 'Show password'">{{ showPw ? 'Hide' : 'Show' }}</button>
            </div>
          </div>
          <div class="field">
            <label for="f-pw3">Confirm New Password</label>
            <div style="display:flex;align-items:center;gap:8px">
              <input id="f-pw3" v-model="pwForm.confirm" :type="showPw ? 'text' : 'password'" autocomplete="new-password" maxlength="50" />
              <button type="button" class="pwd-toggle" @click="showPw = !showPw" :aria-pressed="showPw" :aria-label="showPw ? 'Hide password' : 'Show password'">{{ showPw ? 'Hide' : 'Show' }}</button>
            </div>
          </div>
          <p v-if="pwError" class="error-msg">{{ pwError }}</p>
          <div class="s-actions">
            <button type="submit" class="btn btn-primary" :disabled="savingPw">
              {{ savingPw ? 'Updating…' : 'Update Password' }}
            </button>
          </div>
        </form>
      </section>

      <!-- 4. Public API Key -->
      <section class="settings-section" style="--i:3">
        <h3>Public API Key</h3>
        <p class="s-muted">
          Use this key to access the <a href="/api/docs/public" target="_blank" rel="noopener" class="s-link">Public API</a>.
          Pass it as the <code>X-API-Key</code> header on every request.
        </p>
        <div v-if="apiKeyMsg" class="flash" :class="'flash-' + apiKeyMsg.type" style="margin-bottom:1rem">
          <span class="flash-text">{{ apiKeyMsg.text }}</span>
        </div>

        <template v-if="currentApiKey">
          <div class="key-row">
            <code class="key-value">{{ maskedKey }}</code>
            <div class="key-actions">
              <button class="btn btn-pill btn-secondary" @click="revealKey = !revealKey">
                {{ revealKey ? 'Hide' : 'Reveal' }}
              </button>
              <button class="btn btn-pill btn-secondary copy-btn" @click="copyApiKey">
                Copy
                <span class="copied-tip" :class="{ show: Date.now() - copiedAt < 1500 }">Copied!</span>
              </button>
              <button class="btn btn-pill btn-danger" :disabled="apiKeyLoading" @click="revokeApiKey">Revoke</button>
            </div>
          </div>
          <div class="s-actions s-actions-full">
            <button class="btn btn-primary btn-full" :disabled="apiKeyLoading" @click="generateApiKey">
              {{ apiKeyLoading ? 'Working…' : 'Regenerate Key' }}
            </button>
          </div>
        </template>

        <template v-else>
          <p class="s-muted">No API key generated yet.</p>
          <div class="s-actions">
            <button class="btn btn-primary" :disabled="apiKeyLoading" @click="generateApiKey">
              {{ apiKeyLoading ? 'Working…' : 'Generate Key' }}
            </button>
          </div>
        </template>
      </section>

      <!-- 5. My Data — danger zone -->
      <section class="settings-section danger-zone" style="--i:4">
        <h3>My Data</h3>
        <div class="data-row">
          <div class="s-row-meta">
            <div class="s-row-label">Export My Data</div>
            <p class="s-row-desc">Request a full export of your account data.</p>
          </div>
          <div class="s-row-action">
            <select v-model="exportFormat" class="s-select">
              <option value="json">JSON</option>
              <option value="csv">CSV</option>
              <option value="xml">XML</option>
            </select>
            <button class="btn btn-pill btn-secondary" :disabled="requestingData" @click="requestExport">
              {{ requestingData ? 'Wait…' : 'Export' }}
            </button>
          </div>
        </div>
        <div class="data-row last">
          <div class="s-row-meta">
            <div class="s-row-label danger-text">Delete Account</div>
            <p class="s-row-desc">Permanently delete your account and all associated data. This cannot be undone.</p>
          </div>
          <div class="s-row-action">
            <button class="btn btn-pill btn-danger-solid" :disabled="requestingData" @click="confirmDelete">Delete</button>
          </div>
        </div>
      </section>
    </div>

  </div>
</template>

<script setup>
import { computed, onMounted, ref, watch } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import api from '../services/api.js'
import { devError } from '../services/logger.js'
import { useAuthStore } from '../stores/auth.js'

const authStore = useAuthStore()
const route = useRoute()
const router = useRouter()

// UI State
const activeTab = ref('overview')
const settingsMounted = ref(false)

// Profile Data State
const achievements = ref([])
const allAchievements = ref([])
const unlockedMap = ref({})
const userPosts = ref([])
const postsLoading = ref(true)

const userStats = ref({ xp: 0, level: 0, wins: 0, losses: 0, coins: 0 })

const intoLevel = computed(() => {
  const x = userStats.value.xp ?? 0
  const l = userStats.value.level ?? 0
  return Math.max(0, Math.min(100, x - l * 100))
})
const xpToNextLevel = computed(() => Math.max(0, 100 - intoLevel.value))
const xpProgress = computed(() => intoLevel.value)

// Settings Data State
const globalMsg = ref(null)
const savingProfile = ref(false)
const savingPw = ref(false)
const requestingData = ref(false)
const pwError = ref(null)
const exportFormat = ref('json')

// API Key State
const currentApiKey = ref(null)
const apiKeyLoading = ref(false)
const apiKeyMsg = ref(null)
const revealKey = ref(false)
const copiedAt = ref(0)

const maskedKey = computed(() => {
  if (!currentApiKey.value) return ''
  if (revealKey.value) return currentApiKey.value
  return currentApiKey.value.slice(0, 6) + '•'.repeat(Math.max(0, currentApiKey.value.length - 6))
})

const MAX_AVATAR_BYTES = 10 * 1024 * 1024
const ALLOWED_AVATAR_TYPES = ['image/jpeg', 'image/png', 'image/gif', 'image/webp', 'image/svg+xml']

const profileForm = ref({
  username: '', email: '', bio: '', status: '', avatar: '', is_public: false
})
const pwForm = ref({ current: '', newPw: '', confirm: '' })
const showPw = ref(false)

// Helpers
function formatDate(ts) {
  if (!ts) return ''
  const d = new Date(ts)
  return `${String(d.getDate()).padStart(2, '0')}-${String(d.getMonth() + 1).padStart(2, '0')}-${d.getFullYear()}`
}

function flash(text, type = 'success') {
  globalMsg.value = { text, type }
  setTimeout(() => { globalMsg.value = null }, 4000)
}

function setActiveTab(tab) {
  const nextTab = tab === 'settings' ? 'settings' : 'overview'
  activeTab.value = nextTab
  router.replace({
    name: 'Profile',
    query: nextTab === 'settings' ? { tab: 'settings' } : {},
  })
}

watch(
  () => route.query.tab,
  (tab) => { activeTab.value = tab === 'settings' ? 'settings' : 'overview' },
  { immediate: true },
)

// Stagger animation trigger for settings tab
watch(activeTab, (tab) => {
  if (tab === 'settings') {
    settingsMounted.value = false
    requestAnimationFrame(() => { settingsMounted.value = true })
  }
})

// Initialization
onMounted(async () => {
  // Refresh canonical profile payload when entering own profile.
  try {
    const { data } = await api.get('/users/me')
    if (data?.user) {
      authStore.user = data.user
    }
  } catch (e) { devError(e) }

  // 1. Initialize Profile Form
  const u = authStore.user
  if (u) {
    profileForm.value = {
      username:  u.username  ?? '',
      email:     u.email     ?? '',
      bio:       u.bio       ?? '',
      status:    u.status    ?? '',
      avatar:    u.avatar    ?? '',
      is_public: u.is_public ?? false,
    }
  }

  // 2. Seed xp/level from the authoritative UserStats on the auth payload.
  //    Fall back to 0/0 — never sum from achievements (game wins also
  //    award XP, so summing achievements alone produces drift).
  if (u) {
    userStats.value.xp = u.xp ?? 0
    userStats.value.level = u.level ?? 0
  }

  // 3. Fetch Achievements (display only — XP is server-side)
  try {
    const { data } = await api.get('/game/achievements')
    const all = data.achievements || []
    allAchievements.value = all
    achievements.value = all.filter(a => a.unlocked)
    const map = {}
    for (const a of achievements.value) { map[a.id] = true }
    unlockedMap.value = map
  } catch (e) { devError(e) }

  // 4. Fetch per-game stats (wins/losses). GameStat has no level field —
  //    don't try to read s.level here; the canonical level lives on UserStats.
  try {
    const { data } = await api.get('/game/stats');
    const s = Array.isArray(data.stats) ? data.stats[0] : data.stats
    if (s) {
      userStats.value.wins = s.wins ?? 0
      userStats.value.losses = s.losses ?? 0
    }
  } catch { /* stats endpoint optional */ }

  // 4. Fetch Posts
  try {
    const { data } = await api.get(`/posts/user/${u?.id}?limit=10`)
    userPosts.value = data.posts || []
  } catch (e) { devError(e) } finally {
    postsLoading.value = false
  }

  // 5. Fetch API key status
  try {
    const { data } = await api.get('/users/me/api-key')
    currentApiKey.value = data.apiKey || null
  } catch (e) {
    if (e?.response?.status !== 404) devError(e)
    currentApiKey.value = null
  }

  // Trigger stagger animation if starting on settings tab
  if (activeTab.value === 'settings') {
    requestAnimationFrame(() => { settingsMounted.value = true })
  }
})

// Settings Methods
async function uploadAvatar(event) {
  const file = event.target.files?.[0]
  if (!file) return
  if (!ALLOWED_AVATAR_TYPES.includes(file.type)) {
    flash('Avatar must be a JPEG, PNG, GIF, WebP, or SVG image.', 'error')
    event.target.value = ''
    return
  }
  if (file.size > MAX_AVATAR_BYTES) {
    flash('Avatar must be 10 MB or smaller.', 'error')
    event.target.value = ''
    return
  }
  const formData = new FormData()
  formData.append('files', file)
  try {
    const { data } = await api.post('/uploads', formData)
    const url = data.files?.[0]?.url
    if (url) {
      await authStore.updateProfile({ avatar: url })
      profileForm.value.avatar = url
      flash('Avatar updated!')
    }
  } catch (e) {
    flash(e.response?.data?.error?.message || 'Avatar upload failed.', 'error')
  }
}

async function saveProfile() {
  savingProfile.value = true
  try {
    await authStore.updateProfile(profileForm.value)
    flash('Profile saved.')
  } catch (e) {
    flash(
      e.data?.error?.message ||
        e.response?.data?.error?.message ||
        'Failed to save profile.',
      'error',
    )
  } finally {
    savingProfile.value = false
  }
}

async function changePassword() {
  pwError.value = null
  if (pwForm.value.newPw !== pwForm.value.confirm) {
    pwError.value = 'New passwords do not match.'
    return
  }
  // Mirror the backend password policy so the user gets an immediate, specific
  // reason instead of a bare 400. The server (userValidator.js) enforces the
  // same rules; this is just faster, clearer feedback.
  const pw = pwForm.value.newPw
  if (pw.length < 8) {
    pwError.value = 'Password must be at least 8 characters.'
    return
  }
  if (!/[A-Z]/.test(pw) || !/[a-z]/.test(pw) || !/\d/.test(pw)) {
    pwError.value =
      'Password must include an uppercase letter, a lowercase letter, and a number.'
    return
  }
  savingPw.value = true
  try {
    await api.put('/users/me/password', {
      currentPassword: pwForm.value.current,
      newPassword:     pwForm.value.newPw,
    })
    pwForm.value = { current: '', newPw: '', confirm: '' }
    flash('Password updated.')
  } catch (e) {
    // The api.js client exposes the parsed error body on e.data (not the
    // axios-style e.response.data). Read e.data first so the real server
    // message — e.g. "Current password is incorrect" — actually surfaces.
    pwError.value =
      e.data?.error?.message ||
      e.response?.data?.error?.message ||
      'Failed to update password.'
  } finally {
    savingPw.value = false
  }
}

function flashApiKey(text, type = 'success') {
  apiKeyMsg.value = { text, type }
  setTimeout(() => { apiKeyMsg.value = null }, 5000)
}

async function generateApiKey() {
  if (currentApiKey.value) {
    const ok = window.confirm(
      'Regenerate your API key?\n\nYour current key will be permanently invalidated and any integrations using it will stop working immediately.'
    )
    if (!ok) return
  }
  apiKeyLoading.value = true
  try {
    const { data } = await api.post('/users/me/api-key')
    currentApiKey.value = data.apiKey
    revealKey.value = true
    flashApiKey('API key generated. Copy it now — it will be partially hidden after you leave this page.')
  } catch (e) {
    flashApiKey(e.response?.data?.error?.message || 'Failed to generate API key.', 'error')
  } finally {
    apiKeyLoading.value = false
  }
}

async function revokeApiKey() {
  if (!window.confirm('Revoke your API key? Any integrations using it will stop working.')) return
  apiKeyLoading.value = true
  try {
    await api.delete('/users/me/api-key')
    currentApiKey.value = null
    revealKey.value = false
    flashApiKey('API key revoked.')
  } catch (e) {
    flashApiKey(e.response?.data?.error?.message || 'Failed to revoke API key.', 'error')
  } finally {
    apiKeyLoading.value = false
  }
}

async function copyApiKey() {
  if (!currentApiKey.value) return
  try {
    await navigator.clipboard.writeText(currentApiKey.value)
    copiedAt.value = Date.now()
    flashApiKey('Copied to clipboard.')
  } catch {
    flashApiKey('Could not copy — please select and copy manually.', 'error')
  }
}

async function requestExport() {
  requestingData.value = true
  try {
    const res = await api.get('/users/me/export', {
      params: { format: exportFormat.value },
      responseType: 'blob',
    })
    const ext = exportFormat.value
    const blob = new Blob([res.data])
    const url = URL.createObjectURL(blob)
    const a = document.createElement('a')
    a.href = url
    a.download = `alpacaparty-data.${ext}`
    document.body.appendChild(a)
    a.click()
    document.body.removeChild(a)
    URL.revokeObjectURL(url)
    flash('Data exported successfully.')
  } catch (e) {
    flash(e.response?.data?.error?.message || 'Failed to export data.', 'error')
  } finally {
    requestingData.value = false
  }
}

async function confirmDelete() {
  if (!window.confirm('Are you sure? This will permanently delete your account.')) return
  requestingData.value = true
  try {
    await api.delete('/users/me')
    await authStore.logout()
    flash('Account deleted.')
    await router.replace('/login')
  } catch (e) {
    flash(e.response?.data?.error?.message || 'Failed to delete account.', 'error')
  } finally {
    requestingData.value = false
  }
}
</script>

<style src="../styles/views/Profile.css" scoped></style>
