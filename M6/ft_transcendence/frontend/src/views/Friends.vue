<template>
  <div class="friends-page">
    <div class="page-header">
      <div class="header-left">
        <h1 class="page-title">Friends</h1>
        <p class="page-sub">Connect, play, and compete with your crew</p>
      </div>
      <div class="header-stats">
        <div class="stat-pill">
          <span class="stat-num">{{ totalFriends }}</span>
          <span class="stat-label">Friends</span>
        </div>
        <div v-if="pendingCount" class="stat-pill pend">
          <span class="stat-num">{{ pendingCount }}</span>
          <span class="stat-label">Pending</span>
        </div>
      </div>
    </div>

    <div class="tab-bar">
      <button
        v-for="(tab, i) in tabs"
        :key="tab.key"
        :class="['tab', { active: activeTab === tab.key }]"
        @click="activeTab = tab.key"
      >
        {{ tab.label }}
        <span v-if="tab.key === 'requests' && pendingCount" class="tab-badge">{{
          pendingCount
        }}</span>
      </button>
      <div
        class="tab-underline"
        :style="{ transform: `translateX(${tabIndex * 100}%)` }"
      ></div>
    </div>

    <transition name="err-fade">
      <div v-if="error" class="error-toast" role="alert">
        <svg viewBox="0 0 20 20" fill="currentColor" class="err-icon">
          <path
            fill-rule="evenodd"
            d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-7-4a1 1 0 11-2 0 1 1 0 012 0zM9 9a1 1 0 000 2v3a1 1 0 001 1h1a1 1 0 100-2v-3a1 1 0 00-1-1H9z"
            clip-rule="evenodd"
          />
        </svg>
        <span>{{ error }}</span>
        <button class="err-close" @click="error = null">×</button>
      </div>
    </transition>

    <div v-if="loading" class="skeleton-list">
      <div v-for="n in 4" :key="n" class="skeleton-card"></div>
    </div>

    <transition name="tab-slide" mode="out-in">
      <div
        v-if="activeTab === 'friends' && !loading"
        key="friends"
        class="tab-pane"
      >
        <div class="toolbar">
          <div class="search-wrap">
            <svg class="search-icon" viewBox="0 0 20 20" fill="currentColor">
              <path
                fill-rule="evenodd"
                d="M8 4a4 4 0 100 8 4 4 0 000-8zM2 8a6 6 0 1110.89 3.476l4.817 4.817a1 1 0 01-1.414 1.414l-4.816-4.816A6 6 0 012 8z"
                clip-rule="evenodd"
              />
            </svg>
            <input
              v-model="friendSearch"
              type="text"
              placeholder="Search friends…"
              class="search-input"
              @keyup.enter="() => fetchFriends(true)"
            />
          </div>
          <select
            v-model="friendSort"
            class="sort-select"
            @change="
              friendsFetcher.updateParams({ page: 1 });
              fetchFriends();
            "
          >
            <option value="name">Name</option>
            <option value="online">Online first</option>
            <option value="level">Level</option>
          </select>
        </div>

        <ul v-if="friends.length" class="card-list">
          <li
            v-for="(f, i) in friends"
            :key="f.id"
            class="friend-card"
            :style="{ animationDelay: `${i * 55}ms` }"
          >
            <div class="avatar-wrap">
              <img
                :src="f.avatar || '/avatars/default.svg'"
                class="avatar avatar-link"
                alt=""
                title="View profile"
                @click="goToProfile(f.id)"
              />
              <span class="status-dot" :class="{ online: f.is_online }"></span>
            </div>
            <div class="user-info">
              <span class="username">{{ f.username }}</span>
              <div class="user-meta">
                <span class="level-badge">Lv {{ f.level || 1 }}</span>
                <span class="meta-dot"></span>
                <span :class="['status-text', { online: f.is_online }]">
                  {{ f.is_online ? "Online" : "Offline" }}
                </span>
              </div>
            </div>
            <div class="card-actions">
              <button
                class="btn-ghost btn-danger-ghost"
                @click="removeFriend(f.id)"
              >
                Remove
              </button>
              <button class="btn-ghost" @click="blockUser(f.id)">Block</button>
            </div>
          </li>
        </ul>

        <div v-if="friendsFetcher.multiplePages()" class="pagination">
          <button
            class="btn-ghost pag-btn"
            :disabled="friendsFetcher.isFirstPage()"
            @click="
              friendsFetcher.previousPage();
              fetchFriends();
            "
          >
            ‹ Prev
          </button>
          <span class="page-info"
            >{{ friendsFetcher.page }} /
            {{
              Math.ceil(friendsFetcher.total / friendsFetcher.pageSize)
            }}</span
          >
          <button
            class="btn-ghost pag-btn"
            :disabled="friendsFetcher.isLastPage()"
            @click="
              friendsFetcher.nextPage();
              fetchFriends();
            "
          >
            Next ›
          </button>
        </div>

        <div v-if="!friends.length" class="empty-state">
          <span class="empty-icon">🦙</span>
          <p v-if="friendSearch">No friends matching "{{ friendSearch }}"</p>
          <p v-else>No friends yet — start by discovering users below!</p>
        </div>

        <div class="discover-section">
          <div class="section-header">
            <h3 class="section-title">Discover Users</h3>
          </div>

          <div class="toolbar">
            <div class="search-wrap">
              <svg class="search-icon" viewBox="0 0 20 20" fill="currentColor">
                <path
                  fill-rule="evenodd"
                  d="M8 4a4 4 0 100 8 4 4 0 000-8zM2 8a6 6 0 1110.89 3.476l4.817 4.817a1 1 0 01-1.414 1.414l-4.816-4.816A6 6 0 012 8z"
                  clip-rule="evenodd"
                />
              </svg>
              <input
                v-model="userSearch"
                type="text"
                placeholder="Search all users…"
                @keyup.enter="() => searchUsers(true)"
                class="search-input"
              />
            </div>
            <button
              class="btn-primary"
              @click="() => searchUsers(true)"
              :disabled="searchingUsers"
            >
              {{ searchingUsers ? "…" : "Search" }}
            </button>
          </div>

          <ul v-if="searchResults.length" class="card-list">
            <li
              v-for="(u, i) in searchResults"
              :key="u.id"
              class="friend-card"
              :style="{ animationDelay: `${i * 45}ms` }"
            >
              <div class="avatar-wrap">
                <img
                  :src="u.avatar || '/avatars/default.svg'"
                  class="avatar avatar-link"
                  alt=""
                  title="View profile"
                  @click="goToProfile(u.id)"
                />
              </div>
              <div class="user-info">
                <span class="username">{{ u.username }}</span>
                <div class="user-meta">
                  <span class="level-badge">Lv {{ u.level || 1 }}</span>
                </div>
              </div>
              <div class="card-actions">
                <button
                  class="btn-primary btn-sm"
                  @click="onAddFriendBtnPressed(u)"
                  :disabled="u.request_sent || u.is_friend"
                >
                  {{ u.request_sent || u.is_friend ? "Sent ✓" : "Add Friend" }}
                </button>
              </div>
            </li>
          </ul>

          <div v-if="usersFetcher.multiplePages()" class="pagination">
            <button
              class="btn-ghost pag-btn"
              :disabled="usersFetcher.isFirstPage()"
              @click="
                usersFetcher.previousPage();
                searchUsers();
              "
            >
              ‹ Prev
            </button>
            <span class="page-info"
              >{{ usersFetcher.page }} /
              {{ Math.ceil(usersFetcher.total / usersFetcher.pageSize) }}</span
            >
            <button
              class="btn-ghost pag-btn"
              :disabled="usersFetcher.isLastPage()"
              @click="
                usersFetcher.nextPage();
                searchUsers();
              "
            >
              Next ›
            </button>
          </div>
          <p
            v-else-if="userSearch && !searchResults.length && !searchingUsers"
            class="empty-inline"
          >
            No users found.
          </p>
        </div>
      </div>

      <div
        v-else-if="activeTab === 'requests' && !loading"
        key="requests"
        class="tab-pane"
      >
        <div class="sub-section">
          <h3 class="sub-title">
            <span class="sub-title-dot received"></span>
            Received
          </h3>
          <ul v-if="received.length" class="card-list">
            <li
              v-for="(r, i) in received"
              :key="r.id"
              class="friend-card"
              :style="{ animationDelay: `${i * 55}ms` }"
            >
              <div class="avatar-wrap">
                <img
                  :src="
                    r.sender?.avatar ||
                    r.sender_avatar ||
                    r.senderAvatar ||
                    '/avatars/default.svg'
                  "
                  class="avatar avatar-link"
                  alt=""
                  title="View profile"
                  @click="goToProfile(r.senderId)"
                />
              </div>
              <div class="user-info">
                <span class="username">{{
                  r.sender?.username || r.sender_username || r.senderUsername
                }}</span>
                <span class="req-label incoming">Wants to be your friend</span>
              </div>
              <div class="card-actions">
                <button class="btn-primary btn-sm" @click="acceptRequest(r.id)">
                  Accept
                </button>
                <button
                  class="btn-ghost btn-danger-ghost btn-sm"
                  @click="declineRequest(r.id)"
                >
                  Decline
                </button>
                <button
                  class="btn-ghost btn-sm"
                  @click="blockFromRequest(r.id, r.senderId)"
                >
                  Block
                </button>
              </div>
            </li>
          </ul>
          <div v-else class="empty-state compact">
            <span class="empty-icon">📭</span>
            <p>No incoming requests.</p>
          </div>
        </div>

        <div class="sub-section">
          <h3 class="sub-title">
            <span class="sub-title-dot sent"></span>
            Sent
          </h3>
          <ul v-if="sent.length" class="card-list">
            <li
              v-for="(r, i) in sent"
              :key="r.id"
              class="friend-card"
              :style="{ animationDelay: `${i * 55}ms` }"
            >
              <div class="avatar-wrap">
                <img
                  :src="
                    r.receiver?.avatar ||
                    r.receiver_avatar ||
                    r.receiverAvatar ||
                    '/avatars/default.svg'
                  "
                  class="avatar avatar-link"
                  alt=""
                  title="View profile"
                  @click="goToProfile(r.receiverId)"
                />
              </div>
              <div class="user-info">
                <span class="username">{{
                  r.receiver?.username ||
                  r.receiver_username ||
                  r.receiverUsername
                }}</span>
                <span class="req-label pending">Pending response</span>
              </div>
              <span class="status-tag">Pending</span>
            </li>
          </ul>
          <div v-else class="empty-state compact">
            <span class="empty-icon">📤</span>
            <p>No outgoing requests.</p>
          </div>
        </div>
      </div>

      <div
        v-else-if="activeTab === 'blocked' && !loading"
        key="blocked"
        class="tab-pane"
      >
        <ul v-if="blocked.length" class="card-list">
          <li
            v-for="(b, i) in blocked"
            :key="b.id"
            class="friend-card blocked-card"
            :style="{ animationDelay: `${i * 55}ms` }"
          >
            <div class="avatar-wrap">
              <img
                :src="b.avatar || '/avatars/default.svg'"
                class="avatar avatar-blocked avatar-link"
                alt=""
                title="View profile"
                @click="goToProfile(b.id)"
              />
            </div>
            <div class="user-info">
              <span class="username">{{ b.username }}</span>
              <span class="req-label blocked">Blocked</span>
            </div>
            <button class="btn-ghost btn-sm" @click="unblockUser(b.id)">
              Unblock
            </button>
          </li>
        </ul>
        <div v-else class="empty-state">
          <span class="empty-icon">🚫</span>
          <p>No blocked users.</p>
        </div>
      </div>
    </transition>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, watch } from "vue";
