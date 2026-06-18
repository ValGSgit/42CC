import "dotenv/config";
import path from "path";
import { fileURLToPath } from "url";
import { seedFromCsv } from "./seedUtils.js";
import prisma from "#lib/prisma.js";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

async function main() {
  // Import Achievements (reference data the game relies on)
  await seedFromCsv(
    prisma.achievement,
    path.resolve(__dirname, "seed/achievements.csv"),
  );
}

main()
  .catch((e) => {
    console.error("Seeding failed:", e);
    process.exit(1);
  })
  .finally(async () => {
    await prisma.$disconnect();
  });
