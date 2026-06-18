/**
 * GamificationService — central hub for XP, level-ups, and achievement unlocks.
 *
 * All callers go through this service so that:
 *   • XP is always persisted server-side
 *   • Level-ups trigger a real-time notification
 *   • Achievement XP rewards are awarded automatically on unlock
 *   • Every unlock fires an achievement notification
 */
import prisma from '#config/prisma.js';
import { error as logError } from '#lib/logger.js';
import Achievement from '#models/Achievement.js';
import Game from '#models/Game.js';
import NotificationService from '#services/notificationService.js';

// Notification delivery is fire-and-forget — the XP/achievement state is
// already persisted by the time we get here. We don't want a flaky notify
// to roll back an unlock, but the previous `.catch(() => {})` swallowed
// the error completely. Log it so monitoring can see the failure.
function logNotifyError(context, err) {
  logError(`[gamification] notify failed (${context}):`, err?.message || err);
}

const XP_PER_LEVEL = 100;
const WIN_XP = 30;
const LOSS_XP = 5;

const GamificationService = {
  // ── XP ────────────────────────────────────────────────────────────────────

  /**
   * Increment a user's XP and persist the new level.
   * Fires a level_up notification and checks the level_10 achievement.
   *
   * One round-trip: we read the current xp/level, compute the next state in
   * JS, and upsert both fields together. The previous implementation did an
   * upsert (xp increment) followed by a conditional second update (level) —
   * two round-trips on every XP event.
   */
  async awardXp(userId, amount) {
    userId = Number(userId);

    const existing = await prisma.userStats.findUnique({
      where: { userId },
      select: { xp: true, level: true },
    });
    const oldXp = existing?.xp ?? 0;
    const oldLevel = existing?.level ?? 0;
    const newXp = oldXp + amount;
    const newLevel = Math.floor(newXp / XP_PER_LEVEL);

    await prisma.userStats.upsert({
      where: { userId },
      update: { xp: newXp, level: newLevel },
      create: { userId, xp: newXp, level: newLevel },
    });

    if (newLevel > oldLevel) {
      NotificationService.notify({
        userId,
        type: 'level_up',
        title: 'Level Up!',
        message: `You reached level ${newLevel}!`,
      }).catch((err) => logNotifyError('level_up', err));
      if (newLevel >= 10) await this._unlockNoXp(userId, 'level_10');
    }

    return { xp: newXp, level: newLevel };
  },

  // ── Achievement unlock ────────────────────────────────────────────────────

  /**
   * Public unlock: awards the achievement's XP reward and notifies the user.
   * Returns the achievement object if newly unlocked, null otherwise.
   */
  async unlock(userId, achievementKey) {
    userId = Number(userId);
    const result = await Achievement.unlock(userId, achievementKey);
    if (!result) return null;

    const { achievement } = result;

    if ((achievement.xpReward ?? 0) > 0) {
      // level_10 awards XP directly to avoid an awardXp → unlock('level_10') loop
      if (achievementKey === 'level_10') {
        await prisma.userStats.upsert({
          where: { userId },
          update: { xp: { increment: achievement.xpReward } },
          create: { userId, xp: achievement.xpReward },
        });
      } else {
        await this.awardXp(userId, achievement.xpReward);
      }
    }

    NotificationService
      .achievementUnlocked(userId, achievement.name)
      .catch((err) => logNotifyError(`achievement:${achievementKey}`, err));
    return achievement;
  },

  /** Internal: unlock without touching XP (used inside awardXp to prevent recursion). */
  async _unlockNoXp(userId, achievementKey) {
    userId = Number(userId);
    const result = await Achievement.unlock(userId, achievementKey);
    if (!result)
      return null;
    NotificationService
      .achievementUnlocked(userId, result.achievement.name)
      .catch((err) => logNotifyError(`achievement-nox:${achievementKey}`, err));
    return result.achievement;
  },

  async checkTopPlayerAchievement(userId) {
    userId = Number(userId);
    if (!Number.isFinite(userId)) return null;

    const [topKills, topObstacles, topCoins] = await Promise.all([
      Game.getKillsLeaderboard({ limit: 1 }),
      Game.getObstaclesLeaderboard({ limit: 1 }),
      Game.getCoinsLeaderboard({ limit: 1 }),
    ]);

    const isTopPlayer =
      topKills[0]?.userId === userId ||
      topObstacles[0]?.userId === userId ||
      topCoins[0]?.userId === userId;

    if (!isTopPlayer) return null;
    return this.unlock(userId, 'top_player');
  },

  // ── Game events ───────────────────────────────────────────────────────────

  async onWin(userId, gameType) {
    userId = Number(userId);
    if (!Number.isFinite(userId)) return;

    // Single read, compute next state in JS, then one $transaction batch
    // that persists xp + level + winStreak + coins atomically. The previous
    // implementation issued three separate upserts (awardXp's two queries
    // plus a winStreak upsert plus a coins upsert) — four round-trips per
    // win and three of them on the same row.
    const stats = await prisma.userStats.findUnique({
      where: { userId },
      select: { xp: true, level: true, winStreak: true },
    });
    const oldXp = stats?.xp ?? 0;
    const oldLevel = stats?.level ?? 0;
    const newXp = oldXp + WIN_XP;
    const newLevel = Math.floor(newXp / XP_PER_LEVEL);
    const newStreak = (stats?.winStreak ?? 0) + 1;

    await prisma.$transaction([
      prisma.userStats.upsert({
        where: { userId },
        update: { xp: newXp, level: newLevel, winStreak: newStreak },
        create: { userId, xp: newXp, level: newLevel, winStreak: newStreak },
      }),
    ]);

    if (newLevel > oldLevel) {
      NotificationService.notify({
        userId,
        type: 'level_up',
        title: 'Level Up!',
        message: `You reached level ${newLevel}!`,
      }).catch((err) => logNotifyError('level_up', err));
      if (newLevel >= 10) await this._unlockNoXp(userId, 'level_10');
    }

    // Achievement checks (game stats already updated by the caller).
    const gs = await Game.getStats(userId, gameType);
    if (gs.wins === 1) await this.unlock(userId, 'first_win');
    if (newStreak >= 5) await this.unlock(userId, 'win_streak_5');
    if (gameType === 'spit_royale' && gs.wins >= 10) await this.unlock(userId, 'sharpshooter');

    await this.checkTopPlayerAchievement(userId).catch((err) =>
      logError('[gamification] top-player check failed:', err?.message || err),
    );

    NotificationService.broadcastAll('game:finish');
  },

  async onLoss(userId) {
    userId = Number(userId);
    if (!Number.isFinite(userId)) return;

    // Mirror onWin's pattern: one read, one combined upsert for xp+level+streak.
    const stats = await prisma.userStats.findUnique({
      where: { userId },
      select: { xp: true, level: true },
    });
    const oldXp = stats?.xp ?? 0;
    const oldLevel = stats?.level ?? 0;
    const newXp = oldXp + LOSS_XP;
    const newLevel = Math.floor(newXp / XP_PER_LEVEL);

    await prisma.userStats.upsert({
      where: { userId },
      update: { xp: newXp, level: newLevel, winStreak: 0 },
      create: { userId, xp: newXp, level: newLevel, winStreak: 0 },
    });

    if (newLevel > oldLevel) {
      NotificationService.notify({
        userId,
        type: 'level_up',
        title: 'Level Up!',
        message: `You reached level ${newLevel}!`,
      }).catch((err) => logNotifyError('level_up', err));
      if (newLevel >= 10) await this._unlockNoXp(userId, 'level_10');
    }

    NotificationService.broadcastAll('game:finish');
  },

  /**
   * Alpaca Road "Road Warrior" — Complete 5 stages (reach level 5) in a run.
   * Keyed off the stage reached, not wins/losses: a fast run that ends in a
   * loss still counts. Called by AlpacaRoadMatch (online, with the match's
   * shared level) and by the /game/result handler (offline solo runs).
   */
  async onAlpacaRoadStage(userId, stage) {
    userId = Number(userId);
    if (!Number.isFinite(userId)) return;
    if (Number(stage) >= 5) await this.unlock(userId, 'road_warrior');
  },

  // ── Auth events ───────────────────────────────────────────────────────────

  async onLogin(userId) {
    userId = Number(userId);
    if (!Number.isFinite(userId)) return;
    await this.checkTopPlayerAchievement(userId).catch((err) =>
      logError('[gamification] top-player check failed:', err?.message || err),
    );
    await this.unlock(userId, 'first_login');
  },

  // ── Post events ───────────────────────────────────────────────────────────

  async onPostLiked(postAuthorId, newLikesCount) {
    postAuthorId = Number(postAuthorId);
    if (!Number.isFinite(postAuthorId)) return;

    // CSV: "Get a post with more than 10 likes" → strictly greater than 10.
    if (newLikesCount > 10) await this.unlock(postAuthorId, 'kinda_relatable');
  },

  // ── Farm events ───────────────────────────────────────────────────────────

  async onFarmSave(userId, farmData) {
    userId = Number(userId);
    if (!Number.isFinite(userId)) return;

    const items = farmData?.items ?? [];
    if (Array.isArray(items) && items.length > 0) {
      await this.unlock(userId, 'farm_started');
    }

    const coins = typeof farmData?.coins === 'number' ? farmData.coins : null;
    if (coins !== null && coins >= 1000) {
      await this.unlock(userId, 'coin_hoarder');
    }
  },

  // ── Message events ────────────────────────────────────────────────────────

  async onMessageSent(userId) {
    userId = Number(userId);
    if (!Number.isFinite(userId)) return;
    const count = await prisma.message.count({ where: { senderId: userId } });
    if (count >= 50) await this.unlock(userId, 'chatterbox');
  },
};

export default GamificationService;
