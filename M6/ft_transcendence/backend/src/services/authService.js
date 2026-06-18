/**
 * Auth Service — JWT generation, password hashing
 */
import jwt from "jsonwebtoken";
import bcrypt from "bcrypt";
import config from "../config/index.js";

const SALT_ROUNDS = 12;

const AuthService = {
  /**
   * Hash a plain-text password.
   */
  async hashPassword(password) {
    return bcrypt.hash(password, SALT_ROUNDS);
  },

  /**
   * Compare plain-text password with a hash.
   */
  async comparePassword(password, hash) {
    return bcrypt.compare(password, hash);
  },

  /**
   * Generate access token (short-lived).
   */
  generateAccessToken(user) {
    return jwt.sign(
      { id: user.id, username: user.username },
      config.jwt.secret,
      { expiresIn: config.jwt.expiresIn },
    );
  },

  /**
   * Generate refresh token (long-lived).
   */
  generateRefreshToken(user) {
    return jwt.sign(
      { id: user.id, type: "refresh" },
      config.jwt.refreshSecret,
      {
        expiresIn: config.jwt.refreshExpiresIn,
      },
    );
  },

  /**
   * Generate public api token
   */
  generatePublicApiToken(user) {
    return jwt.sign({ id: user.id }, config.jwt.publicApiSecret, {
      expiresIn: config.jwt.publicApiExpiresIn,
    });
  },

  /**
   * Verify and decode access token.
   * @returns {object|null} decoded payload or null if invalid.
   */
  verifyToken(token) {
    try {
      return jwt.verify(token, config.jwt.secret, { algorithms: ["HS256"] });
    } catch {
      return null;
    }
  },

  /**
   * Verify and decode refresh token.
   * @returns {object|null} decoded payload or null if invalid.
   */
  verifyRefreshToken(token) {
    try {
      return jwt.verify(token, config.jwt.refreshSecret, {
        algorithms: ["HS256"],
      });
    } catch {
      return null;
    }
  },

  /**
   * Verify and decode public api token.
   * @returns {object|null} decoded payload or null if invalid.
   */
  verifyPublicApiToken(token) {
    try {
      return jwt.verify(token, config.jwt.publicApiSecret, {
        algorithms: ["HS256"],
      });
    } catch {
      return null;
    }
  },

  /**
   * Validate password against policy from config.
   * @returns {{ valid: boolean, errors: string[] }}
   */
  validatePassword(password) {
    const errors = [];
    const { minLength, requireUppercase, requireLowercase, requireNumber } =
      config.password;

    if (!password || password.length < minLength) {
      errors.push(`Password must be at least ${minLength} characters`);
    }
    if (requireUppercase && !/[A-Z]/.test(password)) {
      errors.push("Password must contain at least one uppercase letter");
    }
    if (requireLowercase && !/[a-z]/.test(password)) {
      errors.push("Password must contain at least one lowercase letter");
    }
    if (requireNumber && !/\d/.test(password)) {
      errors.push("Password must contain at least one number");
    }

    return { valid: errors.length === 0, errors };
  },
};

export default AuthService;
