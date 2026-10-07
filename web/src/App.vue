<script setup lang="ts">
import { computed, onMounted, ref } from 'vue'
import { Copy, Dices, RotateCcw } from '@lucide/vue'

type NameKind = 'generic' | 'river' | 'ocean-current' | 'person' | 'mountain' | 'house' | 'sword'
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
  | 'fae-inspired'

interface NameResult {
  id: number
  name: string
  seed: string
  kind: NameKind
  profile: NameProfile
}

interface WasmGenerator {
  generateName(seed: string, kind: number, profile: number): string
}

type WasmFactory = (options?: { locateFile?: (path: string) => string }) => Promise<WasmGenerator>

const kinds: { value: NameKind; label: string; code: number }[] = [
  { value: 'generic', label: 'Generic', code: 0 },
  { value: 'river', label: 'River', code: 1 },
  { value: 'ocean-current', label: 'Ocean current', code: 2 },
  { value: 'person', label: 'Person', code: 3 },
  { value: 'mountain', label: 'Mountain', code: 4 },
  { value: 'house', label: 'House', code: 5 },
  { value: 'sword', label: 'Sword', code: 6 },
]

const profiles: { value: NameProfile; label: string; family: string; code: number }[] = [
  { value: 'generic', label: 'Generic', family: 'Foundational', code: 0 },
  { value: 'quenya-inspired', label: 'Quenya-inspired', family: 'Elven', code: 1 },
  { value: 'sindarin-inspired', label: 'Sindarin-inspired', family: 'Elven', code: 2 },
  { value: 'english', label: 'English', family: 'Language', code: 3 },
  { value: 'french', label: 'French', family: 'Language', code: 4 },
  { value: 'german', label: 'German', family: 'Language', code: 5 },
  { value: 'orcish', label: 'Orcish', family: 'Fantasy', code: 6 },
  { value: 'gnomish', label: 'Gnomish', family: 'Fantasy', code: 7 },
  { value: 'infernal', label: 'Infernal', family: 'Otherworldly', code: 8 },
  { value: 'abyssal', label: 'Abyssal', family: 'Otherworldly', code: 9 },
  { value: 'cthulhu-mythos-inspired', label: 'Cthulhu Mythos-inspired', family: 'Otherworldly', code: 10 },
  { value: 'fae-inspired', label: 'Fae-inspired', family: 'Otherworldly', code: 11 },
]

const maxSeed = 18446744073709551615n
const seed = ref('23')
const kind = ref<NameKind>('river')
const profile = ref<NameProfile>('orcish')
const batchSize = ref(8)
const results = ref<NameResult[]>([])
const isGenerating = ref(false)
const errorMessage = ref('')
const copiedId = ref<number | null>(null)
let nextResultId = 0
let wasmGenerator: Promise<WasmGenerator> | undefined

const selectedKind = computed(() => kinds.find((item) => item.value === kind.value)!)
const selectedProfile = computed(() => profiles.find((item) => item.value === profile.value)!)
const latest = computed(() => results.value[0])
const latestKind = computed(() => kinds.find((item) => item.value === latest.value?.kind)?.label ?? '')
const latestProfile = computed(() => profiles.find((item) => item.value === latest.value?.profile)?.label ?? '')

async function getWasmGenerator() {
  if (!wasmGenerator) {
    wasmGenerator = (async () => {
      const moduleUrl = `${import.meta.env.BASE_URL}wasm/name_generator.js`
      const response = await fetch(moduleUrl)
      if (!response.ok) throw new Error('Could not load the WebAssembly module.')
      const source = await response.text()
      const blobUrl = URL.createObjectURL(new Blob([source], { type: 'text/javascript' }))
      try {
        const { default: createGenerator } = await import(/* @vite-ignore */ blobUrl) as { default: WasmFactory }
        return await createGenerator({
          locateFile: (path) => `${import.meta.env.BASE_URL}wasm/${path}`,
        })
      } finally {
        URL.revokeObjectURL(blobUrl)
      }
    })()
  }
  try {
    return await wasmGenerator
  } catch (error) {
    wasmGenerator = undefined
    throw error
  }
}

