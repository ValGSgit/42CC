import { io } from "socket.io-client";

let socket = null;

function isSocketAlive(instance) {
  return !!instance && !instance.disconnected;
}

/**
 * Derive the Socket.io server URL from the API base URL.
 * When running behind nginx (same origin), we connect to '/' so
 * the browser uses the page origin and the /socket.io/ path is
 * proxied by nginx to the backend.
 */
function getSocketUrl() {
  const apiUrl = import.meta.env.VITE_API_URL || "/api";
  // Same-origin (/api) — connect to current host
  if (apiUrl.startsWith("/")) return undefined; // io() with no arg = same origin
  // Full URL — strip the /api suffix
  try {
    const url = new URL(apiUrl);
    return url.origin;
  } catch {
    return undefined;
  }
}

export function connectSocket() {
  if (isSocketAlive(socket)) return socket;
  const url = getSocketUrl();
  socket = io(url, {
    withCredentials: true,
    autoConnect: true,
    transports: ["websocket", "polling"],
    path: "/socket.io/",
  });
  return socket;
}

export function disconnectSocket() {
  if (socket) {
    socket.disconnect();
    socket = null;
  }
}

export function getSocket() {
  return socket;
}

export function isSocketConnected() {
  return socket?.connected === true;
}

export { socket };
