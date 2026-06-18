/**
 * Notification Controller
 */
import Notification from "#models/Notification.js";

/** GET /api/notifications?unreadOnly=true&limit=30&offset=0 */
export const listNotifications = async (req, res, next) => {
  try {
    const { unreadOnly = false, limit = 30, offset = 0 } = req.query;
    const [notifications, unreadCount] = await Promise.all([
      Notification.getForUser(req.user.id, {
        limit: Number(limit),
        offset: Number(offset),
        unreadOnly: unreadOnly === "true",
      }),
      Notification.countUnread(req.user.id),
    ]);
    res.json({ notifications, unreadCount });
  } catch (err) {
    next(err);
  }
};

/** PUT /api/notifications/:id/read */
export const markRead = async (req, res, next) => {
  try {
    const notification = await Notification.markRead(
      Number(req.params.id),
      req.user.id,
    );
    if (!notification)
      return res
        .status(404)
        .json({ error: { message: "Notification not found" } });
    res.json({ notification });
  } catch (err) {
    next(err);
  }
};

/** PUT /api/notifications/read-all */
export const markAllRead = async (req, res, next) => {
  try {
    await Notification.markAllRead(req.user.id);
    res.json({ message: "All notifications marked as read" });
  } catch (err) {
    next(err);
  }
};

/** DELETE /api/notifications/:id */
export const deleteNotification = async (req, res, next) => {
  try {
    const deleted = await Notification.delete(
      Number(req.params.id),
      req.user.id,
    );
    if (!deleted)
      return res
        .status(404)
        .json({ error: { message: "Notification not found" } });
    res.json({ message: "Notification deleted" });
  } catch (err) {
    next(err);
  }
};

/** DELETE /api/notifications — clear every notification for the caller. */
export const deleteAllNotifications = async (req, res, next) => {
  try {
    const count = await Notification.deleteAll(req.user.id);
    res.json({ message: "All notifications cleared", count });
  } catch (err) {
    next(err);
  }
};
