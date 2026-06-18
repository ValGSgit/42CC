_This project has been created as part of the 42 curriculum by ValGSgit, DavidPoetsch, fankahou, LukasStefanek._

# AlpacaParty

> A social gaming web application where users raise virtual alpacas in a 3D farm, play two real-time multiplayer mini-games, chat with friends, and earn achievements — built with Vue, Three.js, Express.js, PostgreSQL, Nginx and deployed via Docker Compose.

---

## Description

**AlpacaParty** is a full-stack social gaming platform that combines a 3D alpaca farm (powered by Three.js) with two real-time multiplayer mini-games (**Spit Royale** and **Alpaca Road**), a social feed, direct messaging, and a friends system. An AI-powered help desk built on the Groq LLM API answers user questions inside the app.

### Key Features

- **3D Alpaca Farm** — Three.js immersive farm with alpaca customization, shop, building and editing
- **Two Real-time Mini-games**: offline or online with real players
  - Spit Royale: battle royale
  - Alpaca Road: real time race
- **Real-time Chat** — Direct messages with friends
- **Social Feed** — Posts with image uploads, likes, comments
- **Friends & Presence** — Add friends, see online status, block users
- **AI Help Desk** — Floating chat widget that streams answers from Groq; system-prompted to know AlpacaParty's features
- **Gamification** — XP/level, achievements, rewards
- **Admin Panel** — Role-based access control (admin/superadmin), content moderation, user management
- **Public API** — 6 RESTful endpoints with `X-API-Key` auth, per-user rate limiting (30 req/min), and an interactive Swagger UI at `/api/docs`
- **HTTPS Everywhere** — nginx + self-signed TLS in dev, ModSecurity WAF (OWASP CRS) in prod
- **Privacy Policy & Terms of Service** — Linked from every page footer

## Instructions

prerequisites:

- **Docker** (≥ v20) and **Docker Compose** (≥ v2)
- **Git**
- **OpenSSL** (for self-signed cert generation in dev)
- A modern browser (latest stable Google Chrome recommended)

to run the project:

- copy the `.env.example` file and change all relevant settings

> [!WARNING]
> Never commit your `.env` file to version control.

| Variable Name             | Default / Example Value                          | Description                                                    |
| :------------------------ | :----------------------------------------------- | :------------------------------------------------------------- |
| **Docker**                |                                                  |                                                                |
| COMPOSE_PROJECT_NAME      | alpacaparty                                      | Name used to identify the Docker services and images.          |
| **PostgreSQL**            |                                                  |                                                                |
| DB_HOST                   | postgres`                                        | Hostname for the PostgreSQL database container.                |
| DB_PORT                   | 5432                                             | Listening port for the PostgreSQL database.                    |
| DB_NAME                   | alpacaparty                                      | Name of the PostgreSQL database.                               |
| DB_USER                   | alpacapartyUser                                  | Database connection username.                                  |
| DB_PASSWORD               | _(blank)_                                        | **[REQUIRED]** Database connection password.                   |
| **JSON Web Token (JWT)**  |                                                  |                                                                |
| JWT_SECRET                | _(blank)_                                        | **[REQUIRED]** Secret key for signing standard JWTs.           |
| JWT_REFRESH_SECRET        | _(blank)_                                        | **[REQUIRED]** Secret key for signing refresh tokens.          |
| JWT_PUBLIC_API_SECRET     | _(blank)_                                        | **[REQUIRED]** Secret key for signing public API tokens.       |
| JWT_ADMIN_SECRET          | _(blank)_                                        | **[REQUIRED]** Secret key for signing admin JWTs.              |
| JWT_EXPIRES_IN            | 1h                                               | Expiration time for standard JWTs.                             |
| JWT_REFRESH_EXPIRES_IN    | 7d                                               | Expiration time for refresh tokens.                            |
| JWT_PUBLIC_API_EXPIRES_IN | 30d                                              | Expiration time for public API tokens.                         |
| JWT_ADMIN_EXPIRES_IN      | 30d                                              | Expiration time for admin JWTs.                                |
| **Backend**               |                                                  |                                                                |
| API_PORT                  | 3000                                             | Port for the backend API server.                               |
| PRISMA_STUDIO_PORT        | 5555                                             | Port for accessing the Prisma Studio database UI.              |
| **Frontend & CORS**       |                                                  |                                                                |
| FRONTEND_URL              | https://{MY_IP}:8443                             | **[CHANGE REQUIRED]** Base URL for the frontend.`.             |
| CORS_ORIGINS              | https://{MY_IP}:8443,https://{MY_IP}.nip.io:8443 | **[CHANGE REQUIRED]** Allowed CORS origins.`.                  |
| PUBLIC_API_HOST           | {MY_IP}:8443                                     | **[CHANGE REQUIRED]** Host for the public API.`.               |
| **Groq AI (Help Desk)**   |                                                  |                                                                |
| GROQ_API_KEY1             | _(blank)_                                        | **[REQUIRED]** API key from console.groq.com.                  |
| GROQ_API_KEY2             | _(blank)_                                        | _(Optional)_ 2nd API key for round-robin rate limit spreading. |
| GROQ_API_KEY3             | _(blank)_                                        | _(Optional)_ 3rd API key for round-robin rate limit spreading. |
| **Miscellaneous**         |                                                  |                                                                |
| UPLOAD_MAX_SIZE           | 10485760                                         | Maximum allowed file upload size in bytes (Default: 10MB).     |

- run `make`
- open `https://localhost:8443` (accept the self-signed cert warning)

