import { error } from "#lib/logger.js";
import Game from "../models/Game.js";
import { BaseMatch } from "./BaseMatch.js";
import GamificationService from "./GamificationService.js";

const GAME_TYPE = "alpaca_road";
const MIN_RANKED_PLAYERS = 2;
const MAX_LANES = 4;
const HIT_TIMEOUT_MS = 1500;

export class AlpacaRoadMatch extends BaseMatch {
  constructor(id, namespace, roomName, onStateChange) {
    super(id, namespace, roomName, onStateChange);
    this.obstacles = [];
    this.tickRate = 33;
    this.isPlaying = false;

    this.level = 1;
    this.totalPoints = 0;
    this.roadSpeed = 0;
    this.timerMultiplier = 1.0;
    this.spawnTimer = 2.0;
    this.finalized = false;
    this.hitTimers = new Map();
    this.countdownTimer = null;
  }

  addPlayer(socket, sessionId, name, color) {
    super.addPlayer(socket, sessionId, name, color);
    const player = this.players.get(sessionId);

    const taken = new Set(
      Array.from(this.players.values())
        .map((p) => p.lane)
        .filter((n) => Number.isInteger(n)),
    );
    let lane = 0;
    while (taken.has(lane) && lane < MAX_LANES) lane++;
    if (lane >= MAX_LANES) {
      super.removePlayer(sessionId);
      return socket.emit('join_error', { reason: 'lobby_full' });
    }

    player.lane = lane;
    player.isJumping = false;
    player.isActive = true;
    player.lastActive = Date.now();
    this._ensureHeartbeat();
    this.syncLobby();
  }

  start() {
    this.status = 'COUNTDOWN';
    this.initObstacles();
    this.broadcast('game_start');
    this.countdownTimer = setTimeout(() => {
      this.countdownTimer = null;
      this.status = 'PLAYING';
      this.isPlaying = true;
      this.roadSpeed = 30;
    }, 4000);
  }

  stop() {
    super.stop();
    this.isPlaying = false;
    this.obstacles = [];
    this.roadSpeed = 0;
    if (this.countdownTimer) {
      clearTimeout(this.countdownTimer);
      this.countdownTimer = null;
    }
    for (const t of this.hitTimers.values()) clearTimeout(t);
    this.hitTimers.clear();
  }

  removePlayer(sessionId) {
    this._clearHitTimer(sessionId);
    super.removePlayer(sessionId);
  }

  _scheduleHitClear(sessionId) {
    const existing = this.hitTimers.get(sessionId);
    if (existing) clearTimeout(existing);
    const t = setTimeout(() => {
      this.hitTimers.delete(sessionId);
      const p = this.players.get(sessionId);
      if (p && p.isHit) {
        p.isHit = false;
        this.syncLobby();
      }
    }, HIT_TIMEOUT_MS);
    this.hitTimers.set(sessionId, t);
  }

  _clearHitTimer(sessionId) {
    const t = this.hitTimers.get(sessionId);
    if (t) {
      clearTimeout(t);
      this.hitTimers.delete(sessionId);
    }
  }

  handlePlayerHit(sessionId) {
    const player = this.players.get(sessionId);
    if (!player || player.hp <= 0 || player.isDead || player.isHit) return;
    player.isHit = true;
    player.hp--;
    if (player.hp <= 0) player.isDead = true;
    this._scheduleHitClear(sessionId);
    this.syncLobby();
  }

  handlePlayerHitComplete(sessionId) {
    const player = this.players.get(sessionId);
    if (player && player.isHit) {
      player.isHit = false;
      this._clearHitTimer(sessionId);
      this.syncLobby();
    }
  }

  handlePlayerJump(sessionId) {
    const player = this.players.get(sessionId);
    if (player && !player.isDead && !player.isJumping) {
      player.isJumping = true;
      setTimeout(() => {
        if (this.players.has(sessionId)) {
          this.players.get(sessionId).isJumping = false;
        }
      }, 600);
    }
  }

  handleActive(sessionId) {
    const player = this.players.get(sessionId);
    if (player && !player.isDead) {
      player.lastActive = Date.now();
      player.isActive = true;
    }
  }

  updateDifficulty() {
    const pointsPerLevel = 4 + this.level;
    const newLevel = Math.floor(this.totalPoints / pointsPerLevel) + 1;

    if (newLevel > this.level) {
      this.level = newLevel;
      const minSpeed = 30;
      const maxSpeed = 100;
      const factor = 0.08;
      const difficultyFactor = 1 - Math.exp(-factor * this.level);

      this.roadSpeed = minSpeed + (maxSpeed - minSpeed) * difficultyFactor;
      this.timerMultiplier = Math.max(0.5, this.timerMultiplier - 0.05);

      this.broadcast('level_up', {
        level: this.level,
      });
    }
  }

