#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
# shellcheck source=./lib.sh
source "$SCRIPT_DIR/lib.sh"

root=$(cd "$SCRIPT_DIR/.." && pwd)
ensure_env_file "$root"

ip=$(detect_ip)
if [[ -z "$ip" ]]; then
	echo 'Error: Could not detect IP address.'
	exit 1
fi

https_port=$(env_value HTTPS_PORT "$root/.env")
https_port=${https_port:-8443}

sed_in_place "s/{MY_IP}/$ip/g" "$root/.env"
sed_in_place "s/{HTTPS_PORT}/$https_port/g" "$root/.env"

printf 'Updated .env with IP %s and HTTPS port %s\n' "$ip" "$https_port"
