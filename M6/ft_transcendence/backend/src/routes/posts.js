/**
 * Post Routes — /api/posts
 */
import express from 'express';
import {
  getFeed, getUserPosts, createPost, getPost,
  updatePost, deletePost, likePost, unlikePost,
} from '../controllers/postController.js';
import {
  getComments, createComment, deleteComment,
} from '../controllers/commentController.js';
import { authenticate, optionalAuth } from '../middleware/auth.js';
import {
  postCreateValidation, postUpdateValidation,
  commentCreateValidation, idParamValidation,
} from '../validators/contentValidator.js';
import { checkValidation } from '../validators/validatorUtils.js';
import {
  postWriteLimiter, commentWriteLimiter, likeLimiter,
} from '../middleware/rateLimiters.js';

const router = express.Router();

/**
 * @openapi
 * /posts:
 *   get:
 *     tags: [Posts]
 *     summary: Get the social feed (public posts + friends' posts)
 *     security:
 *       - X-API-KEY: []
 *     parameters:
 *       - $ref: '#/components/parameters/limitParam'
 *       - $ref: '#/components/parameters/offsetParam'
 *     responses:
 *       200:
 *         description: Feed posts
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 posts: { type: array, items: { $ref: '#/components/schemas/Post' } }
 *   post:
 *     tags: [Posts]
 *     summary: Create a new post
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             required: [content]
 *             properties:
 *               content: { type: string, maxLength: 2000, example: "Just won my first Spit Royale! 🏆" }
 *               imageUrl: { type: string, nullable: true, example: "/uploads/screenshot.png" }
 *               is_public: { type: boolean, default: true }
 *     responses:
 *       201:
 *         description: Post created
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 post: { $ref: '#/components/schemas/Post' }
 *       400: { description: content is required }
 */
router.get('/', authenticate, getFeed);
router.post('/', authenticate, postWriteLimiter, postCreateValidation(), checkValidation, createPost);

/**
 * @openapi
 * /posts/user/{userId}:
 *   get:
 *     tags: [Posts]
 *     summary: Get posts by a specific user
 *     security: [{}]
 *     parameters:
 *       - in: path
 *         name: userId
 *         required: true
 *         schema: { type: integer }
 *       - $ref: '#/components/parameters/limitParam'
 *       - $ref: '#/components/parameters/offsetParam'
 *     responses:
 *       200:
 *         description: User's posts
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 posts: { type: array, items: { $ref: '#/components/schemas/Post' } }
 */
router.get('/user/:userId', optionalAuth, idParamValidation('userId'), checkValidation, getUserPosts);

/**
 * @openapi
 * /posts/{id}:
 *   get:
 *     tags: [Posts]
 *     summary: Get a single post
 *     security: [{}]
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *     responses:
 *       200:
 *         description: Post
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 post: { $ref: '#/components/schemas/Post' }
 *       404: { description: Post not found }
 *   put:
 *     tags: [Posts]
 *     summary: Update my post
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             properties:
 *               content: { type: string, maxLength: 2000 }
 *               imageUrl: { type: string, nullable: true }
 *     responses:
 *       200:
 *         description: Updated post
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 post: { $ref: '#/components/schemas/Post' }
 *       403: { description: Not your post }
 *       404: { description: Post not found }
 *   delete:
 *     tags: [Posts]
 *     summary: Delete my post
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *     responses:
 *       200:
 *         description: Post deleted
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message: { type: string }
 *       403: { description: Not your post }
 *       404: { description: Post not found }
 */
router.get('/:id', optionalAuth, idParamValidation(), checkValidation, getPost);
router.put('/:id', authenticate, postWriteLimiter, idParamValidation(), postUpdateValidation(), checkValidation, updatePost);
router.delete('/:id', authenticate, idParamValidation(), checkValidation, deletePost);

/**
 * @openapi
 * /posts/{id}/like:
 *   post:
 *     tags: [Posts]
 *     summary: Like a post
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *     responses:
 *       200:
 *         description: Liked
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 likes_count: { type: integer }
 *       409: { description: Already liked }
 *   delete:
 *     tags: [Posts]
 *     summary: Unlike a post
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *     responses:
 *       200:
 *         description: Unliked
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 likes_count: { type: integer }
 */
router.post('/:id/like', authenticate, likeLimiter, idParamValidation(), checkValidation, likePost);
router.delete('/:id/like', authenticate, likeLimiter, idParamValidation(), checkValidation, unlikePost);

/**
 * @openapi
 * /posts/{id}/comments:
 *   get:
 *     tags: [Posts]
 *     summary: Get comments on a post
 *     security: [{}]
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *       - $ref: '#/components/parameters/limitParam'
 *       - $ref: '#/components/parameters/offsetParam'
 *     responses:
 *       200:
 *         description: Comments
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 comments: { type: array, items: { $ref: '#/components/schemas/Comment' } }
 *   post:
 *     tags: [Posts]
 *     summary: Add a comment to a post
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             required: [content]
 *             properties:
 *               content: { type: string, maxLength: 1000, example: "GG! 🎉" }
 *     responses:
 *       201:
 *         description: Comment created
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 comment: { $ref: '#/components/schemas/Comment' }
 */
router.get('/:id/comments', optionalAuth, idParamValidation(), checkValidation, getComments);
router.post('/:id/comments', authenticate, commentWriteLimiter, idParamValidation(), commentCreateValidation(), checkValidation, createComment);

/**
 * @openapi
 * /posts/{id}/comments/{commentId}:
 *   delete:
 *     tags: [Posts]
 *     summary: Delete a comment (own comment, or post owner)
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *         description: Post ID
 *       - in: path
 *         name: commentId
 *         required: true
 *         schema: { type: integer }
 *         description: Comment ID
 *     responses:
 *       200:
 *         description: Comment deleted
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message: { type: string }
 *       403: { description: Not authorized }
 *       404: { description: Comment not found }
 */
router.delete(
  '/:id/comments/:commentId',
  authenticate,
  idParamValidation(), idParamValidation('commentId'), checkValidation,
  deleteComment,
);

export default router;
