import { error } from "#lib/logger.js";

export class BaseMatch {
  constructor(matchId, namespace, roomName, onStateChange) {
    this.matchId = matchId;
    this.namespace = namespace;
    this.roomName = roomName;
    this.onStateChange = onStateChange;

    this.players = new Map();
    this.status = 'LOBBY';
    this.heartbeat = null;
    this.minPlayers = 1;
  }

  addPlayer(socket, sessionId, name, color) {
    const player = {
      id: sessionId,
      userId: socket.user?.id ?? null,
      level: socket.user?.level ?? socket.user?.userStats?.level ?? 1,
      name: name || socket.user?.username || 'Vue_Alpaca',
      isReady: false,
      hp: 3,
      points: 0,
      color: color || "0x000000",
      isOffline: false
    };

    this.players.set(sessionId, player);
    this.syncLobby();
  }

  removePlayer(sessionId) {
    this.players.delete(sessionId);
    this.syncLobby();
  }

  setPlayerOffline(sessionId) {
    const player = this.players.get(sessionId);
    if (player) {
      player.isOffline = true;
      this.syncLobby();
    }
  }

  reconnectPlayer(sessionId) {
    const player = this.players.get(sessionId);
    if (player) {
      player.isOffline = false;
      this.syncLobby();
    }
  }

  toggleReady(sessionId, isReady) {
    const player = this.players.get(sessionId);
    if (player) {
      player.isReady = isReady;
      this.syncLobby();
      this.checkStart();
    }
  }

  static _SAFE_PLAYER_FIELDS = [
    'id', 'userId', 'level', 'name', 'isReady', 'hp', 'points', 'color',
    'isDead', 'isHit', 'isJumping', 'isActive', 'point', 'lane',
    'x', 'y', 'z', 'angle', 'isOffline'
  ];

  shapePlayer(p) {
    const out = {};
    for (const k of this.constructor._SAFE_PLAYER_FIELDS) {
      if (p[k] !== undefined) out[k] = p[k];
    }
    return out;
  }

  syncLobby() {
    const playerList = Array.from(this.players.values()).map((p) => this.shapePlayer(p));
    this.broadcast('lobby_update', playerList);
  }

  checkStart() {
    const playerList = Array.from(this.players.values());
    const enoughPlayers = playerList.length >= this.minPlayers;
    const allReady = playerList.length > 0 && playerList.every(p => p.isReady);

    if (enoughPlayers && allReady && this.status === 'LOBBY') {
      this.start();
      if (this.onStateChange) this.onStateChange();
    }
  }

  broadcast(eventName, data) {
    this.namespace.to(this.matchId).emit(eventName, data);
  }

  _ensureHeartbeat() {
    if (this.heartbeat) return;
    this.heartbeat = setInterval(() => this._tick(), this.tickRate);
  }

  _tick() {
    try {
      this.update();
    } catch (err) {
      error(`[match ${this.matchId}] tick failed, stopping match:`, err?.message);
      this.stop();
    }
  }

  start() { }
  update() { }
  stop() {
    if (this.heartbeat) {
      clearInterval(this.heartbeat);
      this.heartbeat = null;
    }
    this.status = 'FINISHED';
  }
}