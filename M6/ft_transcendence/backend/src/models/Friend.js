/**
 * Friend Model — Prisma data access layer
 */
import prisma from "#config/prisma.js";
import CustomError from "#utils/CustomError.js";

/** Map a friend user object from Prisma camelCase to frontend snake_case */
function shapeFriend(u) {
  return {
    id: u.id,
    username: u.username,
    avatar: u.avatar,
    is_online: u.isOnline,
    status: u.status,
    level: u.userStats?.level ?? u.level ?? 1,
  };
}

const Friend = {
  async isBlockedBetween(userA, userB) {
    const a = Number(userA);
    const b = Number(userB);
    const found = await prisma.blockedUser.findFirst({
      where: {
        OR: [
          { userId: a, blockedUserId: b },
          { userId: b, blockedUserId: a },
        ],
      },
    });
    return !!found;
  },
  async sendRequest(senderId, receiverId) {
    if (senderId === receiverId)
      throw new CustomError("Cannot friend yourself", 400);

    const sender = Number(senderId);
    const receiver = Number(receiverId);

    // Prevent sending requests when either user has blocked the other.
    const blocked = await prisma.blockedUser.findFirst({
      where: {
        OR: [
          { userId: receiver, blockedUserId: sender }, // receiver blocked sender
          { userId: sender, blockedUserId: receiver }, // sender blocked receiver
        ],
      },
    });
    if (blocked) {
      throw new CustomError("Cannot send friend request due to block", 403);
    }

    const alreadyFriends = await this.areFriends(sender, receiver);
    if (alreadyFriends)
      throw new CustomError("You are already friends", 409);

    const existing = await prisma.friendRequest.findFirst({
      where: {
        OR: [
          { senderId: sender, receiverId: receiver },
          { senderId: receiver, receiverId: sender },
        ],
      },
      orderBy: { createdAt: "desc" },
    });

    if (existing?.status === "pending") {
      if (existing.senderId === sender && existing.receiverId === receiver) {
        return { request: existing, alreadyPending: true };
      }

      const request = await this.acceptRequest(existing.id, sender);
      return { request, autoAccepted: true };
    }

    if (existing?.status === "declined") {
      return prisma.friendRequest.update({
        where: { id: existing.id },
        data: {
          senderId: sender,
          receiverId: receiver,
          status: "pending",
        },
      });
    }

    return prisma.friendRequest.upsert({
      where: {
        senderId_receiverId: {
          senderId: sender,
          receiverId: receiver,
        },
      },
      update: { status: "pending" },
      create: {
        senderId: sender,
        receiverId: receiver,
        status: "pending",
      },
    });
  },

  async acceptRequest(requestId, receiverId) {
    const request = await prisma.friendRequest.findFirst({
      where: {
        id: Number(requestId),
        receiverId: Number(receiverId),
        status: "pending",
      },
    });
    if (!request)
      throw new CustomError("Request not found or already handled", 404);

    const [updated] = await prisma.$transaction([
      prisma.friendRequest.update({
        where: { id: Number(requestId) },
        data: { status: "accepted" },
      }),
      prisma.friend.createMany({
        data: [
          { userId: request.senderId, friendId: request.receiverId },
          { userId: request.receiverId, friendId: request.senderId },
        ],
        skipDuplicates: true,
      }),
    ]);
    return updated;
  },

  async declineRequest(requestId, receiverId) {
    return prisma.friendRequest
      .updateMany({
        where: {
          id: Number(requestId),
          receiverId: Number(receiverId),
          status: "pending",
        },
        data: { status: "declined" },
      })
      .then((r) =>
        r.count > 0
          ? prisma.friendRequest.findUnique({
              where: { id: Number(requestId) },
            })
          : null,
      );
  },

  async requestInfo(senderId, receiverId) {
    const friendRequest = await prisma.friendRequest.findFirst({
      where: {
        status: "pending",
        OR: [
          { senderId, receiverId },
          { senderId: receiverId, receiverId: senderId },
        ],
      },
    });

    return {
      requestSent: friendRequest?.senderId === senderId,
      requestReceived: friendRequest?.receiverId === senderId,
    };
  },

  async removeFriend(userId, friendId) {
    await prisma.friend.deleteMany({
      where: {
        OR: [
          { userId: Number(userId), friendId: Number(friendId) },
          { userId: Number(friendId), friendId: Number(userId) },
        ],
      },
    });
  },

  filterToPrismaWhere(filter = {}) {
    let whereClause = {};
    let friendFilters = {};

    for (const [key, value] of Object.entries(filter)) {
      if (value === undefined || value === null || value === "") continue;

      if (key === "username") {
        friendFilters.username = { contains: value, mode: "insensitive" };
      }
    }

    if (Object.keys(friendFilters).length > 0) {
      whereClause.friend = friendFilters;
    }

    return whereClause;
  },

  sortToPrismaOrderBy(sort = {}) {
    let orderByArray = [];

    for (const [key, value] of Object.entries(sort)) {
      if (value === undefined || value === null || value === "") continue;

      const direction = String(value).toLowerCase();
      if (direction !== "asc" && direction !== "desc") {
        continue;
      }

      if (key === "username") {
        orderByArray.push({
          friend: { username: direction },
        });
      } else if (key === "level") {
        orderByArray.push({
          friend: { userStats: { level: direction } },
        });
      } else if (key === "online") {
        orderByArray.push({
          friend: { isOnline: direction },
        });
      }
    }
    return orderByArray;
  },

  async getFriends(
    userId,
    { limit = 50, offset = 0, filter = {}, sort = {} } = {},
  ) {
    let whereClause = this.filterToPrismaWhere(filter);
    whereClause.userId = Number(userId);

    const orderByClause = this.sortToPrismaOrderBy(sort);

    const count = await prisma.friend.count({
      where: whereClause,
    });

    const rows = await prisma.friend.findMany({
      where: whereClause,
      include: {
        friend: {
          select: {
            id: true,
            username: true,
            avatar: true,
            isOnline: true,
            status: true,
            userStats: { select: { level: true } },
          },
        },
      },
      orderBy: orderByClause,
      take: Number(limit),
      skip: Number(offset),
    });
    const friends = rows.map((r) => shapeFriend(r.friend));
    return { friends, count };
  },

  async isFriend(userId, friendId) {
    const friend = await prisma.friend.findFirst({
      where: { userId, friendId },
    });
    return friend ? true : false;
  },

  /**
   * For one viewer and a list of other user ids, return relationship flags
   * in a constant number of queries (avoids N+1 in user list endpoints).
   *
   * @param {number} viewerId
   * @param {number[]} otherIds
   * @returns {Promise<Map<number, { isFriend: boolean, isBlocked: boolean, requestSent: boolean, requestReceived: boolean }>>}
   */
  async relationFlagsForMany(viewerId, otherIds) {
    const v = Number(viewerId);
    const ids = [...new Set(otherIds.map(Number).filter(Number.isFinite))];
    const flags = new Map(
      ids.map((id) => [
        id,
        { isFriend: false, isBlocked: false, requestSent: false, requestReceived: false },
      ]),
    );
    if (!ids.length) return flags;

    const [friends, blocked, requests] = await Promise.all([
      prisma.friend.findMany({
        where: { userId: v, friendId: { in: ids } },
        select: { friendId: true },
      }),
      prisma.blockedUser.findMany({
        where: { userId: v, blockedUserId: { in: ids } },
        select: { blockedUserId: true },
      }),
      prisma.friendRequest.findMany({
        where: {
          status: "pending",
          OR: [
            { senderId: v, receiverId: { in: ids } },
            { receiverId: v, senderId: { in: ids } },
          ],
        },
        select: { senderId: true, receiverId: true },
      }),
    ]);

    for (const f of friends)
      flags.get(f.friendId).isFriend = true;
    for (const b of blocked)
      flags.get(b.blockedUserId).isBlocked = true;
    for (const r of requests) {
      if (r.senderId === v) {
        flags.get(r.receiverId).requestSent = true;
      } else {
        flags.get(r.senderId).requestReceived = true;
      }
    }
    return flags;
  },

  async getOnlineFriends(userId) {
    const rows = await prisma.friend.findMany({
      where: { userId: Number(userId), friend: { isOnline: true } },
      include: {
        friend: {
          select: {
            id: true,
            username: true,
            avatar: true,
            status: true,
            isOnline: true,
            userStats: { select: { level: true } },
          },
        },
      },
      orderBy: { friend: { username: "asc" } },
    });
    return rows.map((r) => shapeFriend(r.friend));
  },

  async getPendingReceived(userId) {
    const rows = await prisma.friendRequest.findMany({
      where: {
        receiverId: Number(userId),
        status: "pending",
        sender: {
          // sender has not blocked receiver
          blockedUsers: { none: { blockedUserId: Number(userId) } },
          // receiver has not blocked sender
          blockedBy: { none: { userId: Number(userId) } },
        },
      },
      include: { sender: { select: { username: true, avatar: true } } },
      orderBy: { createdAt: "desc" },
    });
    return rows.map((r) => ({
      ...r,
      senderUsername: r.sender.username,
      senderAvatar: r.sender.avatar,
      sender: undefined,
    }));
  },

  async getPendingSent(userId) {
    const rows = await prisma.friendRequest.findMany({
      where: { senderId: Number(userId), status: "pending" },
      include: { receiver: { select: { username: true, avatar: true } } },
      orderBy: { createdAt: "desc" },
    });
    return rows.map((r) => ({
      ...r,
      receiverUsername: r.receiver.username,
      receiverAvatar: r.receiver.avatar,
      receiver: undefined,
    }));
  },

  async areFriends(userId, otherUserId) {
    const row = await prisma.friend.findFirst({
      where: { userId: Number(userId), friendId: Number(otherUserId) },
    });
    return !!row;
  },

  async count(userId) {
    return prisma.friend.count({ where: { userId: Number(userId) } });
  },

  async getFriendStatus(viewerId, targetId) {
    const v = Number(viewerId);
    const t = Number(targetId);
    const [friendship, sentRequest, receivedRequest] = await Promise.all([
      prisma.friend.findFirst({ where: { userId: v, friendId: t } }),
      prisma.friendRequest.findFirst({
        where: { senderId: v, receiverId: t, status: "pending" },
      }),
      prisma.friendRequest.findFirst({
        where: { senderId: t, receiverId: v, status: "pending" },
      }),
    ]);
    if (friendship) return { status: "friends" };
    if (sentRequest)
      return { status: "pending_sent", requestId: sentRequest.id };
    if (receivedRequest)
      return { status: "pending_received", requestId: receivedRequest.id };
    return { status: "none" };
  },

  async blockUser(userId, blockedUserId) {
    await this.removeFriend(userId, blockedUserId);
    return prisma.blockedUser.upsert({
      where: {
        userId_blockedUserId: {
          userId: Number(userId),
          blockedUserId: Number(blockedUserId),
        },
      },
      update: {},
      create: { userId: Number(userId), blockedUserId: Number(blockedUserId) },
    });
  },

  async isBlocked(userId, blockedUserId) {
    const blocked = await prisma.blockedUser.findFirst({
      where: { userId, blockedUserId },
    });
    return blocked ? true : false;
  },

  async unblockUser(userId, blockedUserId) {
    await prisma.blockedUser.deleteMany({
      where: { userId: Number(userId), blockedUserId: Number(blockedUserId) },
    });
  },

  async getBlocked(userId) {
    const rows = await prisma.blockedUser.findMany({
      where: { userId: Number(userId) },
      include: {
        blockedUser: { select: { id: true, username: true, avatar: true } },
      },
    });
    return rows.map((r) => r.blockedUser);
  },
};

export default Friend;
