/**
 * Chat Routes — /api/chat
 */
import express from 'express';
import {
  listConversations, getConversation, getUnreadCount
} from '../controllers/chatController.js';
import { authenticate } from '../middleware/auth.js';
import { body } from 'express-validator';
import { idParamValidation } from '../validators/contentValidator.js';
import { checkValidation } from '../validators/validatorUtils.js';
import { chatSendLimiter } from '../middleware/rateLimiters.js';

const router = express.Router();
router.use(authenticate);

/**
 * @openapi
 * /chat/conversations:
 *   get:
 *     tags: [Chat]
 *     summary: List all DM conversations (most-recent-message-first)
 *     responses:
 *       200:
 *         description: Conversation previews
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 conversations:
 *                   type: array
 *                   items:
 *                     type: object
 *                     properties:
 *                       userId: { type: integer, example: 99 }
 *                       username: { type: string, example: "alpaca99" }
 *                       avatar: { type: string, nullable: true }
 *                       lastMessage: { type: string, example: "GG!" }
 *                       unreadCount: { type: integer, example: 2 }
 *                       updatedAt: { type: string, format: date-time }
 */
router.get('/conversations', listConversations);

/**
 * @openapi
 * /chat/unread:
 *   get:
 *     tags: [Chat]
 *     summary: Get total unread DM message count
 *     responses:
 *       200:
 *         description: Unread count
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 count: { type: integer, example: 5 }
 */
router.get('/unread', getUnreadCount);

/**
 * @openapi
 * /chat/dm/{userId}:
 *   get:
 *     tags: [Chat]
 *     summary: Get DM history with a user (marks messages as read)
 *     parameters:
 *       - in: path
 *         name: userId
 *         required: true
 *         schema: { type: integer }
 *         description: The other user's ID
 *       - $ref: '#/components/parameters/limitParam'
 *       - $ref: '#/components/parameters/offsetParam'
 *     responses:
 *       200:
 *         description: Messages in chronological order
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 messages: { type: array, items: { $ref: '#/components/schemas/Message' } }
 *       400: { description: Invalid user ID or cannot message yourself }
 */
router.get('/dm/:userId', idParamValidation('userId'), checkValidation, getConversation);

export default router;
