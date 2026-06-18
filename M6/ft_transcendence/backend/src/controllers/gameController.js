/**
 * Game Controller — REST endpoints for game history, stats, leaderboard, farm
 * Real-time game flow is handled by socketService.js
 */
import Achievement from '#models/Achievement.js';
import Game from '#models/Game.js';
import GamificationService from '#services/GamificationService.js';
import { clampInt, parseLimitOffset } from '#utils/pagination.js';

const ALLOWED_GAME_TYPES = ['spit_royale', 'alpaca_road', 'both'];

function resolveGameType(raw, fallback = 'both') {
  if (typeof raw !== 'string') return fallback;
  if (ALLOWED_GAME_TYPES.includes(raw)) return raw;
  return null; // signal "invalid" to the caller
}

/** GET /api/game/stats?gameType=spit_royale */
export const getStats = async (req, res, next) => {
  try {
    const gameType = resolveGameType(req.query.gameType);
    if (gameType === null) {
      return res.status(400).json({
        error: { message: `gameType must be one of: ${ALLOWED_GAME_TYPES.join(', ')}` },
      });
    }
    let userId = Number(req.user.id) ?? -1;
    if (req.query.userId !== undefined && req.query.userId !== null)
      userId = Number(req.query.userId);

    let stats;
    if (gameType == 'both') {
      const stats1 = await Game.getStats(userId, 'spit_royale');
      const stats2 = await Game.getStats(userId, 'alpaca_road');
      stats = Game.combineStats(userId, stats1, stats2);
    } else {
      stats = await Game.getStats(userId, gameType);
    }

    res.json({ stats });
  } catch (err) { next(err); }
};

/** GET /api/game/history?gameType=spit_royale&limit=20&offset=0 */
export const getHistory = async (req, res, next) => {
  try {
    // gameType is optional here — Game.getMatchHistory accepts undefined to
    // return matches across all game types. Only validate it when supplied.
    let gameType;
    if (req.query.gameType !== undefined) {
      gameType = resolveGameType(req.query.gameType, null);
      if (gameType === null) {
        return res.status(400).json({
          error: { message: `gameType must be one of: ${ALLOWED_GAME_TYPES.join(', ')}` },
        });
      }
    }

    const { limit, offset } = parseLimitOffset(req.query, { defaultLimit: 20, maxLimit: 100 });
    const matches = await Game.getMatchHistory(req.user.id, { limit, offset, gameType });
    res.json({ history: matches });
  } catch (err) { next(err); }
};

/** GET /api/game/leaderboard?board=kills|obstacles|coins&limit=20&offset=0
 *
 *  Single endpoint with a `board` selector — the frontend renders three
 *  tables side-by-side, so three round-trips beats three bespoke endpoints.
 */
export const getLeaderboard = async (req, res, next) => {
  try {
    const board = String(req.query.board || 'kills');
    const { limit, offset } = parseLimitOffset(req.query, { defaultLimit: 20, maxLimit: 100 });

    let leaderboard;
    if (board === 'kills') {
      leaderboard = await Game.getKillsLeaderboard({ limit, offset });
    } else if (board === 'obstacles') {
      leaderboard = await Game.getObstaclesLeaderboard({ limit, offset });
    } else if (board === 'coins') {
      leaderboard = await Game.getCoinsLeaderboard({ limit, offset });
    } else {
      return res.status(400).json({
        error: { message: "board must be one of: kills, obstacles, coins" },
      });
    }
    res.json({ board, leaderboard });
  } catch (err) { next(err); }
};

/** GET /api/game/leaderboard/coins — kept for backward compatibility */
export const getCoinsLeaderboard = async (req, res, next) => {
  try {
    const { limit, offset } = parseLimitOffset(req.query, { defaultLimit: 10, maxLimit: 100 });
    const leaderboard = await Game.getCoinsLeaderboard({ limit, offset });
    res.json({ board: 'coins', leaderboard });
  } catch (err) { next(err); }
};

/**
 * POST /api/game/result — Save offline/AI game result.
 *
 * The body is client-supplied, so we intentionally do NOT trust a 'win'
 * here — that would let any authenticated user farm achievements + coins
 * by spamming POSTs without ever playing. Online matches persist server-side
 * via the socket flow (see MatchManager). Only "loss"/"draw" are accepted
 * from this endpoint (you can't cheat XP downward).
 */
