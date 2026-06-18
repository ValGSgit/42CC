import config from "#config/index.js";
import fs from "fs";
import https from "https";

export const createHttpsServer = (app) => {
  if (!config.ssl.certPath || config.ssl.certPath == null)
    throw new Error("SSL_CERT_PATH variable not found in environment");
  if (!config.ssl.keyPath || config.ssl.keyPath == null)
    throw new Error("SSL_KEY_PATH variable not found in environment");

  if (!fs.existsSync(config.ssl.certPath))
    throw new Error(`SSL Certificate file not found: ${config.ssl.certPath}`);
  if (!fs.existsSync(config.ssl.keyPath))
    throw new Error(`SSL Key file not found: ${config.ssl.keyPath}`);

  const cert = fs.readFileSync(config.ssl.certPath, "utf8");
  const key = fs.readFileSync(config.ssl.keyPath, "utf8");

  const httpsServer = https.createServer({ key, cert }, app);

  console.log(
    "[ssl] ✓ HTTPS enabled with certificates from",
    config.ssl.certPath,
  );
  return httpsServer;
};
