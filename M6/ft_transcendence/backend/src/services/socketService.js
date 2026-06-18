/**
 * Socket Service — Real-time WebSocket hub
 *
 * Namespaces:
 *   /          — general: presence, notifications, DM chat
 *   /game      — in-game state, matchmaking, game chat
 *
 * Room conventions:
 *   user:{id}         — user's personal room
 *   dm:{minId}-{maxId} — DM conversation room
 *   room:{id}         — group chat room
 *   game:{id}         — game session room
 */
import cookieParser from "cookie-parser";
import { Server } from "socket.io";
import { debug } from "#lib/logger.js";
import Friend from "../models/Friend.js";
import Message from "../models/Message.js";
import User from "../models/User.js";
import { MatchManager } from "./MatchManager.js";
import NotificationService from "./notificationService.js";
import GamificationService from "./GamificationService.js";
import { ensureSocketAuthed, socketAuthMiddleware } from "./socketAuth.js";

// ── Token-bucket rate limiter (per-socket, per-event) ─────────
// HTTP rate limits do not apply to socket events; without this a connected
// user could flood dm:send freely.
const RATE_LIMITS = {
  "dm:send": { capacity: 20, refillPerSec: 2 }, // ~20 msgs / 10s
  "dm:read": { capacity: 60, refillPerSec: 5 },
};

function takeToken(socket, event) {
  const cfg = RATE_LIMITS[event];
  if (!cfg)
    return true;
  socket._buckets ??= {};
  const now = Date.now();
  const b = socket._buckets[event] ??= { tokens: cfg.capacity, ts: now };
  const elapsed = (now - b.ts) / 1000;
  b.tokens = Math.min(cfg.capacity, b.tokens + elapsed * cfg.refillPerSec);
  b.ts = now;
  if (b.tokens < 1) return false;
  b.tokens -= 1;
  return true;
}

const DM_MAX_LEN = 4000;

export function initializeSocket(httpServer, corsOrigins) {
  const io = new Server(httpServer, {
    cors: {
      origin: corsOrigins,
      credentials: true,
    },
    connectionStateRecovery: { maxDisconnectionDuration: 30_000 },
  });

  // Share io with NotificationService so it can push real-time notifications
  NotificationService.setIo(io);
  const minigamesNamespace = io.of('/minigames');

  // ── Auth middleware ──────────────────────────────────────────
  // io.engine.use() runs once for the engine.io upgrade. io.use() only binds
  // to the default "/" namespace; custom namespaces need their own .use().
  // Without this, /minigames had no JWT check and socket.user was undefined,
  // which silently dropped every achievement / level / win-count persist call.
  io.engine.use(cookieParser());
  io.use(socketAuthMiddleware());
  minigamesNamespace.use(socketAuthMiddleware());

  // Constructor wires up listeners on the namespace — no need to retain it.
  new MatchManager(minigamesNamespace);

  // ── Presence tracking ────────────────────────────────────────
  const onlineSockets = new Map(); // userId -> Set<socketId>

  async function markOnline(userId, socketId) {
    if (!onlineSockets.has(userId)) {
      onlineSockets.set(userId, new Set());
      await User.setOnline(userId);
      io.emit("presence", { userId, isOnline: true });
    }
    onlineSockets.get(userId).add(socketId);
  }

  async function markOffline(userId, socketId) {
    const sockets = onlineSockets.get(userId);
    if (sockets) {
      sockets.delete(socketId);
      if (sockets.size === 0) {
        onlineSockets.delete(userId);
        await User.setOffline(userId);
        io.emit("presence", { userId, isOnline: false });
      }
    }
  }

  // ── Connection handler ───────────────────────────────────────
  io.on("connection", async (socket) => {
    // Belt-and-braces: socketAuthMiddleware should always populate socket.user
    // before we get here, but if a future config change ever bypassed it we'd
    // dereference `user.id` on the very next line and crash the namespace.
    if (!ensureSocketAuthed(socket)) return;
    const { user } = socket;

    try {
      // Join personal room
      socket.join(`user:${user.id}`);
      await markOnline(user.id, socket.id);

      debug(`[socket] ${user.username} connected (${socket.id})`);
    } catch (err) {
      // console.error("[socket] connection setup failed:", err.message);
      socket.disconnect(true);
      return;
    }

    // ── Direct Messages ──────────────────────────────────────
    socket.on("dm:send", async ({ receiverId, content }, ack) => {
      try {
        if (!takeToken(socket, "dm:send"))
          return ack?.({ error: "Sending messages too fast" });
        if (typeof content !== "string" || !content.trim())
          return ack?.({ error: "Empty message" });
        if (content.length > DM_MAX_LEN)
          return ack?.({ error: `message too long (max ${DM_MAX_LEN})` });

        const recvId = Number(receiverId);
        if (!Number.isInteger(recvId) || recvId <= 0)
          return ack?.({ error: "Invalid recipient" });
        if (recvId === user.id)
          return ack?.({ error: "Cannot send a message to yourself" });

        // Prevent sending messages when either user has blocked the other.
        const blocked = await Friend.isBlockedBetween(user.id, recvId);
        if (blocked)
          return ack?.({
            error: "Cannot send message: blocked or you have blocked this user",
          });

        const friends = await Friend.areFriends(user.id, recvId);
        if (!friends)
          return ack?.({ error: "You can only message friends" });
        const msg = await Message.create({
          senderId: user.id,
          receiverId: recvId,
          content: content.trim(),
        });

        const dmRoom = `dm:${Math.min(user.id, recvId)}-${Math.max(user.id, recvId)}`;
        socket.join(dmRoom);

        // Map to snake_case for frontend compatibility
        const shaped = {
          id: msg.id,
          sender_id: msg.senderId,
          receiver_id: msg.receiverId,
          content: msg.content,
          is_read: msg.isRead,
          created_at: msg.createdAt,
          sender_username: user.username,
          sender_avatar: user.avatar,
        };

        // Send to receiver's personal room
        io.to(`user:${recvId}`).emit("dm:message", shaped);
        // Echo back to sender
        socket.emit("dm:message", shaped);

        GamificationService.onMessageSent(user.id).catch(() => {});

        // Notification (non-blocking)
        NotificationService.newMessage(recvId, user.username).catch(
          (err) => { debug("notification error (newMessage):", err.message); },
        );

        ack?.({ ok: true, message: shaped });
      } catch (err) {
        // Don't leak internal error messages (DB error strings, etc.) to the
        // client — log server-side and return a generic response.
        debug("dm:send failed:", err?.message);
        ack?.({ error: "Failed to send message" });
      }
    });

    socket.on("dm:read", async ({ senderId }) => {
      if (!takeToken(socket, "dm:read")) return;
      await Message.markAsRead(user.id, senderId).catch((err) => { debug("markAsRead error:", err.message); });
    });

    // farm:save / farm:load events were never wired up on the frontend.
    // Farm persistence lives entirely on the REST PUT /api/game/farm path.

    // ── Disconnect ───────────────────────────────────────────
    socket.on("disconnect", async (reason) => {
      debug(`[socket] ${user.username} disconnected: ${reason}`);
      try {
        await markOffline(user.id, socket.id);
      } catch (err) {
        debug(`[socket] markOffline failed: ${err.message}`);
      }
    });
  });

  return io;
}

export default initializeSocket;
