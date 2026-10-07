import vue from '@vitejs/plugin-vue'
import { defineConfig } from 'vite'

const pagesBasePath = process.env.VITE_BASE_PATH

export default defineConfig({
  base: pagesBasePath ? `${pagesBasePath.replace(/\/+$/, '')}/` : '/',
  plugins: [vue()],
})
