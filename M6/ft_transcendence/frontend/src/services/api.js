const env = import.meta?.env || {};

const defaultBaseUrl = env.VITE_API_URL || "/api";

function joinApiPath(base, path) {
  const normalizedBase = String(base || "").replace(/\/+$/, "");
  const normalizedPath = String(path || "").replace(/^\/+/, "");
  if (!normalizedPath) return `${normalizedBase}/`;
  return `${normalizedBase}/${normalizedPath}`;
}

class HttpError extends Error {
  constructor(message, response, data) {
    super(message);
    this.name = "HttpError";
    this.response = response;
    this.data = data;
  }
}

function buildUrl(path, params) {
  const url = new URL(path, window.location.origin);

  if (params && typeof params === "object") {
    for (const [key, value] of Object.entries(params)) {
      if (value == null) continue;
      if (Array.isArray(value)) {
        for (const item of value) url.searchParams.append(key, String(item));
      } else {
        url.searchParams.set(key, String(value));
      }
    }
  }

  return url;
}

function isJsonLike(body) {
  return (
    body != null &&
    typeof body !== "string" &&
    !(body instanceof FormData) &&
    !(body instanceof Blob) &&
    !(body instanceof ArrayBuffer) &&
    !(body instanceof URLSearchParams)
  );
}

async function parseResponse(response) {
  const contentType = response.headers.get("content-type") || "";

  if (response.status === 204) {
    return null;
  }

  if (contentType.includes("application/json")) {
    return await response.json();
  }

  return await response.text();
}

// Single in-flight refresh promise. Without this, two concurrent 401s would
// each POST /auth/refresh in parallel — the second loses to the first's
// token rotation and the user gets a spurious "session expired".
let refreshInFlight = null;

function refreshAccessToken() {
  if (refreshInFlight) return refreshInFlight;
  refreshInFlight = performRequest({
    method: "POST",
    path: "/auth/refresh",
    data: {},
    retryOnAuth: false,
  });
  // Clear the slot once the request settles (success OR failure) so the
  // next 401 can issue a fresh refresh. Using then(fn, fn) instead of
  // finally() means the rejection is handled here too, so a failed refresh
  // doesn't surface as an unhandled promise rejection — the awaiting caller
  // still sees the rejection via the returned promise below.
  const clearSlot = () => { refreshInFlight = null; };
  refreshInFlight.then(clearSlot, clearSlot);
  return refreshInFlight;
}

async function performRequest({
  method = "GET",
  path,
  data,
  params,
  headers = {},
  timeout,
  retryOnAuth = true,
}) {
  const controller = timeout ? new AbortController() : null;
  const timer = timeout ? setTimeout(() => controller.abort(), timeout) : null;

  try {
    const url = buildUrl(joinApiPath(defaultBaseUrl, path), params);
    const requestHeaders = {
      Accept: "application/json",
      ...headers,
    };

    let body = data;
    if (isJsonLike(data)) {
      requestHeaders["Content-Type"] =
        requestHeaders["Content-Type"] || "application/json";
      body = JSON.stringify(data);
    }

    const response = await fetch(url, {
      method,
      headers: requestHeaders,
      body: method === "GET" || method === "HEAD" ? undefined : body,
      signal: controller?.signal,
      credentials: "include",
    });

    if (response.ok) {
      return {
        data: await parseResponse(response),
        status: response.status,
        headers: response.headers,
        config: { method, url: url.toString() },
      };
    }

    const errorData = await parseResponse(response).catch(() => null);

    if (response.status === 401 && retryOnAuth) {
      try {
        await refreshAccessToken();
      } catch {
        // Refresh failed — fall through and surface the original 401 below.
        throw new HttpError(
          `Request failed with status ${response.status}`,
          response,
          errorData,
        );
      }
      // One retry only; the inner call disables retryOnAuth so we cannot
      // loop here if the second attempt also returns 401.
      return await performRequest({
        method,
        path,
        data,
        params,
        headers,
        timeout,
        retryOnAuth: false,
      });
    }

    throw new HttpError(
      `Request failed with status ${response.status}`,
      response,
      errorData,
    );
  } catch (error) {
    if (error.name === "AbortError") {
      throw new HttpError("Request timed out", null, null);
    }
    if (error instanceof HttpError) {
      throw error;
    }
    throw new HttpError(error?.message || "Network request failed", null, null);
  } finally {
    if (timer) clearTimeout(timer);
  }
}

const api = {
  request: performRequest,
  get(path, options = {}) {
    return performRequest({ method: "GET", path, ...options });
  },
  post(path, data, options = {}) {
    return performRequest({ method: "POST", path, data, ...options });
  },
  put(path, data, options = {}) {
    return performRequest({ method: "PUT", path, data, ...options });
  },
  patch(path, data, options = {}) {
    return performRequest({ method: "PATCH", path, data, ...options });
  },
  delete(path, options = {}) {
    return performRequest({ method: "DELETE", path, ...options });
  },
};

export default api;
export { HttpError };