to stop the project:

- run `make down`

---

## Team Information

| Member            | Role(s)                                     | Responsibilities                                                                                                                                                                             |
| ----------------- | ------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **ValGSgit**      | Product Owner / Project Manager / Developer | Product vision, backlog & sprint planning, Docker & Makefile infrastructure, backend API architecture, JWT, public API, AI help desk, gamification engine, Spit Royale + Alpaca Road servers |
| **DavidPoetsch**  | Technical Lead / Developer                  | nginx + HTTPS, backend controllers, backend validation, PostgreSQL schema design & query optimization, Prisma setup, Docker setup, tools, Authentication                                     |
| **fankahou**      | Developer                                   | Frontend views, CSS desgin, Three.js core engine (camera, alpaca models, animations, interaction)                                                                                            |
| **LukasStefanek** | Developer                                   | Frontend views, CSS desgin, Three.js core engine (camera, alpaca models, animations, interaction)                                                                                            |

---

## Project Management

### How We Organized Work

- **Task Distribution by domain**
  - Backend & infrastructure (ValGSgit, DavidPoetsch)
  - Frontend & 3D game (fankahou, LukasStefanek).
- **Tools used for project management**
  - Github Issues
  - Discord
- **Weekly syncs** — One team meeting per week to review progress, resolve blockers, and plan the next sprint.
- **Code reviews** — Non-trivial PRs were reviewed by at least one other team member before merge.
- **Communication** — Discord server for daily async chat and quick decisions

---

## Technical Stack

| Layer                   | Technology                                                       | Justification                                                      |
| ----------------------- | ---------------------------------------------------------------- | ------------------------------------------------------------------ |
| **Frontend framework**  | Vue 3 + Vite + Vue Router + Pinia                                | Modern, reactive, excellent DX, mature ecosystem                   |
| **Styling**             | Custom design system (CSS variables + scoped CSS)                | Consistent dark theme, no framework bloat, full control            |
| **3D engine**           | Three.js                                                         | Industry-standard WebGL library                                    |
| **Backend framework**   | Node.js + Express.js                                             | Lightweight, easy Socket.IO integration                            |
| **Real-time**           | Socket.IO                                                        | Reliable WebSocket transport with automatic reconnection and rooms |
| **Database**            | PostgreSQL 16                                                    | Relational integrity + JSONB for flexible farm/game data           |
| **ORM**                 | Prisma                                                           | Type-safe DB access & Clean code                                   |
| **Auth**                | JWT (access + refresh, HTTP-only cookies) + bcrypt + Passport.js | Standard secure pattern                                            |
| **AI**                  | Groq LLM API (llama-3.x) — server-side proxy                     | Fast streaming completions; key never exposed to the browser       |
| **Reverse proxy + WAF** | nginx + ModSecurity (OWASP CRS 3.3.9)                            | HTTPS termination, WS upgrade, attack filtering                    |
| **Containerization**    | Docker Compose (`make`)                                          | Single-command deployment                                          |
| **API docs**            | Swagger UI                                                       | Interactive documentation for the public API                       |

