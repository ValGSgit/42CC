/**
 * User Model — Prisma data access layer
 */
import prisma from "#config/prisma.js";

/** Return a positive integer id or null. Avoids passing NaN to Prisma. */
function toId(v) {
  const n = Number(v);
  return Number.isInteger(n) && n > 0 ? n : null;
}

// Nested selects for all user sub-relations used across the app.
const SAFE_SELECT = {
  id: true,
  username: true,
  email: true,
  avatar: true,
  bio: true,
  status: true,
  isOnline: true,
  lastSeen: true,
  createdAt: true,
  updatedAt: true,
  userStats: { select: { level: true, xp: true } },
  userSettings: { select: { isPublic: true } },
  alpacaFarm: {
    select: { coins: true, alpacas: true, items: true, upgrades: true },
  },
};

/**
 * Flatten nested Prisma relations into a single object for API responses.
 */
export function shapeUserForClient(u) {
  if (!u) return u;
  return {
    id: u.id,
    username: u.username,
    email: u.email,
    avatar: u.avatar,
    bio: u.bio,
    status: u.status,
    is_public: u.userSettings?.isPublic ?? true,
    is_online: u.isOnline,
    isOnline: u.isOnline,
    api_key: u.userSettings?.apiKey ?? null,
    level: u.userStats?.level ?? 1,
    xp: u.userStats?.xp ?? 0,
    coins: u.alpacaFarm?.coins ?? 0,
    alpacas: u.alpacaFarm?.alpacas ?? [],
    items: u.alpacaFarm?.items ?? [],
    upgrades: u.alpacaFarm?.upgrades ?? 0,
    last_seen: u.lastSeen,
    created_at: u.createdAt,
    updated_at: u.updatedAt,
  };
}

