/**
 * Processes raw query parameters into Prisma pagination arguments
 * @param {Object} query - The req.query object
 * @returns {Object} { skip: number, take: number }
 */
export const getPagination = (query) => {
  const page = parseInt(query.page) || 1;
  const pageSize = parseInt(query.pageSize) || 10;

  const pageNumber = Math.max(1, page);
  const pageSizeNumber = Math.max(1, pageSize);

  return {
    pageNumber: pageNumber,
    pageSizeNumber: pageSizeNumber,
    skip: (pageNumber - 1) * pageSizeNumber,
    take: pageSizeNumber,
  };
};

/**
 * Parse and clamp limit/offset query params.
 *
 * Controllers historically destructured `{ limit = 50, offset = 0 } = req.query`
 * and passed the raw strings to Prisma. That accepts unbounded client input and
 * relies on Prisma's implicit string→int coercion. This helper makes the
 * boundary explicit:
 *   const { limit, offset } = parseLimitOffset(req.query, { defaultLimit: 50, maxLimit: 100 });
 *
 * @returns {{ limit: number, offset: number }}
 */
export const parseLimitOffset = (query, opts = {}) => {
  const defaultLimit = opts.defaultLimit ?? 20;
  const maxLimit = opts.maxLimit ?? 100;

  let limit = Number(query.limit);
  if (!Number.isFinite(limit)) limit = defaultLimit;
  if (limit < 1) limit = 1;
  if (limit > maxLimit) limit = maxLimit;

  let offset = Number(query.offset);
  if (!Number.isFinite(offset) || offset < 0) offset = 0;

  return { limit: Math.trunc(limit), offset: Math.trunc(offset) };
};

/**
 * Parse a route param expected to be a positive integer (e.g. `/posts/:id`).
 * Returns the integer on success, or `null` on bad input. Callers should
 * respond with 400 on null rather than passing NaN to Prisma — Prisma
 * silently drops NaN filters and returns unintended results.
 *
 * @param {string} value - raw req.params.X
 * @returns {number | null}
 */
export const parseIdParam = (value) => {
  if (value === undefined || value === null || value === '') return null;
  const n = Number(value);
  if (!Number.isInteger(n) || n <= 0) return null;
  return n;
};

/**
 * Clamp a client-supplied integer to a safe range. Used for game-stat
 * counters and similar inputs the client owns but the server must bound.
 *
 * @param {*} value - raw value from req.body
 * @param {number} min
 * @param {number} max
 * @returns {number}
 */
export const clampInt = (value, min, max) => {
  const n = Number(value);
  if (!Number.isFinite(n)) return min;
  if (n < min) return min;
  if (n > max) return max;
  return Math.trunc(n);
};