import { useRouter } from "vue-router";
import api from "../services/api.js";
import { useAuthStore } from "../stores/auth.js";
import ListFetcher from "../utils/ListFetcher.js";

const authStore = useAuthStore();
const router = useRouter();

function goToProfile(userId) {
  if (userId == null) return;
  router.push(`/users/${userId}`);
}

const activeTab = ref("friends");
const tabs = [
  { key: "friends", label: "Friends" },
  { key: "requests", label: "Requests" },
  { key: "blocked", label: "Blocked" },
];
const tabIndex = computed(() =>
  tabs.findIndex((t) => t.key === activeTab.value),
);

const friends = ref([]);
const totalFriends = ref(0);
const received = ref([]);
const sent = ref([]);
const blocked = ref([]);
const loading = ref(false);
const error = ref(null);
const newFriendId = ref("");

const friendSearch = ref("");
const friendSort = ref("name");
const userSearch = ref("");
const searchResults = ref([]);
const searchingUsers = ref(false);
const usersFetcher = ref(new ListFetcher());
const friendsFetcher = ref(new ListFetcher());

const pendingCount = computed(() => received.value.length);

async function onAddFriendBtnPressed(user) {
  await sendRequestToUser(user.id);
  searchUsers();
}

async function fetchFriends(searchChanged = false) {
  loading.value = true;
  error.value = null;

  let sort = {};
  if (friendSort.value === "online") {
    sort.online = "desc";
  } else if (friendSort.value === "level") {
    sort.level = "desc";
  } else {
    sort.username = "desc";
  }

  let filter = {};
  if (searchChanged) {
    const searchValue = friendSearch.value.trim();
    if (searchValue && searchValue != "") {
      filter.username = searchValue;
    }
    friendsFetcher.value.updateParams({ filter });
  }

  try {
    friendsFetcher.value.updateParams({
      sort,
    });
    const { data } = await friendsFetcher.value.fetch("/friends");
    friends.value = data.friends || [];

    if (filter.username === undefined) {
      totalFriends.value = data.total;
    }
  } catch (e) {
    error.value = "Failed to fetch friends";
  } finally {
    loading.value = false;
  }
}

