/**
 * User Routes
 */
import express from "express";
import { authenticate } from "#middleware/auth.js";
import { getFarm, saveFarm } from "#controllers/gameController.js";
import {
  getMe,
  updateMe,
  changePassword,
  getUser,
  listUsers,
  exportMyData,
  requestDeletion,
  listDataRequests,
  deleteMe,
  getApiKey,
  generateApiKey,
  revokeApiKey,
} from "#controllers/userController.js";
import {
  userPasswordValidation,
  userUpdateValidation,
} from "#validators/userValidator.js";
import { idParamValidation } from "#validators/contentValidator.js";
import { checkValidation } from "#validators/validatorUtils.js";
import { paginationValidation } from "#validators/paginationValidator.js";
import { apiKeyRegenerateLimiter } from "#middleware/rateLimiters.js";

const router = express.Router();
router.use(authenticate);

router.get("/me", getMe);
router.put("/me", userUpdateValidation(), checkValidation, updateMe);
router.delete("/me", deleteMe);
router.put(
  "/me/password",
  userPasswordValidation(),
  checkValidation,
  changePassword,
);
router.get("/me/export", exportMyData);
router.post("/me/delete-request", requestDeletion);
router.get("/me/data-requests", listDataRequests);
router.get("/me/farmdata", getFarm);
router.put("/me/farmdata", saveFarm);
router.get("/me/api-key", getApiKey);
router.post("/me/api-key", apiKeyRegenerateLimiter, generateApiKey);
router.delete("/me/api-key", revokeApiKey);
router.get("/", paginationValidation(100), checkValidation, listUsers);
router.get("/:id", idParamValidation(), checkValidation, getUser);

export default router;
