// Helmet / CSP configuration.
//
// 'unsafe-eval' is never allowed — grep confirms no eval / new Function in
// backend or frontend, and modern Vite + our Vue build do not require it.
// 'unsafe-inline' on script-src is kept only to let Swagger UI bootstrap at
// /api/docs; everything else loads as 'self' ESM bundles.
const isProd = process.env.NODE_ENV === "production";

export const getHelmetConfig = () => ({
  crossOriginResourcePolicy: { policy: "cross-origin" },
  contentSecurityPolicy: {
    useDefaults: false,
    directives: {
      defaultSrc: ["'self'"],
      scriptSrc: ["'self'", "'unsafe-inline'"],
      scriptSrcAttr: ["'none'"],
      styleSrc: ["'self'", "'unsafe-inline'", "https://fonts.googleapis.com"],
      imgSrc: [
        "'self'",
        "data:",
        "blob:",
        "https://picsum.photos",
        "https://*.picsum.photos",
        "https://images.pexels.com",
      ],
      connectSrc: [
        "'self'",
        "blob:",
        "wss:",
        ...(isProd ? [] : ["ws:"]),
      ],
      fontSrc: ["'self'", "data:", "https://fonts.gstatic.com"],
      objectSrc: ["'none'"],
      mediaSrc: ["'self'", "blob:"],
      frameSrc: ["'none'"],
      workerSrc: ["'self'", "blob:"],
      manifestSrc: ["'self'"],
      baseUri: ["'self'"],
      formAction: ["'self'"],
      frameAncestors: ["'none'"],
      ...(isProd ? { upgradeInsecureRequests: [] } : {}),
    },
  },
});
