/**
 * Public API Key Middleware
 *
 * Header:  X-API-Key: <key>
 */
import CustomError from "#utils/CustomError.js";
import User from "../models/User.js";
import AuthService from "../services/authService.js";

export const requireApiKey = async (req, res, next) => {
  try {
    const apiKey = req.headers["x-api-key"];
    if (!apiKey) throw new CustomError("No api key provided", 401);
    const decoded = AuthService.verifyPublicApiToken(apiKey);
    if (!decoded) throw new CustomError("Invalid or expired api key", 401);
    const userId = await User.findByApiKey(apiKey);
    if (!userId) throw new CustomError("Invalid or revoked api key", 401);
    req.userId = userId;
    next();
  } catch (err) {
    next(err);
  }
};
