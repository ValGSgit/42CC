/**
 * Post Model — Prisma data access layer
 */
import prisma from "#config/prisma.js";
import { stripDangerousHtml } from "#utils/htmlSanitizer.js";

const AUTHOR_SELECT = { select: { username: true, avatar: true } };

const sanitizeContent = (c) =>
  typeof c === "string" ? stripDangerousHtml(c) : c;

function shapePost(p, likedIds = null) {
  return {
    id: p.id,
    author_id: p.authorId,
    content: p.content,
    image_url: p.imageUrl,
    is_public: p.isPublic,
    likes_count: p.likesCount,
    likeCount: p.likesCount,
    comments_count: p.commentsCount ?? 0,
    created_at: p.createdAt,
    updated_at: p.updatedAt,
    author_username: p.author?.username,
    author_avatar: p.author?.avatar,
    user_liked: likedIds ? likedIds.has(p.id) : false,
  };
}

const Post = {
  async create({ authorId, content, imageUrl = null, isPublic = true }) {
    const post = await prisma.post.create({
      data: { authorId, content: sanitizeContent(content), imageUrl, isPublic },
      include: { author: AUTHOR_SELECT },
    });
    return shapePost(post);
  },

  async findById(id) {
    const post = await prisma.post.findUnique({
      where: { id: Number(id) },
      include: { author: AUTHOR_SELECT },
    });
    return post ? shapePost(post) : null;
  },

  async update(id, authorId, fields) {
    const data = {};
    if (fields.content !== undefined) data.content = sanitizeContent(fields.content);
    if (fields.imageUrl !== undefined) data.imageUrl = fields.imageUrl;
    if (fields.isPublic !== undefined) data.isPublic = fields.isPublic;
    if (Object.keys(data).length === 0) {
      const post = await this.findById(id);
      return post?.author_id === Number(authorId) ? post : null;
    }

    // updateMany matches on (id, authorId) atomically — portable across all
    // Prisma versions. `update({ where: { id, authorId } })` requires
    // extendedWhereUnique preview on older Prisma and may otherwise silently
    // update posts the caller doesn't own.
    const { count } = await prisma.post.updateMany({
      where: { id: Number(id), authorId: Number(authorId) },
      data,
    });
    if (count === 0) return null;
    return this.findById(id);
  },

  async delete(postId, authorId) {
    const { count } = await prisma.post.deleteMany({
      where: { id: Number(postId), authorId: Number(authorId) },
    });
    return count > 0;
  },

  async getFeed({ limit = 200, offset = 0, viewerId = null } = {}) {
    const vid = viewerId ? Number(viewerId) : null;
    const lim = Number(limit);
    const off = Number(offset);

    // Build base where clause: public posts by public authors,
    // OR posts authored by the viewer themselves (so a private user still
    // sees their own feed).
    const baseWhere = {
      isPublic: true,
      ...(vid !== null
        ? {
            OR: [
              { author: { userSettings: { isPublic: true } } },
              { authorId: vid },
            ],
          }
        : { author: { userSettings: { isPublic: true } } }),
    };

    // If viewerId is provided, exclude posts where the author blocked the viewer
    // or the viewer blocked the author.
    if (vid !== null) {
      baseWhere.NOT = [
        { author: { blockedUsers: { some: { blockedUserId: vid } } } }, // author blocked viewer
        { author: { blockedBy: { some: { userId: vid } } } }, // viewer blocked author
      ];
    }

    const [posts, liked] = await Promise.all([
      prisma.post.findMany({
        where: baseWhere,
        include: { author: AUTHOR_SELECT },
        orderBy: { createdAt: "desc" },
        take: lim,
        skip: off,
      }),
      vid
        ? prisma.postLike.findMany({
            where: { userId: vid },
            select: { postId: true },
          })
        : Promise.resolve([]),
    ]);

    const likedIds = new Set(liked.map((l) => l.postId));
    return posts.map((p) => shapePost(p, likedIds));
  },

  async getByUser(userId, { limit = 20, offset = 0 } = {}) {
    const posts = await prisma.post.findMany({
      where: { authorId: Number(userId) },
      include: { author: AUTHOR_SELECT },
      orderBy: { createdAt: "desc" },
      take: Number(limit),
      skip: Number(offset),
    });
    return posts.map((p) => shapePost(p));
  },

  async like(postId, userId) {
    // Returns true if a new like row was inserted (and likesCount incremented),
    // false if the user had already liked the post. Callers use this to gate
    // notifications + gamification so re-liking doesn't fire them twice.
    return prisma.$transaction(async (tx) => {
      try {
        await tx.postLike.create({
          data: { postId: Number(postId), userId: Number(userId) },
        });
      } catch (e) {
        if (e.code === "P2002") return false; // already liked — no-op
        throw e;
      }
      await tx.post.update({
        where: { id: Number(postId) },
        data: { likesCount: { increment: 1 } },
      });
      return true;
    });
  },

  async unlike(postId, userId) {
    await prisma.$transaction(async (tx) => {
      const { count } = await tx.postLike.deleteMany({
        where: { postId: Number(postId), userId: Number(userId) },
      });
      if (count > 0) {
        await tx.post.update({
          where: { id: Number(postId) },
          data: { likesCount: { decrement: 1 } },
        });
      }
    });
  },



  async count() {
    return prisma.post.count();
  },
};

export default Post;
