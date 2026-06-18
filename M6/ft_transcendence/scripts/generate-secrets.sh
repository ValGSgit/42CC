#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
# shellcheck source=./lib.sh
source "$SCRIPT_DIR/lib.sh"

root=$(cd "$SCRIPT_DIR/.." && pwd)
ensure_env_file "$root"

bash "$SCRIPT_DIR/set-ip.sh"

db_name=$(env_value DB_NAME "$root/.env")
db_name=${db_name:-alpacaparty}
db_user=$(env_value DB_USER "$root/.env")
db_user=${db_user:-alpacaparty}
db_host=$(env_value DB_HOST "$root/.env")
db_host=${db_host:-postgres}
db_port=$(env_value DB_PORT "$root/.env")
db_port=${db_port:-5432}
db_pass=$(openssl rand -hex 24)

sed_in_place "s|^DB_PASSWORD=.*|DB_PASSWORD=$db_pass|" "$root/.env"

db_url="postgresql://$db_user:$db_pass@$db_host:$db_port/$db_name?schema=public"
if grep -q '^DATABASE_URL=' "$root/.env"; then
	sed_in_place "s|^DATABASE_URL=.*|DATABASE_URL=$db_url|" "$root/.env"
else
	printf '\nDATABASE_URL=%s\n' "$db_url" >> "$root/.env"
fi

printf 'Randomized DB_PASSWORD and DATABASE_URL\n'

for var in JWT_SECRET JWT_REFRESH_SECRET JWT_PUBLIC_API_SECRET; do
	secret=$(openssl rand -hex 40)
	sed_in_place "s|^$var=.*|$var=$secret|" "$root/.env"
	printf 'Randomized %s\n' "$var"
done

printf 'Secrets written to .env\n'
