<template>
  <div class="feed-page">

    <!-- ░░░ LEFT SIDEBAR ░░░ -->
    <aside class="col col-left">
      <FakeAd :sidebar="true" />
      <div class="card mini-card">
        <h4>Trending</h4>
        <ul class="trend-list">
          <li><span class="hash">#</span>SpitRoyaleMeta <em>4.2k</em></li>
          <li><span class="hash">#</span>StageBoss <em>2.1k</em></li>
          <li><span class="hash">#</span>HayBalePass <em>1.8k</em></li>
          <li><span class="hash">#</span>AlpacaRoad <em>980</em></li>
        </ul>
      </div>
    </aside>

    <!-- ░░░ CENTER FEED ░░░ -->
    <main class="col col-center">

      <!-- Error banner -->
      <div v-if="error" class="error-banner">
        {{ error }}
        <button class="error-dismiss" @click="error = null" aria-label="Dismiss">×</button>
      </div>

      <!-- Composer -->
      <section
        v-if="authStore.isAuthenticated"
        class="card composer"
        :class="{ focused: composerFocused }"
      >
        <div class="composer-row">
          <div class="avatar" :class="avatarColor(authStore.user?.username)">
            <img v-if="authStore.user?.avatar" :src="authStore.user.avatar" :alt="authStore.user?.username" />
            <span v-else>{{ authStore.user?.username?.slice(0, 2).toUpperCase() }}</span>
          </div>
          <div class="composer-body">
            <textarea
              v-model="newPostContent"
              placeholder="What's on your paddock?"
              rows="2"
              maxlength="2000"
              @focus="composerFocused = true"
              @blur="composerFocused = false"
            ></textarea>
            <div class="composer-tools">
              <div class="tool-icons">
                <label class="tool" title="Add Image" aria-label="Add Image">
                  <svg viewBox="0 0 24 24"><rect x="3" y="4" width="18" height="16" rx="2"/><circle cx="9" cy="10" r="2"/><path d="M21 17l-5-5-9 9"/></svg>
                  <input type="file" accept="image/*" @change="selectImage" hidden />
                </label>
              </div>
              <div class="composer-end">
                <span v-if="selectedImage" class="selected-file">{{ selectedImage.name }}</span>
                <span class="char-counter" :class="{ warn: newPostContent.length > 1800 }">
                  {{ 2000 - newPostContent.length }}
                </span>
                <button
                  class="btn btn-primary"
                  :disabled="!newPostContent.trim() || posting"
                  @click="createPost"
                >{{ posting ? 'Posting…' : 'Spit it' }}</button>
              </div>
            </div>
            <div v-if="uploadProgress > 0 && uploadProgress < 100" class="upload-bar">
              <div class="upload-bar-fill" :style="{ width: uploadProgress + '%' }"></div>
            </div>
          </div>
        </div>
      </section>

      <!-- Skeleton loading -->
      <template v-if="loading">
        <div v-for="n in 3" :key="'sk'+n" class="card skeleton-card">
          <div class="sk-row">
            <div class="sk-circle shimmer"></div>
            <div class="sk-stack">
              <div class="sk-line shimmer" style="width:38%"></div>
              <div class="sk-line shimmer" style="width:18%"></div>
            </div>
          </div>
          <div class="sk-line shimmer" style="width:92%"></div>
          <div class="sk-line shimmer" style="width:74%"></div>
          <div class="sk-line shimmer" style="width:60%"></div>
          <div class="sk-actions">
            <span class="sk-pill shimmer"></span>
            <span class="sk-pill shimmer"></span>
            <span class="sk-pill shimmer"></span>
          </div>
        </div>
      </template>

      <!-- Empty state -->
      <div v-else-if="!posts.length" class="empty">
        <div class="empty-art" aria-hidden="true">
          <div class="ring ring-1"></div>
          <div class="ring ring-2"></div>
          <div class="empty-llama">🦙</div>
        </div>
        <h3>It's quiet on the paddock…</h3>
        <p>Be the first to spit some facts 🦙</p>
      </div>

      <!-- Posts -->
      <article
        v-else
        v-for="post in posts"
        :key="post._displayId || post.id"
        class="card post"
        :class="{
          'post--mine':     post.author_id === authStore.user?.id,
          'post--liked':    post.user_liked && post.author_id !== authStore.user?.id,
        }"
      >
        <!-- Tombstone: original post was deleted -->
        <template v-if="post._deleted">
          <div class="post-tombstone">
            <svg viewBox="0 0 24 24" class="tombstone-ic" aria-hidden="true"><rect x="3" y="11" width="18" height="11" rx="2"/><path d="M7 11V7a5 5 0 0 1 10 0v4"/></svg>
            <span>The original post was deleted.</span>
          </div>
        </template>

        <template v-else>
          <header class="post-head">
            <router-link v-if="post.author_id" :to="`/users/${post.author_id}`" class="author-avatar-link">
              <div class="avatar" :class="avatarColor(post.author_username)">
                <img v-if="post.author_avatar" :src="resolveMediaUrl(post.author_avatar)" :alt="post.author_username" @error="onPostImageError" />
                <span v-else>{{ post.author_username?.slice(0, 2).toUpperCase() }}</span>
              </div>
            </router-link>
            <div v-else class="avatar" :class="avatarColor(post.author_username)">
              <span>{{ post.author_username?.slice(0, 2).toUpperCase() }}</span>
            </div>

            <div class="post-meta">
              <router-link v-if="post.author_id" :to="`/users/${post.author_id}`" class="username">
                {{ post.author_username }}
                <span v-if="post.author_id === authStore.user?.id" class="self-tag">you</span>
              </router-link>
              <span v-else class="username">{{ post.author_username || 'Unknown' }}</span>
              <span class="dot-sep">·</span>
              <time class="ts" :title="new Date(post.created_at).toLocaleString()">{{ formatTime(post.created_at) }}</time>
            </div>

            <button
              v-if="post.author_id === authStore.user?.id"
              class="icon-btn danger"
              title="Delete post"
              @click="deletePost(post.source_post_id || post.id)"
            >
              <svg viewBox="0 0 24 24"><path d="M4 7h16M9 7V4h6v3M6 7l1 13h10l1-13"/></svg>
            </button>
          </header>

          <div class="post-body">
            <p class="post-text">{{ post.content }}</p>
            <img
              v-if="post.image_url"
              :src="resolveMediaUrl(post.image_url)"
              class="post-image"
              alt="post image"
              @error="onPostImageError"
            />
          </div>

          <!-- Action pills -->
          <footer class="post-actions">
            <button class="pill pill-like" :class="{ on: post.user_liked }" @click="toggleLike(post)">
              <svg viewBox="0 0 24 24" class="ic"><path d="M12 21s-7-4.6-9.5-9.1C1 8.5 3 5 6.4 5c2 0 3.4 1 4.6 2.6l1 1.4 1-1.4C14.2 6 15.6 5 17.6 5 21 5 23 8.5 21.5 11.9 19 16.4 12 21 12 21Z"/></svg>
              <span class="count">{{ post.likes_count || 0 }}</span>
            </button>

            <button class="pill pill-comment" :class="{ on: commentsOpen.has(post._displayId) }" @click="toggleComments(post)">
              <svg viewBox="0 0 24 24" class="ic"><path d="M21 12a8 8 0 0 1-11.6 7.1L4 20l1-4.5A8 8 0 1 1 21 12Z"/></svg>
              <span class="count">{{ post.comments_count || 0 }}</span>
            </button>
          </footer>
        </template>

        <!-- Comments panel (slide via max-height) -->
        <section class="comments" :class="{ open: commentsOpen.has(post._threadKey) }">
          <div class="comments-inner">
            <div v-if="commentLoading.has(post._threadKey)" class="no-comments">Loading…</div>
            <template v-else>
              <div v-if="!(postComments[post._threadKey]?.length)" class="no-comments">No comments yet.</div>
              <div v-for="c in (postComments[post._threadKey] || [])" :key="c.id" class="comment">
                <div class="avatar avatar-sm" :class="avatarColor(c.author_username)">
                  <img v-if="c.author_avatar" :src="resolveMediaUrl(c.author_avatar)" :alt="c.author_username" />
                  <span v-else>{{ c.author_username?.slice(0, 2).toUpperCase() }}</span>
                </div>
                <div class="comment-body">
                  <div class="comment-head">
                    <router-link v-if="c.author_id" :to="`/users/${c.author_id}`" class="username">{{ c.author_username }}</router-link>
                    <span v-else class="username">{{ c.author_username }}</span>
                    <time class="ts">{{ formatTime(c.created_at) }}</time>
                    <button
                      v-if="c.author_id === authStore.user?.id"
                      class="icon-btn danger"
                      style="width:22px;height:22px;margin-left:auto;flex-shrink:0"
                      @click="deleteComment(post, c)"
                    >
                      <svg viewBox="0 0 24 24"><path d="M6 6l12 12M18 6L6 18"/></svg>
                    </button>
                  </div>
                  <p>{{ c.content }}</p>
                </div>
              </div>
            </template>

            <form v-if="authStore.isAuthenticated" class="comment-compose" @submit.prevent="submitComment(post)">
              <div class="avatar avatar-sm" :class="avatarColor(authStore.user?.username)">
                <img v-if="authStore.user?.avatar" :src="authStore.user.avatar" :alt="authStore.user?.username" />
                <span v-else>{{ authStore.user?.username?.slice(0, 2).toUpperCase() }}</span>
              </div>
              <input
                v-model="commentDraft[post._threadKey]"
                type="text"
                placeholder="Reply with a witty alpaca-ism…"
                maxlength="1000"
              />
              <button
                class="btn btn-ghost btn-sm"
                type="submit"
                :disabled="!commentDraft[post._threadKey]?.trim()"
              >Reply</button>
            </form>
          </div>
        </section>
      </article>
    </main>

    <!-- ░░░ RIGHT SIDEBAR ░░░ -->
    <aside class="col col-right">
      <ActiveLeaderboard />
    </aside>

    <!-- Mobile FAB -->
    <button class="fab" aria-label="Compose" @click="composerFocused = true">
      <svg viewBox="0 0 24 24"><path d="M12 5v14M5 12h14"/></svg>
    </button>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { useAuthStore } from '../stores/auth.js'
