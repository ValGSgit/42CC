#!/bin/sh
set -e

# Apply database migrations. `migrate deploy` only runs committed migrations;
# if none exist they will be created
if [ -d "prisma/migrations" ] && find prisma/migrations -type f -name "*.sql" | grep -q .; then
  echo "Deploying database migrations"
  npx prisma migrate deploy
else
  echo "No migrations found - create new"
  npx prisma migrate dev --name init
fi

echo "Generating Prisma client"
npx prisma generate

echo "Seeding database"
npm run seed

echo "Building API docs"
rm -f /app/src/docs/swagger-output-public-api.json
npm run buildDocs

exec "$@"
