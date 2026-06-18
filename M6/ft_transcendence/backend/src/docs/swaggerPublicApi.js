import swaggerAutogen from "swagger-autogen";
import { loadEnv } from "#tools/loadEnv.js";

loadEnv();

const swaggerAutogenInstance = swaggerAutogen();

const doc = {
  info: {
    title: "AlpacaParty Public Api",
    description: `Auto-generated Swagger documentation.
## Getting Started

1. **Log in** to your AlpacaParty account.
2. Go to **Profile &rarr; Settings &rarr; Public API Key** and click **Generate Key**.
3. Copy the key. 
4. Press the **Authorize** Button, paste the key and press authorize.

_You can reveal, regenerate, or revoke the key at any time from the same page._
`,
  },
  host: process.env.PUBLIC_API_HOST || "localhost:8443",
  schemes: ["https"],
  basePath: "/api/public",

  securityDefinitions: {
    apiKeyAuth: {
      type: "apiKey",
      in: "header", // can be 'header', 'query' or 'cookie'
      name: "x-api-key", // name of the header, query parameter or cookie
      description: "api-key for public api",
    },
  },
};

const outputFile = "./swagger-output-public-api.json";
const outputFileAbs = "src/docs/swagger-output-public-api.json";
const endpointsFiles = ["../routes/public.js"];

swaggerAutogenInstance(outputFile, endpointsFiles, doc).then(() => {
  console.log("Swagger docs generated!");
});
