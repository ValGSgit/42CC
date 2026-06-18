/**
 * Application Configuration
 *
 * Centralised config from environment variables.
 * See .env.example at the project root for required variables.
 */
import { validateConfig } from "./validateConfig.js";
import ms from "ms";
import { loadEnv } from "#tools/loadEnv.js";

loadEnv();

const config = {
  port: parseInt(process.env.API_PORT, 10), // needed fallback for testing
  nodeEnv: process.env.NODE_ENV,
  envIsProd: process.env.NODE_ENV === "production",
  envIsDev: process.env.NODE_ENV === "development",

  jwt: (() => {
    // Secure-cookie + sameSite=strict is correct in prod, but over plain
    // http://localhost in dev the browser silently drops the cookie on the
    // next request — instant "logged in then immediately logged out" bug.
    const isProd = process.env.NODE_ENV === "production";
    const secureCookies = isProd;
    const sameSite = isProd ? "strict" : "lax";
    return {
      secret: process.env.JWT_SECRET,
      refreshSecret: process.env.JWT_REFRESH_SECRET,
      publicApiSecret: process.env.JWT_PUBLIC_API_SECRET,
      expiresIn: process.env.JWT_EXPIRES_IN,
      refreshExpiresIn: process.env.JWT_REFRESH_EXPIRES_IN,
      publicApiExpiresIn: process.env.JWT_PUBLIC_API_EXPIRES_IN,
      cookieOptions: {
        httpOnly: true,
        secure: secureCookies,
        sameSite,
        maxAge: ms(process.env.JWT_EXPIRES_IN),
        path: "/",
      },
      cookieOptionsRefresh: {
        httpOnly: true,
        secure: secureCookies,
        sameSite,
        maxAge: ms(process.env.JWT_REFRESH_EXPIRES_IN),
        path: "/api/auth/refresh",
      },
    };
  })(),

  // PostgreSQL connection (Issue #7)
  db: {
    host: process.env.DB_HOST,
    port: parseInt(process.env.DB_PORT, 10),
    name: process.env.DB_NAME,
    user: process.env.DB_USER,
    password: process.env.DB_PASSWORD,
  },

  cors: {
    // Empty list ⇒ no origin is allowed. Returning null here would tell the
    // cors package "reflect any origin" — failing open on a missing env is
    // worse than rejecting requests until ops fix the config.
    origins: process.env.CORS_ORIGINS
      ? process.env.CORS_ORIGINS.split(",").map((s) => s.trim()).filter(Boolean)
      : [],
  },

  // Explicit frontend URL used for production HTTPS redirects.
  // Falls back to the first CORS origin when not set.
  frontendUrl: process.env.FRONTEND_URL,

  rateLimit: {
    windowMs: 15 * 60 * 1000, // 15 minutes
    max: 1000,
  },

  rateLimitPublicApi: {
    windowMs: 60 * 1000, // 1 minutes
    max: 30,
  },

  password: {
    minLength: 8,
    requireUppercase: true,
    requireLowercase: true,
    requireNumber: true,
  },

  // SSL/TLS certificates
  ssl: {
    certPath: process.env.SSL_CERT_PATH,
    keyPath: process.env.SSL_KEY_PATH,
  },

  // Groq AI (help desk) — up to 3 keys rotated round-robin
  groq: {
    apiKeys: [
      process.env.GROQ_API_KEY1,
      process.env.GROQ_API_KEY2,
      process.env.GROQ_API_KEY3,
    ].filter(Boolean),
    model: process.env.GROQ_MODEL || "llama-3.1-8b-instant",
  },

  // File uploads
  uploads: {
    dir: process.env.UPLOAD_DIR,
    maxSizeBytes: parseInt(process.env.UPLOAD_MAX_SIZE, 10) || 10 * 1024 * 1024, // 10 MB
    allowedMimeTypes: [
      "image/jpeg",
      "image/png",
      "image/gif",
      "image/webp",
      "image/svg+xml",
      "application/pdf",
      "text/plain",
      "text/csv",
      "application/json",
      "application/xml",
      "text/xml",
    ],
    // SVG is allowed as an upload (above) but is script-capable, so it is
    // intentionally excluded from imageMimeTypes — uploadSecurity.js forces
    // Content-Disposition: attachment for any mime type NOT in this list
    // (and explicitly for SVG via INLINE_BLOCKED_MIME). Defense in depth:
    // X-Content-Type-Options: nosniff + magic-byte validation on upload are
    // the actual safeguards; the inline/attachment toggle is belt-and-braces.
    imageMimeTypes: ["image/jpeg", "image/png", "image/gif", "image/webp"],
  },
};

export default config;

validateConfig();
