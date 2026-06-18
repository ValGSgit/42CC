/**
 * File Model — Prisma data access layer
 */
import prisma from "#config/prisma.js";

/** Convert BigInt fields to Number so JSON.stringify works. */
function safeFile(f) {
  if (!f) return f;
  return { ...f, sizeBytes: f.sizeBytes != null ? Number(f.sizeBytes) : null };
}

const File = {
  async create({
    uploaderId,
    originalName,
    storedName,
    mimeType,
    sizeBytes,
    url,
  }) {
    const record = await prisma.file.create({
      data: {
        uploaderId: Number(uploaderId),
        originalName,
        storedName,
        mimeType,
        sizeBytes: BigInt(sizeBytes),
        url,
      },
    });
    return safeFile(record);
  },

  async findById(id) {
    return safeFile(
      await prisma.file.findUnique({ where: { id: Number(id) } }),
    );
  },

  async findByStoredName(storedName) {
    return safeFile(await prisma.file.findFirst({ where: { storedName } }));
  },

  async getByUploader(uploaderId, { limit = 50, offset = 0 } = {}) {
    const rows = await prisma.file.findMany({
      where: { uploaderId: Number(uploaderId) },
      orderBy: { createdAt: "desc" },
      take: Number(limit),
      skip: Number(offset),
    });
    return rows.map(safeFile);
  },

  async delete(id, uploaderId) {
    const record = await prisma.file.findFirst({
      where: { id: Number(id), uploaderId: Number(uploaderId) },
    });
    if (!record) return null;
    await prisma.file.delete({ where: { id: Number(id) } });
    return safeFile(record);
  },
};

export default File;
