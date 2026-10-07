<script setup lang="ts">
import { computed, onMounted, ref } from 'vue'
import { Copy, Dices, RotateCcw } from '@lucide/vue'

type NameKind = 'generic' | 'river' | 'ocean-current'
type NameProfile =
  | 'generic'
  | 'quenya-inspired'
  | 'sindarin-inspired'
  | 'english'
  | 'french'
  | 'german'
  | 'orcish'
  | 'gnomish'
  | 'infernal'
  | 'abyssal'
  | 'cthulhu-mythos-inspired'

interface NameResult {
  id: number
  name: string
  seed: string
  kind: NameKind
  profile: NameProfile
}

interface GeneratorResponse {
  names: string[]
  error?: string
}

const kinds: { value: NameKind; label: string }[] = [
  { value: 'generic', label: 'Generic' },
  { value: 'river', label: 'River' },
  { value: 'ocean-current', label: 'Ocean current' },
]

const profiles: { value: NameProfile; label: string; family: string }[] = [
  { value: 'generic', label: 'Generic', family: 'Foundational' },
  { value: 'quenya-inspired', label: 'Quenya-inspired', family: 'Elven' },
  { value: 'sindarin-inspired', label: 'Sindarin-inspired', family: 'Elven' },
  { value: 'english', label: 'English', family: 'Language' },
  { value: 'french', label: 'French', family: 'Language' },
  { value: 'german', label: 'German', family: 'Language' },
  { value: 'orcish', label: 'Orcish', family: 'Fantasy' },
  { value: 'gnomish', label: 'Gnomish', family: 'Fantasy' },
  { value: 'infernal', label: 'Infernal', family: 'Otherworldly' },
  { value: 'abyssal', label: 'Abyssal', family: 'Otherworldly' },
  { value: 'cthulhu-mythos-inspired', label: 'Cthulhu Mythos-inspired', family: 'Otherworldly' },
]

const seed = ref('23')
const kind = ref<NameKind>('river')
const profile = ref<NameProfile>('orcish')
const batchSize = ref(8)
const results = ref<NameResult[]>([])
const isGenerating = ref(false)
const errorMessage = ref('')
const copiedId = ref<number | null>(null)
let nextResultId = 0

const selectedProfile = computed(() => profiles.find((item) => item.value === profile.value)!)
const latest = computed(() => results.value[0])
const latestKind = computed(() => kinds.find((item) => item.value === latest.value?.kind)?.label ?? '')
const latestProfile = computed(() => profiles.find((item) => item.value === latest.value?.profile)?.label ?? '')

async function generateNames() {
  errorMessage.value = ''
  copiedId.value = null

  if (!/^\d+$/.test(seed.value) || BigInt(seed.value) > 18446744073709551615n) {
    errorMessage.value = 'Enter a whole-number seed from 0 to 18446744073709551615.'
    return
  }

  isGenerating.value = true
  try {
    const params = new URLSearchParams({
      seed: seed.value,
      kind: kind.value,
      profile: profile.value,
      count: String(batchSize.value),
    })
    const response = await fetch(`/api/generate?${params}`)
    const payload = (await response.json()) as GeneratorResponse
    if (!response.ok) throw new Error(payload.error ?? 'Name generation failed.')

    const firstSeed = BigInt(seed.value)
    const batch = payload.names.map((name, index) => ({
      id: nextResultId++,
      name,
      seed: String((firstSeed + BigInt(index)) & 18446744073709551615n),
      kind: kind.value,
      profile: profile.value,
    }))
    results.value = [...batch, ...results.value].slice(0, 24)
  } catch (error) {
    errorMessage.value = error instanceof Error ? error.message : 'Name generation failed.'
  } finally {
    isGenerating.value = false
  }
}

function randomizeSeed() {
  const words = new Uint32Array(2)
  crypto.getRandomValues(words)
  seed.value = String((BigInt(words[0]!) << 32n) | BigInt(words[1]!))
}

async function copyName(result: NameResult) {
  try {
    await navigator.clipboard.writeText(result.name)
    copiedId.value = result.id
    window.setTimeout(() => {
      if (copiedId.value === result.id) copiedId.value = null
    }, 1400)
  } catch {
    errorMessage.value = 'Clipboard access is unavailable in this browser.'
  }
}

onMounted(generateNames)
</script>

