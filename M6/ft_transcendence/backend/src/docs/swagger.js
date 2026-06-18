/**
 * OpenAPI / Swagger configuration (full internal API)
 * UI available at /api/docs/dev — dev only (gated by NODE_ENV in index.js).
 * The publicly-mounted, externally-safe spec lives at /api/docs/public.
 */
import swaggerJsdoc from "swagger-jsdoc";

const options = {
  definition: {
    openapi: "3.0.0",
    info: {
      title: "AlpacaParty API",
      version: "0.1.0",
      description: `
REST API for the **AlpacaParty** multiplayer social platform.

## Quick start
1. Call **POST /auth/register** or **POST /auth/login**
2. Copy the \`accessToken\` from the response
3. Click **Authorize 🔒** at the top of this page and paste it
4. All protected endpoints will now include your token automatically

## Authentication
Most endpoints require a JWT access token:
\`\`\`
Authorization: Cookie jwt_token=your_access_token_here
\`\`\`
Access tokens expire after **24 h**. Use **POST /auth/refresh** with your \`refreshToken\` to get a new pair.

## Public API
The \`/public/*\` group uses an **X-API-Key** header instead of JWT:
\`\`\`
X-API-Key: ap_your_key_here
\`\`\`

**Generating a key:** log in, go to **Profile → Settings → Public API Key**, and click **Generate Key**. You can revoke and regenerate at any time from the same page.

### Public endpoints
| Method | Path | Description |
|---|---|---|
| GET | /public/users | List public user profiles |
| GET | /public/users/:id | Single public profile |
| GET | /public/leaderboard | Game leaderboard (level-ranked) |
| GET | /public/posts | Public feed posts |
| POST | /public/posts | Create a post (service-level) |
| PUT | /public/posts/:id | Update a post (service-level) |
| DELETE | /public/posts/:id | Delete a post (service-level) |
| GET | /public/mock | Anonymized mock dataset |

## Rate limits
| Scope | Limit |
|---|---|
| Global | 1 000 req / 15 min |
| Auth endpoints | 50 req / 15 min per IP |
| AI help chat | 20 req / min |
| Public API | 100 req / min |
      `.trim(),
    },
    servers: [{ url: "/api", description: "AlpacaParty backend" }],
    tags: [
      {
        name: "Auth",
        description: "Register, login, logout, token refresh",
      },
      {
        name: "Users",
        description: "Profile management, password, GDPR export/deletion",
      },
      {
        name: "Friends",
        description: "Friend requests, online list, blocking",
      },
      {
        name: "Posts",
        description: "Social feed — create, like, comment",
      },
      { name: "Chat", description: "Direct messages and group chat rooms" },
      {
        name: "Game",
        description:
          "Stats, match history, leaderboard, alpaca farm, achievements",
      },
      { name: "Notifications", description: "In-app notification inbox" },
      {
        name: "Uploads",
        description: "File upload and management (images, PDFs, CSV…)",
      },
      {
        name: "Public API",
        description:
          "🔑 API-key auth — public data and service-level writes for external integrations. Generate a key at Profile → Settings → Public API Key.",
      },
    ],
    components: {
      securitySchemes: {
        ApiKeyAuth: {
          type: "apiKey",
          in: "header",
          name: "X-API-Key",
          description:
            "API key for **/public/*** endpoints. Generate yours at **Profile → Settings → Public API Key**. Format: `ap_<uuid-no-dashes>`",
        },
      },
      parameters: {
        limitParam: {
          in: "query",
          name: "limit",
          schema: { type: "integer", default: 20, minimum: 1, maximum: 100 },
          description: "Max results to return",
        },
        offsetParam: {
          in: "query",
          name: "offset",
          schema: { type: "integer", default: 0, minimum: 0 },
          description: "Results to skip (for pagination)",
        },
        anonymizedParam: {
          in: "query",
          name: "anonymized",
          schema: {
            type: "string",
            enum: ["true", "false", "1", "0", "yes", "no"],
            default: "false",
          },
          description:
            "Replace real usernames/avatars/content with placeholder values",
        },
      },
      schemas: {
        // ─── Users ─────────────────────────────────────────────────
        User: {
          type: "object",
          properties: {
            id: { type: "integer", example: 42 },
            username: { type: "string", example: "alpaca42" },
            email: {
              type: "string",
              format: "email",
              example: "alpaca@example.com",
            },
            avatar: {
              type: "string",
              nullable: true,
              example: "/uploads/avatar-42.png",
            },
            bio: { type: "string", nullable: true, example: "I love alpacas!" },
            status: {
              type: "string",
              nullable: true,
              example: "Playing Spit Royale",
            },
            is_public: { type: "boolean", example: true },
            is_online: { type: "boolean", example: true },
            api_key: {
              type: "string",
              nullable: true,
              example: "ap_a1b2c3...",
            },
            coins: { type: "integer", example: 250 },
            created_at: { type: "string", format: "date-time" },
          },
        },
        // ─── Posts / Social ────────────────────────────────────────
        Post: {
          type: "object",
          properties: {
            id: { type: "integer", example: 1 },
            content: {
              type: "string",
              example: "Just won my first Spit Royale game! 🏆",
            },
            image_url: { type: "string", nullable: true },
            is_public: { type: "boolean", example: true },
            likes_count: { type: "integer", example: 5 },
            comments_count: { type: "integer", example: 2 },
            author_id: { type: "integer", example: 42 },
            author_username: { type: "string", example: "alpaca42" },
            author_avatar: { type: "string", nullable: true },
            user_liked: { type: "boolean", example: false },
            created_at: { type: "string", format: "date-time" },
          },
        },
        Comment: {
          type: "object",
          properties: {
            id: { type: "integer", example: 10 },
            post_id: { type: "integer", example: 1 },
            author_id: { type: "integer", example: 42 },
            author_username: { type: "string", example: "alpaca42" },
            author_avatar: { type: "string", nullable: true },
            content: { type: "string", example: "Great game!" },
            created_at: { type: "string", format: "date-time" },
          },
        },
        // ─── Friends ───────────────────────────────────────────────
        FriendRequest: {
          type: "object",
          properties: {
            id: { type: "integer", example: 7 },
            senderId: { type: "integer", example: 42 },
            receiverId: { type: "integer", example: 99 },
            status: {
              type: "string",
              enum: ["pending", "accepted", "declined"],
              example: "pending",
            },
            createdAt: { type: "string", format: "date-time" },
          },
        },
        // ─── Game ──────────────────────────────────────────────────
        GameStats: {
          type: "object",
          properties: {
            userId: { type: "integer", example: 42 },
            gameType: { type: "string", example: "spit_royale" },
            wins: { type: "integer", example: 10 },
            losses: { type: "integer", example: 5 },
            draws: { type: "integer", example: 2 },
            level: { type: "integer", example: 12 },
          },
        },
        Achievement: {
          type: "object",
          properties: {
            key: { type: "string", example: "first_win" },
            name: { type: "string", example: "First Blood" },
            description: { type: "string", example: "Win your first game" },
            icon: { type: "string", example: "🏆" },
            unlocked: { type: "boolean", example: false },
          },
        },
        // ─── Chat ──────────────────────────────────────────────────
        Message: {
          type: "object",
          properties: {
            id: { type: "integer", example: 55 },
            sender_id: { type: "integer", example: 42 },
            receiver_id: { type: "integer", example: 99 },
            content: { type: "string", example: "GG!" },
            is_read: { type: "boolean", example: false },
            created_at: { type: "string", format: "date-time" },
          },
        },
        // ─── Notifications ─────────────────────────────────────────
        Notification: {
          type: "object",
          properties: {
            id: { type: "integer", example: 12 },
            type: {
              type: "string",
              enum: [
                "friend_request",
                "friend_accepted",
                "game_invite",
                "achievement",
                "post_like",
                "message",
                "data_request",
              ],
            },
            title: { type: "string", example: "New friend request" },
            message: {
              type: "string",
              example: "alpaca42 sent you a friend request",
            },
            isRead: { type: "boolean", example: false },
            referenceType: {
              type: "string",
              nullable: true,
              example: "friend_request",
            },
            referenceId: { type: "integer", nullable: true, example: 7 },
            created_at: { type: "string", format: "date-time" },
          },
        },
        // ─── Uploads ───────────────────────────────────────────────
        UploadedFile: {
          type: "object",
          properties: {
            id: { type: "integer", example: 9 },
            url: { type: "string", example: "/uploads/1712345678-abc123.png" },
            originalName: { type: "string", example: "screenshot.png" },
            mimeType: { type: "string", example: "image/png" },
            sizeBytes: { type: "integer", example: 204800 },
            created_at: { type: "string", format: "date-time" },
          },
        },
        // ─── Errors ────────────────────────────────────────────────
        Error: {
          type: "object",
          properties: {
            error: {
              type: "object",
              properties: {
                message: { type: "string", example: "Something went wrong" },
              },
            },
          },
        },
        ValidationError: {
          type: "object",
          properties: {
            error: {
              type: "object",
              properties: {
                message: { type: "string", example: "Validation failed" },
                fields: {
                  type: "object",
                  additionalProperties: { type: "string" },
                  example: { userId: "userId must be a positive integer" },
                },
              },
            },
          },
        },
      },
    },
  },
  apis: ["./src/routes/*.js"],
};

const swaggerSpec = swaggerJsdoc(options);
export default swaggerSpec;
