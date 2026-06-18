import { body } from "express-validator";
import { idParamValidation } from "./contentValidator.js";

const sharedPostValidations = [
  body("content")
    .trim()
    .notEmpty()
    .withMessage("content is required")
    .isLength({ max: 2000 })
    .withMessage("content must be 2000 characters or fewer"),

  body("imageUrl")
    .optional({ values: "falsy" })
    .isString()
    .withMessage("imageUrl must be a string")
    .trim()
    .isURL()
    .withMessage("imageUrl must be a valid URL format")
    .isLength({ max: 2048 })
    .withMessage("imageUrl must be 2048 characters or fewer"),

  body("isPublic")
    .optional({ values: "falsy" })
    .isBoolean()
    .withMessage("isPublic must be boolean"),
];

export const postCreateValidation = () => [...sharedPostValidations];

export const postUpdateValidation = () => [
  idParamValidation(),
  ...sharedPostValidations,
];
