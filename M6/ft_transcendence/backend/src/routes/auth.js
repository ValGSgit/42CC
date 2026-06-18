/**
 * Auth Routes
 */
import express from "express";
import rateLimit from "express-rate-limit";
import {
  register,
  login,
  logout,
  refresh,
  me,
} from "../controllers/authController.js";
import { optionalAuth } from "../middleware/auth.js";
import {
  authLoginValidation,
  authRegisterValidation,
} from "#validators/authValidator.js";

const router = express.Router();

// Keep strict limits in production, but allow larger volume in dev/e2e runs.
// The global /api limiter (1000/15 min) is too loose to prevent brute-force.
// AUTH_RATE_LIMIT_MAX env var overrides the default (useful for CI/E2E runs).
const authLimiterMax = parseInt(process.env.AUTH_RATE_LIMIT_MAX, 10) ||
  (process.env.NODE_ENV === "production" ? 50 : 1000);
const authLimiter = rateLimit({
  windowMs: 15 * 60 * 1000,
  max: authLimiterMax,
  skip: () => process.env.NODE_ENV === "test",
  message: "Too many authentication attempts, please try again later.",
  standardHeaders: true,
  legacyHeaders: false,
});

// Brute-force defence: a much tighter cap on /login specifically.
// Successful logins do not count toward the cap so legitimate users with
// flaky networks aren't penalised.
const loginBruteForceMax = parseInt(process.env.LOGIN_RATE_LIMIT_MAX, 10) ||
  (process.env.NODE_ENV === "production" ? 10 : 200);
const loginLimiter = rateLimit({
  windowMs: 15 * 60 * 1000,
  max: loginBruteForceMax,
  skip: () => process.env.NODE_ENV === "test",
  skipSuccessfulRequests: true,
  message: "Too many failed login attempts, please try again later.",
  standardHeaders: true,
  legacyHeaders: false,
});

/**
 * @openapi
 * /auth/register:
 *   post:
 *     tags: [Auth]
 *     summary: Register a new account
 *     security: []
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             required: [username, email, password]
 *             properties:
 *               username: { type: string, minLength: 3, maxLength: 32, example: alpaca42 }
 *               email: { type: string, format: email, example: alpaca@example.com }
 *               password: { type: string, minLength: 8, description: "Must contain uppercase, lowercase, and a digit", example: Secret123 }
 *     responses:
 *       201:
 *         description: Registered — returns tokens and user object
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 accessToken: { type: string }
 *                 refreshToken: { type: string }
 *                 user: { $ref: '#/components/schemas/User' }
 *       400: { description: Validation error }
 *       409: { description: Username or email already taken }
 */
// Per-route 4 KB body cap. Credentials + username + email are well under
// 1 KB; the global 256 KB limit doesn't help against credential-stuffing
// DoS (lots of tiny requests). Tight per-route cap keeps the parser cheap.
router.post(
  "/register",
  express.json({ limit: "4kb" }),
  authLimiter,
  authRegisterValidation(),
  register,
);

/**
 * @openapi
 * /auth/login:
 *   post:
 *     tags: [Auth]
 *     summary: Login with username (or email) and password
 *     security: []
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             required: [username, password]
 *             properties:
 *               username: { type: string, description: "Username OR email address", example: alpaca42 }
 *               password: { type: string, example: Secret123 }
 *     responses:
 *       200:
 *         description: Login successful — returns tokens and user object
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 accessToken: { type: string }
 *                 refreshToken: { type: string }
 *                 user: { $ref: '#/components/schemas/User' }
 *       401: { description: Invalid credentials }
 */
router.post(
  "/login",
  express.json({ limit: "4kb" }),
  loginLimiter,
  authLimiter,
  authLoginValidation(),
  login,
);

/**
 * @openapi
 * /auth/logout:
 *   post:
 *     tags: [Auth]
 *     summary: Logout (invalidates refresh token)
 *     responses:
 *       200:
 *         description: Logged out
 */
router.post("/logout", optionalAuth, logout);

/**
 * @openapi
 * /auth/refresh:
 *   post:
 *     tags: [Auth]
 *     summary: Exchange a refresh token for a new access token
 *     security: []
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             required: [refreshToken]
 *             properties:
 *               refreshToken: { type: string }
 *     responses:
 *       200:
 *         description: New tokens issued
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 accessToken: { type: string }
 *                 refreshToken: { type: string }
 *       401: { description: Invalid or expired refresh token }
 */
router.post("/refresh", refresh);

/**
 * @openapi
 * /auth/me:
 *   get:
 *     tags: [Auth]
 *     summary: Get the currently authenticated user
 *     responses:
 *       200:
 *         description: Current user
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 user: { $ref: '#/components/schemas/User' }
 *       401: { description: Not authenticated }
 */
router.get("/me", optionalAuth, me);

export default router;
