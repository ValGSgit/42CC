import { body } from "express-validator";
import config from "#config/index.js";

export const userUpdateValidation = () => [
  userNameChain(body("username").optional()),
  userEmailChain(body("email").optional()),
  userBioChain(body("bio")),
  userStatusChain(body("status")),
  userAvatarChain(body("avatar")),
];

export const userPasswordValidation = () => [
  userPasswordChain(body("newPassword")),
];

export const userNameChain = (chain) =>
  chain
    .trim()
    .notEmpty()
    .withMessage("Name is required")
    .matches(/^[a-zA-Z0-9_-]+$/)
    .withMessage(
      "Username may only contain letters, numbers, hyphens and underscores",
    )
    .isLength({ min: 3, max: 32 })
    .withMessage("Username must be 3-32 characters");
// NOTE: no .escape() — characters are whitelisted above. Store raw and let
// the renderer escape (Vue does this for {{ }}); double-encoding breaks
// display of ampersands etc.

export const userEmailChain = (chain) =>
  chain
    .trim()
    .isLength({ max: 254 })
    .withMessage("Email must be 254 characters or fewer")
    .isEmail()
    .withMessage("Please provide a valid email")
    // Plain lowercase instead of express-validator's `.normalizeEmail()` —
    // the latter also strips gmail dots ("a.b@gmail.com" → "ab@gmail.com")
    // and the login chain previously did NOT normalize, so users could
    // register and then never log in. Both chains share this sanitizer now.
    .customSanitizer((value) =>
      typeof value === "string" ? value.toLowerCase() : value,
    );

export const userBioChain = (chain) =>
  chain
    .optional()
    .trim()
    .isLength({ max: 500 })
    .withMessage("Bio must be 500 characters or fewer");

export const userStatusChain = (chain) =>
  chain
    .optional()
    .trim()
    .isLength({ max: 200 })
    .withMessage("Status must be 200 characters or fewer");

export const userAvatarChain = (chain) =>
  chain
    .optional()
    .trim()
    .isLength({ max: 512 })
    .withMessage("Avatar must be under 512 characters");

export const userPasswordChain = (chain) => {
  const minLen = config.password.minLength;
  const maxLen = 50;
  const requireUppercase = config.password.requireUppercase;
  const requireLowercase = config.password.requireLowercase;
  const requireNumber = config.password.requireNumber;
  return chain
    .isLength({ min: minLen, max: maxLen })
    .withMessage(`Password must be between ${minLen} and ${maxLen} characters`)
    .custom((value) => !requireUppercase || /[A-Z]/.test(value))
    .withMessage("Password must contain at least one uppercase letter")
    .custom((value) => !requireLowercase || /[a-z]/.test(value))
    .withMessage("Password must contain at least one lowercase letter")
    .custom((value) => !requireNumber || /\d/.test(value))
    .withMessage("Password must contain at least one number");
};
