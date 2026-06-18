/**
 * Shared Socket.IO authentication middleware.
 * Validates JWT from handshake, attaches socket.user.
 * Reuse across all game namespaces and the default namespace.
 */
import AuthService from "./authService.js";
import User from "../models/User.js";

export function socketAuthMiddleware() {
  return async (socket, next) => {
    try {
      const token = socket.request.cookies.jwt_token;
      if (!token)
        return next(new Error("Authentication required"));

      const decoded = AuthService.verifyToken(token);
      if (!decoded || decoded.type === "refresh")
        return next(new Error("Invalid token"));

      const user = await User.findById(decoded.id);
      if (!user)
        return next(new Error("User not found"));

      socket.user = user;
      next();
    } catch (err) {
      next(err);
    }
  };
}

/**
 * Final safety net for namespace `connection` handlers. Several match-state
 * handlers (MatchManager, BaseMatch, the two match subclasses) deref
 * `socket.user.id` directly; if a future code path ever attached a socket
 * without invoking socketAuthMiddleware (or did so out of order), those
 * derefs would throw inside an io listener and crash the namespace.
 *
 * Call once at the top of each namespace's `on('connection', ...)` callback:
 *   if (!ensureSocketAuthed(socket)) return;
 *
 * Returns true if the socket has a populated `user`. Otherwise disconnects
 * the socket and returns false so the caller can early-return.
 */
export function ensureSocketAuthed(socket) {
  if (socket?.user?.id) return true;
  try { socket?.disconnect?.(true); } catch { /* socket may already be closed */ }
  return false;
}
