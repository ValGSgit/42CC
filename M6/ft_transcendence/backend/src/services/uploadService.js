/**
 * Upload Service — multer configuration for local disk uploads
 */
import multer from "multer";
import path from "path";
import fs from "fs";
import crypto from "crypto";
import config from "../config/index.js";
import File from "../models/File.js";

// ── Magic-byte (file-signature) table for image MIME types ──────────────────
//
// "Magic bytes" are the handful of bytes at the very start of a file that
// reveal its TRUE format, independent of the file extension or the
// Content-Type the client claims. The browser's declared MIME type and the
// filename are both attacker-controlled: someone using curl (or a tampered
// form) can label an SVG full of <script>, or an HTML page, as "image/png".
// Comparing the leading bytes against a known signature is how we catch that
// lie before the file is ever stored or served.
//
// Each entry maps a MIME type to a LIST of accepted byte sequences. A list is
// needed because some formats have more than one legal header — e.g. GIF may
// begin with the ASCII text "GIF87a" or "GIF89a" depending on its version.
//
//   image/jpeg  →  FF D8 FF                  every JPEG/JFIF/EXIF starts here
//   image/png   →  89 50 4E 47               byte 0x89 followed by ASCII "PNG"
//   image/gif   →  47 49 46 38 37 61  ("GIF87a")
//                  47 49 46 38 39 61  ("GIF89a")
//   image/webp  →  handled separately below (see validateFileMagicBytes):
//                  WebP is a RIFF container, so its signature is SPLIT — the
//                  ASCII "RIFF" sits at bytes 0-3 and "WEBP" at bytes 8-11,
//                  with the 4-byte file size in between (bytes 4-7) which we
//                  intentionally don't check. `null` here is just a marker
//                  that the generic comparison below doesn't apply to it.
const IMAGE_MAGIC = {
  "image/jpeg": [[0xff, 0xd8, 0xff]],
  "image/png":  [[0x89, 0x50, 0x4e, 0x47]],
  "image/gif":  [[0x47, 0x49, 0x46, 0x38, 0x37, 0x61],
                 [0x47, 0x49, 0x46, 0x38, 0x39, 0x61]],
  // WebP: bytes 0-3 == "RIFF" AND bytes 8-11 == "WEBP" (see function below).
  "image/webp": null,
};

const MIN_IMAGE_BYTES = 100;

/**
 * validateFileMagicBytes — confirm an uploaded file's real type matches the
 * MIME type the client declared, by inspecting its leading bytes.
 *
 * WHY THIS EXISTS:
 *   multer's `fileFilter` (below) only trusts the Content-Type the browser
 *   sends, which is trivial to forge. Without this second check an attacker
 *   could upload a malicious .svg/.html and have it persisted under an
 *   "image/png" File record, then served to other users. Reading the actual
 *   file signature closes that gap — it is the real safeguard, not the
 *   extension or the declared MIME.
 *
 * HOW IT WORKS, STEP BY STEP:
 *   1. If the declared MIME isn't an image type we have a signature for
 *      (PDF, text, CSV, JSON, XML), skip and return true. Those types are
 *      never served inline and require authentication to download (see
 *      uploadSecurity.js), so a forged one can't execute in someone else's
 *      browser; strict sniffing there would need much larger buffers for
 *      little gain.
 *   2. Open the file and read its first 12 bytes — enough to cover the
 *      longest signature we check (WebP needs through byte 11). diskStorage
 *      always provides a real `file.path`; if multer were ever switched to
 *      memory storage there is no path, so we skip (return true). Any read
 *      error → treat as invalid (return false).
 *   3. Reject implausibly small files (< MIN_IMAGE_BYTES). A genuine image
 *      can't fit a header plus pixel data in fewer bytes, and tiny files are
 *      a common decoy/edge case.
 *   4. WebP: because its signature is split across two non-adjacent slices,
 *      compare "RIFF" at bytes 0-3 AND "WEBP" at bytes 8-11 explicitly.
 *   5. Every other image type: accept the file if its leading bytes equal
 *      ANY one of that type's candidate signatures.
 *
 * @param {{ mimetype: string, path?: string, size: number }} file - multer file
 * @returns {Promise<boolean>} true when the bytes match the declared type
 *   (or the type isn't one we signature-check); false when they don't match,
 *   the file is too small, or it couldn't be read.
 */
