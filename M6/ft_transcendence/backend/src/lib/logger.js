/**
 * Backend logger.
 *
 * `debug()` / `devWarn()` only forward to console in development; in
 * production they're no-ops so stdout stays clean.
 *
 * `error()` always forwards — production still needs to see real failures.
 *
 * Usage:
 *   import { debug, devWarn, error } from '#lib/logger.js'
 *   debug('Match created:', matchId)
 *   error('failed to persist outcome:', err.message)
 */
import config from '#config/index.js';

const noop = () => {};

export const debug = config.envIsDev ? console.log.bind(console) : noop;
export const devWarn = config.envIsDev ? console.warn.bind(console) : noop;
export const error = console.error.bind(console);
