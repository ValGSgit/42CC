import { body, param } from "express-validator";

// Content validators for user-generated strings.
//
// Important: we never call .escape() here. The frontend renders these
// values through Vue's reactive templating (and explicit sanitization
// where raw HTML is rendered), so server-side HTML-entity escaping would
// double-encode and corrupt the displayed text. The job of these chains
// is length/shape validation only — XSS prevention is enforced at render.

const contentChain = (chain, { min = 1, max = 5000, field = "content" } = {}) =>
  chain
    .isString()
    .withMessage(`${field} must be a string`)
    .bail()
    .trim()
    .notEmpty()
    .withMessage(`${field} is required`)
    .bail()
    .isLength({ min, max })
    .withMessage(`${field} must be between ${min} and ${max} characters`);

const optionalContentChain = (chain, opts = {}) =>
  contentChain(chain.optional({ values: "falsy" }), opts);

const urlChain = (chain, field = "imageUrl") =>
  chain
    .optional({ values: "null" })
    .isString()
    .withMessage(`${field} must be a string`)
    .isLength({ max: 2048 })
    .withMessage(`${field} must be 2048 characters or fewer`);

// ── Posts ─────────────────────────────────────────────────────────────────

export const postCreateValidation = () => [
  contentChain(body("content"), { max: 5000, field: "content" }),
  urlChain(body("imageUrl")),
  urlChain(body("image_url")),
  body("isPublic")
    .optional()
    .isBoolean()
    .withMessage("isPublic must be a boolean"),
];

export const postUpdateValidation = () => [
  optionalContentChain(body("content"), { max: 5000, field: "content" }),
  urlChain(body("imageUrl")),
  urlChain(body("image_url")),
  body("isPublic")
    .optional()
    .isBoolean()
    .withMessage("isPublic must be a boolean"),
];

// ── Comments ──────────────────────────────────────────────────────────────

export const commentCreateValidation = () => [
  contentChain(body("content"), { max: 1000, field: "content" }),
];

// ── Chat messages & rooms ─────────────────────────────────────────────────

export const chatMessageValidation = () => [
  contentChain(body("content"), { max: 2000, field: "content" }),
];

// ── Common path params ────────────────────────────────────────────────────

export const idParamValidation = (name = "id") => [
  param(name)
    .isInt({ min: 1 })
    .withMessage(`${name} must be a positive integer`),
];
