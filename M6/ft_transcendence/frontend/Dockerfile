# Frontend Dockerfile (Production)

# ── Stage 1: Build ────────────────────────────────────────────
FROM node:24-alpine3.22 AS builder

WORKDIR /app

ARG VITE_API_URL=/api
ENV VITE_API_URL=$VITE_API_URL
ENV NPM_CONFIG_UPDATE_NOTIFIER=false

# Copy package files and install dependencies
COPY package*.json ./
RUN npm install --no-audit --no-fund --loglevel=error

# Copy source and build
COPY . .
RUN npm run build

# ── Stage 2: Production runtime ──────────────────────────────
FROM nginx:alpine

# SPA nginx config as a template — the official entrypoint runs envsubst over
# /etc/nginx/templates/*.template at startup, filling in ${FRONTEND_PORT}.
COPY conf/nginx.conf /etc/nginx/templates/default.conf.template

# Copy built files from builder
COPY --from=builder /app/dist /usr/share/nginx/html

# Listen port (matches .env FRONTEND_PORT; the top-level nginx proxies here).
ARG FRONTEND_PORT=5173
EXPOSE ${FRONTEND_PORT}

# nginx starts automatically (envsubst-on-templates → conf.d/default.conf)
