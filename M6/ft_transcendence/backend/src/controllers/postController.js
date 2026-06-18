/**
 * Post Controller — social feed / posts
 */
import Post from "#models/Post.js";
import Friend from "#models/Friend.js";
import NotificationService from "#services/notificationService.js";
import { debug } from "#lib/logger.js";
import GamificationService from "#services/GamificationService.js";
import prisma from "#config/prisma.js";
import { parseLimitOffset, parseIdParam } from "#utils/pagination.js";

// Treat a block as if the post does not exist (404 not 403) so the blocked
// user can't learn whether the author exists or has interacted with them.
async function notFoundIfBlocked(req, authorId) {
  if (!req.user || req.user.id === authorId) return false;
  return Friend.isBlockedBetween(req.user.id, authorId);
}

/** GET /api/posts */
export const getFeed = async (req, res, next) => {
  try {
    const { limit, offset } = parseLimitOffset(req.query, { defaultLimit: 20, maxLimit: 100 });
    const posts = await Post.getFeed({
      limit,
      offset,
      viewerId: req.user?.id,
    });
    res.json({ posts });
  } catch (err) {
    next(err);
  }
};

/** GET /api/posts/user/:userId */
export const getUserPosts = async (req, res, next) => {
  try {
    const userId = parseIdParam(req.params.userId);
    if (userId === null) {
      return res.status(400).json({ error: { message: "invalid user id" } });
    }

    const { limit, offset } = parseLimitOffset(req.query, { defaultLimit: 20, maxLimit: 100 });
    const posts = await Post.getByUser(userId, { limit, offset });
    res.json({ posts });
  } catch (err) {
    next(err);
  }
};

/** POST /api/posts */
export const createPost = async (req, res, next) => {
  try {
    const { content, imageUrl, image_url, isPublic = true } = req.body;
    const normalizedImageUrl = imageUrl ?? image_url ?? null;
    if (!content?.trim())
      return res
        .status(400)
        .json({ error: { message: "content is required" } });
    if (content.length > 5000)
      return res.status(400).json({
        error: { message: "content must be 5000 characters or fewer" },
      });
    if (
      normalizedImageUrl &&
      (typeof normalizedImageUrl !== "string" ||
        normalizedImageUrl.length > 2048)
    ) {
      return res.status(400).json({ error: { message: "invalid imageUrl" } });
    }
    const post = await Post.create({
      authorId: req.user.id,
      content: content.trim(),
      imageUrl: normalizedImageUrl,
      isPublic: !!isPublic,
    });
    res.status(201).json({ post });
    const postCount = await prisma.post.count({
      where: { authorId: req.user.id },
    });
    if (postCount === 1)
      await GamificationService.unlock(req.user.id, "spit_facts").catch(
        () => {},
      );
  } catch (err) {
    next(err);
  }
};

/** GET /api/posts/:id */
export const getPost = async (req, res, next) => {
  try {
    const postId = parseIdParam(req.params.id);
    if (postId === null) {
      return res.status(400).json({ error: { message: "invalid post id" } });
    }
    const post = await Post.findById(postId);
    if (!post)
      return res.status(404).json({ error: { message: "Post not found" } });
    res.json({ post });
  } catch (err) {
    next(err);
  }
};

/** PUT /api/posts/:id */
export const updatePost = async (req, res, next) => {
  try {
    const { content, imageUrl, image_url, isPublic } = req.body;
    const normalizedImageUrl = imageUrl ?? image_url;
    if (content !== undefined && (!content?.trim() || content.length > 5000)) {
      return res.status(400).json({
        error: { message: "content must be between 1 and 5000 characters" },
      });
    }
    if (
      normalizedImageUrl !== undefined &&
      normalizedImageUrl !== null &&
      (typeof normalizedImageUrl !== "string" ||
        normalizedImageUrl.length > 2048)
    ) {
      return res.status(400).json({ error: { message: "invalid imageUrl" } });
    }
    const postId = parseIdParam(req.params.id);
    if (postId === null) {
      return res.status(400).json({ error: { message: "invalid post id" } });
    }
    const post = await Post.update(postId, req.user.id, {
      content: content !== undefined ? content.trim() : undefined,
      imageUrl: normalizedImageUrl,
      isPublic,
    });
    if (!post)
      return res
        .status(404)
        .json({ error: { message: "Post not found or not yours" } });
    res.json({ post });
  } catch (err) {
    next(err);
  }
};

/** DELETE /api/posts/:id */
export const deletePost = async (req, res, next) => {
  try {
    const id = parseIdParam(req.params.id);
    if (id === null) {
      return res.status(400).json({ error: { message: "Invalid post id" } });
    }

    // Owner path: fast, no pre-fetch.
    const deleted = await Post.delete(id, req.user.id);
    if (deleted) return res.json({ message: "Post deleted" });

    return res
      .status(404)
      .json({ error: { message: "Post not found or not yours" } });
  } catch (err) {
    next(err);
  }
};

/** POST /api/posts/:id/like */
export const likePost = async (req, res, next) => {
  try {
    const postId = parseIdParam(req.params.id);
    if (postId === null) {
      return res.status(400).json({ error: { message: "invalid post id" } });
    }
    const post = await Post.findById(postId);
    if (!post)
      return res.status(404).json({ error: { message: "Post not found" } });
    if (await notFoundIfBlocked(req, post.author_id))
      return res.status(404).json({ error: { message: "Post not found" } });
    // Post.like returns true only on first-time like — false when the user
    // re-likes (P2002). Gate the side effects on `inserted` so re-clicking
    // the heart doesn't re-fire the author's notification or re-award XP.
    const inserted = await Post.like(post.id, req.user.id);
    res.json({ message: "Liked" });
    if (!inserted) return;
    const newLikesCount = (post.likes_count ?? 0) + 1;
    if (post.author_id !== req.user.id) {
      NotificationService.postLiked(
        post.author_id,
        req.user.username,
        post.id,
      ).catch((err) => {
        debug("notification error (postLiked):", err.message);
      });
    }
    GamificationService.onPostLiked(post.author_id, newLikesCount).catch(
      () => {},
    );
  } catch (err) {
    next(err);
  }
};

/** DELETE /api/posts/:id/like */
export const unlikePost = async (req, res, next) => {
  try {
    const postId = parseIdParam(req.params.id);
    if (postId === null) {
      return res.status(400).json({ error: { message: "invalid post id" } });
    }
    await Post.unlike(postId, req.user.id);
    res.json({ message: "Unliked" });
  } catch (err) {
    next(err);
  }
};
