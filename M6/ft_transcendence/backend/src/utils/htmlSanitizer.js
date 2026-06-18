/**
 * Strip tags capable of executing scripts and dangerous attributes/URLs
 * without entity-encoding the surrounding text. Defense-in-depth — the
 * frontend uses Vue's {{ }} interpolation which already escapes, but this
 * ensures stored content is never a stored-XSS sink if a v-html slips in.
 *
 * Loops until output stabilises so reassembly bypasses like
 * `<scr<script>ipt>alert(1)</script>` collapse rather than just shifting.
 *
 * @param {string} str
 * @returns {string}
 */

const DANGEROUS_TAGS = ["script", "iframe", "object", "form", "style", "embed", "link", "meta"];
const URL_ATTRS = ["href", "src", "xlink:href", "action", "formaction"];

const BAD_URL_SCHEME = /^\s*(?:javascript|vbscript|data|file):/i;

// Opening tag plus (optionally) its body + matching closing tag. The
// optional group covers self-closing/void tags too — `<embed ...>` matches
// without needing `</embed>`.
const DANGEROUS_TAG = new RegExp(
  `<(${DANGEROUS_TAGS.join("|")})\\b[^>]*>(?:[\\s\\S]*?<\\/\\1\\s*>)?`,
  "gi",
);

// Inline event handler: on*="..." | on*='...' | on*=bare
const EVENT_HANDLER = /\son\w+\s*=\s*(?:"[^"]*"|'[^']*'|[^\s>]*)/gi;

// URL-bearing attribute with captured value (double-quoted, single-quoted, bare).
const URL_ATTR = new RegExp(
  `\\s(?:${URL_ATTRS.join("|")})\\s*=\\s*(?:"([^"]*)"|'([^']*)'|([^\\s>]*))`,
  "gi",
);

export function stripDangerousHtml(str) {
  if (typeof str !== "string") return str;

  let out = str;
  let prev;
  let guard = 0;
  do {
    prev = out;
    out = out
      .replace(DANGEROUS_TAG, "")
      .replace(EVENT_HANDLER, "")
      .replace(URL_ATTR, (match, dq, sq, uq) =>
        BAD_URL_SCHEME.test(dq ?? sq ?? uq ?? "") ? "" : match,
      );
  } while (out !== prev && ++guard < 10);

  return out;
}
