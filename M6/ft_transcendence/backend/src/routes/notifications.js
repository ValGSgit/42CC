/**
 * Notification Routes — /api/notifications
 */
import express from 'express';
import {
  listNotifications, markRead, markAllRead, deleteNotification,
} from '../controllers/notificationController.js';
import { authenticate } from '../middleware/auth.js';
import { idParamValidation } from '../validators/contentValidator.js';
import { checkValidation } from '../validators/validatorUtils.js';

const router = express.Router();
router.use(authenticate);

/**
 * @openapi
 * /notifications:
 *   get:
 *     tags: [Notifications]
 *     summary: List my notifications (newest first)
 *     parameters:
 *       - $ref: '#/components/parameters/limitParam'
 *       - $ref: '#/components/parameters/offsetParam'
 *       - in: query
 *         name: unreadOnly
 *         schema: { type: boolean }
 *         description: If true, return only unread notifications
 *     responses:
 *       200:
 *         description: Notifications
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 notifications: { type: array, items: { $ref: '#/components/schemas/Notification' } }
 *                 unreadCount: { type: integer, example: 3 }
 */
router.get('/', listNotifications);

/**
 * @openapi
 * /notifications/read-all:
 *   put:
 *     tags: [Notifications]
 *     summary: Mark all notifications as read
 *     responses:
 *       200:
 *         description: All marked as read
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message: { type: string, example: "All notifications marked as read" }
 */
router.put('/read-all', markAllRead);

/**
 * @openapi
 * /notifications/{id}/read:
 *   put:
 *     tags: [Notifications]
 *     summary: Mark a single notification as read
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *     responses:
 *       200:
 *         description: Notification marked as read
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 notification: { $ref: '#/components/schemas/Notification' }
 *       404: { description: Notification not found }
 */
router.put('/:id/read', idParamValidation(), checkValidation, markRead);

/**
 * @openapi
 * /notifications/{id}:
 *   delete:
 *     tags: [Notifications]
 *     summary: Delete a notification
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *     responses:
 *       200:
 *         description: Notification deleted
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message: { type: string }
 *       404: { description: Notification not found }
 */
router.delete('/:id', idParamValidation(), checkValidation, deleteNotification);

export default router;
