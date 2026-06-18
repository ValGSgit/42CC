/**
 * Upload Routes — /api/uploads
 */
import express from 'express';
import { uploadFiles, listMyFiles, deleteFile } from '#controllers/uploadController.js';
import { authenticate } from '#middleware/auth.js';
import { upload } from '#services/uploadService.js';
import { uploadLimiter } from '#middleware/rateLimiters.js';
import { idParamValidation } from '#validators/contentValidator.js';
import { checkValidation } from '#validators/validatorUtils.js';

const router = express.Router();
router.use(authenticate);

/**
 * @openapi
 * /uploads:
 *   post:
 *     tags: [Uploads]
 *     summary: Upload one or more files (max 10 per request, 10 MB each)
 *     description: |
 *       Accepts `multipart/form-data` with a `files` field.
 *       - **Images** (JPEG, PNG, GIF, WebP, SVG) are served publicly at `/uploads/<filename>` — suitable for `<img>` tags.
 *       - **Non-images** (PDF, CSV, JSON, XML, text) are protected; only the uploader can download them.
 *     requestBody:
 *       required: true
 *       content:
 *         multipart/form-data:
 *           schema:
 *             type: object
 *             required: [files]
 *             properties:
 *               files:
 *                 type: array
 *                 items:
 *                   type: string
 *                   format: binary
 *                 maxItems: 10
 *     responses:
 *       201:
 *         description: Files uploaded
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 files: { type: array, items: { $ref: '#/components/schemas/UploadedFile' } }
 *       400: { description: No files provided or file type not allowed }
 *       413: { description: File too large }
 *   get:
 *     tags: [Uploads]
 *     summary: List files I have uploaded
 *     parameters:
 *       - $ref: '#/components/parameters/limitParam'
 *       - $ref: '#/components/parameters/offsetParam'
 *     responses:
 *       200:
 *         description: My uploaded files
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 files: { type: array, items: { $ref: '#/components/schemas/UploadedFile' } }
 */
router.post('/', uploadLimiter, upload.array('files', 10), uploadFiles);
router.get('/', listMyFiles);

/**
 * @openapi
 * /uploads/{id}:
 *   delete:
 *     tags: [Uploads]
 *     summary: Delete an uploaded file (uploader only)
 *     description: Removes the file record from the database and the file from disk.
 *     parameters:
 *       - in: path
 *         name: id
 *         required: true
 *         schema: { type: integer }
 *         description: File ID from the upload record
 *     responses:
 *       200:
 *         description: File deleted
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message: { type: string }
 *       403: { description: Not your file }
 *       404: { description: File not found }
 */
router.delete('/:id', idParamValidation(), checkValidation, deleteFile);

export default router;
