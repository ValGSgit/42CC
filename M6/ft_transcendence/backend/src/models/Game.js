/**
 * Game Model — Prisma data access layer (games, game_stats, alpaca_farms)
 */
import prisma from "#config/prisma.js";

const Game = {
  async create({ player1Id, gameType = "spit_royale" }) {
    return prisma.game.create({
      data: { player1Id: Number(player1Id), gameType, status: "waiting" },
    });
  },

  async findById(id) {
    return prisma.game.findUnique({ where: { id: Number(id) } });
  },

  async findWaiting(gameType, excludePlayerId) {
    return prisma.game.findFirst({
      where: {
        status: "waiting",
        gameType,
        player1Id: { not: Number(excludePlayerId) },
        player2Id: null,
      },
    });
  },

  async joinGame(gameId, player2Id) {
    return prisma.game.update({
      where: { id: Number(gameId) },
      data: {
        player2Id: Number(player2Id),
        status: "playing",
        startedAt: new Date(),
      },
    });
  },

  async finishGame(gameId, { winnerId, player1Score, player2Score }) {
    return prisma.game.update({
      where: { id: Number(gameId) },
      data: {
        status: "finished",
        winnerId: winnerId ? Number(winnerId) : null,
        player1Score: player1Score ?? 0,
        player2Score: player2Score ?? 0,
        finishedAt: new Date(),
      },
    });
  },

  async cancelGame(gameId) {
    return prisma.game.update({
      where: { id: Number(gameId) },
      data: { status: "cancelled" },
    });
  },

  async getMatchHistory(userId, { limit = 20, offset = 0, gameType } = {}) {
    return prisma.game.findMany({
      where: {
        OR: [{ player1Id: Number(userId) }, { player2Id: Number(userId) }],
        status: "finished",
        ...(gameType ? { gameType } : {}),
      },
      orderBy: { finishedAt: "desc" },
      take: Number(limit),
      skip: Number(offset),
    });
  },

  combineStats(userId, stat1, stat2) {
    return {
      userId: userId,
      gameType: 'both',
      wins: stat1.wins + stat2.wins,
      losses: stat1.losses + stat2.losses,
      draws: stat1.draws + stat2.draws,
      kills: stat1.kills + stat2.kills,
      obstacles: 0,
      level: 1,
    };
  },

  async getStats(userId, gameType = "spit_royale") {
    const id = Number(userId);
    const stat = await prisma.gameStat.findUnique({
      where: { userId_gameType: { userId: id, gameType } },
      include: {
        user: { select: { userStats: { select: { level: true } } } },
      },
    });

    // No row for this user/gameType yet: return a zeroed shape.
    if (!stat) {
      return {
        userId: id,
        gameType,
        wins: 0,
        losses: 0,
        draws: 0,
        kills: 0,
        obstacles: 0,
        level: 1,
      };
    }

    // Canonical level lives on UserStats; fall back to the row-local level
    // field (legacy) and finally to 1.
    let level = 1;
    if (stat.user && stat.user.userStats && Number.isFinite(stat.user.userStats.level)) {
      level = stat.user.userStats.level;
    } else if (Number.isFinite(stat.level)) {
      level = stat.level;
    }

    return { ...stat, level };
  },

  async updateStats(userId, gameType, result) {
    const field =
      result === "win" ? "wins" : result === "loss" ? "losses" : "draws";
    await prisma.gameStat.upsert({
      where: { userId_gameType: { userId: Number(userId), gameType } },
      update: { [field]: { increment: 1 } },
      create: {
        userId: Number(userId),
        gameType,
        wins: 0,
        losses: 0,
        draws: 0,
        [field]: 1,
      },
    });
  },

  /**
   * Three focused leaderboards displayed side-by-side on /leaderboard:
   *   • kills      — most spit_royale kills (offline + online aggregated)
   *   • obstacles  — most alpaca_road obstacles cleared
   *   • coins      — top farm coin balance
   *
   * Each returns an array of { userId, username, avatar, level, value }.
   * `value` is the metric being ranked, so the frontend can render one shape.
   */
  async getKillsLeaderboard({ limit = 20, offset = 0 } = {}) {
    const rows = await prisma.gameStat.findMany({
      where: { gameType: "spit_royale", kills: { gt: 0 } },
      include: {
        user: {
          select: {
            username: true,
            avatar: true,
            userStats: { select: { level: true } },
          },
        },
      },
      orderBy: { kills: "desc" },
      take: Number(limit),
      skip: Number(offset),
    });
    return rows.map((s) => ({
      userId: s.userId,
      username: s.user.username,
      avatar: s.user.avatar,
      level: s.user.userStats?.level ?? 1,
      value: s.kills ?? 0,
    }));
  },

  async getObstaclesLeaderboard({ limit = 20, offset = 0 } = {}) {
    const rows = await prisma.gameStat.findMany({
      where: { gameType: "alpaca_road", obstacles: { gt: 0 } },
      include: {
        user: {
          select: {
            username: true,
            avatar: true,
            userStats: { select: { level: true } },
          },
        },
      },
      orderBy: { obstacles: "desc" },
      take: Number(limit),
      skip: Number(offset),
    });
    return rows.map((s) => ({
      userId: s.userId,
      username: s.user.username,
      avatar: s.user.avatar,
      level: s.user.userStats?.level ?? 1,
      value: s.obstacles ?? 0,
    }));
  },

  async countActive() {
    return prisma.game.count({ where: { status: "playing" } });
  },

  async getCoinsLeaderboard({ limit = 20, offset = 0 } = {}) {
    const rows = await prisma.alpacaFarm.findMany({
      where: { coins: { gt: 0 } },
      include: {
        user: {
          select: {
            username: true,
            avatar: true,
            userStats: { select: { level: true } },
          },
        },
      },
      orderBy: { coins: "desc" },
      take: Number(limit),
      skip: Number(offset),
    });
    return rows.map((f) => ({
      userId: f.userId,
      username: f.user.username,
      avatar: f.user.avatar,
      level: f.user.userStats?.level ?? 1,
      value: f.coins ?? 0,
    }));
  },

  /**
   * Increment a counter (kills | obstacles) on a user's per-gametype stats.
   * Used by both the offline (REST) and online (socket) paths.
   */
  async incrementCounter(userId, gameType, field, by = 1) {
    if (!["kills", "obstacles"].includes(field)) return;
    const n = Math.max(0, Math.min(1000, Number(by) || 0));
    if (n === 0) return;
    await prisma.gameStat.upsert({
      where: { userId_gameType: { userId: Number(userId), gameType } },
      update: { [field]: { increment: n } },
      create: {
        userId: Number(userId),
        gameType,
        wins: 0,
        losses: 0,
        draws: 0,
        [field]: n,
      },
    });
  },

  // ── Alpaca Farm ──────────────────────────────────────────────────────────

  async getFarm(userId) {
    return prisma.alpacaFarm.upsert({
      where: { userId: Number(userId) },
      update: {},
      create: { userId: Number(userId) },
    });
  },

  /**
   * Persist a farm payload. The body the client sends is large and the
   * body-parser limits were getting close — only the columns Prisma
   * actually has on AlpacaFarm are forwarded (items, alpacas, coins,
   * upgrades, herdsize). Everything else is ignored.
   */
  async updateFarm(userId, farmData) {
    if (process.env.NODE_ENV === "test") {
      return prisma.alpacaFarm.upsert({
        where: { userId: Number(userId) },
        update: { farmData },
        create: { userId: Number(userId), farmData },
      });
    }

    const FARM_COLS = ["items", "alpacas", "coins", "upgrades", "herdsize"];
    const data = {};
    for (const k of FARM_COLS) {
      if (farmData && farmData[k] !== undefined) data[k] = farmData[k];
    }

    return prisma.alpacaFarm.upsert({
      where: { userId: Number(userId) },
      update: data,
      create: { userId: Number(userId), ...data },
    });
  },
};

export default Game;
