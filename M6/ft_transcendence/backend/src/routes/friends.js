/**
 * Friend Routes — /api/friends
 */
import express from "express";
import {
  listFriends,
  listOnlineFriends,
  listRequests,
  sendRequest,
  acceptRequest,
  declineRequest,
  removeFriend,
  blockUser,
  unblockUser,
  listBlocked,
} from "../controllers/friendController.js";
import { authenticate } from "../middleware/auth.js";
import { body } from "express-validator";
import { checkValidation } from "#validators/validatorUtils.js";
import { friendRequestLimiter } from "#middleware/rateLimiters.js";
import { idParamValidation } from "#validators/contentValidator.js";
import { paginationValidation } from "#validators/paginationValidator.js";

const router = express.Router();
router.use(authenticate);

/**
 * @openapi
 * /friends:
 *   get:
 *     tags: [Friends]
 *     summary: List my friends
 *     parameters:
 *       - in: query
 *         name: limit
 *         schema: { type: integer, default: 50 }
 *       - in: query
 *         name: offset
 *         schema: { type: integer, default: 0 }
 *     responses:
 *       200:
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 friends: { type: array, items: { $ref: '#/components/schemas/User' } }
 */
router.get("/", paginationValidation(100), checkValidation, listFriends);

router.get("/online", listOnlineFriends);
router.get("/blocked", listBlocked);

/**
 * @openapi
 * /friends/requests:
 *   get:
 *     tags: [Friends]
 *     summary: List friend requests (received and sent)
 *     responses:
 *       200:
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 received: { type: array, items: { $ref: '#/components/schemas/FriendRequest' } }
 *                 sent: { type: array, items: { $ref: '#/components/schemas/FriendRequest' } }
 *   post:
 *     tags: [Friends]
 *     summary: Send a friend request
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             required: [userId]
 *             properties:
 *               userId: { type: integer, description: "ID of the user to send a request to", example: 42 }
 *     responses:
 *       201:
 *         description: Request sent
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 request: { $ref: '#/components/schemas/FriendRequest' }
 *       400: { description: Cannot send request to yourself or request already exists }
 *       404: { description: User not found }
 */
router.get("/requests", listRequests);
router.post(
  "/requests",
  friendRequestLimiter,
  [
    body("userId")
      .isInt({ min: 1 })
      .withMessage("userId must be a positive integer"),
  ],
  checkValidation,
  sendRequest,
);

/**
 * @openapi
 * /friends/requests/{id}/accept:
 *   put:
 *     tags: [Friends]
 *     summary: Accept a received friend request
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *         description: Friend request ID
 *     responses:
 *       200:
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 request: { $ref: '#/components/schemas/FriendRequest' }
 *       403: { description: Not your request to accept }
 *       404: { description: Request not found }
 */
router.put(
  "/requests/:id/accept",
  idParamValidation(),
  checkValidation,
  acceptRequest,
);

/**
 * @openapi
 * /friends/requests/{id}/decline:
 *   put:
 *     tags: [Friends]
 *     summary: Decline a received friend request
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *     responses:
 *       200:
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 request: { $ref: '#/components/schemas/FriendRequest' }
 */
router.put(
  "/requests/:id/decline",
  idParamValidation(),
  checkValidation,
  declineRequest,
);

/**
 * @openapi
 * /friends/{id}:
 *   delete:
 *     tags: [Friends]
 *     summary: Remove a friend
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *         description: User ID of the friend to remove
 *     responses:
 *       200:
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message: { type: string }
 */
router.delete("/:id", idParamValidation(), checkValidation, removeFriend);

/**
 * @openapi
 * /friends/block:
 *   post:
 *     tags: [Friends]
 *     summary: Block a user
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             required: [userId]
 *             properties:
 *               userId: { type: integer, example: 42 }
 *     responses:
 *       200:
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message: { type: string }
 */
router.post(
  "/block",
  [
    body("userId")
      .isInt({ min: 1 })
      .withMessage("userId must be a positive integer"),
  ],
  checkValidation,
  blockUser,
);

/**
 * @openapi
 * /friends/block/{id}:
 *   delete:
 *     tags: [Friends]
 *     summary: Unblock a user
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *         description: User ID to unblock
 *     responses:
 *       200:
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message: { type: string }
 */
router.delete("/block/:id", idParamValidation(), checkValidation, unblockUser);

export default router;
