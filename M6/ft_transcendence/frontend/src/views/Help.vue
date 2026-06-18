<script setup>
import { ref, computed, onMounted, onBeforeUnmount, nextTick, watch } from 'vue'

/* ---------- content ---------- */
const sections = [
  {
    id: 'getting-started', tone: 'cyan', n: '01',
    title: 'Getting started',
    blurb: 'Sign up, pick your alpaca\'s color, and step onto the farm in under a minute.',
  },
  {
    id: 'playing-games', tone: 'magenta', n: '02',
    title: 'Playing the games',
    blurb: 'Spit Royale and Alpaca Road — how matchmaking, scoring, and the public lobby work.',
  },
  {
    id: 'account-privacy', tone: 'gold', n: '03',
    title: 'Account & privacy',
    blurb: 'Data exports, GDPR rights, and what we remember about you.',
  },
  {
    id: 'public-api', tone: 'green', n: '04',
    title: 'Public API',
    blurb: 'REST endpoints, the X-API-Key header, rate limits, and how to generate a key.',
  },
  {
    id: 'contact', tone: 'cyan', n: '05',
    title: 'Contact a human',
    blurb: 'When Paca shrugs — how to reach a moderator or engineer who isn\'t a language model.',
  },
]

/* ---------- TOC + scroll spy ---------- */
const activeId = ref(sections[0].id)
const showToTop = ref(false)
const flashId = ref(null)
const query = ref('')
let observer = null
let flashTimer = null

const filtered = computed(() => {
  const q = query.value.trim().toLowerCase()
  if (!q) return sections
  return sections.filter(s => (s.title + ' ' + s.blurb).toLowerCase().includes(q))
})
const hasResults = computed(() => filtered.value.length > 0)

function jumpTo(id) {
  const el = document.getElementById(id)
  if (!el) return
  el.scrollIntoView({ behavior: 'smooth', block: 'start' })
  flashId.value = id
  clearTimeout(flashTimer)
  flashTimer = setTimeout(() => { flashId.value = null }, 1400)
}
function scrollToTop() { window.scrollTo({ top: 0, behavior: 'smooth' }) }
function onScroll() { showToTop.value = window.scrollY > 600 }

async function rewireObserver() {
  observer && observer.disconnect()
  await nextTick()
  observer = new IntersectionObserver(
    (entries) => {
      const v = entries.filter(e => e.isIntersecting)
        .sort((a, b) => a.boundingClientRect.top - b.boundingClientRect.top)
      if (v[0]) activeId.value = v[0].target.id
    },
    { rootMargin: '-25% 0px -65% 0px', threshold: [0, 1] }
  )
  document.querySelectorAll('.doc-content section[id]').forEach(s => observer.observe(s))
}

onMounted(async () => {
  await rewireObserver()
  window.addEventListener('scroll', onScroll, { passive: true })
})
onBeforeUnmount(() => {
  observer && observer.disconnect()
  clearTimeout(flashTimer)
  window.removeEventListener('scroll', onScroll)
})
watch(query, () => rewireObserver())
</script>

