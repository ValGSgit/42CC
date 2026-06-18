import fs from "fs";
import { parse } from "csv-parse/sync";

/**
 * Generic function to seed a Prisma model from a CSV file
 */
export async function seedFromCsv(prismaModel, csvPath) {
  if (!fs.existsSync(csvPath)) {
    console.warn(`File not found at ${csvPath}`);
    return;
  }

  const fileContent = fs.readFileSync(csvPath, { encoding: "utf-8" });

  const records = parse(fileContent, {
    columns: true,
    skip_empty_lines: true,
    cast: (value) => {
      const lower = value.toLowerCase().trim();
      if (lower === "true") return true;
      if (lower === "false") return false;
      if (!isNaN(value) && value.trim() !== "") return Number(value);
      return value;
    },
  });

  console.log(`Seeding ${records.length} records into ${prismaModel.name}...`);

  return await prismaModel.createMany({
    data: records,
    skipDuplicates: true,
  });
}

/**
 * Generic function to seed a Prisma model from a CSV file
 */
export function readCsv(csvPath) {
  if (!fs.existsSync(csvPath)) {
    console.warn(`File not found at ${csvPath}`);
    return;
  }

  const fileContent = fs.readFileSync(csvPath, { encoding: "utf-8" });
  return fileContent;
}

export async function seedRecords(prismaModel, records) {
  console.log(`Seeding ${records.length} records into ${prismaModel.name}...`);

  return await prismaModel.createMany({
    data: records,
    skipDuplicates: true,
  });
}
