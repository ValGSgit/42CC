/**
 * Upload Controller
 */
import File from '#models/File.js';
import { saveFileRecord, deleteFileFromDisk, validateFileMagicBytes } from '#services/uploadService.js';
import { parseLimitOffset, parseIdParam } from '#utils/pagination.js';
import { error as logError } from '#lib/logger.js';

/** POST /api/uploads — upload one or more files */
export const uploadFiles = async (req, res, next) => {
  try {
    if (!req.files?.length && !req.file) {
      return res.status(400).json({ error: { message: 'No file uploaded' } });
    }
    const multerFiles = req.files || [req.file];

    // Validate file content against declared MIME type before persisting.
    for (const file of multerFiles) {
      const valid = await validateFileMagicBytes(file);
      if (!valid) {
        // Cleanup the on-disk artifacts multer just wrote. Errors here are
        // logged (so we notice a read-only mount) but not surfaced — the
        // user's real problem is the rejected upload, not the cleanup.
        await Promise.all(
          multerFiles.map((f) =>
            deleteFileFromDisk(f.filename).catch((err) => {
              logError(`[uploads] orphan cleanup failed for ${f.filename}:`, err?.message || err);
            }),
          ),
        );
        return res.status(400).json({
          error: { message: `${file.originalname} is not a valid ${file.mimetype} file` },
        });
      }
    }

    const records = await Promise.all(multerFiles.map((f) => saveFileRecord(req.user.id, f)));
    res.status(201).json({ files: records });
  } catch (err) { next(err); }
};

/** GET /api/uploads — list my uploads */
export const listMyFiles = async (req, res, next) => {
  try {
    const { limit, offset } = parseLimitOffset(req.query, { defaultLimit: 50, maxLimit: 100 });
    const files = await File.getByUploader(req.user.id, { limit, offset });
    res.json({ files });
  } catch (err) { next(err); }
};

/** DELETE /api/uploads/:id */
export const deleteFile = async (req, res, next) => {
  try {
    const fileId = parseIdParam(req.params.id);
    if (fileId === null) {
      return res.status(400).json({ error: { message: 'invalid file id' } });
    }

    // 1. Delete the DB row first. If this fails (FK constraint, race with
    //    another request, ...) the file stays on disk and the caller learns
    //    why — far better than orphaning a referenced row.
    const record = await File.delete(fileId, req.user.id);
    if (!record) {
      return res.status(404).json({ error: { message: 'File not found or not yours' } });
    }

    // 2. DB row is gone; now remove the bytes. A failure here leaves an
    //    orphan blob (no DB pointer) which the operator can sweep later;
    //    we log it so it doesn't go unnoticed.
    try {
      await deleteFileFromDisk(record.storedName);
    } catch (err) {
      logError(`[uploads] disk cleanup failed for ${record.storedName}:`, err?.message || err);
    }

    res.json({ message: 'File deleted' });
  } catch (err) { next(err); }
};
