import { io } from 'socket.io-client';
import { debug, devError } from '../../services/logger.js';
import { gMinigame, gUI } from '../core/globals';
import { makeAnnouncement, playCountDown } from './annoucement';
import { changeGame } from './init';

export class GameClient {
  constructor() {
    this.socket = null;
    this.serverData = {};
    this.spitQueue = [];
    this._handlers = null;
    this.sessionId = localStorage.getItem('alpaca_session_id');
    if (!this.sessionId) {
      this.sessionId = Math.random().toString(36).substring(2, 15);
      localStorage.setItem('alpaca_session_id', this.sessionId);
    }
  }

  connect() {
    if (this.socket) return;

    this.socket = io('/minigames', { transports: ['websocket'], withCredentials: true });

    this._handlers = this._buildHandlers();
    for (const [event, fn] of Object.entries(this._handlers)) {
      this.socket.on(event, fn);
    }
  }

  /**
   * Build the handler table. Kept in one place so attach/detach is symmetric
   * — no chance of a missing .off() when the lifecycle changes.
   */
  _buildHandlers() {
    return {
      connect: () => {
        debug("GameClient connected:", this.socket?.id);
        this.socket.emit('restore_session', { sessionId: this.sessionId });
      },
      connect_error: (err) => { devError("GameClient connection failed:", err.message); },

      // Drop the singleton reference on disconnect so a new connect() works
      // cleanly after a tab close / network drop / hot reload. Also detach
      // listeners so a stale io() instance can be GC'd.
      disconnect: () => {
        this._detachHandlers();
        this.socket = null;
        this.serverData = {};
        this.spitQueue = [];
      },

      available_rooms: (roomList) => {
        gMinigame.value.publicRooms = roomList;
      },

      reconnect_success: (data) => {
        this.serverData = {};
        this.spitQueue = [];
        const mode = data.gameType;
        gMinigame.value.mode = mode;
        changeGame(mode, 1);
        gMinigame.value.currentRoomName = data.roomName;
        if (data.status === 'PLAYING') {
          gUI.lobbyMenu = false;
          gMinigame.value.isActive = true;
          if (data.spawn) gMinigame.value.spawnData = data.spawn;
        } else {
          gUI.lobbyMenu = true;
        }
      },

      join_success: (data) => {
        this.serverData = {};
        this.spitQueue = [];
        const mode = data.gameType;
        gMinigame.value.mode = mode;
        changeGame(mode, 1);
        gMinigame.value.currentRoomName = data.roomName;
        gMinigame.value.isReady = false;
        gUI.lobbyMenu = mode === 2 || mode === 4;
      },

      lobby_update: (playerList) => {
        gMinigame.value.players = playerList;
      },

      game_start: (data) => {
        gUI.lobbyMenu = false;
        gMinigame.value.isActive = true;
        // Spit Royale carries a per-player spawn; store it whether or not the
        // start is instant. `instant` (legacy) skips the countdown — both game
        // types now run the 3-2-1 countdown after the lobby ready screen.
        if (data && data.spawn) {
          gMinigame.value.spawnData = data.spawn;
        }
        if (!(data && data.instant) && gMinigame.value.mode === 4) {
          playCountDown(3);
        }
      },

      level_up: (data) => {
        makeAnnouncement(`LEVEL ${data.level}`, 2000);
      },

      tick: (data) => {
        gMinigame.value.players = data.players;
        this.serverData.players = data.players;
        this.serverData.obstacles = data.obstacles || [];
        this.serverData.roadSpeed = data.roadSpeed || 0;
      },

      player_spit: (data) => {
        this.spitQueue.push(data);
      },

      game_over: (data) => {
        gMinigame.value.isGameOver = true;
        if (!data) return;
        if (data.reason === 'eliminated') makeAnnouncement('Eliminated!', 3000);
        if (data.reason === 'lastone_standing' && data.winnerId === this.sessionId) {
          makeAnnouncement('You won!', 3000);
        }
      },
    };
  }

  _detachHandlers() {
    if (!this.socket || !this._handlers) return;
    for (const [event, fn] of Object.entries(this._handlers)) {
      this.socket.off(event, fn);
    }
    this._handlers = null;
  }

  disconnect() {
    if (!this.socket) return;
    try { this.socket.emit('leave_room'); } catch { /* socket may already be torn down */ }
    this._detachHandlers();
    this.socket.disconnect();
    this.socket = null;
    this.serverData = {};
    this.spitQueue = [];
  }

  leaveRoom() {
    if (this.socket) {
      this.socket.emit('leave_room');
      this.serverData = {};
      this.spitQueue = [];
    }
  }

  createRoom(playerName, color, gameType) {
    if (this.socket) this.socket.emit('create_room', { name: playerName, color: color, gameType: gameType });
  }

  joinRoom(playerName, roomId, color) {
    if (this.socket) this.socket.emit('join_room', { name: playerName, roomId: roomId, color: color });
  }

  sendReady(isReadyStatus) {
    if (this.socket) this.socket.emit('ready_toggle', { isReady: isReadyStatus });
  }

  sendHit() {
    if (this.socket) this.socket.emit('player_hit');
  }

  sendHitComplete() {
    if (this.socket) this.socket.emit('player_hit_complete');
  }

  sendJump() {
    if (this.socket) this.socket.emit('player_jump');
  }

  sendActive() {
    if (this.socket) this.socket.emit('player_active');
  }

  sendPlayerInput(x, y, z, angle) {
    if (this.socket) {
      this.socket.emit('player_input', { x, y, z, angle });
    }
  }

  sendSpit(direction) {
    if (this.socket) {
      this.socket.emit('player_spit', { direction: direction });
    }
  }

  sendSpitHit(targetId) {
    if (this.socket) {
      this.socket.emit('spit_hit', { targetId: targetId });
    }
  }

}

export const activeClient = new GameClient();