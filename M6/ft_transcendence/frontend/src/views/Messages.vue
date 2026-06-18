<template>
  <div class="messages-page">
    <!-- Sidebar -->
    <aside class="sidebar">
      <div class="sidebar-tabs">
        <button :class="['stab', { active: sideTab === 'dms' }]" @click="sideTab = 'dms'">
          DMs
          <span v-if="totalUnread" class="tab-badge">{{ totalUnread }}</span>
        </button>
      </div>

      <!-- DMs panel -->
      <div v-if="sideTab === 'dms'" class="sidebar-panel">
        <!-- Search friends -->
        <div class="search-bar">
          <input v-model="friendSearch" type="text" placeholder="Search friends…" class="sidebar-input" />
        </div>

        <!-- Existing conversations -->
        <div v-if="filteredConversations.length" class="section-label">Recent</div>
        <ul class="conv-list">
          <li
            v-for="c in filteredConversations"
            :key="c.other_user_id"
            :class="['conv-item', { active: selected?.type === 'dm' && selected.id === c.other_user_id }]"
            @click="selectConv(c)"
          >
            <img
              :src="c.avatar || '/avatars/default.svg'"
              class="mini-avatar avatar-link"
              alt=""
              title="View profile"
              @click.stop="goToProfile(c.other_user_id)"
            />
            <div class="conv-info">
              <span class="conv-name">{{ c.username }}</span>
              <span class="conv-preview">{{ c.last_message || '…' }}</span>
            </div>
            <span v-if="c.unread_count" class="unread-badge">{{ c.unread_count }}</span>
          </li>
        </ul>

        <!-- Friends list -->
        <div class="section-label">Friends</div>
        <p v-if="loadingFriends" class="loading small">Loading…</p>
        <p v-else-if="!filteredFriends.length && friendSearch" class="empty small">No matches</p>
        <ul v-else class="conv-list">
          <li
            v-for="f in filteredFriends"
            :key="f.id"
            :class="['conv-item', { active: selected?.type === 'dm' && selected.id === f.id }]"
            @click="openDmWithFriend(f)"
          >
            <div class="avatar-wrap">
              <img
                :src="f.avatar || '/avatars/default.svg'"
                class="mini-avatar avatar-link"
                alt=""
                title="View profile"
                @click.stop="goToProfile(f.id)"
              />
              <span :class="['online-dot', { online: f.is_online }]"></span>
            </div>
            <div class="conv-info">
              <span class="conv-name">{{ f.username }}</span>
              <span class="conv-preview">{{ f.is_online ? 'Online' : 'Offline' }}</span>
            </div>
          </li>
          <li v-if="!filteredFriends.length && !friendSearch" class="conv-item empty-friend">
            <span class="empty small">Add some friend!</span>
          </li>
        </ul>

        <!-- Recommended users (non-friends) -->
        <template v-if="recommendedUsers.length && !friendSearch">
          <div class="section-label">People you may know</div>
          <ul class="conv-list">
            <li
              v-for="u in recommendedUsers"
              :key="u.id"
              class="conv-item"
              @click="openDmWithUser(u)"
            >
              <img
                :src="u.avatar || '/avatars/default.svg'"
                class="mini-avatar avatar-link"
                alt=""
                title="View profile"
                @click.stop="goToProfile(u.id)"
              />
              <div class="conv-info">
                <span class="conv-name">{{ u.username }}</span>
                <span class="conv-preview">Start a conversation</span>
              </div>
            </li>
          </ul>
        </template>
      </div>
    </aside>

    <!-- Chat area -->
    <main class="chat-area">
      <div v-if="!selected" class="no-selection">
        <p>Select a conversation to start chatting.</p>
        <p class="hint">Click a friend on the left to start a DM.</p>
      </div>
      <template v-else>
        <header class="chat-header">
          <div class="chat-title-wrap">
            <img
              v-if="selected.type === 'dm'"
              :src="selected.avatar || '/avatars/default.svg'"
              class="chat-avatar avatar-link"
              alt=""
              title="View profile"
              @click="goToProfile(selected.id)"
            />
            <span class="chat-title">
              {{ selected.type === 'dm' ? selected.username : `# ${selected.name}` }}
            </span>
          </div>
        </header>

        <div v-if="msgError" class="error-banner">{{ msgError }}</div>

        <div class="messages-list" ref="msgBox">
          <p v-if="loadingMessages" class="loading">Loading messages…</p>
          <div v-for="m in messages" :key="m.id" :class="['msg', { mine: m.sender_id === currentUserId }]">
            <span class="msg-author">{{ m.sender_username || m.username }}</span>
            <span class="msg-content">{{ m.content }}</span>
            <span class="msg-time">{{ formatTime(m.created_at) }}</span>
          </div>
          <p v-if="!messages.length && !loadingMessages" class="empty">No messages yet.</p>
        </div>

        <form class="message-form" @submit.prevent="sendMessage">
          <input v-model="newMessage" type="text" placeholder="Type a message…" autocomplete="off" maxlength="2000" />
          <button type="submit" class="btn-primary" :disabled="!newMessage.trim() || newMessage.trim().length > 2000">Send</button>
        </form>
      </template>
    </main>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, onUnmounted, nextTick } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '../stores/auth.js'
import api from '../services/api.js'
import { socket, connectSocket, disconnectSocket } from '../services/socket.js'

const emit = defineEmits(['close'])
const router = useRouter()
const authStore = useAuthStore()
const currentUserId = computed(() => authStore.user?.id)