---

## Database Schema

To see:

- Visual representation of the database
- Tables/collections and their relationships.
- Key fields and data types.

run

```
make debug
// once the backend container is running run
make prisma-studio
```

open `http://localhost:5555`

---

## Features List

| Feature                   | Description                                                                                                         | Team Member(s)          |
| ------------------------- | ------------------------------------------------------------------------------------------------------------------- | ----------------------- |
| User Registration & Login | Email/password (bcrypt), JWT access + refresh in HTTP-only cookies                                                  | ValGSgit, DavidPoetsch  |
| User Profiles             | Editable username, bio, status, avatar; XP/level, online indicator, public/private toggle                           | ValGSgit, fankahou      |
| User Settings             | Profile edit, password change, privacy toggle, API key management, data export, account deletion                    | ValGSgit, fankahou      |
| Friends System            | Send/accept/decline/cancel requests; online status; block / unblock                                                 | ValGSgit, fankahou      |
| Direct Messaging          | Real-time 1-to-1 messaging via Socket.IO; typing indicators; read receipts; persistent history; in-chat game invite | ValGSgit, LukasStefanek |
| Social Feed               | Posts with image uploads; likes, comments; public/private                                                           | ValGSgit, fankahou      |
| 3D Alpaca Farm            | Three.js farm world: alpaca customization, shop, building, cloud save (JSONB)                                       | fankahou, LukasStefanek |
| Spit Royale (Game 1)      | Real-time arena: 1v1 matchmaking, survival vs AI bots, spectator, rematch                                           | ValGSgit, LukasStefanek |
| Alpaca Road (Game 2)      | Real-time race: matchmaking, history & ELO tracked separately from Spit Royale                                      | ValGSgit, LukasStefanek |
| Gamification              | XP / level / coins, 6+ achievements, daily challenges, ELO leaderboard                                              | ValGSgit                |
| Admin Panel               | Role-based access control, system statistics, content moderation (admin/superadmin only)                            | ValGSgit                |
| AI Help Desk              | Floating widget; backend proxies user messages + system prompt to Groq LLM API; rate-limited                        | ValGSgit                |
| Notifications             | Real-time notifications for friend requests, messages, likes, comments, achievements, game invites                  | ValGSgit                |
| Public API + Swagger      | 6 endpoints under `/api/public/*` with X-API-Key auth, 30 req/min rate limit, Swagger UI                            | ValGSgit, DavidPoetsch  |
| File Uploads              | Multi-type, MIME whitelist, 10 MB cap, hashed filenames, preview, delete                                            | ValGSgit, DavidPoetsch  |
| HTTPS                     | Self-signed TLS in dev, HSTS, security headers (CSP, X-Frame-Options)                                               | DavidPoetsch, ValGSgit  |
| Privacy Policy / Terms    | Full GDPR-aligned content, footer links from every page                                                             | ValGSgit                |

---

## Modules

### Module Point Calculation

