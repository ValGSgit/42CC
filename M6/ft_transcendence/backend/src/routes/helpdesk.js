import express from "express";
import { body } from "express-validator";
import { checkValidation } from "../validators/validatorUtils.js";
import { helpdeskLimiter } from "../middleware/rateLimiters.js";
import { authenticate } from "../middleware/auth.js";
import config from "#config/index.js";

const router = express.Router();

// Tight enough to keep replies short and finish-able on a single Groq call,
// loose enough for normal Q&A. Mirrored on the frontend (HelpDeskChat.vue).
const MAX_HISTORY = 10; // messages of context
const MAX_CHARS = 1000; // per user message
const MAX_TOKENS = 350; // ~250 words — enough for a complete answer,
// small enough that the model never runs out.

let keyIndex = 0;
function nextApiKey() {
  const keys = config.groq.apiKeys;
  if (!keys.length) return null;
  const key = keys[keyIndex % keys.length];
  keyIndex = (keyIndex + 1) % keys.length;
  return key;
}

const SYSTEM_PROMPT = `You are Paca, the friendly help desk assistant for AlpacaParty — a social gaming platform built around alpacas.

## About AlpacaParty
AlpacaParty is a web-based platform where users can:
- Play AlpacaFarm, a multiplayer top-down farming/party game where you raise and customize alpacas
- Build a social profile and share posts on the Feed
- Add friends and manage friend requests
- Send direct messages to other players
- Earn coins and achievements through gameplay
- Compete on leaderboards
- The project repository is at https://github.com/ValGSgit/AlpacaParty-ft_transcendence

## The Game — AlpacaFarm
- Players control alpacas on a farm map
- Collect coins by farming and completing tasks
- Buy items and new alpacas from the in-game shop
- Customize alpaca names, colors, and accessories
- Multiplayer minigames: SpitRoyale (battle royale-style), and road races
- The game runs directly in the browser — no download needed

## Getting Started
1. Register at /register or log in at /login
2. After logging in, you land on the AlpacaFarm game automatically
3. Navigate using the top navbar: AlpacaFarm, Feed, Friends, Profile

## Key Features
- **Profile**: Customize your avatar, view your stats and achievements, see your post history
- **Feed**: Share posts, like and comment on content from other players, see trending alpaca content
- **Friends**: Send/accept friend requests, see who's online, view friend profiles
- **Messages**: Real-time direct messaging with friends (click the chat bubble icon in the bottom-left)
- **Notifications**: Bell icon in the navbar — friend requests, game invites, post likes, achievements

## Alpaca Facts (always happy to share!)
- Alpacas are South American camelids, related to llamas, camels, and vicuñas
- They are native to the Andes mountains of Peru, Bolivia, and Chile
- Alpacas come in 22 natural colors — more than any other fiber-producing animal
- Their fiber (called alpaca wool or fleece) is hypoallergenic, warmer than sheep wool, and very soft
- Alpacas communicate by humming, and they spit when annoyed or establishing dominance
- A group of alpacas is called a herd; a baby alpaca is called a cria
- Alpacas live 15–20 years
- There are two breeds: Huacaya (fluffy, teddy-bear look) and Suri (silky, dreadlock-like fleece)
- Alpacas have three-chambered stomachs and are very efficient grazers
- They are gentle, curious, and highly social animals

## Support Guidelines
- Be warm, helpful, and a little playful — this is a fun gaming platform.
- **Keep every reply to at most 4–5 short sentences (~250 words). Finish your thought before stopping — never trail off mid-sentence.**
- If a user reports a bug, suggest the in-game feedback or GitHub issues.
- For account issues (can't log in, forgot password), point them to /login and /register.
- Only answer questions about AlpacaParty or alpacas — politely redirect anything else.
- When appropriate, sign off short answers with a friendly alpaca-themed closing (e.g. "Happy farming! 🦙").`;

router.post(
  "/chat",
  authenticate,
  helpdeskLimiter,
  express.json({ limit: "16kb" }),
  [
    body("messages")
      .isArray({ min: 1, max: MAX_HISTORY })
      .withMessage(`messages must be an array of 1–${MAX_HISTORY} items`),
    body("messages.*.role")
      .isIn(["user", "assistant"])
      .withMessage('Each message role must be "user" or "assistant"'),
    body("messages.*.content")
      .isString()
      .trim()
      .isLength({ min: 1, max: MAX_CHARS })
      .withMessage(`Each message content must be 1–${MAX_CHARS} characters`),
  ],
  checkValidation,
  async (req, res) => {
    const apiKey = nextApiKey();
    if (!apiKey) {
      return res
        .status(503)
        .json({ error: { message: "Help desk is not configured." } });
    }

    const abortCtrl = new AbortController();
    req.on("close", () => abortCtrl.abort());

    let upstream;
    try {
      upstream = await fetch(
        "https://api.groq.com/openai/v1/chat/completions",
        {
          method: "POST",
          headers: {
            "Content-Type": "application/json",
            Authorization: `Bearer ${apiKey}`,
          },
          body: JSON.stringify({
            model: config.groq.model,
            messages: [
              { role: "system", content: SYSTEM_PROMPT },
              ...req.body.messages,
            ],
            max_tokens: MAX_TOKENS,
            temperature: 0.7,
            stream: false,
          }),
          signal: abortCtrl.signal,
        },
      );
    } catch (err) {
      if (err.name === "AbortError") return;
      console.error("[helpdesk] fetch error:", err.message);
      return res
        .status(502)
        .json({ error: { message: "Could not reach AI service." } });
    }

    if (!upstream.ok) {
      const errBody = await upstream.text().catch(() => "");
      console.error("[helpdesk] Groq error:", errBody);
      return res
        .status(502)
        .json({ error: { message: "AI service unavailable." } });
    }

    const data = await upstream.json().catch(() => null);
    const content = data?.choices?.[0]?.message?.content?.trim();
    if (!content) {
      return res
        .status(502)
        .json({ error: { message: "Empty response from the assistant." } });
    }
    res.json({ content });
  },
);

export default router;
