import { execFile } from 'node:child_process'
import type { IncomingMessage, ServerResponse } from 'node:http'
import { dirname, resolve } from 'node:path'
import { fileURLToPath } from 'node:url'
import vue from '@vitejs/plugin-vue'
import { defineConfig, type Plugin } from 'vite'

const projectRoot = resolve(dirname(fileURLToPath(import.meta.url)), '..')
const executable = resolve(
  projectRoot,
  'build',
  'release',
  process.platform === 'win32' ? 'name_generator_cli.exe' : 'name_generator_cli',
)
const validKinds = new Set(['generic', 'river', 'ocean-current'])
const validProfiles = new Set([
  'generic', 'quenya-inspired', 'sindarin-inspired', 'english', 'french', 'german',
  'orcish', 'gnomish', 'infernal', 'abyssal', 'cthulhu-mythos-inspired',
])
const maxSeed = 18446744073709551615n

function sendJson(response: ServerResponse, status: number, payload: object) {
  response.statusCode = status
  response.setHeader('Content-Type', 'application/json; charset=utf-8')
  response.end(JSON.stringify(payload))
}

function generateRequest(request: IncomingMessage, response: ServerResponse) {
  if (request.method !== 'GET') {
    sendJson(response, 405, { error: 'Use GET to request generated names.' })
    return
  }

  const url = new URL(request.url ?? '/', 'http://localhost')
  const seed = url.searchParams.get('seed') ?? ''
  const kind = url.searchParams.get('kind') ?? ''
  const profile = url.searchParams.get('profile') ?? ''
  const count = Number(url.searchParams.get('count'))
  if (!/^\d+$/.test(seed) || BigInt(seed) > maxSeed || !validKinds.has(kind)
      || !validProfiles.has(profile) || !Number.isInteger(count) || count < 1 || count > 24) {
    sendJson(response, 400, { error: 'Check the seed, kind, profile, and batch size.' })
    return
  }

  execFile(executable, [seed, kind, profile, String(count)], { windowsHide: true }, (error, stdout) => {
    if (error) {
      const missingExecutable = (error as NodeJS.ErrnoException).code === 'ENOENT'
      sendJson(response, missingExecutable ? 503 : 500, {
        error: missingExecutable
          ? 'Build the name_generator_cli target with the release CMake preset, then retry.'
          : 'The C++ name generator could not complete this request.',
      })
      return
    }
    sendJson(response, 200, { names: stdout.trimEnd().split(/\r?\n/) })
  })
}

const generatorApi: Plugin = {
  name: 'name-generator-api',
  configureServer(server) {
    server.middlewares.use('/api/generate', generateRequest)
  },
  configurePreviewServer(server) {
    server.middlewares.use('/api/generate', generateRequest)
  },
}

export default defineConfig({
  plugins: [vue(), generatorApi],
})