| #   | Module                                                                                                       | Category  | Type  | Points | Status | Team Member(s)          |
| --- | ------------------------------------------------------------------------------------------------------------ | --------- | ----- | ------ | ------ | ----------------------- |
| 1   | **Use a framework for both frontend and backend** (Vue 3 + Express.js)                                       | Web       | Major | **2**  | ✅     | All                     |
| 2   | **Real-time features (WebSockets)** — Socket.IO: presence, DMs, group chat, game state, notifications        | Web       | Major | **2**  | ✅     | ValGSgit                |
| 3   | **User interaction** — chat (DMs + rooms), profile pages, friends system                                     | Web       | Major | **2**  | ✅     | All                     |
| 4   | **Public API** — 6 RESTful endpoints, X-API-Key auth, rate limit, Swagger UI                                 | Web       | Major | **2**  | ✅     | ValGSgit, DavidPoetsch  |
| 5   | **ORM** — Prisma                                                                                             | Web       | Minor | **1**  | ✅     | ValGSgit, DavidPoetsch  |
| 6   | **Notification system** — real-time create/update/delete notifications                                       | Web       | Minor | **1**  | ✅     | ValGSgit                |
| 7   | **Custom made design** — Proper color palette, typography, icons, reuseable components                       | Web       | Minor | **1**  | ✅     | fankahou, LukasStefanek |
| 8   | **Advanced search functionality** — Filter, sorting, pageination                                             | Web       | Minor | **1**  | ✅     | ValGSgit, DavidPoetsch  |
| 9   | **File upload and management** — multi-type, validation, secure storage, preview, delete                     | Web       | Minor | **1**  | ✅     | ValGSgit, DavidPoetsch  |
| 10  | **Support for additional browsers** — Full compatibility with at least 2 additional browsers                 | Web       | Minor | **1**  | ✅     | All                     |
| 11  | **Standard user management** — profile edit, avatar upload, friends, online status                           | User Mgmt | Major | **2**  | ✅     | ValGSgit, fankahou      |
| 12  | **Advanced permission system** — view, edit and delete users, user roles                                     | User Mgmt | Major | **2**  | ✅     | ValGSgit                |
| 13  | **LLM system interface** — Groq-backed help desk; backend streams completions, rate limited, system-prompted | AI        | Major | **2**  | ✅     | ValGSgit                |
| 14  | **Web-based game** — Alpaca Road: offline multiplayer game with clear win/loss rules                         | Gaming    | Major | **2**  | ✅     | fankahou, LukasStefanek |
| 15  | **Remote players play the same game** — Alpaca Road: online multiplayer game with clear win/loss rules       | Gaming    | Major | **2**  | ✅     | fankahou, LukasStefanek |
| 16  | **Multiplayer game** — Alpaca Road: online game, can be played by 4 people at the same time                  | Gaming    | Major | **2**  | ✅     | fankahou, LukasStefanek |
| 17  | **Advanced 3D graphics (Three.js)** — immersive farm world, lighting, cameras, animations                    | Gaming    | Major | **2**  | ✅     | fankahou, LukasStefanek |
| 18  | **Game customization** — power-ups, multiple maps/themes, customizable settings, defaults                    | Gaming    | Minor | **1**  | ✅     | fankahou, LukasStefanek |
| 19  | **Gamification System** — achievements, leaderboards, XP/level, persistence, visual feedback                 | Gaming    | Minor | **1**  | ✅     | ValGSgit                |
| 20  | **Module of choice** — Sandbox game, Edit mode, shop, custom 3D models, visit others                         | Gaming    | Major | **2**  | ✅     | fankahou, LukasStefanek |

### **Total: 32 points** (14 required + bonus headroom)

### Module Implementation Details