<template>
  <main class="workbench">
    <header class="topbar">
      <a class="wordmark" href="#top" aria-label="Name Generator home">
        <span class="wordmark-mark">N<span>G</span></span>
        <span>NAME<br />GENERATOR</span>
      </a>
      <div class="engine-status"><span class="status-dot"></span> C++ ENGINE <span class="status-divider">/</span> LOCAL</div>
    </header>

    <section id="top" class="title-row">
      <div>
        <p class="eyebrow">PROCEDURAL ONOMASTICS <span>/</span> DEMO 01</p>
        <h1>Name<span class="title-slash">/</span>foundry</h1>
      </div>
      <div class="edition-mark" aria-hidden="true"><span>FIELD</span><strong>NG-01</strong><span>SEED STUDY</span></div>
    </section>

    <div class="rule-heading"><span>GENERATOR</span><span>DETERMINISTIC OUTPUT</span></div>

    <section class="generator-layout" aria-label="Name generator">
      <aside class="controls">
        <div class="section-title"><span class="section-index">01</span><h2>Parameters</h2></div>

        <label class="field-label" for="seed">Seed</label>
        <div class="seed-control">
          <input id="seed" v-model="seed" inputmode="numeric" autocomplete="off" spellcheck="false" />
          <button class="icon-button seed-button" type="button" aria-label="Choose a random seed" title="Choose a random seed" @click="randomizeSeed">
            <Dices :size="17" :stroke-width="1.8" />
          </button>
        </div>

        <label class="field-label" for="kind">Name kind</label>
        <div class="select-wrap">
          <select id="kind" v-model="kind">
            <option v-for="item in kinds" :key="item.value" :value="item.value">{{ item.label }}</option>
          </select>
          <span class="select-caret" aria-hidden="true"></span>
        </div>

        <label class="field-label" for="profile">Sound profile</label>
        <div class="select-wrap">
          <select id="profile" v-model="profile">
            <option v-for="item in profiles" :key="item.value" :value="item.value">{{ item.label }}</option>
          </select>
          <span class="select-caret" aria-hidden="true"></span>
        </div>
        <div class="profile-family"><span>{{ selectedProfile.family }}</span><span>{{ selectedProfile.label }}</span></div>

        <div class="field-label batch-label"><label for="batch-size">Names per batch</label><output>{{ batchSize }}</output></div>
        <input id="batch-size" v-model.number="batchSize" class="batch-range" type="range" min="1" max="24" />
        <div class="range-labels"><span>01</span><span>24</span></div>

        <div class="control-actions">
          <button class="generate-button" type="button" :disabled="isGenerating" @click="generateNames">
            <Dices :size="17" :stroke-width="1.8" />
            {{ isGenerating ? 'Generating' : 'Generate names' }}
          </button>
          <button class="icon-button reset-button" type="button" aria-label="Reset parameters" title="Reset parameters" @click="seed = '23'; kind = 'river'; profile = 'orcish'; batchSize = 8">
            <RotateCcw :size="16" :stroke-width="1.8" />
          </button>
        </div>
        <p v-if="errorMessage" class="error-message" role="alert">{{ errorMessage }}</p>

        <div class="parameter-note"><span>INPUT SIGNATURE</span><code>{{ kind }} · {{ profile }}</code></div>
      </aside>

      <section class="results" aria-live="polite" aria-label="Generated names">
        <div class="results-topline">
          <div class="section-title"><span class="section-index">02</span><h2>Specimens</h2></div>
          <span class="result-count">{{ results.length.toString().padStart(2, '0') }} <i>/ 24</i></span>
        </div>

        <div v-if="latest" class="feature-result">
          <div class="feature-meta"><span>NEWEST SPECIMEN</span><span>SEED {{ latest.seed }}</span></div>
          <p class="feature-name">{{ latest.name }}</p>
          <div class="feature-foot"><span>{{ latestKind }} <b>x</b> {{ latestProfile }}</span><span class="feature-serial">001-{{ latest.seed.slice(-4).padStart(4, '0') }}</span></div>
        </div>
        <div v-else class="empty-result"><span>NG-</span><p>Your specimens will appear here.</p></div>

        <div class="list-heading"><span>RECENT OUTPUT</span><span>SEED / NAME</span></div>
        <ol class="result-list">
          <li v-for="(result, index) in results" :key="result.id" class="result-row">
            <span class="row-index">{{ String(index + 1).padStart(2, '0') }}</span>
            <span class="row-seed">{{ result.seed }}</span>
            <span class="row-name">{{ result.name }}</span>
            <button class="icon-button copy-button" type="button" :aria-label="`Copy ${result.name}`" :title="copiedId === result.id ? 'Copied' : 'Copy name'" @click="copyName(result)">
              <span v-if="copiedId === result.id" class="copied-mark">OK</span>
              <Copy v-else :size="15" :stroke-width="1.8" />
            </button>
          </li>
        </ol>
      </section>
    </section>

    <footer class="footer-line"><span>NAME GENERATOR <b>/</b> C++20</span><span>REPEATABLE BY SEED <span class="footer-star">*</span></span></footer>
  </main>
</template>
