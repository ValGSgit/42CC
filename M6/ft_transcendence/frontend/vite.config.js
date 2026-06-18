/**
 * Vite Configuration
 */
import { fileURLToPath, URL } from 'node:url'
import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'

export default defineConfig({
  base: process.env.VITE_BASE_PATH || '/',
  plugins: [vue()],
  build: {
    chunkSizeWarningLimit: 700,
  },
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url)),
    },
  },
  server: {
    allowedHosts: ['localhost', '10.13.10.7.nip.io'],
    host: true,
    port: 5173,
    watch: {
      usePolling: true,
    },
    hmr: {
      path: '/ws',
    },
    proxy: {
      '/api': {
        target: 'https://localhost:3000',
        changeOrigin: true,
        secure: false,
      },
      '/uploads': {
        target: 'https://localhost:3000',
        changeOrigin: true,
        secure: false,
      },
      '/socket.io': {
        target: 'https://localhost:3000',
        changeOrigin: true,
        ws: true,
        secure: false,
      },
    },
  },
})
