#!/usr/bin/env bash

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
root=$(cd "$SCRIPT_DIR/.." && pwd)

echo "Create directory 'ssl'"
mkdir -p "$root/ssl"

echo "Create directory 'backend/uploads'"
mkdir -p "$root/backend/uploads"

echo "Create directory 'backend/prisma/migrations'"
mkdir -p "$root/backend/prisma/migrations"
