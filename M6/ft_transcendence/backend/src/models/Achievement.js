/**
 * Achievement Model — Prisma data access layer (achievements + daily challenges)
 */
import prisma from "#config/prisma.js";

// The achievement table is seeded once at boot and never changes at runtime,
// so we can cache the key → row lookup. Saves a findUnique per unlock attempt
// (which fires on every game finish, message sent, post liked, etc.).
const achievementCache = new Map();

async function getAchievement(key) {
  let row = achievementCache.get(key);
  if (row) return row;
  row = await prisma.achievement.findUnique({ where: { key } });
  if (row) achievementCache.set(key, row);
  return row;
}

// Exposed for tests; production code never needs it.
export function _resetAchievementCache() {
  achievementCache.clear();
}

const Achievement = {
  async getAll() {
    return prisma.achievement.findMany({ orderBy: { id: "asc" } });
  },

  async getUserAchievements(userId) {
    const rows = await prisma.userAchievement.findMany({
      where: { userId: Number(userId) },
      include: { achievement: true },
    });
    return rows.map((r) => ({ ...r.achievement, unlockedAt: r.unlockedAt }));
  },

  /**
   * Unlock an achievement for a user.
   * Returns { achievement } if newly unlocked, null if already unlocked or key not found.
   *
   * Uses createMany({ skipDuplicates: true }) so the unique constraint isn't
   * a thrown exception — that pattern (try/create/catch P2002) costs an extra
   * round-trip and litters Prisma error logs under concurrent unlocks
   * (e.g. a win that pushes the user to leaderboard #1 fires two unlocks).
   */
  async unlock(userId, achievementKey) {
    const achievement = await getAchievement(achievementKey);
    if (!achievement) return null;

    const { count } = await prisma.userAchievement.createMany({
      data: [{ userId: Number(userId), achievementId: achievement.id }],
      skipDuplicates: true,
    });
    return count > 0 ? { achievement } : null;
  },

  async getUserChallengeProgress(userId) {
    const today = new Date();
    today.setHours(0, 0, 0, 0);

    const challenges = await prisma.dailyChallenge.findMany({
      where: { activeDate: { gte: today } },
      include: {
        userChallenges: {
          where: { userId: Number(userId) },
        },
      },
    });

    return challenges.map((c) => ({
      ...c,
      completed: c.userChallenges[0]?.completed ?? false,
      completedAt: c.userChallenges[0]?.completedAt ?? null,
      userChallenges: undefined,
    }));
  },
};

export default Achievement;
