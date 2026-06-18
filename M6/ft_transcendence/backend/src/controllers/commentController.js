/**
 * Comment Controller
 */
import Comment from '#models/Comment.js';
import Post from '#models/Post.js';
import Friend from '#models/Friend.js';
import NotificationService from '#services/notificationService.js';
import { debug } from '#lib/logger.js';
import { parseLimitOffset, parseIdParam } from '#utils/pagination.js';

async function blocked(req, authorId) {
  if (!req.user || req.user.id === authorId) return false;
  return Friend.isBlockedBetween(req.user.id, authorId);
}

/** GET /api/posts/:id/comments */
export const getComments = async (req, res, next) => {
  try {
    const postId = parseIdParam(req.params.id);
    if (postId === null) {
      return res.status(400).json({ error: { message: 'invalid post id' } });
    }

    const { limit, offset } = parseLimitOffset(req.query, { defaultLimit: 50, maxLimit: 100 });

    const comments = await Comment.getByPost(postId, { limit, offset });
    res.json({ comments });
  } catch (err) { next(err); }
};

/** POST /api/posts/:id/comments */
export const createComment = async (req, res, next) => {
  try {
    const postId = parseIdParam(req.params.id);
    if (postId === null) {
      return res.status(400).json({ error: { message: 'invalid post id' } });
    }

    const { content } = req.body;
    if (!content || !content.trim()) {
      return res.status(400).json({ error: { message: 'content is required' } });
    }
    if (content.length > 2000) {
      return res.status(400).json({ error: { message: 'comment must be 2000 characters or fewer' } });
    }

    const post = await Post.findById(postId);
    if (!post) {
      return res.status(404).json({ error: { message: 'Post not found' } });
    }
    if (await blocked(req, post.author_id)) {
      return res.status(404).json({ error: { message: 'Post not found' } });
    }

    const comment = await Comment.create({
      postId: post.id,
      authorId: req.user.id,
      content: content.trim(),
    });

    if (post.author_id !== req.user.id) {
      NotificationService
        .postCommented(post.author_id, req.user.username, post.id)
        .catch((err) => { debug("notification error (postCommented):", err.message); });
    }

    res.status(201).json({ comment });
  } catch (err) { next(err); }
};

/** DELETE /api/posts/:id/comments/:commentId */
export const deleteComment = async (req, res, next) => {
  try {
    const commentId = parseIdParam(req.params.commentId);
    if (commentId === null) {
      return res.status(400).json({ error: { message: 'invalid comment id' } });
    }

    const deleted = await Comment.delete(commentId, req.user.id);
    if (!deleted) return res.status(404).json({ error: { message: 'Comment not found or not yours' } });
    res.json({ message: 'Comment deleted' });
  } catch (err) { next(err); }
};
