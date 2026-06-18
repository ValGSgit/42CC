import { validationResult } from "express-validator";

// Result formatter
export const customValidationResult = validationResult.withDefaults({
  formatter: (error) => error.msg,
});

/**
 * A Type Guard to check if an error is a Result from express-validator.
 * todo maybe this duck typing can be improved, but i dont know how
 */
export const isValidationError = (err) => {
  return (
    err && typeof err.array === "function" && typeof err.mapped === "function"
  );
};

/**
 * Middleware that checks express-validator results and forwards any
 * validation error to the global error handler.
 * Use between validation chains and the route handler.
 */
export const checkValidation = (req, _res, next) => {
  try {
    customValidationResult(req).throw();
    next();
  } catch (err) {
    next(err);
  }
};
