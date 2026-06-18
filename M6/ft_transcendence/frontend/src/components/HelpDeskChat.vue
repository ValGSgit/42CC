<template>
  <div :class="['helpdesk-root', { 'with-footer': withFooter }]">
    <!-- Floating trigger button (hidden while chat is open; panel has its own close) -->
    <button
      v-if="!isOpen"
      class="helpdesk-fab"
      @click="toggleChat"
      title="Help Desk — Ask Paca!"
      aria-label="Open help desk chat"
    >
      <AppIcon name="alpaca" :size="32" />
    </button>

    <!-- Chat panel -->
    <transition name="chat-slide">
      <div v-if="isOpen" class="helpdesk-panel" role="dialog" aria-label="Help Desk Chat">
        <div class="hd-header">
          <div class="hd-header-info">
            <span class="hd-avatar"><AppIcon name="alpaca" :size="28" /></span>
            <div>
              <div class="hd-title">Paca — Help Desk</div>
              <div class="hd-subtitle">Ask me anything about AlpacaParty!</div>
            </div>
          </div>
          <button class="hd-close" @click="isOpen = false" aria-label="Close">&times;</button>
        </div>

        <div class="hd-messages" ref="messagesEl">
          <div v-if="!messages.length" class="hd-welcome">
            <p>Hi! I'm <strong>Paca</strong>, your AlpacaParty guide. 🌿</p>
            <p>Ask me about the game, features, or even fun alpaca facts!</p>
            <div class="hd-suggestions">
              <button
                v-for="s in suggestions"
                :key="s"
                class="hd-suggestion"
                @click="sendSuggestion(s)"
              >{{ s }}</button>
            </div>
          </div>

          <div
            v-for="(msg, i) in messages"
            :key="i"
            :class="['hd-msg', msg.role]"
          >
            <span v-if="msg.role === 'assistant'" class="msg-avatar"><AppIcon name="alpaca" :size="22" /></span>
            <div class="msg-bubble">{{ msg.content }}</div>
          </div>

          <div v-if="loading" class="hd-msg assistant">
            <span class="msg-avatar"><AppIcon name="alpaca" :size="22" /></span>
            <div class="msg-bubble typing">
              <span></span><span></span><span></span>
            </div>
          </div>
        </div>

        <div v-if="error" class="hd-error">{{ error }}</div>

        <div class="hd-input-wrapper">
          <div class="hd-input-row">
            <textarea
              ref="inputEl"
              v-model="draft"
              class="hd-input"
              placeholder="Ask a question…"
              rows="1"
              :maxlength="MAX_CHARS"
              :disabled="loading"
              @keydown.enter.exact.prevent="send"
              @input="autoResize"
            />
            <button
              class="hd-send"
              :disabled="!draft.trim() || loading || draft.length > MAX_CHARS"
              @click="send"
              aria-label="Send"
            >
            <svg width="16" height="16" viewBox="0 0 24 24" fill="currentColor">
              <path d="M2 21l21-9L2 3v7l15 2-15 2z"/>
            </svg>
          </button>
          </div>
          <div class="hd-char-counter" :class="{ 'hd-char-limit': draft.length >= MAX_CHARS * 0.9 }">
            {{ draft.length }}/{{ MAX_CHARS }}
          </div>
        </div>
      </div>
    </transition>
  </div>
</template>

<script setup>
import { ref, nextTick } from 'vue'
import AppIcon from './AppIcon.vue'

defineProps({
  withFooter: { type: Boolean, default: false },
})

const MAX_CHARS = 2000
const MAX_HISTORY = 20

const isOpen = ref(false)
const draft = ref('')
const messages = ref([])
const loading = ref(false)
const error = ref('')
const messagesEl = ref(null)
const inputEl = ref(null)

const suggestions = [
  'How do I start playing?',
  'What is AlpacaFarm?',
  'Tell me an alpaca fact!',
  'How do I add friends?',
]

function toggleChat() {
  isOpen.value = !isOpen.value
  if (isOpen.value) {
    nextTick(() => inputEl.value?.focus())
  }
}

function autoResize(e) {
  const el = e.target
  el.style.height = 'auto'
  el.style.height = Math.min(el.scrollHeight, 100) + 'px'
}

async function sendSuggestion(text) {
  draft.value = text
  await send()
}

async function send() {
  const text = draft.value.trim()
  if (!text || loading.value || text.length > MAX_CHARS) return

  error.value = ''
  messages.value.push({ role: 'user', content: text })
  draft.value = ''
  if (inputEl.value) {
    inputEl.value.style.height = 'auto'
  }
  loading.value = true
  scrollToBottom()

  const history = messages.value.slice(-MAX_HISTORY).map(m => ({ role: m.role, content: m.content }))

  try {
    const resp = await fetch('/api/helpdesk/chat', {
      method: 'POST',
      credentials: 'include',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ messages: history }),
    })

    if (!resp.ok) {
      const errBody = await resp.json().catch(() => null)
      throw new Error(errBody?.error?.message || `Request failed (${resp.status})`)
    }

    const body = await resp.json().catch(() => null)
    const content = body?.content?.trim()
    if (!content) {
      throw new Error('No response from the assistant.')
    }

    messages.value.push({ role: 'assistant', content })
    scrollToBottom()
  } catch (err) {
    error.value = err?.message || 'Something went wrong. Please try again.'
    // Roll the user's outbound message back if the assistant placeholder
    // wasn't pushed (i.e. the failure was pre-stream).
    if (messages.value[messages.value.length - 1]?.role === 'user') {
      messages.value.pop()
    }
  } finally {
    loading.value = false
    await nextTick()
    scrollToBottom()
  }
}

function scrollToBottom() {
  nextTick(() => {
    if (messagesEl.value) {
      messagesEl.value.scrollTop = messagesEl.value.scrollHeight
    }
  })
}
</script>

<style src="../styles/components/HelpDeskChat.css" scoped></style>
