/**
 * Chat Controller — DM + group rooms
 */
import Message from "#models/Message.js";
import Friend from "#models/Friend.js";
import { parseLimitOffset, parseIdParam } from "#utils/pagination.js";

// ── Direct Messages ──────────────────────────────────────────────────────────

/** GET /api/chat/conversations */
export const listConversations = async (req, res, next) => {
  try {
    const conversations = await Message.getConversationsList(req.user.id);
    res.json({ conversations });
  } catch (err) {
    next(err);
  }
};

/** GET /api/chat/dm/:userId */
export const getConversation = async (req, res, next) => {
  try {
    const otherId = parseIdParam(req.params.userId);
    if (otherId === null) {
      return res.status(400).json({ error: { message: "Invalid user ID" } });
    }
    if (otherId === req.user.id) {
      return res
        .status(400)
        .json({ error: { message: "Cannot have messages with yourself" } });
    }

    const blocked = await Friend.isBlockedBetween(req.user.id, otherId);
    if (blocked) {
      return res.status(403).json({ error: { message: "Cannot access this conversation" } });
    }

    const { limit, offset } = parseLimitOffset(req.query, { defaultLimit: 50, maxLimit: 100 });
    const messages = await Message.getConversation(req.user.id, otherId, { limit, offset });
    await Message.markAsRead(req.user.id, otherId);
    res.json({ messages });
  } catch (err) {
    next(err);
  }
};

/** GET /api/chat/unread */
export const getUnreadCount = async (req, res, next) => {
  try {
    const count = await Message.countUnread(req.user.id);
    res.json({ count });
  } catch (err) {
    next(err);
  }
};