1. **Frontend + Backend Frameworks** — Vue 3 + Vite + Pinia + Vue Router on the client; Express.js with modular controller/service/route architecture on the server.
2. **Real-time features (WebSockets)** — Socket.IO with two namespaces. The `/` namespace handles presence, DMs, notifications The `/minigames` namespace hosts `MatchManager`, which dispatches to `SpitRoyalMatch` and `AlpacaRoadMatch` instances per match room (each running its own tick loop).
3. **User interaction** — Direct messaging, profile pages with stats, friends with online presence, block/unblock.
4. **Public API** — The endpoints are implemented with a regualr express controller. But for each enpoint a API key for authentication is needed. This API key can be generated in the User Settings view. The documentation was done with Swagger.
5. **ORM** — Prisma. Used for clean code. Migrations are generated in the docker entrypoint script if they do not exist.
6. **Notification system** — Notifications are inserted on friend-request CRUD, post likes, comments, achievement unlocks, and game invites; pushed in real time over Socket.IO and persisted in the `Notification` table.
7. **Custom made design** — styles are defined in styles.css and used across all frontend components
8. **Advanced search functionality** — In the friends view you are able to search, filter and sort friends. More than 10 friends will be pageinated.
9. **Support for additional browsers\*** — Just works.
10. **File upload and management** — `multer` accepts images & documents under a MIME whitelist; 10 MB cap; hashed stored names; per-user listing; uploader-only delete; image preview; non-image files require auth to download.
11. **Standard user management** — Editable profile (username, email, bio, status, avatar). Default avatar served when none uploaded. Friends with real-time presence. Profile page shows level, XP progress, achievements, stats.
12. **Advanced permission system** — Seperate Admin panel to view and edit users.
13. **LLM system interface** — `POST /api/helpdesk/chat` proxies user messages to Groq's LLM API. The backend keeps the API keys (rotates across multiple keys), injects a system prompt that explains AlpacaParty's features, applies a per-user rate limiter, and streams completions back to the floating `HelpDeskChat.vue` widget.
14. **Web-based game** — Alpaca Road:
15. **Remote players play the same game** — Alpaca Road: A websocket is used to sync game information across users.
16. **Multiplayer game** — Same as remote players module.
17. **Advanced 3D graphics** — Three.js scene graph with custom lighting, multiple cameras, alpaca model rigging + animation, and an interactive farm world with shop, customization, and editing.
18. **Game customization** — Both games expose customizable settings (power-ups, maps/themes, match rules) with sensible defaults; the AI bot in Spit Royale plays under the same rule set.
19. **Gamification** — XP awarded for wins, losses, posts, and challenges. Auto level-up. Achievements: `first_win`, `win_streak_5`, `level_10`, `social_butterfly`, `first_post`. Notifications and progress bars provide visual feedback.
20. **Module of choice** — Sandbox game, edit mode, shop, custom 3D models, visit others farms, custom alpaca design, custom object size, rotation and placement, day / night transition.

---

## Individual Contributions

### ValGSgit — Product Owner / Project Manager / Developer

- **Infrastructure**: Docker setup, Makefile
- **Backend core**: Express server bootstrap, configuration system, middleware stack (helmet, cookie-parser, CORS, rate limiters, `authenticate`, `optionalAuth`, `requireApiKey`, `admin`, error handler)
- **Auth**: JWT access + refresh in HTTP-only cookies, bcrypt hashing.
- **Database**: Prisma schema (27 models), migrations, seed data
- **API surface**: All controllers
- **Services**: Gamification engine, NotificationService, dataExportService (JSON/CSV/XML), uploadService, adminAuthService, `socketService` for the `/` namespace, `MatchManager` on the `/minigames` namespace dispatching `SpitRoyalMatch` (with AI bot tactics) and `AlpacaRoadMatch`
- **Admin Panel**: Role-based access control (admin/superadmin), `adminController`, `adminAuthService`, `admin.js` middleware, admin route definitions, system statistics endpoints
- **AI**: Groq LLM proxy (`/helpdesk`) with key rotation and rate limiting; floating `HelpDeskChat.vue` widget
- **Frontend**: `Feed.vue`, `Profile.vue`, settings page, public API key management UI, `ApiDocs.vue`, `AdminPanel.vue`, `AdminLogin.vue`, notifications
- **Compliance**: `PrivacyPolicy.vue`, `TermsOfService.vue`, GDPR export + delete-request flows

### David Poetsch — Technical Lead / Developer

- **Devops**: Docker setup, Testing setup, Makefile
- **prisma**: Setup prisma, make migrations work properly
- **nginx**: Reverse-proxy config for dev + prod, HTTPS termination, WS upgrade for Socket.IO, security headers.
- **Backend**: Controller implementations, route definitions, query optimization
- **Database**: Schema refinements, index strategy, data integrity constraints
- **Auth**: proper authentication with json web tokens and HTTP-Only Cookies
- **backend-validation**: query paramter and body validation with express-validator

