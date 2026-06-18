/**
 * Per-user (authenticated) and per-IP (anonymous) rate limiters for the
 * user-generated content surface: posts, comments, chat, uploads, friend
 * requests. Applied per-route so a user abusing one
 * endpoint does not starve others.
 */
import rateLimit from "express-rate-limit";

const userKey = (req) =>
  (req.user?.id ? `u:${req.user.id}` : `ip:${req.ip}`);

const make = (windowMs, max, message) =>
  rateLimit({
    windowMs,
    max,
    standardHeaders: true,
    legacyHeaders: false,
    keyGenerator: userKey,
    message: { error: { message } },
  });

export const postWriteLimiter     = make(60_000,  10, "Too many posts — slow down.");
export const commentWriteLimiter  = make(60_000,  30, "Too many comments — slow down.");
export const likeLimiter          = make(60_000, 120, "Too many like/unlike actions.");
export const uploadLimiter        = make(60_000,  20, "Too many uploads — try again in a minute.");
export const chatSendLimiter      = make(10_000,  20, "Sending messages too fast.");
export const friendRequestLimiter = make(60_000,  20, "Too many friend requests — slow down.");
export const helpdeskLimiter      = make(60_000,  20, "Too many help desk messages — slow down.");
export const apiKeyRegenerateLimiter = make(3_600_000, 3, "Too many API key regenerations — try again in an hour.");
