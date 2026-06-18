#!/bin/sh

npx prisma studio --port 5555 --url "$DATABASE_URL" --browser none