import api from '../services/api.js'
import FakeAd from '../components/FakeAd.vue'
import ActiveLeaderboard from '../components/ActiveLeaderboard.vue'

const authStore = useAuthStore()
const posts = ref([])
const loading = ref(true)
const posting = ref(false)
const error = ref(null)
const newPostContent = ref('')
const selectedImage = ref(null)
const uploadProgress = ref(0)
const composerFocused = ref(false)

// Comments state
const commentsOpen = reactive(new Set())
const commentLoading = reactive(new Set())
const postComments = reactive({})
const commentDraft = reactive({})

function avatarColor(username) {
  const colors = ['av-cyan', 'av-gold', 'av-pink', 'av-aqua', 'av-mint', 'av-violet']
  let hash = 0
  for (const c of (username || '')) hash = (hash * 31 + c.charCodeAt(0)) & 0x7fffffff
  return colors[hash % colors.length]
}

function getPostDisplayId(post) {
  return `post|${post.id}`
}

function getThreadKey(post) {
  return `${post.thread_type || 'post'}|${post.thread_id || post.id}`
}

function formatTime(ts) {
  if (!ts) return ''
  const d = new Date(ts)
  const now = new Date()
  const diff = now - d
  if (diff < 60000) return 'just now'
  if (diff < 3600000) return `${Math.floor(diff / 60000)}m ago`
  if (diff < 86400000) return `${Math.floor(diff / 3600000)}h ago`
  return `${String(d.getDate()).padStart(2, '0')}-${String(d.getMonth() + 1).padStart(2, '0')}-${d.getFullYear()}`
}

