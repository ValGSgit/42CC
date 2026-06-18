#!/bin/sh
set -e

# Creates package lock if it does not exist
echo "run npm install"
npm install

exec "$@"
