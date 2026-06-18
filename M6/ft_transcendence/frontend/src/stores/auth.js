import { defineStore } from "pinia";
import { ref, computed } from "vue";
import api from "../services/api.js";
import { devError } from "../services/logger.js";
import { disconnectSocket } from "../services/socket.js";

export const useAuthStore = defineStore("auth", () => {
  // ── State ───────────────────────────────────────────────────
  const user = ref(null);
  const loading = ref(false);
  const error = ref(null);

  // ── Getters ─────────────────────────────────────────────────
  const isAuthenticated = computed(() => !!user.value);
  const username = computed(() => user.value?.username ?? "");

  // ── Actions ─────────────────────────────────────────────────

  /**
   * Register a new account.
   */
  async function register({ username, email, password }) {
    loading.value = true;
    error.value = null;
    try {
      const { data } = await api.post(
        "/auth/register",
        {
          username,
          email,
          password,
        },
        { retryOnAuth: false },
      );
      user.value = data.user;
      return data;
    } catch (err) {
      // HttpError carries the parsed JSON body on `err.data` (not on `err.response`,
      // which is the raw Fetch Response). Read the backend's error message there.
      error.value = err.data?.error?.message || "Registration failed";
      throw err;
    } finally {
      loading.value = false;
    }
  }

  /**
   * Log in with username/email + password.
   */
  async function login({ username, password }) {
    loading.value = true;
    error.value = null;
    try {
      const { data } = await api.post(
        "/auth/login",
        { username, password },
        { retryOnAuth: false },
      );
      user.value = data.user;
      return data;
    } catch (err) {
      error.value = err.data?.error?.message || "Login failed";
      throw err;
    } finally {
      loading.value = false;
    }
  }

  /**
   * Log out — clears tokens and user state.
   */
  async function logout() {
    try {
      disconnectSocket();
      await api.post("/auth/logout");
    } catch (e) {
      // Ignore errors on logout — clear local state regardless.
      devError(e);
    } finally {
      user.value = null;
    }
  }

  /**
   * Fetch the current user from /auth/me (used on app init to restore session).
   */
  async function fetchUser() {
    loading.value = true;
    try {
      const { data } = await api.get("/auth/me", { retryOnAuth: false });
      if (data.user)
        user.value = data.user;
    } catch (e) {
      // 401 here just means "no valid session" — expected on first load.
      user.value = null;
    } finally {
      loading.value = false;
    }
  }

  /**
   * Update the current user's profile.
   */
  async function updateProfile(fields) {
    loading.value = true;
    error.value = null;
    try {
      const { data } = await api.put("/users/me", fields);
      user.value = data.user;
      return data.user;
    } catch (err) {
      error.value = err.data?.error?.message || "Update failed";
      throw err;
    } finally {
      loading.value = false;
    }
  }

  return {
    // state
    user,
    loading,
    error,
    // getters
    isAuthenticated,
    username,
    // actions
    register,
    login,
    logout,
    fetchUser,
    updateProfile,
  };
});
