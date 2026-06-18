/**
 * Dev-only logger.
 *
 * In development (`vite dev`) these forward to the real console methods so
 * developers see what's happening. In production builds (`vite build`)
 * they become no-ops, so the browser console stays clean — which the
 * project subject explicitly requires.
 *
 * Usage:
 *   import { debug, devError } from '@/services/logger'
 *   debug('FarmData:', farmData)
 *   try { … } catch (e) { devError(e) }
 */
const noop = () => {};
const isDev = !!import.meta?.env?.DEV;

export const debug = isDev ? console.log.bind(console) : noop;
export const devWarn = isDev ? console.warn.bind(console) : noop;
export const devError = isDev ? console.error.bind(console) : noop;
