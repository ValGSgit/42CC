import { createApp } from 'vue'
import { createPinia } from 'pinia'
import App from './App.vue'
import { useAuthStore } from './stores/auth.js'
import './styles/style.css'

import router from './router/index.js'

const app = createApp(App)
const pinia = createPinia()
app.use(pinia)

;(async () => {
  const authStore = useAuthStore()
  await authStore.fetchUser()
  app.use(router)
  app.mount('#app')
})()
