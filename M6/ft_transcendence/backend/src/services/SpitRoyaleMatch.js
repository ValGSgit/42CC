import { error } from "#lib/logger.js";
import Game from "../models/Game.js";
import { BaseMatch } from "./BaseMatch.js";
import GamificationService from "./GamificationService.js";

const ARENA_RADIUS = 25;
const PLAYER_RADIUS = 2;
const GAME_TYPE = 'spit_royale';
const MIN_RANKED_PLAYERS = 2;
const SPIT_COOLDOWN_MS = 500;
const SPIT_MAX_RANGE = 12;
const SPIT_VALID_FOR_MS = 1500;
const MAX_SPEED_MPS = 25;

export class SpitRoyalMatch extends BaseMatch {
  constructor(id, namespace, roomName, onStateChange) {
    super(id, namespace, roomName, onStateChange);
    this.tickRate = 33;
    this.isPlaying = false;
    this.playersJoined = 0;
    this.finalized = false;
    this.minPlayers = 2;
  }

  getValidSpawn() {
    for (let attempts = 0; attempts < 50; attempts++) {
      const angle = Math.random() * Math.PI * 2;
      const r = Math.random() * (ARENA_RADIUS - 2);
      const x = Math.cos(angle) * r;
      const z = Math.sin(angle) * r;

      let isOverlapping = false;
      for (const p of this.players.values()) {
        const dx = p.x - x;
        const dz = p.z - z;
        if (Math.sqrt(dx * dx + dz * dz) < PLAYER_RADIUS * 2.5) {
          isOverlapping = true;
          break;
        }
      }
      if (!isOverlapping) return { x, z, angle: -angle };
    }
    return { x: 0, z: 0, angle: 0 };
  }

  start() {
    if (this.status === 'PLAYING') return;
    this.status = 'PLAYING';
    this.isPlaying = true;
    this.playersJoined = this.players.size;
    this._ensureHeartbeat();

    const now = Date.now();

    for (const [id, player] of this.players) {
      const spawn = this.getValidSpawn();
      player.x = spawn.x;
      player.y = 0;
      player.z = spawn.z;
      player.angle = spawn.angle;
      player.hp = 3;
      player.isDead = false;
      player.point = 0;
      this.namespace.to(id).emit('game_start', { spawn });
      player.lastInputAt = now;
    }
  }

  reconnectPlayer(sessionId) {
    super.reconnectPlayer(sessionId);
    const player = this.players.get(sessionId);
    if (player) {
      player.lastInputAt = Date.now();
    }
  }

  removePlayer(sessionId) {
    const player = this.players.get(sessionId);
    if (player) {
      player.isDead = true;
    }
    super.removePlayer(sessionId);
    this.checkWinCondition();
  }

  handlePlayerInput(sessionId, { x, y, z, angle }) {
    const player = this.players.get(sessionId);
    if (!player || player.isDead) return;

    const validNum = (n) => typeof n === "number" && Number.isFinite(n);

    if (validNum(x) && validNum(z)) {
      let safeX = x;
      let safeZ = z;

      const r2 = safeX * safeX + safeZ * safeZ;
      const maxR = ARENA_RADIUS - PLAYER_RADIUS;

      if (r2 > maxR * maxR) {
        const scale = maxR / Math.sqrt(r2);
        safeX *= scale;
        safeZ *= scale;
      }

      const now = Date.now();

      if (!player.lastInputAt || (now - player.lastInputAt) > 1000) {
        player.lastInputAt = now;
        player.x = safeX;
        player.z = safeZ;
        if (validNum(y)) player.y = y;
        if (validNum(angle)) player.angle = angle;
        return;
      }

      const dt = Math.max(0.001, (now - player.lastInputAt) / 1000);
      const step = Math.hypot(safeX - (player.x ?? 0), safeZ - (player.z ?? 0));

      if (step > MAX_SPEED_MPS * dt * 1.5) return;

      player.lastInputAt = now;
      player.x = safeX;
      player.z = safeZ;
    }

    if (validNum(y)) player.y = y;
    if (validNum(angle)) player.angle = angle;
  }


