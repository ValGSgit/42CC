#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
# shellcheck source=./lib.sh
source "$SCRIPT_DIR/lib.sh"

root=$(cd "$SCRIPT_DIR/.." && pwd)
mkdir -p "$root/ssl"

if [[ ! -f "$root/ssl/cert.pem" ]]; then
	ip=$(detect_ip)
	if [[ -z "$ip" ]]; then
		echo 'Error: Could not detect IP address.'
		exit 1
	fi

	echo "Generating certificate for IP: $ip"
	openssl req -x509 -newkey rsa:2048 -nodes \
		-keyout "$root/ssl/key.pem" -out "$root/ssl/cert.pem" -days 365 \
		-subj '/CN=localhost' \
		-addext "subjectAltName=DNS:localhost,DNS:frontend,DNS:backend,DNS:nginx,IP:127.0.0.1,IP:$ip,DNS:$ip.nip.io" \
		2>/dev/null
	chmod +rw "$root/ssl/key.pem" "$root/ssl/cert.pem"
	printf 'Self-signed certificate generated in ssl/\n'
else
	printf 'Certificate already exists, skipping\n'
fi
