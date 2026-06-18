#!/usr/bin/env bash

set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/.."
printf 'Running aggressive Docker cleanup (global).\n'
docker compose down --rmi all --volumes --remove-orphans || true

# Remove root-owned generated dirs via a throwaway alpine container (a host `rm`
# would need sudo, since these were created by root inside containers). Do this
# BEFORE pruning images: otherwise the prune wipes alpine and we have to re-pull
# it from the network just to delete files — leaving a fresh image behind. Run
# here, an existing alpine layer is reused and then swept up by the prune below.
docker run --rm -v "$(pwd):/workdir" -w /workdir alpine rm -rf backend/uploads backend/prisma/migrations

docker system prune -af --volumes
docker builder prune -af

printf 'Aggressive Docker cleanup complete\n'
