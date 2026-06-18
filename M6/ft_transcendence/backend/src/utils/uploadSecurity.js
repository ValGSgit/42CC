import path from "path";
import fs from "fs";
import File from "#models/File.js";
import config from "#config/index.js";
import { authenticate } from "#middleware/auth.js";

// Treat SVG as a script-capable document — never serve it inline.
const INLINE_BLOCKED_MIME = new Set(["image/svg+xml"]);

export const uploadSecurityCheck = async (req, res, next) => {
  // Strip leading slash to get the stored filename
  const storedName = req.path.replace(/^\//, "");
  if (!storedName) return next();

  // Path-traversal guard — belt-and-braces. Express already normalises
  // req.path so traversal sequences are stripped before we see them, but
  // stored filenames are generated UUIDs/hashes that never contain slashes,
  // backslashes, or parent-dir references. Anything else is an attempt to
  // escape config.uploads.dir and we 404 immediately.
  if (
    storedName.includes("/") ||
    storedName.includes("\\") ||
    storedName.includes("..") ||
    path.isAbsolute(storedName)
  ) {
    return res.status(404).json({ error: { message: "File not found" } });
  }

  try {
    const imgMimeTypes = new Set(config.uploads.imageMimeTypes);
    const record = await File.findByStoredName(storedName);

    // AI-generated files are saved to disk but not tracked in the File table —
    // allow serving them if the file exists on disk (images only).
    if (!record) {
      const diskPath = path.join(config.uploads.dir, storedName);
      const mime = storedName.endsWith(".png")
        ? "image/png"
        : storedName.endsWith(".jpg") || storedName.endsWith(".jpeg")
          ? "image/jpeg"
          : storedName.endsWith(".webp")
            ? "image/webp"
            : null;

      if (mime && imgMimeTypes.has(mime) && fs.existsSync(diskPath)) {
        res.setHeader("Content-Type", mime);
        res.setHeader("Content-Disposition", "inline");
        res.setHeader("X-Content-Type-Options", "nosniff");
        return next();
      }
      return res.status(404).json({ error: { message: "File not found" } });
    }

    // Non-image files require authentication
    if (!imgMimeTypes.has(record.mimeType)) {
      await new Promise((resolve, reject) => {
        authenticate(req, res, (err) => (err ? reject(err) : resolve()));
      });

      // authenticate() replies with 401 if the token is missing/invalid — if we reach
      // here, req.user is set and the requester is authenticated.
      if (record.uploaderId !== req.user.id) {
        return res.status(403).json({ error: { message: "Access denied" } });
      }
    }

    // Use 'inline' for safe image types; force 'attachment' for SVG and
    // anything non-image so they cannot execute scripts in the viewer.
    if (imgMimeTypes.has(record.mimeType) && !INLINE_BLOCKED_MIME.has(record.mimeType)) {
      res.setHeader("Content-Disposition", "inline");
    } else {
      res.setHeader("Content-Disposition", "attachment");
    }
    res.setHeader("X-Content-Type-Options", "nosniff");

    next();
  } catch {
    return res
      .status(500)
      .json({ error: { message: "File access check failed" } });
  }
};
