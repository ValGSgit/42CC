import config from "./index.js";

const requireConfig = (value, name) => {
  if (value == null || Number.isNaN(value) || value === "") {
    throw new Error(`Missing required configuration: ${name}`);
  }
};

export const validateConfig = () => {
  // Server
  requireConfig(config.port, "API_PORT");
  requireConfig(config.nodeEnv, "NODE_ENV");

  // Database
  requireConfig(config.db.host, "DB_HOST");
  requireConfig(config.db.port, "DB_PORT");
  requireConfig(config.db.name, "DB_NAME");
  requireConfig(config.db.user, "DB_USER");
  requireConfig(config.db.password, "DB_PASSWORD");

  // JWT
  requireConfig(config.jwt.secret, "JWT_SECRET");
  requireConfig(config.jwt.refreshSecret, "JWT_REFRESH_SECRET");
  requireConfig(config.jwt.publicApiSecret, "JWT_PUBLIC_API_SECRET");
  requireConfig(config.jwt.expiresIn, "JWT_EXPIRES_IN");
  requireConfig(config.jwt.refreshExpiresIn, "JWT_REFRESH_EXPIRES_IN");
  requireConfig(config.jwt.publicApiExpiresIn, "JWT_PUBLIC_API_EXPIRES_IN");

  // SSL
  requireConfig(config.ssl.certPath, "SSL_CERT_PATH");
  requireConfig(config.ssl.keyPath, "SSL_KEY_PATH");

  // File uploads
  requireConfig(config.uploads.dir, "UPLOAD_DIR");
};
