<script setup>
import { ref, onMounted, onBeforeUnmount, nextTick } from 'vue'

const sections = [
  { id: 'overview',        tone: 'cyan',    n: '01', title: 'Overview' },
  { id: 'data-collected',  tone: 'cyan',    n: '02', title: 'Data we collect' },
  { id: 'how-we-use',      tone: 'cyan',    n: '03', title: 'How we use it' },
  { id: 'cookies',         tone: 'magenta', n: '04', title: 'Cookies & tracking' },
  { id: 'sharing',         tone: 'magenta', n: '05', title: 'Sharing & subprocessors' },
  { id: 'retention',       tone: 'gold',    n: '06', title: 'Data retention' },
  { id: 'rights',          tone: 'gold',    n: '07', title: 'Your rights (GDPR)' },
  { id: 'children',        tone: 'green',   n: '08', title: 'Children\'s privacy' },
  { id: 'contact',         tone: 'green',   n: '09', title: 'Contact the DPO' },
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
  <main class="doc-page doc-page--privacy">
    <!-- ============ HERO ============ -->
    <header class="hero" aria-labelledby="privacy-title">
      <div class="hero__bg" aria-hidden="true">
        <span class="hero__scan"></span>
        <span class="hero__glow"></span>
      </div>

      <div class="hero__inner">
        <!-- Shield SVG -->
        <div class="shield" aria-hidden="true">
          <svg viewBox="0 0 160 180" fill="none">
            <defs>
              <linearGradient id="shieldFill" x1="0" y1="0" x2="0" y2="1">
                <stop offset="0%" stop-color="#00f0ff" stop-opacity="0.20"/>
                <stop offset="100%" stop-color="#00f0ff" stop-opacity="0.02"/>
              </linearGradient>
              <linearGradient id="shieldStroke" x1="0" y1="0" x2="0" y2="1">
                <stop offset="0%" stop-color="#00f0ff"/>
                <stop offset="100%" stop-color="#7a5cff"/>
              </linearGradient>
            </defs>
            <!-- shield -->
            <path d="M80 10 L150 36 V90 c0 38 -28 64 -70 78 -42-14 -70-40-70-78 V36 Z"
                  fill="url(#shieldFill)" stroke="url(#shieldStroke)" stroke-width="1.5"/>
            <!-- circuit lines -->
            <path d="M20 60 H58 L66 68 H94 L102 60 H140" stroke="#00f0ff" stroke-opacity="0.45" stroke-width="0.8" stroke-dasharray="3 3"/>
            <path d="M20 110 H44 L52 102 H108 L116 110 H140" stroke="#00f0ff" stroke-opacity="0.35" stroke-width="0.8" stroke-dasharray="3 3"/>
            <circle cx="80" cy="90" r="3" fill="#00f0ff"/>
            <!-- lock -->
            <rect x="62" y="80" width="36" height="32" rx="4" stroke="#e8e8f0" stroke-width="1.4" fill="rgba(10,10,15,0.7)"/>
            <path d="M68 80 v-8 a12 12 0 0 1 24 0 v8" stroke="#e8e8f0" stroke-width="1.4" fill="none"/>
            <circle cx="80" cy="94" r="2.6" fill="#00f0ff"/>
            <line x1="80" y1="96" x2="80" y2="104" stroke="#00f0ff" stroke-width="1.6" stroke-linecap="round"/>
          </svg>
          <span class="shield__scan-line" />
        </div>

        <div class="hero__copy">
          <span class="hero__eyebrow"><span class="dot"></span> Legal · Effective May 27, 2026</span>
          <h1 id="privacy-title" class="hero__title">Privacy, in plain language.</h1>
          <p class="hero__lede">
            What we collect, why we collect it, and the controls we put in your hands.
            No dark patterns. No ad targeting. Just a 3D farm that respects you.
          </p>

          <div class="badges">
            <span class="badge badge--gdpr">
              <svg viewBox="0 0 16 16" fill="none" aria-hidden="true">
                <path d="M8 1.5l5.5 2v4c0 3.4-2.4 6-5.5 7-3.1-1-5.5-3.6-5.5-7v-4L8 1.5Z" stroke="currentColor" stroke-width="1.2" stroke-linejoin="round"/>
                <path d="M5.5 8l2 2 3.5-4" stroke="currentColor" stroke-width="1.4" stroke-linecap="round" stroke-linejoin="round"/>
              </svg>
              GDPR-compliant
            </span>
            <span class="badge">
              <svg viewBox="0 0 16 16" fill="none" aria-hidden="true">
                <rect x="2.5" y="6.5" width="11" height="7" rx="1.5" stroke="currentColor" stroke-width="1.2"/>
                <path d="M5 6.5V4.5a3 3 0 0 1 6 0v2" stroke="currentColor" stroke-width="1.2" fill="none"/>
              </svg>
              No ad tracking
            </span>
            <span class="badge">
              <svg viewBox="0 0 16 16" fill="none" aria-hidden="true">
                <circle cx="8" cy="8" r="6" stroke="currentColor" stroke-width="1.2"/>
                <path d="M5 8.5l2 2 4-5" stroke="currentColor" stroke-width="1.4" stroke-linecap="round" stroke-linejoin="round"/>
              </svg>
              Hosted in the EU
            </span>
          </div>
        </div>
      </div>
    </header>

    <!-- ============ SHELL ============ -->
    <div class="shell">
      <aside class="toc" aria-label="On this page">
        <h2>Sections</h2>
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
          <p class="toc__panel-title">Exercise your rights</p>
          <p class="toc__panel-body">Export or delete your data from <router-link to="/profile?tab=settings">Profile → Settings</router-link>.</p>
        </div>
      </aside>

      <div class="jump">
        <label for="jump-select">Jump to</label>
        <select id="jump-select" @change="(e) => { jumpTo(e.target.value); e.target.value='' }">
          <option value="" disabled selected>Choose a section…</option>
          <option v-for="s in sections" :key="s.id" :value="s.id">{{ s.n }} · {{ s.title }}</option>
        </select>
      </div>

      <article class="doc-content">
        <section id="overview" :class="['sec','tone-cyan',{ 'is-flash': flashId==='overview' }]">
          <div class="sec__head"><span class="sec__num">01</span>
            <h2>Overview <a class="anchor" href="#overview">#</a></h2>
          </div>
          <p>AlpacaParty ("we", "us") is a 3D multiplayer browser game built around two arena games — Spit Royale and Alpaca Road — plus social features (friends, direct messages, a feed, and an AI help desk). This policy describes how we handle personal data of players and visitors. Every line below reflects what we actually do, not what a template says.</p>

          <div class="key">
            <span class="key__tag">TL;DR</span>
            <p>We collect what we need to run the service. We don't sell it. We don't use it for advertising. You can export or delete it whenever you want.</p>
          </div>
        </section>

        <section id="data-collected" :class="['sec','tone-cyan',{ 'is-flash': flashId==='data-collected' }]">
          <div class="sec__head"><span class="sec__num">02</span>
            <h2>Data we collect <a class="anchor" href="#data-collected">#</a></h2>
          </div>
          <h3>Account data</h3>
          <p>Username, email address, bcrypt-hashed password, and an optional avatar.</p>
          <h3>Gameplay data</h3>
          <p>Per-game stats (wins, losses, kills, level), match history for 1-vs-1 games, and your alpaca's colour. We do <em>not</em> use a microphone — there is no voice chat.</p>
          <h3>Social data</h3>
          <p>Posts, comments, likes, friend relationships, direct messages, and AI help-desk conversations are stored in our database so they show up next time you open the app.</p>
          <h3>Operational data</h3>
          <p>Standard HTTP access logs (IP, user-agent, path) kept for short-term security and abuse prevention. No third-party analytics or advertising SDKs are loaded.</p>
        </section>

        <section id="how-we-use" :class="['sec','tone-cyan',{ 'is-flash': flashId==='how-we-use' }]">
          <div class="sec__head"><span class="sec__num">03</span>
            <h2>How we use it <a class="anchor" href="#how-we-use">#</a></h2>
          </div>
          <p>To operate the service, authenticate you, persist your matches and social activity, enforce community rules, and answer your questions through the help-desk assistant. We do not sell personal data, and we do not use it for advertising on any platform.</p>
          <p>Help-desk messages are forwarded to a third-party large-language-model provider (Groq) to generate a reply. The provider receives the text of your question and recent conversation context, nothing else.</p>
        </section>

        <section id="cookies" :class="['sec','tone-magenta',{ 'is-flash': flashId==='cookies' }]">
          <div class="sec__head"><span class="sec__num">04</span>
            <h2>Cookies &amp; tracking <a class="anchor" href="#cookies">#</a></h2>
          </div>
          <p>We use first-party cookies for authentication only: <code>jwt_token</code> (short-lived access) and <code>refresh_token</code> (longer-lived refresh). Both are <code>HttpOnly</code> and <code>Secure</code>. No third-party advertising or analytics cookies are set.</p>
        </section>

        <section id="sharing" :class="['sec','tone-magenta',{ 'is-flash': flashId==='sharing' }]">
          <div class="sec__head"><span class="sec__num">05</span>
            <h2>Sharing &amp; subprocessors <a class="anchor" href="#sharing">#</a></h2>
          </div>
          <p>We use a small number of subprocessors to run the service: the hosting provider that runs our containers, and Groq for help-desk replies. We do not sell, rent, or otherwise share personal data with anyone else. There is no payment processor — AlpacaParty is free and nothing is sold.</p>
        </section>

        <section id="retention" :class="['sec','tone-gold',{ 'is-flash': flashId==='retention' }]">
          <div class="sec__head"><span class="sec__num">06</span>
            <h2>Data retention <a class="anchor" href="#retention">#</a></h2>
          </div>
          <div class="grid">
            <div class="grid__card">
              <span class="grid__metric">lifetime</span>
              <p>Account, stats, posts &amp; matches — kept while your account exists</p>
            </div>
            <div class="grid__card">
              <span class="grid__metric">immediate</span>
              <p>Account deletion erases your personal data on request</p>
            </div>
            <div class="grid__card">
              <span class="grid__metric">30 days</span>
              <p>Server access logs for security and abuse prevention</p>
            </div>
          </div>
        </section>

        <section id="rights" :class="['sec','tone-gold',{ 'is-flash': flashId==='rights' }]">
          <div class="sec__head"><span class="sec__num">07</span>
            <h2>Your rights (GDPR) <a class="anchor" href="#rights">#</a></h2>
          </div>

          <aside class="callout">
            <span class="callout__icon" aria-hidden="true">
              <svg viewBox="0 0 24 24" fill="none">
                <path d="M12 3l8 3v5c0 5-3.5 8.5-8 10-4.5-1.5-8-5-8-10V6l8-3Z" stroke="currentColor" stroke-width="1.5" stroke-linejoin="round"/>
                <path d="M9 12.5l2 2 4-4.5" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
              </svg>
            </span>
            <div class="callout__body">
              <h3 class="callout__title">Your rights under the GDPR</h3>
              <p>If you are in the EU/EEA, UK, or Switzerland you have the right to access, rectify, erase, restrict, port, and object to the processing of your personal data. You also have the right to lodge a complaint with your local supervisory authority.</p>
              <p>Access and erasure are self-service: <strong>Export my data</strong> and <strong>Delete my account</strong> both live in <router-link to="/profile?tab=settings">Profile → Settings → Danger zone</router-link>. For any other request, contact a project maintainer through the repository.</p>
            </div>
          </aside>
        </section>

        <section id="children" :class="['sec','tone-green',{ 'is-flash': flashId==='children' }]">
          <div class="sec__head"><span class="sec__num">08</span>
            <h2>Children's privacy <a class="anchor" href="#children">#</a></h2>
          </div>
          <p>AlpacaParty is not directed to children under 13 (16 in the EEA). We do not knowingly collect data from them. If you believe a child has provided us with personal data, contact a maintainer and we will erase it.</p>
        </section>

        <section id="contact" :class="['sec','tone-green',{ 'is-flash': flashId==='contact' }]">
          <div class="sec__head"><span class="sec__num">09</span>
            <h2>Contact us <a class="anchor" href="#contact">#</a></h2>
          </div>
          <p>For privacy questions, open the in-app <router-link to="/help">help desk</router-link> and request a human, or reach a project maintainer through the source repository. Most requests (data export, account deletion) are already self-service in your <router-link to="/profile?tab=settings">settings</router-link>.</p>
        </section>

        <footer class="signoff">
          <div class="signoff__seal" aria-hidden="true">
            <svg viewBox="0 0 40 40" fill="none">
              <circle cx="20" cy="20" r="18" stroke="currentColor" stroke-width="1" stroke-dasharray="2 2"/>
              <path d="M12 20l5 5 11-12" stroke="currentColor" stroke-width="1.6" stroke-linecap="round" stroke-linejoin="round"/>
            </svg>
          </div>
          <div>
            <p class="signoff__line">Signed off by your friendly herd. We'll revise this policy when the law or our practices change.</p>
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
.doc-page--privacy {
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
  isolation: isolate; overflow: hidden;
  border-bottom: 1px solid var(--border-color, #2a2a3a);
  background:
    radial-gradient(800px 400px at 20% 0%, rgba(0,240,255,0.08), transparent 70%),
    linear-gradient(180deg, var(--bg-primary), var(--bg-secondary, #12121a));
}
.hero__bg { position: absolute; inset: 0; z-index: -1; }
.hero__scan {
  position: absolute; inset: 0;
  background-image:
    linear-gradient(rgba(0,240,255,0.04) 1px, transparent 1px),
    linear-gradient(90deg, rgba(0,240,255,0.04) 1px, transparent 1px);
  background-size: 40px 40px;
  mask-image: radial-gradient(ellipse 80% 60% at 70% 30%, #000 30%, transparent 80%);
}
.hero__glow {
  position: absolute; width: 600px; height: 600px;
  top: -260px; right: -160px;
  border-radius: 50%; filter: blur(120px);
  background: radial-gradient(circle, rgba(0,240,255,0.35), transparent 70%);
}

.hero__inner {
  max-width: 1200px; margin: 0 auto;
  display: grid; grid-template-columns: 1fr; gap: 40px;
  align-items: center;
}
@media (min-width: 880px) {
  .hero__inner { grid-template-columns: 260px 1fr; gap: 56px; }
}

.shield {
  position: relative; width: 200px; margin: 0 auto;
  filter: drop-shadow(0 12px 30px rgba(0,240,255,0.35));
}
.shield svg { width: 100%; height: auto; display: block; }
.shield__scan-line {
  position: absolute; left: 8%; right: 8%; top: 30%;
  height: 1px;
  background: linear-gradient(90deg, transparent, var(--primary), transparent);
  animation: scan 3.6s ease-in-out infinite;
  opacity: 0.7;
}
@keyframes scan {
  0%, 100% { transform: translateY(-30px); opacity: 0.3; }
  50%      { transform: translateY(60px);  opacity: 1; }
}

.hero__copy { min-width: 0; }
.hero__eyebrow {
  display: inline-flex; align-items: center; gap: 8px;
  font-size: 12px; font-weight: 600; letter-spacing: 0.14em; text-transform: uppercase;
  color: var(--text-secondary, #a0a0b0); margin-bottom: 14px;
}
.dot {
  width: 8px; height: 8px; border-radius: 50%;
  background: var(--primary); box-shadow: 0 0 10px var(--primary);
  animation: blink 2.4s ease-in-out infinite;
}
@keyframes blink { 0%, 100% { opacity: 1; } 50% { opacity: 0.4; } }

.hero__title {
  font-family: 'Space Grotesk', 'Inter', sans-serif;
  font-size: clamp(2.2rem, 1.5rem + 3vw, 3.4rem);
  font-weight: 700; letter-spacing: -0.02em; line-height: 1.05;
  margin: 0 0 14px;
  background: linear-gradient(180deg, #fff 0%, #8a8a9c 140%);
  -webkit-background-clip: text; background-clip: text; color: transparent;
}
.hero__lede {
  margin: 0 0 24px; font-size: 1.0625rem; color: var(--text-secondary); max-width: 56ch;
}

.badges { display: flex; flex-wrap: wrap; gap: 10px; }
.badge {
  display: inline-flex; align-items: center; gap: 8px;
  padding: 7px 13px; border-radius: 999px;
  font-size: 0.8125rem; font-weight: 600;
  background: var(--bg-secondary, #12121a); color: var(--text-secondary);
  border: 1px solid var(--border-color);
}
.badge svg { width: 13px; height: 13px; }
.badge--gdpr {
  background: rgba(0,240,255,0.10);
  color: var(--primary);
  border-color: color-mix(in oklab, var(--primary) 35%, transparent);
  box-shadow: 0 0 24px -8px rgba(0,240,255,0.5);
}

/* ---------- SHELL (same skeleton as Help, slightly different accents) ---------- */
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
    color: var(--text-muted, #6a6c7c); margin: 0 0 14px 4px;
  }
  .toc ol { list-style: none; margin: 0; padding: 0; }
  .toc a {
    display: grid; grid-template-columns: 28px 1fr 8px;
    align-items: center; gap: 10px;
    padding: 9px 12px; border-radius: 8px;
    color: var(--text-secondary); text-decoration: none;
    font-size: 0.875rem; line-height: 1.3;
    transition: color 140ms ease, background 140ms ease;
  }
  .toc__num {
    font-family: 'JetBrains Mono', monospace; font-size: 11px;
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
  .toc__panel-body code { font-family: 'JetBrains Mono', monospace; color: var(--primary); }
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

/* ---------- content ---------- */
.doc-content { min-width: 0; }
.sec {
  padding: 36px 0;
  border-top: 1px solid var(--border-color);
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
  font-size: 0.8125rem; font-weight: 700;
  padding: 6px 10px; border-radius: 8px;
  color: var(--tone);
  background: color-mix(in oklab, var(--tone) 14%, transparent);
  border: 1px solid color-mix(in oklab, var(--tone) 35%, transparent);
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

.sec h3 {
  font-family: 'Space Grotesk', sans-serif;
  font-size: 1.0625rem; font-weight: 600;
  margin: 22px 0 6px; color: var(--text-primary);
}
.sec p { max-width: var(--measure); margin: 0 0 12px; }
.sec a { color: var(--tone); text-decoration: underline; text-underline-offset: 3px; }
.sec a:focus-visible { outline: 2px solid var(--tone); outline-offset: 2px; border-radius: 2px; }
.sec code {
  font-family: 'JetBrains Mono', monospace; font-size: 0.85em;
  background: var(--bg-tertiary); border: 1px solid var(--border-color);
  padding: 1px 6px; border-radius: 4px; color: var(--text-primary);
}

/* key + callout + grid + signoff */
.key {
  margin: 20px 0; padding: 18px 20px;
  border: 1px solid color-mix(in oklab, var(--tone) 35%, var(--border-color));
  background: color-mix(in oklab, var(--tone) 8%, var(--bg-secondary));
  border-radius: 12px; max-width: var(--measure);
}
.key__tag {
  display: inline-block; font-family: 'JetBrains Mono', monospace; font-size: 11px;
  color: var(--tone); text-transform: uppercase; letter-spacing: 0.12em; margin-bottom: 6px;
}
.key p { margin: 0; }

.callout {
  display: grid; grid-template-columns: 36px 1fr; gap: 16px;
  padding: 22px 24px; margin: 16px 0; border-radius: 14px;
  background: linear-gradient(180deg, color-mix(in oklab, var(--tone) 12%, var(--bg-secondary)), var(--bg-secondary));
  border: 1px solid color-mix(in oklab, var(--tone) 35%, var(--border-color));
  max-width: var(--measure);
}
.callout__icon {
  width: 36px; height: 36px; display: grid; place-items: center;
  border-radius: 10px; background: color-mix(in oklab, var(--tone) 18%, transparent);
  color: var(--tone);
}
.callout__icon svg { width: 18px; height: 18px; }
.callout__title { margin: 0 0 6px; font-family: 'Space Grotesk', sans-serif; color: var(--tone); font-size: 1rem; font-weight: 700; }
.callout__body p:last-child { margin-bottom: 0; }

.grid {
  display: grid; grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
  gap: 12px; margin: 16px 0; max-width: var(--measure);
}
.grid__card {
  padding: 18px; border-radius: 12px;
  background: var(--bg-secondary); border: 1px solid var(--border-color);
}
.grid__metric {
  display: block; font-family: 'Space Grotesk', sans-serif;
  font-size: 1.5rem; font-weight: 700; color: var(--tone);
  margin-bottom: 4px;
}
.grid__card p { margin: 0; font-size: 0.875rem; color: var(--text-secondary); }

.signoff {
  margin-top: 56px; padding-top: 24px;
  border-top: 1px solid var(--border-color);
  display: flex; align-items: center; gap: 16px;
}
.signoff__seal {
  width: 44px; height: 44px; display: grid; place-items: center;
  color: var(--primary);
  border-radius: 12px;
  background: rgba(0,240,255,0.08);
  border: 1px solid color-mix(in oklab, var(--primary) 30%, transparent);
}
.signoff__seal svg { width: 22px; height: 22px; }
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
  .shield__scan-line, .dot, .sec.is-flash::before { animation: none !important; }
  .totop, .toc a { transition: none; }
}
</style>
