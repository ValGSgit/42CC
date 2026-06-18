/**
 * Notification Service — creates notifications and pushes them via socket
 */
import Notification from '../models/Notification.js';

// Reference to socket.io server — set by socketService after init
let io = null;

const NotificationService = {
  setIo(ioInstance) {
    io = ioInstance;
  },

  /**
   * Create and optionally push a real-time notification.
   */
  async notify({ userId, type, title, message, referenceType, referenceId }) {
    const notification = await Notification.create({
      userId, type, title, message, referenceType, referenceId,
    });

    // Push via socket if user is connected
    if (io) {
      io.to(`user:${userId}`).emit('notification', notification);
    }

    return notification;
  },

  /**
   * Convenience helpers for common notification types.
   */
  async friendRequest(receiverId, senderUsername) {
    return this.notify({
      userId: receiverId,
      type: 'friend_request',
      title: 'Friend Request',
      message: `${senderUsername} sent you a friend request.`,
      referenceType: 'friend_request',
    });
  },

  async friendAccepted(senderId, receiverUsername) {
    return this.notify({
      userId: senderId,
      type: 'friend_accepted',
      title: 'Friend Accepted',
      message: `${receiverUsername} accepted your friend request.`,
      referenceType: 'friend_request',
    });
  },

  async gameInvite(userId, inviterUsername, gameId) {
    return this.notify({
      userId,
      type: 'game_invite',
      title: 'Game Invite',
      message: `${inviterUsername} invited you to a game.`,
      referenceType: 'game',
      referenceId: gameId,
    });
  },

  async achievementUnlocked(userId, achievementName) {
    return this.notify({
      userId,
      type: 'achievement',
      title: 'Achievement Unlocked!',
      message: `You unlocked "${achievementName}".`,
      referenceType: 'achievement',
    });
  },

  async postCommented(authorId, commenterUsername, postId) {
    return this.notify({
      userId: authorId,
      type: 'post_comment',
      title: 'New Comment',
      message: `${commenterUsername} commented on your post.`,
      referenceType: 'post',
      referenceId: postId,
    });
  },

  async postLiked(authorId, likerUsername, postId) {
    return this.notify({
      userId: authorId,
      type: 'post_like',
      title: 'Post Liked',
      message: `${likerUsername} liked your post.`,
      referenceType: 'post',
      referenceId: postId,
    });
  },

  async newMessage(receiverId, senderUsername) {
    return this.notify({
      userId: receiverId,
      type: 'message',
      title: 'New Message',
      message: `${senderUsername} sent you a message.`,
      referenceType: 'message',
    });
  },

  async dataRequestCompleted(userId, type) {
    return this.notify({
      userId,
      type: 'data_request',
      title: 'Data Request Complete',
      message: `Your data ${type} request has been completed.`,
      referenceType: 'data_request',
    });
  },

  broadcastAll(event, data) {
    if (io) io.emit(event, data);
  },
};

export default NotificationService;
