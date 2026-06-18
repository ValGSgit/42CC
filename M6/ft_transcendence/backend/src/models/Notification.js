/**
 * Notification Model — Prisma data access layer
 */
import prisma from "#config/prisma.js";

function shapeNotification(n) {
  if (!n) return n;
  return {
    id: n.id,
    user_id: n.userId,
    type: n.type,
    title: n.title,
    message: n.message,
    is_read: n.isRead,
    reference_type: n.referenceType,
    reference_id: n.referenceId,
    created_at: n.createdAt,
  };
}

const Notification = {
  async create({ userId, type, title, message, referenceType, referenceId }) {
    const row = await prisma.notification.create({
      data: {
        userId: Number(userId),
        type,
        title: title || "",
        message,
        referenceType: referenceType || null,
        referenceId: referenceId ? Number(referenceId) : null,
      },
    });
    return shapeNotification(row);
  },

  async getForUser(
    userId,
    { limit = 30, offset = 0, unreadOnly = false } = {},
  ) {
    const rows = await prisma.notification.findMany({
      where: {
        userId: Number(userId),
        ...(unreadOnly ? { isRead: false } : {}),
      },
      orderBy: { createdAt: "desc" },
      take: Number(limit),
      skip: Number(offset),
    });
    return rows.map(shapeNotification);
  },

  async markRead(id, userId) {
    const result = await prisma.notification.updateMany({
      where: { id: Number(id), userId: Number(userId) },
      data: { isRead: true },
    });
    if (result.count > 0) {
      const row = await prisma.notification.findUnique({
        where: { id: Number(id) },
      });
      return shapeNotification(row);
    }
    return null;
  },

  async markAllRead(userId) {
    await prisma.notification.updateMany({
      where: { userId: Number(userId), isRead: false },
      data: { isRead: true },
    });
  },

  async countUnread(userId) {
    return prisma.notification.count({
      where: { userId: Number(userId), isRead: false },
    });
  },

  async delete(id, userId) {
    const { count } = await prisma.notification.deleteMany({
      where: { id: Number(id), userId: Number(userId) },
    });
    return count > 0;
  },

  /** Wipe every notification for one user. Returns the count removed. */
  async deleteAll(userId) {
    const { count } = await prisma.notification.deleteMany({
      where: { userId: Number(userId) },
    });
    return count;
  },
};

export default Notification;