const User = {
  async create({ username, email, passwordHash }) {
    return await prisma.user.create({
      data: {
        username,
        email,
        userAuth: { create: { passwordHash } },
        userStats: { create: {} },
        userSettings: { create: {} },
      },
      select: SAFE_SELECT,
    });
  },

  async findById(id) {
    const n = toId(id);
    if (n === null)
      return null;
    return prisma.user.findUnique({
      where: { id: n },
      select: SAFE_SELECT,
    });
  },

  async findByIdWithPassword(id) {
    const n = toId(id);
    if (n === null)
      return null;
    return prisma.user.findUnique({
      where: { id: n },
      include: {
        userAuth: true,
        userStats: true,
        userSettings: { select: { userId: true, isPublic: true } },
      },
    });
  },

  async findByUsername(username) {
    return prisma.user.findUnique({
      where: { username },
      include: { userAuth: true },
    });
  },

  async findByEmail(email) {
    return prisma.user.findUnique({
      where: { email },
      include: { userAuth: true },
    });
  },

  async update(id, fields) {
    const userAllowed = ["username", "email", "avatar", "bio", "status"];
    const settingsFields = ["isPublic"];
    const farmFields = ["coins", "alpacas", "items", "upgrades"];

    const userData = {};
    const settingsData = {};
    const farmData = {};

    for (const key of userAllowed) {
      if (fields[key] !== undefined) userData[key] = fields[key];
    }
    for (const key of settingsFields) {
      if (fields[key] !== undefined) settingsData[key] = fields[key];
    }
    for (const key of farmFields) {
      if (fields[key] !== undefined) farmData[key] = fields[key];
    }

    if (
      !Object.keys(userData).length &&
      !Object.keys(settingsData).length &&
      !Object.keys(farmData).length
    ) {
      return this.findById(id);
    }

    const ops = [];
    let updatedUser = null;
    if (Object.keys(userData).length) {
      ops.push(
        prisma.user
          .update({ where: { id: Number(id) }, data: userData })
          .then((u) => {
            updatedUser = u;
            return u;
          }),
      );
    }
    if (Object.keys(settingsData).length) {
      ops.push(
        prisma.userSettings.upsert({
          where: { userId: Number(id) },
          create: { userId: Number(id), ...settingsData },
          update: settingsData,
        }),
      );
    }
    if (Object.keys(farmData).length) {
      ops.push(
        prisma.alpacaFarm.upsert({
          where: { userId: Number(id) },
          create: { userId: Number(id), ...farmData },
          update: farmData,
        }),
      );
    }

    await Promise.all(ops);
    if (updatedUser === null) {
      return Object.keys(userData).length ? null : this.findById(id);
    }
    if (
      updatedUser &&
      !Object.keys(settingsData).length &&
      !Object.keys(farmData).length
    )
      return updatedUser;
    return this.findById(id);
  },

  async updatePassword(id, passwordHash) {
    await prisma.userAuth.update({
      where: { userId: Number(id) },
      data: { passwordHash },
    });
  },

  async setOnline(id, isOnline = true) {
    // updateMany returns { count } and does not throw P2025 if the user was
    // removed between socket connect and this call.
    await prisma.user.updateMany({
      where: { id: Number(id) },
      data: { isOnline, lastSeen: new Date() },
    });
  },

  async setOffline(id) {
    await prisma.user.updateMany({
      where: { id: Number(id) },
      data: { isOnline: false, lastSeen: new Date() },
    });
  },

  filterToPrismaWhere(filter = {}) {
    let whereClause = {
      AND: [],
    };

    for (const [key, value] of Object.entries(filter)) {
      if (value === undefined || value === null || value === "") continue;
      if (key === "username" || key === "bio") {
        whereClause.AND.push({
          [key]: { contains: value, mode: "insensitive" },
        });
      } else if (key === "public" && value === true) {
        whereClause.AND.push({ userSettings: { isPublic: true } });
      }
    }

    if (whereClause.AND.length === 0) {
      delete whereClause.AND;
    }

    return whereClause;
  },

  sortToPrismaOrderBy(sort = {}) {
    let orderByArray = [];

    for (const [key, value] of Object.entries(sort)) {
      if (value !== "asc" && value !== "desc") continue;
      if (
        key === "createdAt" ||
        key === "id" ||
        key === "online" ||
        key === "username"
      ) {
        orderByArray.push({ [key]: value });
      } else if (key === "level" || key === "xp") {
        orderByArray.push({ userStats: { [key]: value } });
      }
    }
    return orderByArray;
  },

  async findAll({ limit = 50, offset = 0, filter = {}, sort = {} } = {}) {
    const whereClause = this.filterToPrismaWhere(filter);

    const orderByObj = this.sortToPrismaOrderBy(sort);

    const userCount = await prisma.user.count({ where: whereClause });

    const usersFound = await prisma.user.findMany({
      where: whereClause,
      select: {
        id: true,
        username: true,
        avatar: true,
        bio: true,
        status: true,
        isOnline: true,
        lastSeen: true,
        createdAt: true,
        userStats: { select: { level: true } },
        userSettings: { select: { isPublic: true } },
      },
      orderBy: orderByObj,
      take: Number(limit),
      skip: Number(offset),
    });

    return { usersFound, userCount };
  },

  async count() {
    return prisma.user.count();
  },

  async search(
    { limit = 20, offset = 0, filter = {}, sort = {} } = {},
    excludeUserId = -1,)
  {
    const whereClause = this.filterToPrismaWhere(filter);
    whereClause.NOT = [];
    whereClause.NOT.push({ id: excludeUserId });

    const orderByObj = this.sortToPrismaOrderBy(sort);

    const userCount = await prisma.user.count({ where: whereClause });

    const usersFound = await prisma.user.findMany({
      where: whereClause,
      select: {
        id: true,
        username: true,
        avatar: true,
        isOnline: true,
        userStats: { select: { level: true } },
        userSettings: { select: { isPublic: true } },
      },
      orderBy: orderByObj,
      take: Number(limit),
      skip: Number(offset),
    });
    return { usersFound, userCount };
  },

  async deleteById(id) {
    const n = toId(id);
    if (n === null) return false;
    try {
      await prisma.user.delete({ where: { id: n } });
      return true;
    } catch (err) {
      // "Record to delete does not exist" — distinguish a true 404 from a
      // real failure (FK conflict, DB down) so the controller can stop
      // telling the user "Account deleted" when it wasn't.
      if (err.code === "P2025") return false;
      throw err;
    }
  },

  /** Return the raw apiKey for a user (null if not set). */
  async getApiKey(userId) {
    const publicApi = await prisma.publicApi.findUnique({
      where: { userId: Number(userId) },
      select: { apiKey: true },
    });
    return publicApi?.apiKey ?? null;
  },

  /** Upsert an API key for a user. Returns the new key. */
  async setApiKey(userId, key) {
    await prisma.publicApi.upsert({
      where: { userId: Number(userId) },
      create: { userId: Number(userId), apiKey: key },
      update: { apiKey: key },
    });
    return key;
  },

  /** Remove the API key for a user. */
  async revokeApiKey(userId) {
    await prisma.publicApi.deleteMany({
      where: { userId: userId },
    });
  },

  /** Validate an API key against the DB. Returns the userId or null. */
  async findByApiKey(key) {
    if (!key) return null;
    const settings = await prisma.publicApi.findUnique({
      where: { apiKey: key },
      select: { userId: true },
    });
    return settings?.userId ?? null;
  },

  async getFullExport(id) {
    const [user, friends, messages, games, posts] = await Promise.all([
      prisma.user.findUnique({
        where: { id: Number(id) },
        select: {
          id: true,
          username: true,
          email: true,
          bio: true,
          status: true,
          createdAt: true,
        },
      }),
      prisma.friend.findMany({
        where: { userId: Number(id) },
        include: { friend: { select: { id: true, username: true } } },
      }),
      prisma.message.findMany({
        where: { senderId: Number(id) },
        select: { id: true, receiverId: true, content: true, createdAt: true },
        orderBy: { createdAt: "asc" },
      }),
      prisma.game.findMany({
        where: { OR: [{ player1Id: Number(id) }, { player2Id: Number(id) }] },
        orderBy: { createdAt: "asc" },
      }),
      prisma.post.findMany({
        where: { authorId: Number(id) },
        select: { id: true, content: true, imageUrl: true, createdAt: true },
        orderBy: { createdAt: "asc" },
      }),
    ]);

    return {
      user: user ?? null,
      friends: friends.map((f) => ({
        friendId: f.friend.id,
        username: f.friend.username,
      })),
      messages,
      games,
      posts,
    };
  },
};

export default User;