<template>
  <main class="doc-page doc-page--help">
    <!-- ============ HERO ============ -->
    <header class="hero" aria-labelledby="help-title">
      <div class="hero__bg" aria-hidden="true">
        <span class="hero__grid"></span>
        <span class="hero__glow hero__glow--cyan"></span>
        <span class="hero__glow hero__glow--magenta"></span>
      </div>

      <div class="hero__inner">
        <!-- Paca avatar orb -->
        <div class="paca" aria-hidden="true">
          <span class="paca__ring paca__ring--3"></span>
          <span class="paca__ring paca__ring--2"></span>
          <span class="paca__ring paca__ring--1"></span>
          <svg class="paca__face" viewBox="0 0 80 80" fill="none">
            <defs>
              <radialGradient id="pacaCore" cx="50%" cy="40%" r="60%">
                <stop offset="0%" stop-color="#00f0ff" stop-opacity="0.9"/>
                <stop offset="100%" stop-color="#b744ff" stop-opacity="0.15"/>
              </radialGradient>
            </defs>
            <circle cx="40" cy="40" r="22" fill="url(#pacaCore)"/>
            <!-- abstract alpaca silhouette -->
            <path
              d="M28 50V42c0-2 1-3 2-4-1.5-1-1.5-3 0-4 1-1 3-1 4 0 0-3 2-5 5-5 2 0 3 1 4 3 0-2 2-3 4-3 3 0 5 2 5 5 0 1 0 2-.5 3 1.5.5 2 2 1.5 3.5-.5 1.5-2 2-3 1.5 0 2 0 4-1 5-1 1-2 1.5-2 3"
              stroke="currentColor" stroke-width="1.2" stroke-linecap="round" stroke-linejoin="round"
              fill="rgba(232,232,240,0.04)" color="#e8e8f0" opacity="0.85"/>
            <circle cx="45" cy="36" r="1.4" fill="#0a0a0f"/>
            <circle cx="38" cy="38" r="1.1" fill="#0a0a0f"/>
          </svg>
          <span class="paca__pulse" />
        </div>

        <div class="hero__copy">
          <span class="hero__eyebrow"><span class="dot dot--cyan"></span> Help center · Paca is online</span>
          <h1 id="help-title" class="hero__title">Need a hand on the farm?</h1>
          <p class="hero__lede">
            Browse the answers below, or just ask <strong>Paca</strong> —
            our AI farmhand — anything about AlpacaParty.
          </p>

          <div class="hero__actions">
            <a href="#contact" class="btn btn--ghost" @click.prevent="jumpTo('contact')">
              Talk to a human
            </a>
          </div>

          <form class="search" role="search" @submit.prevent>
            <svg viewBox="0 0 16 16" fill="none" aria-hidden="true">
              <circle cx="7" cy="7" r="4.5" stroke="currentColor" stroke-width="1.5"/>
              <path d="M10.5 10.5l3 3" stroke="currentColor" stroke-width="1.5" stroke-linecap="round"/>
            </svg>
            <input
              v-model="query"
              type="search"
              placeholder="Search the help center…"
              aria-label="Search the help center"
            />
            <kbd>⌘K</kbd>
          </form>
        </div>
      </div>
    </header>

    <!-- ============ SHELL ============ -->
    <div class="shell">
      <!-- TOC -->
      <aside class="toc" aria-label="On this page">
        <h2>On this page</h2>
        <ol>
          <li v-for="s in filtered" :key="s.id" :class="`tone-${s.tone}`">
            <a :href="'#' + s.id"
               :class="{ 'is-active': activeId === s.id }"
               @click.prevent="jumpTo(s.id)">
              <span class="toc__num">{{ s.n }}</span>
              <span class="toc__label">{{ s.title }}</span>
              <span class="toc__dot" aria-hidden="true"></span>
            </a>
          </li>
        </ol>
        <div class="toc__panel" aria-hidden="true">
          <p class="toc__panel-title">Still stuck?</p>
          <p class="toc__panel-body">Jump to “Contact a human” below to reach a moderator.</p>
        </div>
      </aside>

      <!-- mobile jump -->
      <div class="jump">
        <label for="jump-select">Jump to</label>
        <select id="jump-select" @change="(e) => { jumpTo(e.target.value); e.target.value='' }">
          <option value="" disabled selected>Choose a section…</option>
          <option v-for="s in filtered" :key="s.id" :value="s.id">{{ s.n }} · {{ s.title }}</option>
        </select>
      </div>

      <!-- ============ CONTENT ============ -->
      <article class="doc-content">
        <template v-if="hasResults">
          <section v-for="s in filtered" :key="s.id" :id="s.id"
                   :class="['sec', `tone-${s.tone}`, { 'is-flash': flashId === s.id }]">
            <div class="sec__head">
              <span class="sec__num">{{ s.n }}</span>
              <h2>{{ s.title }}<a class="anchor" :href="'#'+s.id" aria-label="Anchor link">#</a></h2>
            </div>
            <p class="sec__blurb">{{ s.blurb }}</p>

            <!-- per-section content -->
            <template v-if="s.id === 'getting-started'">
              <h3>Create your account</h3>
              <p>Register with a username, email, and password. You'll pick your alpaca's color on first launch — change it any time from <router-link to="/profile?tab=settings">Profile → Settings</router-link>.</p>
              <h3>Join your first room</h3>
              <p>From the home screen, browse the public room list or create a new one. Each game type has its own room — Spit Royale (up to 10 players) and Alpaca Road (up to 4 lanes).</p>

              <aside class="quote">
                <p>"The trick is to spit <em>before</em> they spit. Source: every Spit Royale champion ever."</p>
                <cite>— Paca</cite>
              </aside>
            </template>

            <template v-else-if="s.id === 'playing-games'">
              <h3>Spit Royale</h3>
              <p>Up to <strong>10 alpacas</strong> in a single arena. Last alpaca standing wins. Each spit has a <strong>0.5s cooldown</strong> and a server-validated <strong>12m range</strong>, so aiming matters more than spamming.</p>
              <h3>Alpaca Road</h3>
              <p>Up to <strong>4 players</strong> dodge obstacles across procedurally spawned lanes. Survive longer, climb the level — falling off ends the run.</p>
              <h3>Stats &amp; leaderboard</h3>
              <p>Wins, kills, and matches played are persisted to your profile and appear on the global leaderboard. Solo matches don't count toward ranked stats.</p>
            </template>

            <template v-else-if="s.id === 'account-privacy'">
              <div class="key">
                <span class="key__tag">Key takeaway</span>
                <p>Export or fully delete your account from <router-link to="/profile?tab=settings">Profile → Settings → Danger zone</router-link> — no email loop required.</p>
              </div>
              <p>We keep gameplay records as long as your account exists; deleting the account removes them. Full details live in our <router-link to="/privacy">Privacy Policy</router-link>.</p>
            </template>

            <template v-else-if="s.id === 'public-api'">
              <p>The REST API lives under <code>/api/public/*</code> on this host. Authenticate by sending an <code>X-API-Key</code> header — generate the key from <router-link to="/profile?tab=settings">Profile → Settings → Public API Key</router-link>. Keys start with <code>ap_</code> followed by a 32-character hex string.</p>
              <p>Default rate limit is <strong>30 requests / minute</strong>. The interactive Swagger UI is at <a href="/api/docs/public" target="_blank" rel="noopener">/api/docs/public</a>.</p>
            </template>

            <template v-else-if="s.id === 'contact'">
              <p>Paca handles most incoming questions in the in-app chat. For anything Paca can't solve — moderation escalations, security reports, GDPR requests — open the help desk and ask for a human handoff, or contact a project maintainer directly through the repository.</p>
            </template>
          </section>
        </template>

        <div v-else class="empty" role="status">
          <svg viewBox="0 0 32 32" fill="none" aria-hidden="true">
            <circle cx="14" cy="14" r="9" stroke="currentColor" stroke-width="1.5"/>
            <path d="M21 21l6 6" stroke="currentColor" stroke-width="1.5" stroke-linecap="round"/>
          </svg>
          <strong>No matching articles</strong>
          <p>Try different keywords, or <a href="#" @click.prevent="query=''">clear your search</a>.</p>
        </div>

        <!-- sign-off -->
        <footer class="signoff">
          <div class="signoff__mark" aria-hidden="true">🦙</div>
          <div>
            <p class="signoff__line">That's all we've got. If you find a gap — tell Paca.</p>
            <p class="signoff__meta">
              Last updated · May 27, 2026
              <span>·</span> <router-link to="/profile?tab=settings">your settings</router-link>
              <span>·</span> <a href="#" @click.prevent="scrollToTop">back to top</a>
            </p>
          </div>
        </footer>
      </article>
    </div>

    <button type="button" class="totop" :class="{ 'is-visible': showToTop }"
            aria-label="Back to top" @click="scrollToTop">
      <svg viewBox="0 0 16 16" fill="none" aria-hidden="true">
        <path d="M8 13V4M4 7.5L8 3.5l4 4" stroke="currentColor" stroke-width="1.6" stroke-linecap="round" stroke-linejoin="round"/>
      </svg>
    </button>
  </main>
</template>

<style scoped>
/* ---------- shared tokens ---------- */
.doc-page--help {
  --tone-cyan: var(--primary, #00f0ff);
  --tone-magenta: var(--magenta, #ff8ec4);
  --tone-gold: var(--gold, #f5c842);
  --tone-green: var(--green, #36e07a);

  --measure: 64ch;
  min-height: 100vh;
  background: var(--bg-primary, #0a0a0f);
  color: var(--text-primary, #e8e8f0);
  font-family: 'Inter', system-ui, sans-serif;
  font-size: 16px;
  line-height: 1.7;
}

.tone-cyan    { --tone: var(--tone-cyan); }
.tone-magenta { --tone: var(--tone-magenta); }
.tone-gold    { --tone: var(--tone-gold); }
.tone-green   { --tone: var(--tone-green); }

/* ---------- HERO ---------- */
.hero {
  position: relative;
  padding: 80px 24px 56px;
  isolation: isolate;
  overflow: hidden;
  border-bottom: 1px solid var(--border-color, #2a2a3a);
}
.hero__bg { position: absolute; inset: 0; z-index: -1; }
.hero__grid {
  position: absolute; inset: 0;
  background-image:
    linear-gradient(rgba(0,240,255,0.04) 1px, transparent 1px),
    linear-gradient(90deg, rgba(0,240,255,0.04) 1px, transparent 1px);
  background-size: 48px 48px;
  mask-image: radial-gradient(ellipse at 50% 0%, #000 30%, transparent 75%);
}
.hero__glow {
  position: absolute; width: 520px; height: 520px; border-radius: 50%;
  filter: blur(110px); opacity: 0.45; animation: drift 20s ease-in-out infinite alternate;
}
.hero__glow--cyan { top: -180px; left: 10%; background: radial-gradient(circle, var(--primary), transparent 70%); }
.hero__glow--magenta { top: -120px; right: 5%; background: radial-gradient(circle, var(--secondary, #b744ff), transparent 70%); animation-duration: 26s; }
@keyframes drift {
  0% { transform: translate3d(0,0,0); }
  100% { transform: translate3d(60px,-30px,0) scale(1.06); }
}

.hero__inner {
  max-width: 1200px; margin: 0 auto;
  display: grid; grid-template-columns: 1fr; gap: 40px;
  align-items: center;
}
@media (min-width: 880px) {
  .hero__inner { grid-template-columns: 280px 1fr; gap: 56px; }
}

.hero__copy { min-width: 0; }
.hero__eyebrow {
  display: inline-flex; align-items: center; gap: 8px;
  font-size: 12px; font-weight: 600; letter-spacing: 0.14em; text-transform: uppercase;
  color: var(--text-secondary, #a0a0b0);
  margin-bottom: 14px;
}
.dot { width: 8px; height: 8px; border-radius: 50%; box-shadow: 0 0 10px currentColor; }
.dot--cyan { background: var(--primary); color: var(--primary); animation: blink 2.4s ease-in-out infinite; }
@keyframes blink { 0%, 100% { opacity: 1; } 50% { opacity: 0.4; } }

.hero__title {
  font-family: 'Space Grotesk', 'Inter', sans-serif;
  font-size: clamp(2.2rem, 1.5rem + 3vw, 3.4rem);
  font-weight: 700;
  letter-spacing: -0.02em;
  line-height: 1.05;
  margin: 0 0 14px;
  background: linear-gradient(180deg, #fff 0%, #a0a0b0 140%);
  -webkit-background-clip: text;
  background-clip: text;
  color: transparent;
}
.hero__lede {
  margin: 0 0 24px; font-size: 1.0625rem; color: var(--text-secondary);
  max-width: 56ch;
}
.hero__lede strong { color: var(--primary); font-weight: 600; }

.hero__actions {
  display: flex; flex-wrap: wrap; gap: 10px; margin-bottom: 22px;
}

/* ---------- Paca orb ---------- */
.paca {
  position: relative; width: 240px; height: 240px;
  margin: 0 auto;
  display: grid; place-items: center;
  color: var(--primary);
}
.paca__ring {
  position: absolute; inset: 0; border-radius: 50%;
  border: 1px solid currentColor;
  opacity: 0.35;
  animation: orbit 14s linear infinite;
}
.paca__ring--2 { inset: 24px; opacity: 0.6; border-style: dashed; animation-duration: 10s; animation-direction: reverse; }
.paca__ring--3 { inset: -16px; opacity: 0.15; border-color: var(--secondary); animation-duration: 22s; }
.paca__face {
  width: 140px; height: 140px;
  filter: drop-shadow(0 0 24px rgba(0,240,255,0.45));
  color: var(--primary);
}
.paca__pulse {
  position: absolute; inset: 50px; border-radius: 50%;
  background: radial-gradient(circle, rgba(0,240,255,0.3), transparent 70%);
  animation: pulse 3.2s ease-in-out infinite;
}
@keyframes orbit { to { transform: rotate(360deg); } }
@keyframes pulse {
  0%, 100% { transform: scale(0.95); opacity: 0.6; }
  50% { transform: scale(1.08); opacity: 1; }
}

/* ---------- buttons ---------- */
.btn {
  display: inline-flex; align-items: center; gap: 8px;
  padding: 11px 18px; border-radius: 10px;
  font: inherit; font-size: 0.9375rem; font-weight: 600;
  cursor: pointer; text-decoration: none;
  border: 1px solid transparent;
  transition: transform 120ms ease, box-shadow 180ms ease, background 120ms ease, color 120ms ease, border-color 120ms ease;
}
.btn--small { padding: 8px 14px; font-size: 0.8125rem; }
.btn--primary {
  background: var(--primary); color: #06121a;
  box-shadow: 0 0 0 0 rgba(0,240,255,0.45);
}
.btn--primary:hover {
  transform: translateY(-1px);
  box-shadow: 0 8px 28px -8px rgba(0,240,255,0.7);
}
.btn--ghost {
  background: rgba(255,255,255,0.02);
  color: var(--text-primary);
  border-color: var(--border-color);
}
.btn--ghost:hover { border-color: var(--primary); color: var(--primary); }
.btn:focus-visible { outline: 2px solid var(--primary); outline-offset: 2px; }

/* ---------- search ---------- */
.search {
  position: relative; max-width: 480px;
}
.search svg {
  position: absolute; left: 14px; top: 50%; transform: translateY(-50%);
  width: 16px; height: 16px; color: var(--text-muted, #6a6c7c); pointer-events: none;
}
.search input {
  width: 100%; padding: 13px 56px 13px 40px;
  background: var(--bg-secondary, #12121a);
  border: 1px solid var(--border-color);
  border-radius: 12px;
  color: var(--text-primary);
  font: inherit; font-size: 0.95rem;
  transition: border-color 120ms ease, box-shadow 120ms ease;
}
.search input::placeholder { color: var(--text-muted); }
.search input:focus {
  outline: none; border-color: var(--primary);
  box-shadow: 0 0 0 3px rgba(0,240,255,0.18);
}
.search kbd {
  position: absolute; right: 12px; top: 50%; transform: translateY(-50%);
  font: inherit; font-family: 'JetBrains Mono', ui-monospace, monospace; font-size: 11px;
  padding: 3px 7px; border-radius: 5px;
  background: var(--bg-tertiary, #1a1a2a); border: 1px solid var(--border-color);
  color: var(--text-secondary);
}

/* ---------- SHELL ---------- */
.shell {
  max-width: 1200px; margin: 0 auto;
  padding: 40px 24px 80px;
  display: grid; grid-template-columns: 1fr; gap: 32px;
}
@media (min-width: 1024px) {
  .shell { grid-template-columns: 260px minmax(0, 1fr); gap: 56px; padding: 56px 40px 120px; }
}

/* ---------- TOC ---------- */
.toc { display: none; }
@media (min-width: 1024px) {
  .toc {
    display: block;
    position: sticky; top: 32px; align-self: start;
    max-height: calc(100vh - 64px); overflow-y: auto;
    padding-right: 8px;
  }
  .toc h2 {
    font-family: 'Space Grotesk', sans-serif;
    font-size: 11px; font-weight: 700; letter-spacing: 0.18em; text-transform: uppercase;
    color: var(--text-muted); margin: 0 0 14px 4px;
  }
  .toc ol { list-style: none; margin: 0; padding: 0; }
  .toc li { position: relative; }
  .toc a {
    display: grid; grid-template-columns: 28px 1fr 8px;
    align-items: center; gap: 10px;
    padding: 10px 12px;
    border-radius: 8px;
    color: var(--text-secondary);
    text-decoration: none; font-size: 0.9rem; line-height: 1.3;
    transition: color 140ms ease, background 140ms ease;
  }
  .toc__num {
    font-family: 'JetBrains Mono', monospace; font-size: 11px;
    color: var(--text-muted);
    transition: color 140ms ease;
  }
  .toc__dot {
    width: 6px; height: 6px; border-radius: 50%;
    background: transparent;
    transition: background 180ms ease, box-shadow 180ms ease;
  }
  .toc a:hover {
    color: var(--text-primary);
    background: var(--bg-secondary);
  }
  .toc a.is-active {
    color: var(--text-primary);
    background: linear-gradient(90deg, color-mix(in oklab, var(--tone) 14%, transparent), transparent 80%);
  }
  .toc a.is-active .toc__num { color: var(--tone); }
  .toc a.is-active .toc__dot {
    background: var(--tone);
    box-shadow: 0 0 12px var(--tone), 0 0 4px var(--tone);
  }
  .toc a:focus-visible { outline: 2px solid var(--tone, var(--primary)); outline-offset: 2px; }

  .toc__panel {
    margin-top: 24px; padding: 18px;
    background: linear-gradient(180deg, var(--bg-secondary), var(--bg-tertiary));
    border: 1px solid var(--border-color);
    border-radius: 12px;
  }
  .toc__panel-title {
    margin: 0 0 4px; font-family: 'Space Grotesk', sans-serif;
    font-size: 0.875rem; font-weight: 700; color: var(--text-primary);
  }
  .toc__panel-body { margin: 0 0 12px; font-size: 0.8125rem; color: var(--text-secondary); }
}

/* ---------- mobile jump ---------- */
.jump {
  position: sticky; top: 12px; z-index: 5;
  display: flex; align-items: center; gap: 10px;
  padding: 10px 14px;
  background: color-mix(in oklab, var(--bg-secondary) 92%, transparent);
  backdrop-filter: blur(8px);
  border: 1px solid var(--border-color); border-radius: 10px;
}
.jump label { font-size: 0.8125rem; color: var(--text-secondary); }
.jump select {
  flex: 1; min-width: 0;
  background: var(--bg-tertiary); color: var(--text-primary);
  border: 1px solid var(--border-color); border-radius: 6px;
  padding: 8px 10px; font: inherit; font-size: 0.9rem;
}
.jump select:focus-visible { outline: 2px solid var(--primary); outline-offset: 2px; }
@media (min-width: 1024px) { .jump { display: none; } }

/* ---------- content ---------- */
.doc-content { min-width: 0; }
.sec {
  padding: 36px 0;
  border-top: 1px solid var(--border-color);
  scroll-margin-top: 24px;
  position: relative;
}
.sec:first-of-type { border-top: 0; padding-top: 8px; }
.sec.is-flash::before {
  content: ''; position: absolute; inset: 0 -16px;
  border-radius: 16px;
  background: color-mix(in oklab, var(--tone) 12%, transparent);
  animation: flash 1.4s ease-out forwards;
  pointer-events: none;
}
@keyframes flash { to { background: transparent; } }

.sec__head {
  display: flex; align-items: center; gap: 14px;
  margin-bottom: 8px;
}
.sec__num {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.8125rem; font-weight: 700;
  padding: 6px 10px; border-radius: 8px;
  color: var(--tone);
  background: color-mix(in oklab, var(--tone) 14%, transparent);
  border: 1px solid color-mix(in oklab, var(--tone) 35%, transparent);
}
.sec h2 {
  font-family: 'Space Grotesk', sans-serif;
  font-size: clamp(1.4rem, 1.1rem + 0.8vw, 1.75rem);
  font-weight: 700; letter-spacing: -0.01em; line-height: 1.2;
  margin: 0; padding-left: 14px;
  border-left: 3px solid var(--tone);
  display: flex; align-items: center; gap: 10px;
}
.sec .anchor {
  opacity: 0; color: var(--text-muted); text-decoration: none;
  font-weight: 400; transition: opacity 150ms ease, color 120ms ease;
}
.sec h2:hover .anchor, .sec .anchor:focus-visible { opacity: 1; }
.sec .anchor:hover { color: var(--tone); }

.sec__blurb {
  color: var(--text-secondary); margin: 0 0 22px;
  max-width: var(--measure);
}
.sec h3 {
  font-family: 'Space Grotesk', sans-serif;
  font-size: 1.0625rem; font-weight: 600;
  margin: 22px 0 6px; color: var(--text-primary);
}
.sec p { max-width: var(--measure); margin: 0 0 12px; }
.sec a { color: var(--tone); text-decoration: underline; text-underline-offset: 3px; }
.sec a:focus-visible { outline: 2px solid var(--tone); outline-offset: 2px; border-radius: 2px; }
.sec code, .sec kbd {
  font-family: 'JetBrains Mono', monospace; font-size: 0.85em;
  background: var(--bg-tertiary); border: 1px solid var(--border-color);
  padding: 1px 6px; border-radius: 4px;
}
.sec kbd { color: var(--primary); border-color: color-mix(in oklab, var(--primary) 35%, var(--border-color)); }

/* pull quote */
.quote {
  margin: 24px 0; padding: 20px 24px;
  border-left: 3px solid var(--tone);
  background: linear-gradient(90deg, color-mix(in oklab, var(--tone) 10%, transparent), transparent 80%);
  border-radius: 0 12px 12px 0;
  max-width: var(--measure);
}
.quote p { font-family: 'Space Grotesk', sans-serif; font-size: 1.125rem; line-height: 1.4; margin: 0 0 6px; color: var(--text-primary); }
.quote cite { font-style: normal; font-size: 0.8125rem; color: var(--text-muted); font-family: 'JetBrains Mono', monospace; }

/* key takeaway */
.key {
  margin: 20px 0; padding: 18px 20px;
  border: 1px solid color-mix(in oklab, var(--tone) 35%, var(--border-color));
  background: color-mix(in oklab, var(--tone) 8%, var(--bg-secondary));
  border-radius: 12px;
  max-width: var(--measure);
}
.key__tag {
  display: inline-block; font-family: 'JetBrains Mono', monospace; font-size: 11px;
  color: var(--tone); text-transform: uppercase; letter-spacing: 0.12em;
  margin-bottom: 6px;
}
.key p { margin: 0; }

/* empty state */
.empty {
  display: grid; place-items: center; gap: 8px;
  padding: 60px 24px; text-align: center;
  border: 1px dashed var(--border-color); border-radius: 14px;
  background: var(--bg-secondary); color: var(--text-secondary);
}
.empty svg { width: 32px; height: 32px; color: var(--text-muted); margin-bottom: 6px; }
.empty strong { color: var(--text-primary); font-family: 'Space Grotesk', sans-serif; }

/* sign-off */
.signoff {
  margin-top: 56px; padding-top: 24px;
  border-top: 1px solid var(--border-color);
  display: flex; align-items: center; gap: 16px;
}
.signoff__mark {
  width: 44px; height: 44px; display: grid; place-items: center;
  border-radius: 12px;
  background: linear-gradient(135deg, var(--primary), var(--secondary, #b744ff));
  font-size: 22px; line-height: 1;
  box-shadow: 0 8px 24px -8px rgba(0,240,255,0.5);
}
.signoff__line { font-family: 'Space Grotesk', sans-serif; font-size: 1rem; margin: 0 0 2px; color: var(--text-primary); }
.signoff__meta { margin: 0; font-size: 0.8125rem; color: var(--text-muted); display: flex; flex-wrap: wrap; gap: 6px; }
.signoff__meta a { color: var(--text-secondary); text-decoration: none; }
.signoff__meta a:hover { color: var(--primary); }

/* totop */
.totop {
  position: fixed; right: 20px; bottom: 24px; z-index: 10;
  width: 44px; height: 44px; display: grid; place-items: center;
  border-radius: 999px;
  background: var(--bg-secondary); border: 1px solid var(--border-color);
  color: var(--text-primary); cursor: pointer;
  opacity: 0; transform: translateY(8px); pointer-events: none;
  transition: opacity 180ms ease, transform 180ms ease, color 120ms ease, border-color 120ms ease;
}
.totop.is-visible { opacity: 1; transform: translateY(0); pointer-events: auto; }
.totop:hover { color: var(--primary); border-color: var(--primary); box-shadow: 0 0 18px rgba(0,240,255,0.3); }
.totop:focus-visible { outline: 2px solid var(--primary); outline-offset: 2px; }
.totop svg { width: 18px; height: 18px; }

/* reduced motion */
@media (prefers-reduced-motion: reduce) {
  .hero__glow, .paca__ring, .paca__pulse, .dot--cyan,
  .sec.is-flash::before { animation: none !important; }
  .btn, .totop, .toc a { transition: none; }
}
</style>