  update() {
    if (this.players.size === 0) return;

    const tick = this.tickRate / 1000;
    let pointGained = false;

    if (this.isPlaying) {
      this.obstacles.forEach(obs => {
        obs.z -= this.roadSpeed * tick;
        if (obs.z < -0.25 && !obs.pointGiven) {
          obs.pointGiven = true;
          let awardedPoint = false;

          for (const player of this.players.values()) {
            if (!player.isDead && !player.isHit && player.isActive && !player.isOffline) {
              if (obs.isFull || player.lane === obs.lane) {
                player.points++;
                awardedPoint = true;
              }
            } else if (!player.isActive || player.isOffline) {
              if (obs.isFull || player.lane === obs.lane) {
                player.hp--;
                if (player.hp <= 0) {
                  player.isDead = true;
                }
              }
            }
          }
          if (awardedPoint) {
            this.totalPoints++;
            pointGained = true;
          }
        }
      })

      if (pointGained) this.updateDifficulty();

      this.obstacles = this.obstacles.filter(obs => obs.z > -50);

      this.spawnTimer -= tick;
      if (this.spawnTimer <= 0) {
        this.createObstacle();
        this.spawnTimer = (1.0 + Math.random() * 2.0) * this.timerMultiplier;
      }
    }

    this.checkActivity();

    const playersArr = Array.from(this.players.values()).map(p => ({
      id: p.id,
      name: p.name,
      hp: p.hp,
      point: p.points,
      isDead: p.isDead,
      isHit: p.isHit,
      isJumping: p.isJumping,
      isReady: p.isReady,
      lane: p.lane,
      color: p.color
    }));

    const allDead = playersArr.length > 0 && playersArr.every(p => p.isDead === true);
    if (allDead && this.status !== 'GAME_OVER') {
      this.status = 'GAME_OVER';
      this.persistOutcome().catch((err) => {
        error('[alpaca-road] failed to persist outcome:', err.message);
      });

      setTimeout(() => {
        this.isPlaying = false;
        this.broadcast('game_over');
        this.stop()
      }, 1500);
    }

    this.broadcast('tick', {
      obstacles: this.obstacles,
      players: playersArr,
      roadSpeed: this.roadSpeed
    });
  }

  initObstacles() {
    const amount = 8;
    const roadLength = 700;
    for (let i = 0; i < amount; ++i) {
      this.createObstacle(75 + (roadLength / amount) * i);
    }
  }

  createObstacle(pos = 700) {
    const activeLanes = Array.from(this.players.values()).filter(p => !p.isDead).map(p => p.lane);
    const targetLane = activeLanes.length > 0
      ? activeLanes[Math.floor(Math.random() * activeLanes.length)]
      : Math.floor(Math.random() * 4);

    this.obstacles.push({
      id: Math.random().toString(36),
      typeId: Math.floor(Math.random() * 2),
      lane: targetLane,
      z: pos,
      isFull: Math.random() > 0.6,
      pointGiven: false
    });
  }

  checkActivity() {
    if (!this.isPlaying) return;

    const now = Date.now();
    for (const player of this.players.values()) {
      if (!player.isDead && player.isActive) {
        if (now - player.lastActive > 3000) {
          player.isActive = false;
        }
      }
    }
  }

  async persistOutcome() {
    if (this.finalized) return;
    this.finalized = true;

    const ranked = Array.from(this.players.values()).filter((p) =>
      Number.isFinite(p.userId),
    );
    if (ranked.length < MIN_RANKED_PLAYERS) return;

    const winner = ranked.reduce((best, p) =>
      p.points > best.points ? p : best,
    );

    await Promise.all(
      ranked.map(async (p) => {
        const isWinner = p.userId === winner.userId;
        const result = isWinner ? "win" : "loss";
        await Game.updateStats(p.userId, GAME_TYPE, result);
        if (p.points > 0) {
          await Game.incrementCounter(p.userId, GAME_TYPE, 'obstacles', p.points);
        }

        if (isWinner) {
          await GamificationService.onWin(p.userId, GAME_TYPE);
        } else {
          await GamificationService.onLoss(p.userId);
        }
        await GamificationService.onAlpacaRoadStage(p.userId, this.level);
      }),
    );

    if (ranked.length === 2) {
      try {
        const [a, b] = ranked;
        const game = await Game.create({ player1Id: a.userId, gameType: GAME_TYPE });
        await Game.joinGame(game.id, b.userId);
        await Game.finishGame(game.id, {
          winnerId: winner?.userId ?? null,
          player1Score: a.points ?? 0,
          player2Score: b.points ?? 0,
        });
      } catch (err) {
        error('[alpaca-road] game-row persist failed:', err.message);
      }
    }
  }
}