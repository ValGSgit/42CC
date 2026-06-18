/**
 * Error-handling Middleware
 */

import { Prisma } from "@prisma/client";
import CustomError from "#utils/CustomError.js";
import { isValidationError } from "#validators/validatorUtils.js";
import config from "#config/index.js";

export const notFoundHandler = (_req, _res, next) => {
  const err = new Error("Not Found");
  err.status = 404;
  next(err);
};

/**
  ### Error handler
  Prisma error codes:
    - https://www.prisma.io/docs/orm/reference/error-reference
*/
// eslint-disable-next-line no-unused-vars
export const errorHandler = (err, _req, res, _next) => {
  let prismaCode = -1; // -1 = no prisma error
  let error;

  if (err instanceof CustomError) {
    error = err;
  } else if (isValidationError(err)) {
    if (err.isEmpty()) {
      // A Result with no errors should never reach us; treat it as a bug
      // rather than silently leaking a 500 with an empty body.
      error = new CustomError("Internal Server Error", 500);
    } else {
      const mapped = err.mapped();
      const firstError = Object.values(mapped)[0];
      return res.status(400).json({
        error: { message: firstError || "Validation failed", fields: mapped },
        errors: mapped,
      });
    }
  } else if (err instanceof Prisma.PrismaClientKnownRequestError) {
    prismaCode = err.code;
    error = handlePrismaError(err);
  } else {
    // Preserve any explicit message when present. The tests exercise custom
    // 4xx/5xx errors, plus plain objects that only expose `status`.
    const status = err.status || err.statusCode || 500;
    const safeMessage = err.message && String(err.message).trim()
      ? err.message
      : "Internal Server Error";
    error = new CustomError(safeMessage, status);
  }

  if (config.envIsDev)
    console.log(`errorHandler: ${error.statusCode}: ${error.message}`);

  res.status(error.statusCode).json({
    error: {
      message: error.message,
      // Surface a machine-readable code when the error carries one so the
      // client can react to specific cases (e.g. TOKEN_EXPIRED → refresh flow).
      ...(error.code && { code: error.code }),
      ...(process.env.NODE_ENV === "development" && {
        stack: err.stack,
        prismaCode,
      }),
    },
  });
};

const handlePrismaError = (err) => {
  if (config.envIsDev) {
    console.log(`#### PRISMA_CODE: ${err.code}`);
    console.log(`#### PRISMA_META: ${JSON.stringify(err.meta, null, 2)}`);
  }

  const prefix = "Prisma error:";

  switch (err.code) {
    case "P2000": {
      const message = `${prefix} The value is too long`;
      return new CustomError(message, 400);
    }
    case "P2002": {
      const message = `${prefix} Unique constraint failed`;
      return new CustomError(message, 409);
    }
    case "P2025": {
      const message = `${prefix} The requested record was not found`;
      return new CustomError(message, 404);
    }
    default: {
      // Don't leak schema/column names from err.message to the client.
      const message = config.envIsDev
        ? `${prefix} Unknown prisma error ${err.message}`
        : "Internal Server Error";
      return new CustomError(message, 500);
    }
  }
};
