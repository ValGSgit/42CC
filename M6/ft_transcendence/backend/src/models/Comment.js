/**
 * Comment Model — Prisma data access layer
 */
import prisma from "#config/prisma.js";
import { stripDangerousHtml } from "#utils/htmlSanitizer.js";

const AUTHOR_SELECT = { select: { username: true, avatar: true } };

function shapeComment(c) {
  return {
    id: c.id,
    post_id: c.postId,
    author_id: c.authorId,
    content: c.content,
    created_at: c.createdAt,
    updated_at: c.updatedAt,
    author_username: c.author?.username,
    author_avatar: c.author?.avatar,
  };
}

const Comment = {
  async create({ postId = null, authorId, content }) {
    const comment = await prisma.$transaction(async (tx) => {
      const c = await tx.comment.create({
        data: {
          postId: postId !== null ? Number(postId) : null,
          authorId: Number(authorId),
          content: typeof content === "string" ? stripDangerousHtml(content) : content,
        },
        include: { author: AUTHOR_SELECT },
      });
      if (postId !== null) {
        const count = await tx.comment.count({ where: { postId: Number(postId) } });
        await tx.post.update({ where: { id: Number(postId) }, data: { commentsCount: count } });
      }
      return c;
    });
    return shapeComment(comment);
  },

  async getByThread({ postId = null, limit = 50, offset = 0 } = {}) {
    if (postId == null) {
      throw new Error("Comment.getByThread requires postId");
    }
    const comments = await prisma.comment.findMany({
      where: { postId: Number(postId) },
      include: { author: AUTHOR_SELECT },
      orderBy: { createdAt: "asc" },
      take: Number(limit),
      skip: Number(offset),
    });
    return comments.map(shapeComment);
  },

  async getByPost(postId, options = {}) {
    return this.getByThread({ postId, ...options });
  },

  /**
   * Delete a comment. Authorised when the requester is the comment author or
   * owns the parent post. Returns true on delete, false on not-found / not
   * authorised.
   */
  async delete(id, requesterId) {
    const comment = await prisma.comment.findUnique({ where: { id: Number(id) } });
    if (!comment) return false;

    const isAuthor = comment.authorId === Number(requesterId);
    let isThreadOwner = false;
    if (!isAuthor && comment.postId !== null) {
      const post = await prisma.post.findUnique({
        where: { id: comment.postId },
        select: { authorId: true },
      });
      isThreadOwner = post?.authorId === Number(requesterId);
    }
    if (!isAuthor && !isThreadOwner) return false;

    await prisma.$transaction(async (tx) => {
      await tx.comment.delete({ where: { id: Number(id) } });
      if (comment.postId !== null) {
        const count = await tx.comment.count({ where: { postId: comment.postId } });
        await tx.post.update({ where: { id: comment.postId }, data: { commentsCount: count } });
      }
    });
    return true;
  },
};

export default Comment;
