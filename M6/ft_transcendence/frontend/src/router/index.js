import { createRouter, createWebHashHistory } from 'vue-router'
import { useAuthStore } from '../stores/auth.js'

const AuthV3   = () => import('../views/AuthV3.vue')
const Profile  = () => import('../views/Profile.vue')
const UserProfile = () => import('../views/UserProfile.vue')
const Friends  = () => import('../views/Friends.vue')
const Game     = () => import('../games/Game.vue')
const Feed     = () => import('../views/Feed.vue')
const NotFound = () => import('../views/NotFound.vue')
const PrivacyPolicy  = () => import('../views/PrivacyPolicy.vue')
const TermsOfService = () => import('../views/TermsOfService.vue')
const Help     = () => import('../views/Help.vue')

const routes = [
  {
    path: '/',
    name: 'Home',
    component: Game,
    meta: { requiresAuth: false },
  },
  {
    path: '/login',
    name: 'Login',
    component: AuthV3,
    meta: { requiresAuth: false, guestOnly: true },
  },
  {
    path: '/register',
    name: 'Register',
    component: AuthV3,
    meta: { requiresAuth: false, guestOnly: true },
  },
  {
    path: '/settings',
    redirect: { name: 'Profile', query: { tab: 'settings' } },
    meta: { requiresAuth: true },
  },
  {
    path: '/profile',
    name: 'Profile',
    component: Profile,
    meta: { requiresAuth: true },
  },
  {
    path: '/friends',
    name: 'Friends',
    component: Friends,
    meta: { requiresAuth: true },
  },
  {
    path: '/game',
    name: 'Game',
    component: Game,
    meta: { requiresAuth: false },
  },
  {
    path: '/feed',
    name: 'Feed',
    component: Feed,
    meta: { requiresAuth: true },
  },
  {
    path: '/help',
    name: 'Help',
    component: Help,
    meta: { requiresAuth: false },
  },
  {
    path: '/privacy',
    name: 'PrivacyPolicy',
    component: PrivacyPolicy,
    meta: { requiresAuth: false },
  },
  {
    path: '/terms',
    name: 'TermsOfService',
    component: TermsOfService,
    meta: { requiresAuth: false },
  },
  {
    path: '/users/:id',
    name: 'UserProfile',
    component: UserProfile,
    meta: { requiresAuth: false },
  },
  {
    path: '/:pathMatch(.*)*',
    name: 'NotFound',
    component: NotFound,
    meta: { requiresAuth: false },
  },
]

const router = createRouter({
  history: createWebHashHistory(),
  routes,
})

// Navigation guard — redirect to login if route requires auth.
//
// Session restore (calling /auth/me on a cold load so a still-valid cookie
// rehydrates the store) lives in main.js, BEFORE the router is mounted
// previously there was a `if (isAuthenticated && !user) fetchUser()` block
// here, but isAuthenticated is derived as `!!user`, so the condition was
// logically unreachable and only confused readers debugging session bugs.
router.beforeEach(async (to) => {
  const authStore = useAuthStore()

  if (to.meta.requiresAuth && !authStore.isAuthenticated) {
    return { name: 'Login', query: { redirect: to.fullPath } }
  }

  if (to.meta.guestOnly && authStore.isAuthenticated) {
    return { name: 'Home' }
  }
})

export default router
