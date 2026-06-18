import { body } from "express-validator";
import {
  userEmailChain,
  userNameChain,
  userPasswordChain,
} from "./userValidator.js";

// todo match lengths with database schema

export const authRegisterValidation = () => [
  userNameChain(body("username")),
  userEmailChain(body("email")),
  userPasswordChain(body("password")),
];

// Login accepts either a username or an email in the "username" field.
// Apply the same email-lowercasing as the register chain (see userValidator.js
// `userEmailChain`) so a user who signed up as "Alice@Example.com" can still
// log in typing the same string — otherwise the lookup would miss the
// lowercase row in the DB.
const authUsernameOrEmailChain = (chain) =>
  chain
    .trim()
    .notEmpty()
    .withMessage("Username or email is required")
    .isLength({ min: 3, max: 254 })
    .withMessage("Username or email must be 3-254 characters")
    .customSanitizer((value) => {
      // Only touch values that look like an email — usernames are stored
      // case-sensitively and "User" must not collapse to "user".
      if (typeof value !== "string" || !value.includes("@")) return value;
      return value.toLowerCase();
    });

export const authLoginValidation = () => [
  authUsernameOrEmailChain(body("username")),
  userPasswordChain(body("password")),
];
