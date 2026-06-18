/**
 * ### CustomError class
 */
class CustomError extends Error {
  constructor(message, statusCode, code = undefined) {
    super(message);
    this.statusCode = statusCode;
    this.status = statusCode;
    // Optional machine-readable code (e.g. "TOKEN_EXPIRED") so clients can
    // react to specific errors without string-matching the human message.
    this.code = code;
    this.name = this.constructor.name;

    if (Error.captureStackTrace) {
      Error.captureStackTrace(this, this.constructor);
    }
  }
}

export default CustomError;
