import { defineConfig } from "prisma/config";

export default defineConfig({
  schema: "prisma/schema.prisma",
  migrations: {
    path: "prisma/migrations",
    seed: "node prisma/seed.js",
  },
  datasource: {
    // Fallback keeps CI/unit workflows from crashing when DATABASE_URL is not injected.
    url: process.env.DATABASE_URL || "postgresql://alpacaparty:alpacaparty@localhost:5432/alpacaparty",
  },
});
