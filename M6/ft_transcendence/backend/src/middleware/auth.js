/**
 * Auth Middleware — JWT verification
 */
import AuthService from "../services/authService.js";
import User from "../models/User.js";
import CustomError from "#utils/CustomError.js";

/**
 * Require a valid access token.
 * Attaches `req.user` (full user row, no password_hash) on success.
 */
export const authenticate = async (req, res, next) => {
  try {
    let token = req.cookies.jwt_token;
    if (!token) throw new CustomError("No token provided", 401);
    const decoded = AuthService.verifyToken(token);
    if (!decoded || decoded.type === "refresh") throw new CustomError("Invalid or expired token", 401);

    const user = await User.findById(decoded.id);
    if (!user) throw new CustomError("User not found", 401);
    req.user = user;
    next();
  } catch (err) {
    next(err);
  }
};

/**
 * Optional auth — attaches req.user if a valid token is present, but doesn't
 * reject the request if there's no token.
 */
export const optionalAuth = async (req, _res, next) => {
  try {
    let token = req.cookies.jwt_token;
    if (token) {
      const decoded = AuthService.verifyToken(token);
      if (decoded) {
        req.user = await User.findById(decoded.id);
      }
    }
    next();
  } catch {
    next();
  }
};
