# 42CC — Portfolio showcase site

The animated GitHub Pages site for this repo. A single self-contained
[`index.html`](./index.html) — vanilla HTML/CSS/JS, no build step. Fonts load
from Google Fonts; everything else (SVG icons, the particle-network background,
animations) is inline.

Live capstone app: **[42alpaca.party](https://42alpaca.party)**

## Run locally

Just open the file:

```sh
open docs/index.html        # macOS
xdg-open docs/index.html    # Linux
```

Or serve it (any static server works):

```sh
python3 -m http.server -d docs 8000
# then visit http://localhost:8000
```

## Deploy on GitHub Pages

Two paths are wired up — pick **one**:

**A. GitHub Actions (recommended)** — `.github/workflows/pages.yml` is already
included. In the repo: **Settings → Pages → Build and deployment → Source:
GitHub Actions**. Every push to `main` that touches `docs/` redeploys.

**B. Deploy from a branch** — **Settings → Pages → Source: Deploy from a branch
→ Branch: `main` / folder: `/docs`**. No workflow needed; disable the Action if
you go this route.

Either way the site publishes at:

```
https://valgsgit.github.io/42CC/
```

## Editing content

Project cards, the M1–M6 timeline, and tech badges are all driven by the
`MODULES`, `PROJECTS` and `TECH` arrays in the `<script>` block of
`index.html`. Add or edit an entry there and the grid, filters and counts
update automatically.
