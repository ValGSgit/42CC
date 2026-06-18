#!/usr/bin/env bash

set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/.."
docker compose down --rmi all --volumes --remove-orphans
docker volume prune -f >/dev/null 2>&1 || true
printf 'Full cleanup complete\n'
