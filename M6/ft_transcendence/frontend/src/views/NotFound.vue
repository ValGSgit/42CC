<template>
  <div class="not-found">
    <!-- ambient background -->
    <div class="nf-bg" aria-hidden="true">
      <span class="blob blob-cyan"></span>
      <span class="blob blob-magenta"></span>
      <div class="dot-grid"></div>
      <div class="scanlines"></div>
      <div class="floor"></div>
      <div class="vignette"></div>
    </div>

    <!-- corner brackets -->
    <span class="bracket b-tl" aria-hidden="true"></span>
    <span class="bracket b-tr" aria-hidden="true"></span>
    <span class="bracket b-bl" aria-hidden="true"></span>
    <span class="bracket b-br" aria-hidden="true"></span>

    <main class="nf-stage">
      <!-- breadcrumbs / status pill -->
      <div class="status-pill">
        <span class="dot"></span>
        <span class="status-text">SIGNAL LOST</span>
        <span class="sep">/</span>
        <span class="status-meta">SECTOR_07</span>
      </div>

      <!-- the big glitched 404 -->
      <h1 class="code" data-text="404" aria-label="404">
        <span class="layer base">404</span>
        <span class="layer r" aria-hidden="true">404</span>
        <span class="layer g" aria-hidden="true">404</span>
        <span class="layer b" aria-hidden="true">404</span>
        <span class="slice s1" aria-hidden="true">404</span>
        <span class="slice s2" aria-hidden="true">404</span>
        <span class="slice s3" aria-hidden="true">404</span>
      </h1>

      <p class="message">Page not found.</p>
      <p class="sub">
        The path you followed leads off the map. The alpacas haven't scouted this far.
      </p>

      <!-- terminal diagnostic -->
      <pre class="terminal" aria-hidden="true"><span class="t-line"><span class="t-prompt">root@alpaca-party</span><span class="t-colon">:</span><span class="t-path">~/router</span><span class="t-dollar">$</span> trace --route <span class="t-val">$REQUEST</span></span><span class="t-line"><span class="t-dim">  → hop 01  </span><span class="t-ok">OK</span><span class="t-dim">    gateway.alpaca.party</span></span><span class="t-line"><span class="t-dim">  → hop 02  </span><span class="t-ok">OK</span><span class="t-dim">    edge-na-04</span></span><span class="t-line"><span class="t-dim">  → hop 03  </span><span class="t-err">FAIL</span><span class="t-dim">  resource not found</span></span><span class="t-line"><span class="t-dim">  → status </span><span class="t-err">404</span><span class="t-dim"> · ttl exceeded · 0 packets returned</span></span><span class="t-line"><span class="t-prompt">root@alpaca-party</span><span class="t-colon">:</span><span class="t-path">~/router</span><span class="t-dollar">$</span> <span class="caret">█</span></span></pre>

      <!-- action -->
      <div class="actions">
        <router-link to="/" class="btn-home">
          <svg viewBox="0 0 24 24" class="ico" aria-hidden="true">
            <path d="M4 11l8-7 8 7v9a1 1 0 0 1-1 1h-4v-6h-6v6H5a1 1 0 0 1-1-1v-9z"
                  fill="none" stroke="currentColor" stroke-width="2" stroke-linejoin="round"/>
          </svg>
          <span>Return to Base</span>
          <span class="kbd">↵</span>
        </router-link>

        <button class="btn-ghost" type="button" @click="$router && $router.go(-1)">
          <svg viewBox="0 0 24 24" class="ico" aria-hidden="true">
            <path d="M15 6l-6 6 6 6" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
          </svg>
          <span>Go Back</span>
        </button>
      </div>

      <!-- coords -->
      <div class="coords">
        <span class="c"><span class="c-k">ERR</span><span class="c-v">RES_NOT_FOUND</span></span>
        <span class="c-sep"></span>
        <span class="c"><span class="c-k">LAT</span><span class="c-v">∅.∅∅∅∅</span></span>
        <span class="c-sep"></span>
        <span class="c"><span class="c-k">LON</span><span class="c-v">∅.∅∅∅∅</span></span>
        <span class="c-sep"></span>
        <span class="c"><span class="c-k">REF</span><span class="c-v">{{ randomRef }}</span></span>
      </div>
    </main>
  </div>
</template>

<script setup>
// Reference id is generated once on mount; pure presentation, no logic.
const randomRef = (() => {
  const a = Math.random().toString(16).slice(2, 6).toUpperCase();
  const b = Math.random().toString(16).slice(2, 6).toUpperCase();
  return `0x${a}-${b}`;
})();
</script>

<style src="../styles/views/NotFound.css" scoped></style>
