#!/usr/bin/env bash

set -euo pipefail

env_value() {
	local key=$1
	local file=$2
	awk -F= -v key="$key" '$1 == key { sub("^[^=]*=", ""); value = $0 } END { if (value != "") print value }' "$file"
}

sed_in_place() {
	if [[ $(uname -s) == Darwin ]]; then
		sed -i '' "$@"
	else
		sed -i "$@"
	fi
}

detect_ip() {
	if [[ $(uname -s) == Darwin ]]; then
		local mac_iface
		mac_iface=$(route get default | awk '/interface:/ {print $2}')
		ipconfig getifaddr "$mac_iface"
	else
		hostname -I | awk '{print $1}'
	fi
}

ensure_env_file() {
	local root=$1
	if [[ ! -f "$root/.env" ]]; then
		cp "$root/.env.example" "$root/.env"
		printf 'Created .env from .env.example\n'
	fi
}