function resolveMediaUrl(url) {
  if (!url) return ''
  if (/^https?:\/\//i.test(url)) return url
  return url.startsWith('/') ? url : `/${url}`
}

function onPostImageError(event) {
  event.target.src = '/avatars/default.svg'
}

async function fetchPosts() {
  loading.value = true
  error.value = null
  try {
    const { data } = await api.get('/posts')
    const newPosts = (data.posts || []).map(post => {
      post._displayId = getPostDisplayId(post)
      post._threadKey = getThreadKey(post)
      return post
    })

    // Clear comment state for posts that are no longer in the feed
    const newDisplayIds = new Set(newPosts.map(p => p._threadKey))
    for (const key of Object.keys(postComments)) {
      if (!newDisplayIds.has(key)) {
        delete postComments[key]
        delete commentDraft[key]
      }
    }

    posts.value = newPosts
  } catch (e) {
    error.value = 'Failed to load posts'
  } finally {
    loading.value = false
  }
}

const MAX_UPLOAD_BYTES = 10 * 1024 * 1024
const ALLOWED_TYPES = ['image/jpeg', 'image/png', 'image/gif', 'image/webp']

function selectImage(event) {
  const file = event.target.files?.[0] || null
  if (file) {
    if (!ALLOWED_TYPES.includes(file.type)) {
      error.value = 'Only JPEG, PNG, GIF, and WebP images are allowed'
      event.target.value = ''
      return
    }
    if (file.size > MAX_UPLOAD_BYTES) {
      error.value = `Image is too large (max 10 MB). Your file: ${(file.size / 1024 / 1024).toFixed(1)} MB`
      event.target.value = ''
      return
    }
  }
  selectedImage.value = file
  error.value = null
}

async function createPost() {
  if (!newPostContent.value.trim()) return
  posting.value = true
  error.value = null
  uploadProgress.value = 0
  try {
    let imageUrl = null
    if (selectedImage.value) {
      const formData = new FormData()
      formData.append('files', selectedImage.value)
      const uploadRes = await api.post('/uploads', formData)
      imageUrl = uploadRes.data.files?.[0]?.url
      if (!imageUrl) {
        error.value = 'Image upload failed — try a smaller file or different format'
        return
      }
    }
    await api.post('/posts', {
      content: newPostContent.value.trim(),
      imageUrl,
    })
    newPostContent.value = ''
    selectedImage.value = null
    uploadProgress.value = 0
    await fetchPosts()
  } catch (e) {
    const status = e.response?.status
    if (status === 413) {
      error.value = 'Image is too large — please use a file under 10 MB'
    } else {
      error.value = e.response?.data?.error?.message || 'Failed to create post'
    }
    uploadProgress.value = 0
  } finally {
    posting.value = false
  }
}

async function toggleLike(post) {
  const targetPostId = post.source_post_id || post.id
  const wasLiked = post.user_liked
  post.user_liked = !wasLiked
  post.likes_count = wasLiked
    ? Math.max(0, (post.likes_count || 1) - 1)
    : (post.likes_count || 0) + 1
  try {
    if (wasLiked) {
      await api.delete(`/posts/${targetPostId}/like`)
    } else {
      await api.post(`/posts/${targetPostId}/like`)
    }
  } catch {
    post.user_liked = wasLiked
    post.likes_count = wasLiked
      ? (post.likes_count || 0) + 1
      : Math.max(0, (post.likes_count || 1) - 1)
  }
}

async function deletePost(postId) {
  if (!confirm('Delete this post?')) return
  try {
    await api.delete(`/posts/${postId}`)
    posts.value = posts.value.filter(p => p.id !== postId)
  } catch (e) {
    error.value = 'Failed to delete post'
  }
}

// ── Comments ──────────────────────────────────────────────────────────────

async function toggleComments(post) {
  const threadKey = post._threadKey || getThreadKey(post)
  if (commentsOpen.has(threadKey)) {
    commentsOpen.delete(threadKey)
    return
  }
  commentsOpen.add(threadKey)
  if (!postComments[threadKey]) {
    await loadComments(post.thread_id || post.id, threadKey, post.thread_type || 'post')
  }
}

async function loadComments(threadId, threadKey, threadType = 'post') {
  commentLoading.add(threadKey)
  try {
    const { data } = await api.get(`/posts/${threadId}/comments?thread=${threadType}`)
    postComments[threadKey] = data.comments || []
  } catch {
    postComments[threadKey] = []
  } finally {
    commentLoading.delete(threadKey)
  }
}

async function submitComment(post) {
  const threadKey = post._threadKey || getThreadKey(post)
  const content = commentDraft[threadKey]?.trim()
  if (!content) return
  if (content.length > 1000) {
    error.value = 'Comments must be 1000 characters or fewer'
    return
  }
  try {
    const { data } = await api.post(`/posts/${post.thread_id || post.id}/comments?thread=${post.thread_type || 'post'}`, { content })
    if (!postComments[threadKey])
      postComments[threadKey] = []
    postComments[threadKey].push(data.comment)
    commentDraft[threadKey] = ''
    post.comments_count = (post.comments_count || 0) + 1
  } catch {
    error.value = 'Failed to post comment'
  }
}

async function deleteComment(post, comment) {
  const threadKey = post._threadKey || getThreadKey(post)
  try {
    await api.delete(`/posts/${post.thread_id || post.id}/comments/${comment.id}?thread=${post.thread_type || 'post'}`)
    postComments[threadKey] = postComments[threadKey].filter(c => c.id !== comment.id)
    post.comments_count = Math.max(0, (post.comments_count || 1) - 1)
  } catch {
    error.value = 'Failed to delete comment'
  }
}

// ── Lifecycle ─────────────────────────────────────────────────────────────

onMounted(fetchPosts)
</script>

<style src="../styles/views/Feed.css" scoped></style>
