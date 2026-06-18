import { debug } from "#lib/logger.js";
import { randomUUID } from "crypto";
import { AlpacaRoadMatch } from "./AlpacaRoadMatch.js";
import { SpitRoyalMatch } from "./SpitRoyaleMatch.js";
import { ensureSocketAuthed } from "./socketAuth.js";

const GAME_REGISTRY = {
  2: SpitRoyalMatch,
  4: AlpacaRoadMatch,
};

// Reverse lookup so we can resolve match → typeKey in O(1) instead of
// walking GAME_REGISTRY for every public-rooms broadcast.
const TYPE_KEY_BY_CTOR = new Map(
  Object.entries(GAME_REGISTRY).map(([k, Ctor]) => [Ctor, Number(k)]),
);

// Max concurrent players per game type — used by join/create gating.
const PLAYER_CAP = {
  2: 10, // SpitRoyale: up to 10 players in the arena.
  4: 4,  // AlpacaRoad: 4 lanes, 4 players max.
};

function acceptsJoin(typeKey, status) {
  // Both game types now use the same lobby model: a room is joinable only
  // while it is gathering players in the LOBBY. Once everyone readies up and
  // the match starts (PLAYING), it leaves the public list and no longer
  // accepts joins — Spit Royale used to accept mid-game joins, which skipped
  // the ready screen entirely.
  if (typeKey === 4 || typeKey === 2) return status === 'LOBBY';
  return false;
}

export class MatchManager {
  constructor(ioNamespace) {
    this.io = ioNamespace;
    this.matches = new Map();
    this.socketToSession = new Map();
    this.sessionToMatch = new Map();
    this.disconnectTimers = new Map();

    this.setupListeners();
  }

  listPublicRooms() {
    const publicRooms = [];
    for (const match of this.matches.values()) {
      const typeKey = TYPE_KEY_BY_CTOR.get(match.constructor);
      if (typeKey == null) continue;
      const cap = PLAYER_CAP[typeKey];
      if (!acceptsJoin(typeKey, match.status)) continue;
      if (match.players.size === 0 || match.players.size >= cap) continue;

      let levelSum = 0;
      let levelCount = 0;
      for (const player of match.players.values()) {
        const level = Number(player.level);
        if (Number.isFinite(level)) {
          levelSum += level;
          levelCount++;
        }
      }
      publicRooms.push({
        id: match.matchId,
        name: match.roomName,
        playerCount: match.players.size,
        gameType: typeKey,
        averageLevel: levelCount ? Math.round(levelSum / levelCount) : 1,
      });
    }
    return publicRooms;
  }

  broadcastPublicRooms() {
    this.io.emit('available_rooms', this.listPublicRooms());
  }

