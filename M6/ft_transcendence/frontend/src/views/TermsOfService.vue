<script setup>
import { ref, onMounted, onBeforeUnmount, nextTick } from 'vue'

const sections = [
  { id: 'acceptance',   tone: 'gold',    n: '§1', title: 'Acceptance of terms' },
  { id: 'accounts',     tone: 'gold',    n: '§2', title: 'Accounts' },
  { id: 'conduct',      tone: 'magenta', n: '§3', title: 'Acceptable behavior' },
  { id: 'content',      tone: 'magenta', n: '§4', title: 'User content & IP' },
  { id: 'public-api',   tone: 'cyan',    n: '§5', title: 'Public API & automation' },
  { id: 'termination',  tone: 'cyan',    n: '§6', title: 'Termination' },
  { id: 'liability',    tone: 'gold',    n: '§7', title: 'Limitation of liability' },
  { id: 'governing',    tone: 'green',   n: '§8', title: 'Disputes & governing law' },
]

const activeId = ref(sections[0].id)
const showToTop = ref(false)
const flashId = ref(null)
let observer = null
let flashTimer = null

function jumpTo(id) {
  const el = document.getElementById(id); if (!el) return
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
</script>

<template>
  <main class="doc-page doc-page--terms">
    <!-- ============ HERO ============ -->
    <header class="hero" aria-labelledby="terms-title">
      <div class="hero__bg" aria-hidden="true">
        <span class="hero__grid"></span>
        <span class="hero__glow"></span>
      </div>

      <div class="hero__inner">
        <!-- Document + seal motif -->
        <div class="doc-art" aria-hidden="true">
          <svg viewBox="0 0 200 220" fill="none">
            <defs>
              <linearGradient id="paperFill" x1="0" y1="0" x2="0" y2="1">
                <stop offset="0%" stop-color="#1a1a2a"/>
                <stop offset="100%" stop-color="#12121a"/>
              </linearGradient>
            </defs>
            <!-- back paper -->
            <rect x="30" y="20" width="130" height="170" rx="6"
                  fill="url(#paperFill)" stroke="#2a2a3a" stroke-width="1"
                  transform="rotate(-4 95 105)"/>
            <!-- front paper -->
            <rect x="40" y="30" width="130" height="170" rx="6"
                  fill="url(#paperFill)" stroke="#3a3a4d" stroke-width="1"/>
            <!-- header -->
            <rect x="52" y="46" width="60" height="6" rx="2" fill="#e8e8f0" opacity="0.85"/>
            <rect x="52" y="58" width="40" height="3" rx="1.5" fill="#a0a0b0" opacity="0.5"/>
            <!-- lines -->
            <g stroke="#3a3a4d" stroke-width="1">
              <line x1="52" y1="78"  x2="158" y2="78"/>
              <line x1="52" y1="88"  x2="158" y2="88"/>
              <line x1="52" y1="98"  x2="140" y2="98"/>
              <line x1="52" y1="115" x2="158" y2="115"/>
              <line x1="52" y1="125" x2="158" y2="125"/>
              <line x1="52" y1="135" x2="120" y2="135"/>
              <line x1="52" y1="152" x2="158" y2="152"/>
              <line x1="52" y1="162" x2="100" y2="162"/>
            </g>
            <!-- signature swoosh -->
            <path d="M52 180 q15 -8 28 -2 t30 -2" stroke="#00f0ff" stroke-width="1.3" fill="none" stroke-linecap="round"/>
            <!-- wax seal -->
            <g transform="translate(140 162)">
              <circle r="26" fill="#f5c842" opacity="0.18"/>
              <circle r="20" fill="#1a1a2a" stroke="#f5c842" stroke-width="1.4"/>
              <circle r="20" fill="none" stroke="#f5c842" stroke-width="0.6" stroke-dasharray="2 2"/>
              <text x="0" y="3" font-family="JetBrains Mono, monospace" font-size="8" font-weight="700"
                    text-anchor="middle" fill="#f5c842">SEAL</text>
              <text x="0" y="11" font-family="Inter, sans-serif" font-size="3.4"
                    text-anchor="middle" fill="#f5c842" opacity="0.7">EST. 2026</text>
            </g>
          </svg>
        </div>

        <div class="hero__copy">
          <span class="hero__eyebrow"><span class="dot"></span> Legal · §1–§8 · Eight clauses</span>
          <h1 id="terms-title" class="hero__title">Terms of Service.</h1>
          <p class="hero__lede">
            The rules of the room — what we owe you, what you owe us, and what
            happens when an alpaca gets out of line.
          </p>

          <div class="meta-strip">
            <div class="meta-strip__cell">
              <span class="meta-strip__label">Effective</span>
              <span class="meta-strip__value">May 27, 2026</span>
            </div>
            <div class="meta-strip__cell">
              <span class="meta-strip__label">Version</span>
              <span class="meta-strip__value">v3.2</span>
            </div>
            <div class="meta-strip__cell">
              <span class="meta-strip__label">Reading time</span>
              <span class="meta-strip__value">~6 min</span>
            </div>
          </div>
        </div>
      </div>
    </header>

    <!-- ============ SHELL ============ -->
    <div class="shell">
      <aside class="toc" aria-label="Clauses">
        <h2>Clauses</h2>
        <ol>
          <li v-for="s in sections" :key="s.id" :class="`tone-${s.tone}`">
            <a :href="'#'+s.id"
               :class="{ 'is-active': activeId === s.id }"
               @click.prevent="jumpTo(s.id)">
              <span class="toc__num">{{ s.n }}</span>
              <span class="toc__label">{{ s.title }}</span>
              <span class="toc__dot" aria-hidden="true"></span>
            </a>
          </li>
        </ol>
        <div class="toc__panel" aria-hidden="true">
          <p class="toc__panel-title">Read the short version</p>
          <p class="toc__panel-body">Be kind, don't cheat, respect the API rate limit, and we'll all have a nice time.</p>
        </div>
      </aside>

      <div class="jump">
        <label for="jump-select">Jump to clause</label>
        <select id="jump-select" @change="(e) => { jumpTo(e.target.value); e.target.value='' }">
          <option value="" disabled selected>Choose a clause…</option>
          <option v-for="s in sections" :key="s.id" :value="s.id">{{ s.n }} · {{ s.title }}</option>
        </select>
      </div>

      <article class="doc-content">

        <section id="acceptance" :class="['sec','tone-gold',{ 'is-flash': flashId==='acceptance' }]">
          <div class="sec__head"><span class="sec__num">§1</span>
            <h2>Acceptance of terms <a class="anchor" href="#acceptance">#</a></h2>
          </div>
          <p>By accessing or using AlpacaParty (the "Service") you agree to be bound by these Terms. If you do not agree, do not use the Service. AlpacaParty is a non-commercial student project — these Terms describe how the Service may be used, not a commercial contract.</p>
        </section>

        <section id="accounts" :class="['sec','tone-gold',{ 'is-flash': flashId==='accounts' }]">
          <div class="sec__head"><span class="sec__num">§2</span>
            <h2>Accounts <a class="anchor" href="#accounts">#</a></h2>
          </div>
          <p>You are responsible for all activity under your account. Keep your credentials private. You must be at least 13 years old (16 in the EEA) to register. We support email/password registration.</p>
          <p>One account per person, please. Selling, sharing, or renting accounts is prohibited. If you forget your password, you can request a reset; we do not store passwords in a recoverable form.</p>
        </section>

        <section id="conduct" :class="['sec','tone-magenta',{ 'is-flash': flashId==='conduct' }]">
          <div class="sec__head"><span class="sec__num">§3</span>
            <h2>Acceptable behavior <a class="anchor" href="#conduct">#</a></h2>
          </div>
          <p>No harassment, hate speech, doxxing, threats, sexual content involving minors, cheating, exploiting bugs, scraping, or attempts to disrupt the Service. The game enforces server-side anti-cheat (move-speed clamps, spit cooldowns and range checks); circumventing those checks is a ban-worthy offence. Administrators may mute, kick, or ban accounts that violate these rules.</p>

          <div class="rules">
            <div class="rules__col">
              <h3 class="rules__title rules__title--ok">
                <svg viewBox="0 0 16 16" fill="none" aria-hidden="true"><path d="M3 8.5l3 3 7-7.5" stroke="currentColor" stroke-width="1.6" stroke-linecap="round" stroke-linejoin="round"/></svg>
                Do
              </h3>
              <ul>
                <li>Be kind to other alpacas (and humans behind them)</li>
                <li>Report bugs and abuse</li>
                <li>Respect room moderators' decisions</li>
              </ul>
            </div>
            <div class="rules__col">
              <h3 class="rules__title rules__title--no">
                <svg viewBox="0 0 16 16" fill="none" aria-hidden="true"><path d="M4 4l8 8M12 4l-8 8" stroke="currentColor" stroke-width="1.6" stroke-linecap="round"/></svg>
                Don't
              </h3>
              <ul>
                <li>Harass, threaten, or impersonate</li>
                <li>Cheat, exploit bugs, or use automation</li>
                <li>Scrape or resell account data</li>
              </ul>
            </div>
          </div>
        </section>

        <section id="content" :class="['sec','tone-magenta',{ 'is-flash': flashId==='content' }]">
          <div class="sec__head"><span class="sec__num">§4</span>
            <h2>User content &amp; IP <a class="anchor" href="#content">#</a></h2>
          </div>
          <p>You retain rights to content you post on the Service (display name, avatar, feed posts, comments, direct messages, screenshots). You grant us a non-exclusive, worldwide license to host, display, and transmit that content as needed to operate the Service. How we actually handle that data is set out in our <router-link to="/privacy">Privacy Policy</router-link>.</p>
          <p>The AlpacaParty name, the alpaca logo, and the in-game 3D art and source code remain the property of the project authors and their respective licensors.</p>
        </section>

        <section id="public-api" :class="['sec','tone-cyan',{ 'is-flash': flashId==='public-api' }]">
          <div class="sec__head"><span class="sec__num">§5</span>
            <h2>Public API &amp; automation <a class="anchor" href="#public-api">#</a></h2>
          </div>
          <p>The Public API is provided "as is" for personal, non-commercial use. You authenticate with a key generated from your profile, prefixed <code>ap_</code>. The key is yours alone — do not share or publish it.</p>
          <p>Respect the published rate limit (30 requests / minute by default). Bypassing it, scraping data en masse, or using the API to harass other users will result in revocation of the key and may lead to account suspension. We may rotate or revoke any key at any time.</p>
        </section>

        <section id="termination" :class="['sec','tone-cyan',{ 'is-flash': flashId==='termination' }]">
          <div class="sec__head"><span class="sec__num">§6</span>
            <h2>Termination <a class="anchor" href="#termination">#</a></h2>
          </div>
          <p>You may close your account at any time from <router-link to="/profile?tab=settings">Profile → Settings → Danger zone</router-link>. Deletion is immediate and removes your personal data; posts and messages you authored are anonymized or removed. We may suspend or terminate accounts for violations of these Terms; serious violations (cheating, harassment) may result in a permanent ban.</p>
        </section>

        <section id="liability" :class="['sec','tone-gold',{ 'is-flash': flashId==='liability' }]">
          <div class="sec__head"><span class="sec__num">§7</span>
            <h2>Limitation of liability <a class="anchor" href="#liability">#</a></h2>
          </div>

          <aside class="callout callout--warning">
            <span class="callout__icon" aria-hidden="true">
              <svg viewBox="0 0 24 24" fill="none">
                <path d="M12 3l10 18H2L12 3Z" stroke="currentColor" stroke-width="1.5" stroke-linejoin="round"/>
                <path d="M12 10v4.5M12 17v.5" stroke="currentColor" stroke-width="1.5" stroke-linecap="round"/>
              </svg>
            </span>
            <div class="callout__body">
              <h3 class="callout__title">Limitation of liability</h3>
              <p>The Service is provided <strong>"as is"</strong>, without warranty of any kind. To the maximum extent permitted by law, the project authors are not liable for indirect, incidental, special, consequential, or punitive damages, or any loss of profits, revenues, data, or goodwill arising from your use of the Service.</p>
              <p>Because AlpacaParty is a free, non-commercial project, no payment ever flows from you to us — and our aggregate liability for any claim relating to the Service is correspondingly limited to the amount you paid us, which is <strong>zero</strong>.</p>
            </div>
          </aside>
        </section>

        <section id="governing" :class="['sec','tone-green',{ 'is-flash': flashId==='governing' }]">
          <div class="sec__head"><span class="sec__num">§8</span>
            <h2>Disputes &amp; governing law <a class="anchor" href="#governing">#</a></h2>
          </div>
          <p>These Terms are governed by the laws of France, excluding its conflict-of-laws rules. Disputes are resolved in the competent courts of Paris, France, unless a binding consumer-protection clause applies in your region under local law.</p>
          <p>For questions, open the in-app <router-link to="/help">help desk</router-link> or contact a project maintainer through the source repository.</p>
        </section>

        <footer class="signoff">
          <div class="signoff__seal" aria-hidden="true">
            <svg viewBox="0 0 40 40" fill="none">
              <circle cx="20" cy="20" r="18" stroke="currentColor" stroke-width="1" stroke-dasharray="2 2"/>
              <circle cx="20" cy="20" r="13" stroke="currentColor" stroke-width="1.3" fill="none"/>
              <text x="20" y="24" font-family="JetBrains Mono, monospace" font-size="8" font-weight="700"
                    text-anchor="middle" fill="currentColor">v3.2</text>
            </svg>
          </div>
          <div>
            <p class="signoff__line">Signed, sealed, and herded together. We'll publish material changes 14 days before they take effect.</p>
            <p class="signoff__meta">
              Effective May 27, 2026
              <span>·</span> <router-link to="/help">help desk</router-link>
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
.doc-page--terms {
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

/* ---------- HERO (contract-leaning, gold accent) ---------- */
.hero {
  position: relative; padding: 80px 24px 56px;
  isolation: isolate; overflow: hidden;
  border-bottom: 1px solid var(--border-color, #2a2a3a);
  background:
    radial-gradient(700px 350px at 80% 10%, rgba(245,200,66,0.10), transparent 70%),
    linear-gradient(180deg, var(--bg-primary), var(--bg-secondary, #12121a));
}
.hero__bg { position: absolute; inset: 0; z-index: -1; }
.hero__grid {
  position: absolute; inset: 0;
  background-image:
    linear-gradient(rgba(245,200,66,0.05) 1px, transparent 1px),
    linear-gradient(90deg, rgba(245,200,66,0.05) 1px, transparent 1px);
  background-size: 56px 56px;
  mask-image: radial-gradient(ellipse 70% 60% at 30% 30%, #000 30%, transparent 80%);
}
.hero__glow {
  position: absolute; width: 520px; height: 520px;
  top: -200px; left: -160px;
  border-radius: 50%; filter: blur(120px);
  background: radial-gradient(circle, rgba(0,240,255,0.22), transparent 70%);
}

.hero__inner {
  max-width: 1200px; margin: 0 auto;
  display: grid; grid-template-columns: 1fr; gap: 40px;
  align-items: center;
}
@media (min-width: 880px) {
  .hero__inner { grid-template-columns: 1fr 220px; gap: 56px; }
  .doc-art { order: 2; }
}

.doc-art { width: 220px; margin: 0 auto; }
.doc-art svg { width: 100%; height: auto; display: block;
  filter: drop-shadow(0 18px 36px rgba(0,0,0,0.5)); }

.hero__copy { min-width: 0; }
.hero__eyebrow {
  display: inline-flex; align-items: center; gap: 8px;
  font-size: 12px; font-weight: 600; letter-spacing: 0.14em; text-transform: uppercase;
  color: var(--text-secondary, #a0a0b0); margin-bottom: 14px;
}
.dot {
  width: 8px; height: 8px; border-radius: 50%;
  background: var(--gold); box-shadow: 0 0 10px var(--gold);
  animation: blink 2.4s ease-in-out infinite;
}
@keyframes blink { 0%, 100% { opacity: 1; } 50% { opacity: 0.45; } }

.hero__title {
  font-family: 'Space Grotesk', 'Inter', sans-serif;
  font-size: clamp(2.2rem, 1.5rem + 3vw, 3.4rem);
  font-weight: 700; letter-spacing: -0.02em; line-height: 1.05;
  margin: 0 0 14px;
  background: linear-gradient(180deg, #fff 0%, #8a8a9c 140%);
  -webkit-background-clip: text; background-clip: text; color: transparent;
}
.hero__lede { margin: 0 0 24px; font-size: 1.0625rem; color: var(--text-secondary, #a0a0b0); max-width: 56ch; }

.meta-strip {
  display: grid; grid-template-columns: repeat(3, 1fr);
  border: 1px solid var(--border-color);
  border-radius: 12px;
  background: var(--bg-secondary, #12121a);
  overflow: hidden;
  max-width: 480px;
}
.meta-strip__cell {
  padding: 14px 16px;
  border-right: 1px solid var(--border-color);
  display: flex; flex-direction: column; gap: 2px;
}
.meta-strip__cell:last-child { border-right: 0; }
.meta-strip__label {
  font-family: 'JetBrains Mono', monospace; font-size: 10px;
  text-transform: uppercase; letter-spacing: 0.12em;
  color: var(--text-muted, #6a6c7c);
}
.meta-strip__value {
  font-family: 'Space Grotesk', sans-serif; font-size: 0.95rem; font-weight: 600;
  color: var(--text-primary);
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

/* TOC */
.toc { display: none; }
@media (min-width: 1024px) {
  .toc {
    display: block; position: sticky; top: 32px; align-self: start;
    max-height: calc(100vh - 64px); overflow-y: auto; padding-right: 8px;
  }
  .toc h2 {
    font-family: 'Space Grotesk', sans-serif;
    font-size: 11px; font-weight: 700; letter-spacing: 0.18em; text-transform: uppercase;
    color: var(--text-muted); margin: 0 0 14px 4px;
  }
  .toc ol { list-style: none; margin: 0; padding: 0;
    border-left: 1px solid var(--border-color); }
  .toc a {
    display: grid; grid-template-columns: 30px 1fr 8px;
    align-items: center; gap: 10px;
    padding: 10px 12px; margin-left: -1px;
    border-left: 2px solid transparent;
    color: var(--text-secondary); text-decoration: none;
    font-size: 0.875rem; line-height: 1.3;
    transition: color 140ms ease, background 140ms ease, border-color 140ms ease;
  }
  .toc__num {
    font-family: 'JetBrains Mono', monospace; font-size: 11px; font-weight: 700;
    color: var(--text-muted); transition: color 140ms ease;
  }
  .toc__dot {
    width: 6px; height: 6px; border-radius: 50%;
    background: transparent; transition: background 180ms ease, box-shadow 180ms ease;
  }
  .toc a:hover { color: var(--text-primary); background: var(--bg-secondary); }
  .toc a.is-active {
    color: var(--text-primary);
    background: linear-gradient(90deg, color-mix(in oklab, var(--tone) 14%, transparent), transparent 80%);
    border-left-color: var(--tone);
  }
  .toc a.is-active .toc__num { color: var(--tone); }
  .toc a.is-active .toc__dot { background: var(--tone); box-shadow: 0 0 12px var(--tone), 0 0 4px var(--tone); }
  .toc a:focus-visible { outline: 2px solid var(--tone, var(--primary)); outline-offset: 2px; }

  .toc__panel {
    margin-top: 24px; padding: 16px;
    background: var(--bg-secondary);
    border: 1px solid var(--border-color); border-radius: 12px;
  }
  .toc__panel-title { margin: 0 0 4px; font-family: 'Space Grotesk', sans-serif; font-size: 0.875rem; font-weight: 700; color: var(--text-primary); }
  .toc__panel-body { margin: 0; font-size: 0.8125rem; color: var(--text-secondary); }
}

/* mobile jump */
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
  background: var(--bg-tertiary, #1a1a2a); color: var(--text-primary);
  border: 1px solid var(--border-color); border-radius: 6px;
  padding: 8px 10px; font: inherit; font-size: 0.9rem;
}
.jump select:focus-visible { outline: 2px solid var(--primary); outline-offset: 2px; }
@media (min-width: 1024px) { .jump { display: none; } }

/* ---------- content (contract-grid feel) ---------- */
.doc-content { min-width: 0; }
.sec {
  padding: 36px 0;
  border-top: 1px dashed var(--border-color);
  scroll-margin-top: 24px; position: relative;
}
.sec:first-of-type { border-top: 0; padding-top: 8px; }
.sec.is-flash::before {
  content: ''; position: absolute; inset: 0 -16px; border-radius: 16px;
  background: color-mix(in oklab, var(--tone) 12%, transparent);
  animation: flash 1.4s ease-out forwards; pointer-events: none;
}
@keyframes flash { to { background: transparent; } }

.sec__head { display: flex; align-items: center; gap: 14px; margin-bottom: 8px; }
.sec__num {
  font-family: 'JetBrains Mono', monospace;
  font-size: 0.875rem; font-weight: 700;
  padding: 6px 10px; border-radius: 8px;
  color: var(--tone);
  background: color-mix(in oklab, var(--tone) 14%, transparent);
  border: 1px solid color-mix(in oklab, var(--tone) 35%, transparent);
  letter-spacing: 0.02em;
}
.sec h2 {
  font-family: 'Space Grotesk', sans-serif;
  font-size: clamp(1.35rem, 1.05rem + 0.8vw, 1.625rem);
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

.sec p { max-width: var(--measure); margin: 12px 0; }
.sec a { color: var(--tone); text-decoration: underline; text-underline-offset: 3px; }
.sec a:focus-visible { outline: 2px solid var(--tone); outline-offset: 2px; border-radius: 2px; }
.sec code {
  font-family: 'JetBrains Mono', monospace; font-size: 0.85em;
  background: var(--bg-tertiary, #1a1a2a); border: 1px solid var(--border-color);
  padding: 1px 6px; border-radius: 4px;
}

/* Do/Don't grid */
.rules {
  display: grid; grid-template-columns: 1fr; gap: 12px;
  margin: 18px 0; max-width: var(--measure);
}
@media (min-width: 600px) {
  .rules { grid-template-columns: 1fr 1fr; }
}
.rules__col {
  padding: 16px 18px; border-radius: 12px;
  background: var(--bg-secondary, #12121a);
  border: 1px solid var(--border-color);
}
.rules__title {
  display: flex; align-items: center; gap: 8px;
  font-family: 'Space Grotesk', sans-serif;
  font-size: 0.875rem; font-weight: 700;
  text-transform: uppercase; letter-spacing: 0.12em;
  margin: 0 0 10px;
}
.rules__title svg { width: 14px; height: 14px; }
.rules__title--ok { color: var(--success, #00ff88); }
.rules__title--no { color: var(--danger, #ff006e); }
.rules__col ul { list-style: none; padding: 0; margin: 0; }
.rules__col li {
  padding: 4px 0; font-size: 0.9rem; color: var(--text-secondary);
  border-top: 1px dashed var(--border-color);
}
.rules__col li:first-child { border-top: 0; }

/* warning callout */
.callout {
  display: grid; grid-template-columns: 36px 1fr; gap: 16px;
  padding: 22px 24px; margin: 16px 0; border-radius: 14px;
  background: linear-gradient(180deg, color-mix(in oklab, var(--tone) 12%, var(--bg-secondary)), var(--bg-secondary));
  border: 1px solid color-mix(in oklab, var(--tone) 35%, var(--border-color));
  max-width: var(--measure);
  position: relative; overflow: hidden;
}
.callout::after {
  content: ''; position: absolute; top: 0; right: 0; width: 60px; height: 60px;
  background: repeating-linear-gradient(45deg,
    color-mix(in oklab, var(--tone) 18%, transparent) 0 6px,
    transparent 6px 12px);
  border-bottom-left-radius: 14px;
  opacity: 0.7;
  pointer-events: none;
}
.callout__icon {
  width: 36px; height: 36px; display: grid; place-items: center;
  border-radius: 10px; background: color-mix(in oklab, var(--tone) 18%, transparent);
  color: var(--tone);
}
.callout__icon svg { width: 18px; height: 18px; }
.callout__title { margin: 0 0 6px; font-family: 'Space Grotesk', sans-serif; color: var(--tone); font-size: 1rem; font-weight: 700; }
.callout__body p:last-child { margin-bottom: 0; }
.callout strong { color: var(--text-primary); font-weight: 700; }

/* signoff */
.signoff {
  margin-top: 56px; padding-top: 24px;
  border-top: 1px solid var(--border-color);
  display: flex; align-items: center; gap: 16px;
}
.signoff__seal {
  width: 52px; height: 52px; display: grid; place-items: center;
  color: var(--gold);
  border-radius: 50%;
  background: rgba(245,200,66,0.08);
  border: 1px solid color-mix(in oklab, var(--gold) 30%, transparent);
}
.signoff__seal svg { width: 38px; height: 38px; }
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

@media (prefers-reduced-motion: reduce) {
  .dot, .sec.is-flash::before { animation: none !important; }
  .totop, .toc a { transition: none; }
}
</style>
