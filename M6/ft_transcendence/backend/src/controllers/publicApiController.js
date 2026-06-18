/**
 * Public API Controller — documented endpoints with API key auth
 *
 * Endpoints:
 *   GET    /api/public/users          — list public users
 *   GET    /api/public/users/:id      — get a public user profile
 *   GET    /api/public/posts          — public feed
 *   POST   /api/public/posts          — create a post
 *   PUT    /api/public/posts/:id      — update a post
 *   DELETE /api/public/posts/:id      — delete a post
 */
import User from "../models/User.js";
import Post from "../models/Post.js";
import CustomError from "#utils/CustomError.js";
import { parseLimitOffset } from "#utils/pagination.js";

const toPublicUser = (user) => ({
  id: user.id,
  username: user.username,
  avatar: user.avatar,
  bio: user.bio,
  status: user.status,
  is_online: user.isOnline,
  created_at: user.createdAt,
});

const isUserPublic = (user) => {
  return user?.userSettings?.isPublic === true;
};

/** GET /api/public/users?filter[username]=&limit=20&offset=0 */
export const listUsers = async (req, res, next) => {
  try {
    // Use the shared limit/offset clamp instead of passing raw query strings
    // to the DB. A valid API key + `?limit=1000000` could otherwise stream
    // the full user table on a single request.
    const { limit, offset } = parseLimitOffset(req.query, { defaultLimit: 20, maxLimit: 100 });
    // Force a public-only predicate so pagination + total reflect filtered set.
    const filter = { ...(req.query.filter || {}), public: true };
    const sort = req.query.sort;

    const searchRes = await User.search({ limit, offset, filter, sort });
    res.json({
      users: searchRes.usersFound.map((u) => toPublicUser(u)),
      total: searchRes.userCount,
    });
  } catch (err) {
    next(err);
  }
};

/** GET /api/public/users/:id */
export const getUser = async (req, res, next) => {
  try {
    const user = await User.findById(Number(req.params.id));
    if (!user || !isUserPublic(user))
      throw new CustomError("User not found", 404);
    res.json({ user: toPublicUser(user) });
  } catch (err) {
    next(err);
  }
};

/**
 * GET /api/public/posts?limit=20&offset=0
 */
export const getPosts = async (req, res, next) => {
  try {
    // Shared clamp — same reasoning as listUsers: never pass raw, unbounded
    // pagination input to the DB.
    const { limit, offset } = parseLimitOffset(req.query, { defaultLimit: 20, maxLimit: 100 });
    const posts = await Post.getFeed({
      limit,
      offset,
    });
    const shaped = posts.map((p) => ({
      id: p.id,
      author_id: p.author_id,
      author_username: p.author_username,
      author_avatar: p.author_avatar,
      content: p.content,
      image_url: p.image_url,
      likes_count: p.likes_count,
      comments_count: p.comments_count,
      created_at: p.created_at,
    }));
    res.json({ posts: shaped });
  } catch (err) {
    next(err);
  }
};

// Same length cap the in-app post controller enforces. Kept inline (rather
// than imported from postController) so the public API has no surprising
// drift if the in-app limit changes — bump both sides intentionally.
const POST_CONTENT_MAX = 5000;
const POST_IMAGE_URL_MAX = 2048;

/** Shape-validates a post body. Returns an error message string, or null. */
function validatePostBody(body, { contentRequired }) {
  const { content, imageUrl } = body;
  if (contentRequired || content !== undefined) {
    if (typeof content !== "string" || !content.trim()) {
      return "content is required";
    }
    if (content.length > POST_CONTENT_MAX) {
      return `content must be ${POST_CONTENT_MAX} characters or fewer`;
    }
  }
  if (imageUrl !== undefined && imageUrl !== null) {
    if (typeof imageUrl !== "string" || imageUrl.length > POST_IMAGE_URL_MAX) {
      return "invalid imageUrl";
    }
  }
  return null;
}

/**
 * POST /api/public/posts — create a post via API key.
 *
 * - the apiKey it tied to a user and you can create posts for the user
 */
export const createPost = async (req, res, next) => {
  try {
    // Previously this endpoint passed req.body.content straight to Post.create
    // with no checks — `{}` would store an empty post and a 1MB body would
    // store the lot. Mirror the in-app validator at postController.js.
    const errMsg = validatePostBody(req.body, { contentRequired: true });
    if (errMsg) throw new CustomError(errMsg, 400);

    const { content, imageUrl, isPublic } = req.body;
    const post = await Post.create({
      authorId: req.userId,
      content: content.trim(),
      imageUrl: imageUrl || null,
      isPublic,
    });
    res.status(201).json({ post });
  } catch (err) {
    next(err);
  }
};

/**
 * PUT /api/public/posts/:id — update a post via API key.
 */
export const updatePost = async (req, res, next) => {
  try {
    // Same shape rules as createPost, but content is optional on update so
    // an API client can edit just the image without resending the body.
    const errMsg = validatePostBody(req.body, { contentRequired: false });
    if (errMsg) throw new CustomError(errMsg, 400);

    const { content, imageUrl, isPublic } = req.body;
    const postId = Number(req.params.id);

    const existingPost = await Post.findById(postId);
    if (!existingPost) throw new CustomError("Post not found", 404);

    if (existingPost.author_id !== req.userId)
      throw new CustomError("Can not modify post of other user", 403);

    const post = await Post.update(postId, req.userId, {
      content: content !== undefined ? content.trim() : undefined,
      imageUrl: imageUrl || null,
      isPublic,
    });
    res.json({ post });
  } catch (err) {
    next(err);
  }
};

/**
 * DELETE /api/public/posts/:id — delete a post via API key.
 */
export const deletePost = async (req, res, next) => {
  try {
    const id = Number(req.params.id);
    if (!Number.isFinite(id)) throw new CustomError("Invalid post id", 400);

    const existingPost = await Post.findById(id);
    if (!existingPost) throw new CustomError("Post not found", 404);

    if (existingPost.author_id !== req.userId)
      throw new CustomError("Can not delete post of other user", 403);

    await Post.delete(id, req.userId);
    res.json({ message: "Post deleted" });
  } catch (err) {
    next(err);
  }
};