export async function validateFileMagicBytes(file) {
  const mime = file.mimetype;
  // Not an image type we fingerprint → nothing to verify here.
  if (!(mime in IMAGE_MAGIC)) return true;

  const filePath = file.path;
  if (!filePath) return true; // memory storage — no path to read, skip

  // Read just the first 12 bytes; that's all any signature we check needs.
  let buf;
  try {
    const handle = await fs.promises.open(filePath, "r");
    const raw = Buffer.alloc(12);
    const { bytesRead } = await handle.read(raw, 0, 12, 0);
    await handle.close();
    buf = raw.subarray(0, bytesRead);
  } catch {
    // Couldn't read the file we just wrote → fail closed.
    return false;
  }

  // Reject implausibly tiny files. NOTE: `buf` holds at most 12 bytes, so the
  // first clause is effectively always true and the real gate is `file.size`.
  // The redundancy is harmless; the intent is "anything under 100 bytes is too
  // small to be a real image with a header + pixel data."
  if (buf.length < MIN_IMAGE_BYTES && file.size < MIN_IMAGE_BYTES) return false;

  // WebP signature is split: "RIFF" (0-3) … 4-byte size (4-7, ignored) … "WEBP" (8-11).
  if (mime === "image/webp") {
    return (
      buf.length >= 12 &&
      buf.subarray(0, 4).equals(Buffer.from([0x52, 0x49, 0x46, 0x46])) && // "RIFF"
      buf.subarray(8, 12).equals(Buffer.from([0x57, 0x45, 0x42, 0x50]))   // "WEBP"
    );
  }

  // All other image types: leading bytes must equal one of the candidate
  // signatures for this MIME type.
  const candidates = IMAGE_MAGIC[mime];
  return candidates.some((magic) =>
    buf.length >= magic.length &&
    buf.subarray(0, magic.length).equals(Buffer.from(magic)),
  );
}

// Ensure upload directory exists
fs.mkdirSync(config.uploads.dir, { recursive: true });

const storage = multer.diskStorage({
  destination: (_req, _file, cb) => cb(null, config.uploads.dir),
  filename: (_req, file, cb) => {
    const ext = path.extname(file.originalname).toLowerCase();
    const name = `${Date.now()}-${crypto.randomBytes(8).toString("hex")}${ext}`;
    cb(null, name);
  },
});

const fileFilter = (_req, file, cb) => {
  if (config.uploads.allowedMimeTypes.includes(file.mimetype)) {
    cb(null, true);
  } else {
    cb(
      Object.assign(new Error(`File type ${file.mimetype} not allowed`), {
        status: 400,
      }),
      false,
    );
  }
};

export const upload = multer({
  storage,
  fileFilter,
  limits: { fileSize: config.uploads.maxSizeBytes },
});

/**
 * Given a multer file object, record it in the DB and return the File record.
 */
export async function saveFileRecord(uploaderId, file) {
  const url = `/uploads/${file.filename}`;
  return File.create({
    uploaderId,
    originalName: file.originalname,
    storedName: file.filename,
    mimeType: file.mimetype,
    sizeBytes: file.size,
    url,
  });
}

/**
 * Delete a file from disk.
 */
export async function deleteFileFromDisk(storedName) {
  const filePath = path.join(config.uploads.dir, storedName);
  try {
    await fs.promises.unlink(filePath);
  } catch {
    // File may not exist — ignore
  }
}

export default { upload, saveFileRecord, deleteFileFromDisk, validateFileMagicBytes };