async function fetchRequests() {
  loading.value = true;
  error.value = null;
  try {
    const { data } = await api.get("/friends/requests");
    received.value = data.received;
    sent.value = data.sent;
  } catch (e) {
    error.value = e.response?.data?.error?.message || "Failed to load requests";
  } finally {
    loading.value = false;
  }
}

async function fetchBlocked() {
  loading.value = true;
  error.value = null;
  try {
    const { data } = await api.get("/friends/blocked");
    blocked.value = data.blocked;
  } catch (e) {
    error.value =
      e.response?.data?.error?.message || "Failed to load blocked users";
  } finally {
    loading.value = false;
  }
}

async function sendRequest() {
  try {
    await api.post("/friends/requests", { userId: Number(newFriendId.value) });
    newFriendId.value = "";
    await fetchRequests();
    activeTab.value = "requests";
  } catch (e) {
    error.value = e.response?.data?.error?.message || "Failed to send request";
  }
}

async function searchUsers(searchChanged = false) {
  searchingUsers.value = true;
  error.value = null;

  let sort = { createdAt: "desc" };

  if (searchChanged === true) {
    const searchValue = userSearch.value.trim();
    let filter = {};
    if (searchValue) {
      filter.username = searchValue;
    }
    usersFetcher.value.updateParams({ filter });
  }

  try {
    usersFetcher.value.updateParams({
      sort,
      additionalParams: { excludeUserId: authStore.user?.id },
    });
    const { data } = await usersFetcher.value.fetch("/users");
    searchResults.value = (data.users || []).filter(
      (user) => Number(user.id) !== Number(authStore.user?.id),
    );
  } catch (e) {
    error.value = e.response?.data?.error?.message || "Search failed";
  } finally {
    searchingUsers.value = false;
  }
}

