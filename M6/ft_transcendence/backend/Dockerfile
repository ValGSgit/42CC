# Backend Dockerfile (Production)

# ── Stage 1: Build ────────────────────────────────────────────
FROM node:24-alpine3.22 AS builder

WORKDIR /app

# Install deps before copying source — layer is only re-built when package*.json changes.
COPY package*.json ./
RUN npm install --no-audit --no-fund --loglevel=error --omit=dev

# Copy source (after install so code changes don't bust the npm cache layer).
COPY . .

# prune devDeps, and clear npm cache — all in one layer
# so no intermediate fat state is committed to the image.
RUN npm prune --omit=dev \
  && npm cache clean --force

# ── Stage 2: Runtime ──────────────────────────────────────────
FROM node:24-alpine3.22

WORKDIR /app

# Healthcheck uses curl, so it must exist in the runtime image.
RUN apk add --no-cache curl

# Copy only what the server needs to run — no devDeps, no build tooling, no test files.
COPY --chown=node:node --from=builder /app/node_modules ./node_modules
COPY --chown=node:node --from=builder /app/src          ./src
COPY --chown=node:node --from=builder /app/prisma       ./prisma
COPY --chown=node:node --from=builder /app/prisma.config.js ./prisma.config.js
COPY --chown=node:node --from=builder /app/package.json ./package.json

COPY --chmod=755 tools/docker-entrypoint.sh /usr/local/bin/docker-entrypoint.sh
COPY --chmod=755 tools/start-prisma-studio.sh /usr/local/bin/start-prisma-studio.sh
RUN sed -i 's/\r$//' /usr/local/bin/docker-entrypoint.sh
RUN sed -i 's/\r$//' /usr/local/bin/start-prisma-studio.sh

# Create uploads directories with correct ownership
RUN mkdir -p /app/uploads && \
  chown -R node:node /app/uploads

USER node

# Port the Express server binds to (matches .env API_PORT; documentation only).
ARG API_PORT=3000
EXPOSE ${API_PORT}

ENTRYPOINT ["docker-entrypoint.sh"]
CMD ["node", "src/index.js"]
