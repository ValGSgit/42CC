/**
 * Game Routes — /api/game
 */
import express from 'express';
import {
  getStats, getHistory, getLeaderboard, getCoinsLeaderboard, saveGameResult,
  getFarm, saveFarm, getAchievements, getChallenges,
} from '../controllers/gameController.js';
import { authenticate } from '../middleware/auth.js';

const router = express.Router();
router.use(authenticate);

/**
 * @openapi
 * /game/stats:
 *   get:
 *     tags: [Game]
 *     summary: Get my game statistics
 *     parameters:
 *       - in: query
 *         name: gameType
 *         schema: { type: string, example: spit_royale }
 *         description: Filter stats by game type (omit for all types)
 *     responses:
 *       200:
 *         description: Game stats
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 stats: { type: array, items: { $ref: '#/components/schemas/GameStats' } }
 */
router.get('/stats', getStats);

/**
 * @openapi
 * /game/history:
 *   get:
 *     tags: [Game]
 *     summary: Get my match history
 *     parameters:
 *       - $ref: '#/components/parameters/limitParam'
 *       - $ref: '#/components/parameters/offsetParam'
 *     responses:
 *       200:
 *         description: Match history
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 history:
 *                   type: array
 *                   items:
 *                     type: object
 *                     properties:
 *                       id: { type: integer }
 *                       gameType: { type: string, example: spit_royale }
 *                       result: { type: string, enum: [win, loss, draw] }
 *                       opponentUsername: { type: string }
 *                       levelDelta: { type: integer, example: 1 }
 *                       playedAt: { type: string, format: date-time }
 */
router.get('/history', getHistory);

/**
 * @openapi
 * /game/leaderboard:
 *   get:
 *     tags: [Game]
 *     summary: Get a leaderboard table
 *     parameters:
 *       - in: query
 *         name: board
 *         schema: { type: string, enum: [kills, obstacles, coins], default: kills }
 *         description: Which metric to rank by
 *       - $ref: '#/components/parameters/limitParam'
 *       - $ref: '#/components/parameters/offsetParam'
 *     responses:
 *       200:
 *         description: Leaderboard rows ranked by the requested metric
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 board: { type: string, enum: [kills, obstacles, coins] }
 *                 leaderboard:
 *                   type: array
 *                   items:
 *                     type: object
 *                     properties:
 *                       userId: { type: integer }
 *                       username: { type: string }
 *                       avatar: { type: string, nullable: true }
 *                       level: { type: integer, example: 12 }
 *                       value: { type: integer, description: "The metric value (kills / obstacles / coins)" }
 */
router.get('/leaderboard/coins', getCoinsLeaderboard);
router.get('/leaderboard', getLeaderboard);

/**
 * @openapi
 * /game/result:
 *   post:
 *     tags: [Game]
 *     summary: Save offline/AI game result
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             required: [gameType, result]
 *             properties:
 *               gameType: { type: string, example: spit_royale }
 *               result: { type: string, enum: [win, loss, draw], example: win }
 *     responses:
 *       200:
 *         description: Updated game stats
 */
router.post('/result', saveGameResult);

/**
 * @openapi
 * /game/farm:
 *   get:
 *     tags: [Game]
 *     summary: Get my alpaca farm state
 *     responses:
 *       200:
 *         description: Farm data (alpacas, items, upgrades)
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 farm:
 *                   type: object
 *                   properties:
 *                     alpacas: { type: array }
 *                     items: { type: array }
 *                     upgrades: { type: array }
 *   put:
 *     tags: [Game]
 *     summary: Save my alpaca farm state
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             properties:
 *               alpacas: { type: array, description: "Array of alpaca objects to persist" }
 *               items: { type: array }
 *               upgrades: { type: array }
 *     responses:
 *       200:
 *         description: Farm saved
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message: { type: string }
 */
router.get('/farm', getFarm);
router.put('/farm', saveFarm);

/**
 * @openapi
 * /game/achievements:
 *   get:
 *     tags: [Game]
 *     summary: Get my achievements (unlocked and locked)
 *     responses:
 *       200:
 *         description: All achievements with unlock status
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 achievements: { type: array, items: { $ref: '#/components/schemas/Achievement' } }
 */
router.get('/achievements', getAchievements);

/**
 * @openapi
 * /game/challenges:
 *   get:
 *     tags: [Game]
 *     summary: Get active daily/weekly challenges
 *     responses:
 *       200:
 *         description: Current challenges and progress
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 challenges:
 *                   type: array
 *                   items:
 *                     type: object
 *                     properties:
 *                       id: { type: integer }
 *                       title: { type: string, example: "Win 3 games today" }
 *                       description: { type: string }
 *                       progress: { type: integer, example: 1 }
 *                       goal: { type: integer, example: 3 }
 *                       rewardCoins: { type: integer, example: 50 }
 *                       expiresAt: { type: string, format: date-time }
 */
router.get('/challenges', getChallenges);

export default router;