  setupListeners() {
    this.io.on('connection', (socket) => {
      // BaseMatch.addPlayer reads `socket.user.id`; bail early if the auth
      // middleware ever failed open. ensureSocketAuthed disconnects the
      // socket and returns false in that case.
      if (!ensureSocketAuthed(socket)) return;
      // Send the room list to the new socket only — broadcasting to every
      // connected client on every new connection floods the namespace.
      socket.emit('available_rooms', this.listPublicRooms());

      const leaveCurrentRoom = (sessionId) => {
        const matchId = this.sessionToMatch.get(sessionId);
        if (matchId) {
          const match = this.matches.get(matchId);
          if (match) {
            match.removePlayer(sessionId);
            if (match.players.size === 0) {
              match.stop();
              this.matches.delete(matchId);
            }
          }
          this.sessionToMatch.delete(sessionId);

          const timer = this.disconnectTimers.get(sessionId);
          if (timer) {
            clearTimeout(timer);
            this.disconnectTimers.delete(sessionId);
          }
        }
      };

      socket.on('restore_session', ({ sessionId }) => {
        if (!sessionId) return;
        socket.join(sessionId);
        this.socketToSession.set(socket.id, sessionId);

        const matchId = this.sessionToMatch.get(sessionId);
        if (matchId) {
          const match = this.matches.get(matchId);
          if (match) {
            if (match.status === 'GAME_OVER') {
              return socket.emit('game_over');
            }

            const timer = this.disconnectTimers.get(sessionId);
            if (timer) {
              clearTimeout(timer);
              this.disconnectTimers.delete(sessionId);
            }

            socket.join(matchId);
            match.reconnectPlayer(sessionId);

            const typeKey = TYPE_KEY_BY_CTOR.get(match.constructor);
            const player = match.players.get(sessionId);

            socket.emit('reconnect_success', {
              roomId: matchId,
              roomName: match.roomName,
              gameType: typeKey,
              status: match.status,
              spawn: match.status === 'PLAYING' && player ? { x: player.x, z: player.z, angle: player.angle } : null
            });
            match.syncLobby();
          }
        }
      });

      socket.on('create_room', ({ name, color, gameType }) => {
        debug(`BACKEND: Received create_room request from ${name}`);
        const sessionId = this.socketToSession.get(socket.id);
        if (!sessionId) return;

        const typeKey = Number(gameType);
        const MatchClass = GAME_REGISTRY[typeKey];
        if (!MatchClass) {
          return socket.emit('join_error', { reason: 'invalid_game_type' });
        }

        leaveCurrentRoom(sessionId);

        const roomId = randomUUID();
        const roomName = `${name}'s Room`;
        const match = new MatchClass(roomId, this.io, roomName, () => {
          this.broadcastPublicRooms();
        });

        this.matches.set(roomId, match);
        this.sessionToMatch.set(sessionId, roomId);

        // Add to the match before signalling success so the client can't fire
        // gameplay events into an empty match between the two emits.
        match.addPlayer(socket, sessionId, name, color);
        socket.join(roomId);
        socket.emit('join_success', { roomId, roomName, gameType: typeKey });
        match.syncLobby();
        this.broadcastPublicRooms();
      });

      socket.on('join_room', ({ name, roomId, color }) => {
        const sessionId = this.socketToSession.get(socket.id);
        if (!sessionId) return;

        const match = this.matches.get(roomId);
        if (!match) return socket.emit('join_error', { reason: 'room_not_found' });

        const typeKey = TYPE_KEY_BY_CTOR.get(match.constructor);
        const cap = PLAYER_CAP[typeKey];
        if (!acceptsJoin(typeKey, match.status) || match.players.size >= cap) {
          return socket.emit('join_error', { reason: 'lobby_full' });
        }

        leaveCurrentRoom(sessionId);
        this.sessionToMatch.set(sessionId, roomId);
        match.addPlayer(socket, sessionId, name, color);
        socket.join(roomId);
        socket.emit('join_success', { roomId, roomName: match.roomName, gameType: typeKey });
        match.syncLobby();
        this.broadcastPublicRooms();
      });

      socket.on('leave_room', () => {
        const sessionId = this.socketToSession.get(socket.id);
        if (sessionId) leaveCurrentRoom(sessionId);
        this.broadcastPublicRooms();
      });

      // ── Helpers for the per-event handlers below ─────────────────
      //
      // Every game event has the same dispatch pattern: look up the match
      // by socket, verify the handler exists, then invoke it. Hand-inlining
      // this six times bred subtle copy-paste bugs (forgotten typeof checks,
      // mismatched delete order). One helper, one place to read.
      const dispatch = (handlerName, ...args) => {
        const sessionId = this.socketToSession.get(socket.id);
        if (!sessionId) return;
        const matchId = this.sessionToMatch.get(sessionId);
        if (!matchId) return;
        const match = this.matches.get(matchId);
        if (!match) return;
        const fn = match[handlerName];
        if (typeof fn !== 'function') return;
        fn.call(match, sessionId, ...args);
      };

      // Inbound socket payloads are untrusted. Shape-guard before dispatch
      // so the match classes can assume well-typed inputs. A bad payload is
      // dropped silently — a cheating/buggy client doesn't get a 4xx.
      const isObject = (v) => v !== null && typeof v === 'object';
      const isFiniteN = (v) => typeof v === 'number' && Number.isFinite(v);
      const isPlayerInput = (p) =>
        isObject(p) && isFiniteN(p.x) && isFiniteN(p.y) && isFiniteN(p.z) && isFiniteN(p.angle);
      const isSpitDirection = (p) =>
        isObject(p) && isFiniteN(p.x) && isFiniteN(p.y) && isFiniteN(p.z);

      socket.on('ready_toggle', (payload) => {
        const isReady = !!(payload && payload.isReady);
        dispatch('toggleReady', isReady);
      });

      socket.on('player_hit', () => dispatch('handlePlayerHit'));
      socket.on('player_hit_complete', () => dispatch('handlePlayerHitComplete'));
      socket.on('player_jump', () => dispatch('handlePlayerJump'));
      socket.on('player_active', () => dispatch('handleActive'));

      socket.on('player_spit', (data) => {
        if (!isObject(data) || !isSpitDirection(data.direction)) return;
        dispatch('handlePlayerSpit', data.direction);
      });

      socket.on('player_input', (data) => {
        if (!isPlayerInput(data)) return;
        dispatch('handlePlayerInput', data);
      });

      socket.on('spit_hit', (payload) => {
        if (!isObject(payload)) return;
        const targetId = payload.targetId;
        // Socket ids are strings; reject anything else outright.
        if (typeof targetId !== 'string' || !targetId) return;
        dispatch('handleSpitHit', targetId);
      });

      // Disconnect: tear down the socket's room membership, free the match if
      // it just emptied. The previous version called playerToMatch.delete()
      // twice on the success path (once inside, once outside the `if (match)`
      // branch) — flatten to a single delete after the optional cleanup.
      socket.on('disconnect', () => {
        const sessionId = this.socketToSession.get(socket.id);
        if (!sessionId) return;
        this.socketToSession.delete(socket.id);

        const matchId = this.sessionToMatch.get(sessionId);
        if (!matchId) return;

        const match = this.matches.get(matchId);
        if (match) {
          match.setPlayerOffline(sessionId);

          const timer = setTimeout(() => {
            this.disconnectTimers.delete(sessionId);
            const currentMatch = this.matches.get(this.sessionToMatch.get(sessionId));
            if (currentMatch) {
              currentMatch.removePlayer(sessionId);
              if (currentMatch.players.size <= 0) {
                currentMatch.stop();
                this.matches.delete(matchId);
              }
            }
            this.sessionToMatch.delete(sessionId);
            this.broadcastPublicRooms();
          }, 30000);
          this.disconnectTimers.set(sessionId, timer);
        }
      });
    });
  }
}