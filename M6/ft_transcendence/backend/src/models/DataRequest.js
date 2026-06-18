/**
 * DataRequest Model — Prisma data access layer (GDPR)
 */
import prisma from "#config/prisma.js";

const DataRequest = {
  async create({ userId, type }) {
    return prisma.dataRequest.create({
      data: { userId: Number(userId), type },
    });
  },

  async findById(id) {
    return prisma.dataRequest.findUnique({ where: { id: Number(id) } });
  },

  async getByUser(userId) {
    return prisma.dataRequest.findMany({
      where: { userId: Number(userId) },
      orderBy: { createdAt: "desc" },
    });
  },

  async updateStatus(id, status, fileUrl = null) {
    const isTerminal = status === "completed" || status === "cancelled";
    return prisma.dataRequest.update({
      where: { id: Number(id) },
      data: {
        status,
        ...(fileUrl ? { fileUrl } : {}),
        ...(isTerminal ? { completedAt: new Date() } : { completedAt: null }),
      },
    });
  },

  async getPending() {
    const rows = await prisma.dataRequest.findMany({
      where: { status: { in: ["pending", "processing"] } },
      include: { user: { select: { username: true } } },
      orderBy: { createdAt: "asc" },
    });
    return rows.map((r) => ({
      ...r,
      username: r.user.username,
      user: undefined,
    }));
  },
};

export default DataRequest;