  handlePlayerSpit(sessionId, direction) {
    const player = this.players.get(sessionId);
    if (!player || player.isDead) return;
    const now = Date.now();
    if (now - (player.lastSpitAt || 0) < SPIT_COOLDOWN_MS) return;
    player.lastSpitAt = now;

    const safeDir = direction && typeof direction === 'object'
      ? { x: Number(direction.x) || 0, y: Number(direction.y) || 0, z: Number(direction.z) || 0 }
      : null;
    this.broadcast('player_spit', { ownerId: sessionId, direction: safeDir });
  }

  handleSpitHit(ownerId, targetId) {
    const owner = this.players.get(ownerId);
    const target = this.players.get(targetId);
    if (!owner || owner.isDead || !target || target.isDead) return;

    const now = Date.now();
    if (!owner.lastSpitAt || now - owner.lastSpitAt > SPIT_VALID_FOR_MS) return;

    const dx = (target.x ?? 0) - (owner.x ?? 0);
    const dz = (target.z ?? 0) - (owner.z ?? 0);
    if (Math.hypot(dx, dz) > SPIT_MAX_RANGE) return;

    owner.lastSpitAt = 0;

    target.hp -= 1;
    if (target.hp <= 0) {
      owner.point++;
      target.isDead = true;
      this.eliminations = (this.eliminations || 0) + 1;
      setTimeout(() => {
        this.namespace.to(targetId).emit('game_over', { reason: 'eliminated' });
      }, 500);
      this.checkWinCondition();
    }
  }

  checkWinCondition() {
    if (!this.isPlaying) return;

    const alivePlayers = Array.from(this.players.values()).filter(p => !p.isDead);

    if (
      this.playersJoined > 1 &&
      alivePlayers.length <= 1) {
      this.status = 'GAME_OVER';
      const winnerSocketId = alivePlayers.length === 1 ? alivePlayers[0].id : null;
      this.endMatch(winnerSocketId, 'lastone_standing');
      return;
    }

    if (this.playersJoined === 1 && alivePlayers.length === 0) {
      this.status = 'GAME_OVER';
      this.endMatch(null, 'no_survivors');
    }
  }

  endMatch(winnerSocketId, reason) {
    this.update()
    this.isPlaying = false;
    setTimeout(() => {
      this.broadcast('game_over', { reason, winnerId: winnerSocketId });
    }, 500);
    this._persistOutcome(winnerSocketId).catch((err) =>
      error('[spit-royale] failed to persist outcome:', err.message),
    );
    this.stop();
  }

  async _persistOutcome(winnerSocketId) {
    if (this.finalized && this.eliminations <= 0) return;
    this.finalized = true;

    const ranked = Array.from(this.players.values()).filter((p) =>
      Number.isFinite(p.userId),
    );
    if (ranked.length < MIN_RANKED_PLAYERS) return;

    const winnerPlayer = winnerSocketId
      ? ranked.find((p) => p.id === winnerSocketId)
      : null;

    await Promise.all(
      ranked.map(async (p) => {
        const isWinner = winnerPlayer != null && p.userId === winnerPlayer.userId;
        const result = isWinner ? 'win' : 'loss';

        await Game.updateStats(p.userId, GAME_TYPE, result);
        if (p.point > 0) {
          await Game.incrementCounter(p.userId, GAME_TYPE, 'kills', p.point);
        }

        if (isWinner) {
          await GamificationService.onWin(p.userId, GAME_TYPE);
        } else {
          await GamificationService.onLoss(p.userId);
        }
      }),
    );

    if (ranked.length === 2) {
      try {
        const [a, b] = ranked;
        const game = await Game.create({ player1Id: a.userId, gameType: GAME_TYPE });
        await Game.joinGame(game.id, b.userId);
        await Game.finishGame(game.id, {
          winnerId: winnerPlayer?.userId ?? null,
          player1Score: a.point ?? 0,
          player2Score: b.point ?? 0,
        });
      } catch (err) {
        error('[spit-royale] game-row persist failed:', err.message);
      }
    }
  }

  update() {
    if (!this.isPlaying) return;

    const playersArr = Array.from(this.players.values()).map(p => ({
      id: p.id,
      name: p.name,
      hp: p.hp,
      point: p.point,
      isDead: p.isDead,
      color: p.color,
      x: p.x,
      y: p.y,
      z: p.z,
      angle: p.angle
    }));

    this.broadcast('tick', { players: playersArr });
  }
}