// Messages renders inside a modal owned by App.vue, so navigating away has to
// also tell the parent to close the modal — otherwise the overlay stays on top
// of the profile page.
function goToProfile(userId) {
  if (userId == null) return
  emit('close')
  router.push(`/users/${userId}`)
}

const sideTab = ref('dms')
const conversations = ref([])
const friends = ref([])
const recommendedUsers = ref([])
const selected = ref(null)
const messages = ref([])
const loading = ref(false)
const loadingFriends = ref(false)
const loadingMessages = ref(false)
const msgError = ref(null)
const newMessage = ref('')
const newRoomName = ref('')
const showCreateRoom = ref(false)
const showAddMember = ref(false)
const selectedMembers = ref([])
const friendSearch = ref('')
const msgBox = ref(null)

function handleSocketConnect() {
  msgError.value = null
}

function handleSocketDisconnect() {}

function handleSocketConnectError(err) {
  msgError.value = `Socket error: ${err.message}`
}

function handleDmMessage(data) {
  const message = data.id ? data : data.message
  if (
    selected.value?.type === 'dm' &&
    (message.sender_id === selected.value.id || message.receiver_id === selected.value.id)
  ) {
    if (!messages.value.some((m) => m.id === message.id)) {
      messages.value.push(message)
      scrollToBottom()
    }
  }
  fetchConversations()
}

const totalUnread = computed(() =>
  conversations.value.reduce((sum, c) => sum + (c.unread_count || 0), 0)
)

const filteredConversations = computed(() => {
  if (!friendSearch.value) return conversations.value
  const q = friendSearch.value.toLowerCase()
  return conversations.value.filter(c => c.username?.toLowerCase().includes(q))
})

const filteredFriends = computed(() => {
  const convIds = new Set(conversations.value.map(c => c.other_user_id))
  const base = friends.value.filter(f => !convIds.has(f.id))
  if (!friendSearch.value) return base
  const q = friendSearch.value.toLowerCase()
  return base.filter(f => f.username?.toLowerCase().includes(q))
})

function formatTime(ts) {
  if (!ts) return ''
  return new Date(ts).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' })
}

async function fetchConversations() {
  loading.value = true
  try {
    const { data } = await api.get('/chat/conversations')
    conversations.value = data.conversations || []
  } catch {
    // fail silently
  } finally {
    loading.value = false
  }
}

async function fetchFriends() {
  loadingFriends.value = true
  try {
    const { data } = await api.get('/friends')
    friends.value = data.friends || []
  } catch {} finally {
    loadingFriends.value = false
  }
}

async function fetchRecommended() {
  try {
    const { data } = await api.get('/users?limit=10')
    const friendIds = new Set(friends.value.map(f => f.id))
    friendIds.add(currentUserId.value)
    recommendedUsers.value = (data.users || []).filter(u => !friendIds.has(u.id)).slice(0, 5)
  } catch {}
}

function selectConv(c) {
  selected.value = { type: 'dm', id: c.other_user_id, username: c.username, avatar: c.avatar }
  loadDmMessages(c.other_user_id)
}

function openDmWithFriend(f) {
  selected.value = { type: 'dm', id: f.id, username: f.username, avatar: f.avatar }
  loadDmMessages(f.id)
}

function openDmWithUser(u) {
  selected.value = { type: 'dm', id: u.id, username: u.username, avatar: u.avatar }
  loadDmMessages(u.id)
}

async function loadDmMessages(userId) {
  if (userId === currentUserId.value) {
    msgError.value = 'Cannot message yourself'
    messages.value = []
    return
  }
  loadingMessages.value = true
  msgError.value = null
  try {
    const { data } = await api.get(`/chat/dm/${userId}`)
    messages.value = data.messages || []
    await nextTick()
    scrollToBottom()
  } catch (e) {
    msgError.value = e.response?.data?.error?.message || 'Failed to load messages'
  } finally {
    loadingMessages.value = false
  }
}

function sendMessage() {
  const content = newMessage.value.trim()
  if (!content || !selected.value) return
  if (content.length > 2000) {
    msgError.value = 'Messages must be 2000 characters or fewer'
    return
  }

  if (!socket?.connected) {
    msgError.value = 'Not connected. Reconnecting…'
    connectSocket()
    return
  }

  newMessage.value = ''

  if (selected.value.type === 'dm') {
    socket.emit('dm:send', { receiverId: selected.value.id, content }, (ack) => {
      if (ack?.error) {
        msgError.value = ack.error
      } else if (ack?.message) {
        scrollToBottom()
      }
    })
  }
}

function scrollToBottom() {
  nextTick(() => {
    if (msgBox.value) msgBox.value.scrollTop = msgBox.value.scrollHeight
  })
}

function setupSocket() {
  connectSocket()

  socket.off('connect', handleSocketConnect)
  socket.off('disconnect', handleSocketDisconnect)
  socket.off('connect_error', handleSocketConnectError)
  socket.off('dm:message', handleDmMessage)

  socket.on('connect', handleSocketConnect)
  socket.on('disconnect', handleSocketDisconnect)
  socket.on('connect_error', handleSocketConnectError)
  socket.on('dm:message', handleDmMessage)
}

onMounted(async () => {
  await Promise.all([fetchConversations(), fetchFriends()])
  await fetchRecommended()
  setupSocket()
})

onUnmounted(() => {
  if (socket) {
    socket.off('connect', handleSocketConnect)
    socket.off('disconnect', handleSocketDisconnect)
    socket.off('connect_error', handleSocketConnectError)
    socket.off('dm:message', handleDmMessage)
  }
})
</script>

<style src="../styles/views/Messages.css" scoped></style>