async function generateNames() {
  errorMessage.value = ''
  copiedId.value = null

  if (!/^\d+$/.test(seed.value) || BigInt(seed.value) > 18446744073709551615n) {
    errorMessage.value = 'Enter a whole-number seed from 0 to 18446744073709551615.'
    return
  }

  isGenerating.value = true
  try {
    const generator = await getWasmGenerator()
    const firstSeed = BigInt(seed.value)
    const batch = Array.from({ length: batchSize.value }, (_, index) => {
      const itemSeed = (firstSeed + BigInt(index)) & maxSeed
      return {
      id: nextResultId++,
      name: generator.generateName(String(itemSeed), selectedKind.value.code, selectedProfile.value.code),
      seed: String(itemSeed),
      kind: kind.value,
      profile: profile.value,
      }
    })
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
      <div class="engine-status"><span class="status-dot"></span> C++ <span class="status-divider">/</span> WEBASSEMBLY</div>
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

    <section class="reference-section" aria-labelledby="about-heading">
      <div class="reference-heading">
        <div class="section-title"><span class="section-index">03</span><h2 id="about-heading">About &amp; sources</h2></div>
        <p class="reference-intro">This is a small, dependency-free C++20 name generator and web demo built for fun with GitHub Copilot, to test its capabilities and learn about AI agent development. It generates deterministic names from a seed, a name kind, and a broad sound profile. Please try it out and share feedback by <a href="https://github.com/cyberpunk42/NameGenerator/issues" target="_blank" rel="noreferrer">creating an issue in the GitHub repository</a>.</p>
        <p class="reference-intro">The Quenya- and Sindarin-inspired profiles use invented syllable combinations informed by phonological descriptions; they are not translations. English, French, and German use broad sound patterns informed by the references below. Orcish, Gnomish, Infernal, and Abyssal are original, genre-inspired profiles, not representations of a particular published language. The Cthulhu Mythos profile recombines fragments adapted from Mythos nomenclature and can produce recognizable canonical names. The Fae-inspired profile uses original syllable combinations informed by historical Celtic fair-folk traditions; it does not represent a single language or copy names from modern fiction.</p>
      </div>

      <div class="reference-content">
        <article class="reference-group">
          <h3>Phonological references</h3>
          <ul class="source-list">
            <li><cite>Tolkien, J. R. R. The Lord of the Rings.</cite> Appendices E and F. George Allen &amp; Unwin, 1954-1955. These profiles draw on broad sound descriptions; they do not implement either language.</li>
            <li><cite>Roach, Peter. English Phonetics and Phonology: A Practical Course.</cite> 4th ed. Cambridge University Press, 2009.</li>
            <li><cite>Tranel, Bernard. The Sounds of French: An Introduction.</cite> Cambridge University Press, 1987.</li>
            <li><cite>Wiese, Richard. The Phonology of German.</cite> Clarendon Press, 1996.</li>
            <li><cite>Keightley, Thomas. The Fairy Mythology: Illustrative of the Romance and Superstition of Various Countries.</cite> Revised and enlarged edition, 1870. <a href="https://www.gutenberg.org/ebooks/41006" target="_blank" rel="noreferrer">Project Gutenberg eBook 41006</a>.</li>
            <li><cite>Evans-Wentz, W. Y. The Fairy-Faith in Celtic Countries.</cite> 1911. <a href="https://www.gutenberg.org/ebooks/34853" target="_blank" rel="noreferrer">Project Gutenberg eBook 34853</a>.</li>
          </ul>
          <p class="source-note">These works inform general phonological tendencies only. The syllable inventories are original combinations, not copied examples or a substitute for linguistic analysis. The folklore references provide broad cultural context, not a universal system of Fae names.</p>
        </article>

        <article class="reference-group">
          <h3>Cthulhu Mythos references</h3>
          <ul class="source-list">
            <li><cite>Lovecraft, H. P. “The Call of Cthulhu.” Weird Tales, February 1928.</cite> <a href="https://www.hplovecraft.com/writings/texts/fiction/cc.aspx" target="_blank" rel="noreferrer">Read the story</a>.</li>
            <li><cite>Lovecraft, H. P. “The Dunwich Horror.” Weird Tales, April 1929.</cite> <a href="https://www.hplovecraft.com/writings/texts/fiction/dh.aspx" target="_blank" rel="noreferrer">Read the story</a>.</li>
            <li><cite>Lovecraft, H. P. “The Whisperer in Darkness.” Weird Tales, August 1931.</cite> <a href="https://www.hplovecraft.com/writings/texts/fiction/wid.aspx" target="_blank" rel="noreferrer">Read the story</a>.</li>
          </ul>
          <p class="source-note">These stories inform the Mythos profile’s fragment choices. That profile is an homage for procedural naming, not an attempt to extend or define Mythos canon.</p>
        </article>
      </div>
    </section>

    <footer class="footer-line"><span>NAME GENERATOR <b>/</b> C++20</span><span>REPEATABLE BY SEED <span class="footer-star">*</span></span></footer>
  </main>
</template>