export const saveGameResult = async (req, res, next) => {
  try {
    const gameType = resolveGameType(req.body.gameType, null);

    if (gameType === null) {
      return res.status(400).json({
        error: { message: `gameType must be one of: ${ALLOWED_GAME_TYPES.join(', ')}` },
      });
    }
    if (!req.body.result) {
      return res.status(400).json({ error: { message: 'result required' } });
    }
    // Normalize before the whitelist check — clients have shipped "LOSS" /
    // "Loss" / "loss" historically and the previous strict-case match
    // rejected the first two as 400 even though the intent was obvious.
    const result =
      typeof req.body.result === "string" ? req.body.result.toLowerCase() : req.body.result;
    if (!['loss', 'draw'].includes(result)) {
      return res.status(400).json({
        error: { message: "Only 'loss' or 'draw' may be reported here; wins are persisted server-side." },
      });
    }

    // Offline games may also report counters (kills cleared per wave,
    // obstacles jumped per run). We cap them server-side so a client
    // can't farm the leaderboard with arbitrary numbers.
    const killsThisRun = clampInt(req.body.kills, 0, 200);
    const obstaclesThisRun = clampInt(req.body.obstacles, 0, 500);
    // Stage/level reached this run — drives the road_warrior achievement
    // ("complete 5 stages"). Clamped like the other client-supplied counters.
    const stageReached = clampInt(req.body.stage, 0, 1000);

    await Game.updateStats(req.user.id, gameType, result);
    if (gameType === 'spit_royale' && killsThisRun > 0) {
      await Game.incrementCounter(req.user.id, gameType, 'kills', killsThisRun);
    }
    if (gameType === 'alpaca_road' && obstaclesThisRun > 0) {
      await Game.incrementCounter(req.user.id, gameType, 'obstacles', obstaclesThisRun);
    }

    const stats = await Game.getStats(req.user.id, gameType);
    res.json({ stats });

    if (result === 'loss') {
      GamificationService.onLoss(req.user.id).catch(() => { });
    }
    if (gameType === 'alpaca_road') {
      GamificationService.onAlpacaRoadStage(req.user.id, stageReached).catch(() => { });
    }
  } catch (err) { next(err); }
};

/** GET /api/game/farm?userId=X — own farm or another user's farm */
export const getFarm = async (req, res, next) => {
  try {
    const userId = req.query.userId ? Number(req.query.userId) : req.user.id;
    const farm = await Game.getFarm(userId);
    res.json({ farm });
  } catch (err) { next(err); }
};

/**
 * PUT /api/game/farm — accepts { farmData }, { farm }, or flat
 * { items, alpacas, coins, upgrades, herdsize }.
 *
 * The client routinely sends large payloads on autosave; we cap the
 * arrays here so a single bad client can't blow up the JSONB column.
 */
const FARM_FLAT_KEYS = ['items', 'alpacas', 'coins', 'upgrades', 'herdsize'];
const MAX_ITEMS = 500;
const MAX_ALPACAS = 100;

export const saveFarm = async (req, res, next) => {
  try {
    const isFlat = FARM_FLAT_KEYS.some((k) => k in req.body);
    const raw = req.body.farmData ?? req.body.farm ?? (isFlat ? req.body : null);
    if (!raw || typeof raw !== 'object') {
      return res.status(400).json({ error: { message: 'farmData is required' } });
    }

    // Clamp arrays + numerics. Anything outside the schema is dropped.
    // Upper bound on the integer fields is generous (1e9) — we just want to
    // reject negatives, NaN, and non-numeric junk from the JSONB write.
    const COIN_CAP = 1_000_000_000;
    const clean = {};
    if (Array.isArray(raw.items)) clean.items = raw.items.slice(0, MAX_ITEMS);
    if (Array.isArray(raw.alpacas)) clean.alpacas = raw.alpacas.slice(0, MAX_ALPACAS);
    if (raw.coins !== undefined) clean.coins = clampInt(raw.coins, 0, COIN_CAP);
    if (raw.upgrades !== undefined) clean.upgrades = clampInt(raw.upgrades, 0, COIN_CAP);
    if (raw.herdsize !== undefined) clean.herdsize = clampInt(raw.herdsize, 0, COIN_CAP);

    // If no recognized field survived clamping, the body was either empty or
    // pure junk — return 400 instead of silently writing nothing and 200-ing.
    if (Object.keys(clean).length === 0) {
      return res.status(400).json({
        error: { message: 'farmData must contain at least one of: items, alpacas, coins, upgrades, herdsize' },
      });
    }

    const farm = await Game.updateFarm(req.user.id, clean);
    res.json({ farm });
    GamificationService.onFarmSave(req.user.id, clean).catch(() => { });
  } catch (err) { next(err); }
};

/** GET /api/game/achievements */
export const getAchievements = async (req, res, next) => {
  try {
    const [all, unlocked] = await Promise.all([
      Achievement.getAll(),
      Achievement.getUserAchievements(req.user.id),
    ]);
    const unlockedKeys = new Set(unlocked.map((a) => a.key));
    const achievements = all.map((a) => ({ ...a, unlocked: unlockedKeys.has(a.key) }));
    res.json({ achievements });
  } catch (err) { next(err); }
};

/** GET /api/game/challenges */
export const getChallenges = async (req, res, next) => {
  try {
    const challenges = await Achievement.getUserChallengeProgress(req.user.id);
    res.json({ challenges });
  } catch (err) { next(err); }
};