async function sendRequestToUser(userId) {
  if (Number(userId) === Number(authStore.user?.id)) {
    error.value = "Cannot friend yourself";
    return;
  }
  try {
    await api.post("/friends/requests", { userId });
    // requestedIds.value = new Set([...requestedIds.value, Number(userId)]);
  } catch (e) {
    if (e.response?.status === 403) {
      error.value =
        e.response?.data?.error?.message ||
        "You cannot send a friend request to this user";
    } else {
      error.value =
        e.response?.data?.error?.message || "Failed to send request";
    }
  }
}

async function acceptRequest(id) {
  try {
    await api.put(`/friends/requests/${id}/accept`);
    await Promise.all([fetchFriends(), fetchRequests()]);
    activeTab.value = "friends";
  } catch (e) {
    error.value =
      e.response?.data?.error?.message || "Failed to accept request";
  }
}

async function declineRequest(id) {
  try {
    await api.put(`/friends/requests/${id}/decline`);
    await fetchRequests();
  } catch (e) {
    error.value =
      e.response?.data?.error?.message || "Failed to decline request";
  }
}

async function blockFromRequest(requestId, senderId) {
  try {
    await api.put(`/friends/requests/${requestId}/decline`);
    await api.post("/friends/block", { userId: senderId });
    await Promise.all([fetchRequests(), fetchBlocked()]);
    activeTab.value = "blocked";
  } catch (e) {
    error.value = e.response?.data?.error?.message || "Failed to block user";
  }
}

async function removeFriend(id) {
  try {
    await api.delete(`/friends/${id}`);
    await fetchFriends();
  } catch (e) {
    error.value = e.response?.data?.error?.message || "Failed to remove friend";
  }
}

async function blockUser(id) {
  try {
    await api.post("/friends/block", { userId: id });
    await Promise.all([fetchFriends(), fetchBlocked()]);
  } catch (e) {
    error.value = e.response?.data?.error?.message || "Failed to block user";
  }
}

async function unblockUser(id) {
  try {
    await api.delete(`/friends/block/${id}`);
    await fetchBlocked();
  } catch (e) {
    error.value = e.response?.data?.error?.message || "Failed to unblock user";
  }
}

function loadTab(tab) {
  if (tab === "friends") fetchFriends();
  if (tab === "requests") fetchRequests();
  if (tab === "blocked") fetchBlocked();
}

watch(activeTab, loadTab);
onMounted(() => {
  fetchFriends();
  fetchRequests();
  fetchBlocked();
});
</script>

<style src="../styles/views/Friends.css" scoped></style>
