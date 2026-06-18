/**
 * Shared helpers for reading auth fields off a user record.
 *
 * The schema has the password hash on the joined `userAuth` row, but some
 * legacy callers receive the hash directly on the user object. Both
 * authController and userController need to read this consistently — keep
 * the lookup in one place so future schema moves (e.g. dropping the legacy
 * field) only touch this file.
 */

/**
 * Returns the bcrypt hash for a user record, or null if neither field is set.
 * Callers should treat a null return as "no password configured for this
 * account" and respond with 401 — never pass null/undefined into
 * bcrypt.compare (it throws and surfaces as a 500).
 *
 * @param {object|null|undefined} user
 * @returns {string|null}
 */
export function extractPasswordHash(user) {
  if (!user) return null;
  if (user.userAuth && user.userAuth.passwordHash) return user.userAuth.passwordHash;
  if (user.passwordHash) return user.passwordHash;
  return null;
}
