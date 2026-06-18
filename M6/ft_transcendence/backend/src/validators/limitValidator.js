import { query } from "express-validator";

export const limitValidation = (max) => [limitChain(query("limit"), max)];

const limitChain = (chain, max) =>
  chain
    .optional()
    .trim()
    .notEmpty()
    .isInt({ min: 1, max: max })
    .withMessage(`limit must be between 1 and ${max}`);