### fankahou — Developer

- **3D world**: Three.js farm environment, world rendering, asset management (models + textures)
- **Game UI**: Shop, HUD overlay, edit mode, lighting controls
- **Frontend**: `Friends.vue`, parts of `Profile.vue`, CSS design system, dark theme & responsive layouts

### Lukas Stefanek — Developer

- **3D engine**: Three.js core (camera system, alpaca models + animations), interaction mechanics
- **Game logic**: Alpaca customization, farm building, coin economy, cloud save/load over Socket.IO
- **Game clients**: Spit Royale + Alpaca Road client code under `frontend/src/games/mini_games/`
- **Frontend**: `Login.vue`, `Register.vue`, `Messages.vue` (DM client with Socket.IO integration, typing indicators, read receipts, infinite scroll)

---

## Resources

### Documentation & References

- [Vue 3](https://vuejs.org/guide/) · [Vue Router](https://router.vuejs.org/) · [Pinia](https://pinia.vuejs.org/)
- [Express.js](https://expressjs.com/) · [Socket.IO](https://socket.io/docs/) . [express-validator](https://express-validator.github.io/docs/guides/getting-started/)
- [Three.js](https://threejs.org/docs/)
- [Prisma](https://www.prisma.io/docs) · [PostgreSQL 16](https://www.postgresql.org/docs/16/)
- [nginx reverse proxy](https://docs.nginx.com/nginx/admin-guide/web-server/reverse-proxy/)
- [Groq API](https://console.groq.com/docs)
- [JWT Authentication](https://reintech.io/blog/nodejs-jwt-authentication-complete-implementation-guide#why-jwt-authentication-makes-sense-for-modern-apis)

### AI Usage

AI tools (GitHub Copilot, ChatGPT, Claude) were used throughout the project to assist with:

- **Boilerplate generation** — initial CRUD scaffolds and repetitive controller patterns
- **Configuration** — Docker Compose snippets, nginx templates, ModSecurity tuning starting points
- **Debugging** — narrowing down Socket.IO disconnect issues, CORS edge cases, JWT refresh races
- **Documentation** — README structure, Privacy Policy / Terms of Service drafting, OpenAPI tags
- **Tests** — test-case scaffolding and mock setup patterns

AI tools (Claude, GitHub Copilot, ChatGPT) were used throughout the project. We treated them as a fast collaborator, not an author — every AI-assisted line was reviewed, tested, and adapted by the team. We can explain and defend any code in the project.

Specifically, AI assistance covered:

- **Frontend visual design & styling** — the look-and-feel of several pages, in particular the standalone `Help.vue` help center, `PrivacyPolicy.vue`, `TermsOfService.vue`, and parts of `NotFound.vue` / `AuthV3.vue` / `Feed.vue` were prototyped with AI (mock-ups, CSS hero treatments, scoped colour tokens, micro-animations). The structure and copy were then rewritten by the team so each page accurately describes what the project actually ships.
- **Boilerplate generation** — initial CRUD scaffolds and repetitive controller patterns
- **Configuration** — Docker Compose snippets, nginx templates
- **Debugging** — narrowing down Socket.IO disconnect issues, CORS edge cases, JWT refresh races
- **Documentation** — README structure, OpenAPI tags
- **The in-app help desk itself** — answers to user questions are generated by Groq's LLM API, never by code we wrote

---

## Known Limitations

- SSL uses a self-signed certificate in development — production must supply a CA-signed cert.
- Firefox / Safari / Edge: tested informally and broadly compatible, but Chrome is the primary supported browser per the subject's requirements.
- AlpacaParty matches are not matchmade — players manually join rooms via the in-game lobby. There is no ELO ranking; leaderboards rank by aggregate stats (kills, obstacles, coins).
- There is no group chat or chat-room feature — only 1-to-1 direct messages.
- There is no in-app moderation UI. Operator-level actions (e.g. banning a user) are performed directly against the database.

---

## License

See [LICENSE](LICENSE) for details.
