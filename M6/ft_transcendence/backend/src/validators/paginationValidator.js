import { query } from "express-validator";

export const paginationValidation = (max = 100) => [
  limitChain(query("limit"), max),
  offsetChain(query("offset")),
  filterChain(query("filter")),
  sortChain(query("sort")),
];

const limitChain = (chain, max) =>
  chain
    .optional()
    .trim()
    .notEmpty()
    .isInt({ min: 1, max: max })
    .withMessage(`limit must be between 1 and ${max}`)
    .toInt();

const offsetChain = (chain) =>
  chain
    .optional()
    .trim()
    .notEmpty()
    .isInt({ min: 0 })
    .withMessage("offset must be an integer greater than or equal to 0")
    .toInt();

// ...&filter[id]=a_value,another_value
const filterChain = (chain) =>
  chain.optional().isObject().withMessage("filter must be an object");

const sortChain = (chain) =>
  chain.optional().isObject().withMessage("sort must be an object");
