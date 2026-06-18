#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
# shellcheck source=./lib.sh
source "$SCRIPT_DIR/lib.sh"

root=$(cd "$SCRIPT_DIR/.." && pwd)
frontend_url_raw=$(env_value FRONTEND_URL "$root/.env" 2>/dev/null || true)
frontend_url=${frontend_url_raw:-localhost}
if [[ -n "$frontend_url_raw" ]]; then
	frontend_url=$(printf '%s' "$frontend_url_raw" | sed -E 's#.*://([^:/]+).*#\1#')
fi
https_port=$(env_value HTTPS_PORT "$root/.env" 2>/dev/null || true)

https_port=${https_port:-8443}

printf '\n========== AlpacaParty ==========\n'
printf '  App          https://%s:%s\n' "$frontend_url" "$https_port"
printf '  API health   https://%s:%s/api/health\n' "$frontend_url" "$https_port"
printf '  API docs     https://%s:%s/api/docs   (Swagger UI)\n' "$frontend_url" "$https_port"
printf '\n  Showcase pointers\n'
printf '    Realtime + games   open two tabs, play Spit Royale / Alpaca Road\n'
printf '    Help-desk LLM      bottom-right chat widget (Groq)\n'
printf "    Public API         curl -k -H 'X-API-Key: <key>' https://%s:%s/api/public/users\n" "$frontend_url" "$https_port"
printf "    3D farm            'AlpacaFarm' tab (Three.js)\n"
printf '\n  make logs  ·  make ps  ·  make down  ·  make re\n\n'
