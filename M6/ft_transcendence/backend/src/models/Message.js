/**
 * Message Model — Prisma data access layer
 */
import prisma from "#config/prisma.js";

const Message = {
  async create({ senderId, receiverId, content }) {
    return prisma.message.create({
      data: {
        senderId: Number(senderId),
        receiverId: Number(receiverId),
        content,
      },
    });
  },

  async getConversation(userId, otherUserId, { limit = 50, offset = 0 } = {}) {
    const messages = await prisma.message.findMany({
      where: {
        OR: [
          { senderId: Number(userId), receiverId: Number(otherUserId) },
          { senderId: Number(otherUserId), receiverId: Number(userId) },
        ],
      },
      include: { sender: { select: { username: true, avatar: true } } },
      orderBy: { createdAt: "desc" },
      take: Number(limit),
      skip: Number(offset),
    });
    return messages.reverse().map((m) => ({
      id: m.id,
      sender_id: m.senderId,
      receiver_id: m.receiverId,
      content: m.content,
      is_read: m.isRead,
      created_at: m.createdAt,
      sender_username: m.sender.username,
      sender_avatar: m.sender.avatar,
    }));
  },

  // Returns one entry per conversation partner, with the latest message and
  // the unread count from that partner. Dedupe + counts are done in JS to
  // keep this portable across DBs (the previous DISTINCT ON variant was
  // PostgreSQL-only).
  async getConversationsList(userId) {
    const uid = Number(userId);

    // 1. Build the set of users blocked in either direction.
    const blocked = await prisma.blockedUser.findMany({
      where: { OR: [{ userId: uid }, { blockedUserId: uid }] },
      select: { userId: true, blockedUserId: true },
    });
    const blockedIds = new Set(
      blocked.flatMap((b) => [b.userId, b.blockedUserId]).filter((id) => id !== uid),
    );

    // 2. Pull every message touching this user, newest first, with both
    // sides' username/avatar in one round-trip.
    const messages = await prisma.message.findMany({
      where: {
        OR: [{ senderId: uid }, { receiverId: uid }],
        NOT: { AND: [{ senderId: uid }, { receiverId: uid }] }, // skip self-msgs
      },
      include: {
        sender: { select: { id: true, username: true, avatar: true } },
        receiver: { select: { id: true, username: true, avatar: true } },
      },
      orderBy: { createdAt: "desc" },
    });

    // 3. Walk newest → oldest, keep the first hit per other-user id, skip
    // blocked partners.
    const seen = new Set();
    const conversations = [];
    for (const m of messages) {
      const other = m.senderId === uid ? m.receiver : m.sender;
      if (!other || other.id === uid || seen.has(other.id)) continue;
      if (blockedIds.has(other.id)) continue;
      seen.add(other.id);
      conversations.push({
        other_user_id: other.id,
        username: other.username,
        avatar: other.avatar,
        last_message: m.content,
        created_at: m.createdAt,
        is_read: m.isRead,
        unread_count: 0, // filled below
      });
    }

    if (conversations.length === 0) return conversations;

    // 4. Unread counts: one groupBy keyed by sender (rows where the other
    // party wrote to us and we haven't read it yet).
    const unread = await prisma.message.groupBy({
      by: ["senderId"],
      where: {
        receiverId: uid,
        isRead: false,
        senderId: { in: conversations.map((c) => c.other_user_id) },
      },
      _count: { _all: true },
    });
    const unreadMap = new Map(
      unread.map((row) => [row.senderId, row._count._all]),
    );
    for (const c of conversations) {
      c.unread_count = unreadMap.get(c.other_user_id) ?? 0;
    }

    return conversations;
  },

  async markAsRead(receiverId, senderId) {
    await prisma.message.updateMany({
      where: {
        receiverId: Number(receiverId),
        senderId: Number(senderId),
        isRead: false,
      },
      data: { isRead: true },
    });
  },

  async countUnread(userId) {
    return prisma.message.count({
      where: { receiverId: Number(userId), isRead: false },
    });
  },
};

export default Message;
