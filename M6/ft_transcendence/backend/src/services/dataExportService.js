/**
 * Data Export Service — GDPR-compliant data export in JSON, CSV, XML
 */
import User from '../models/User.js';

/**
 * Convert array of objects to CSV string.
 *
 * Defends against CSV injection: a cell starting with =, +, -, @, |, \t, \r
 * is prefixed with a single quote so Excel/Sheets treat it as text rather
 * than a formula. A user with bio `=cmd|'/c calc'!A0` cannot weaponise their
 * own export.
 */
const FORMULA_PREFIX = /^[=+\-@|\t\r]/;
function cellSafe(val) {
  if (val == null) return '';
  let s = String(val);
  if (FORMULA_PREFIX.test(s)) s = "'" + s;
  return s.replace(/"/g, '""');
}

function toCsv(rows) {
  if (!rows || rows.length === 0) return '';
  // Union of keys across all rows so heterogeneous shapes don't mis-align
  // columns when later rows have fields not present in rows[0].
  const headers = Array.from(
    rows.reduce((acc, r) => {
      Object.keys(r || {}).forEach((k) => acc.add(k));
      return acc;
    }, new Set()),
  );
  // Headers are hard-coded object keys, never user-controlled — no formula
  // injection risk, no need to wrap them in quotes (keeps the diff against
  // pre-existing CSV consumers minimal).
  const lines = [headers.join(',')];
  for (const row of rows) {
    lines.push(headers.map((h) => `"${cellSafe(row[h])}"`).join(','));
  }
  return lines.join('\n');
}

/**
 * Convert object/array to simple XML string.
 */
function toXml(obj, rootName = 'data') {
  function escapeXml(s) {
    return String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
  }
  function serialize(data, tagName) {
    if (Array.isArray(data)) {
      return data.map((item, i) => serialize(item, tagName || `item`)).join('\n');
    }
    if (data && typeof data === 'object') {
      const inner = Object.entries(data)
        .map(([k, v]) => serialize(v, k))
        .join('\n');
      return `<${tagName}>\n${inner}\n</${tagName}>`;
    }
    return `<${tagName}>${escapeXml(data ?? '')}</${tagName}>`;
  }
  return `<?xml version="1.0" encoding="UTF-8"?>\n${serialize(obj, rootName)}`;
}

const DataExportService = {
  /**
   * Export all user data in the requested format.
   * @param {number} userId
   * @param {'json'|'csv'|'xml'} format
   * @returns {{ data: string, contentType: string, extension: string }}
   */
  async exportUserData(userId, format = 'json') {
    const exportData = await User.getFullExport(userId);

    switch (format) {
      case 'csv': {
        // CSV: flatten each section into its own block
        const sections = Object.entries(exportData).map(([key, val]) => {
          const rows = Array.isArray(val) ? val : [val];
          return `# ${key}\n${toCsv(rows)}`;
        });
        return { data: sections.join('\n\n'), contentType: 'text/csv', extension: 'csv' };
      }
      case 'xml':
        return { data: toXml(exportData, 'userExport'), contentType: 'application/xml', extension: 'xml' };
      case 'json':
      default:
        return { data: JSON.stringify(exportData, null, 2), contentType: 'application/json', extension: 'json' };
    }
  },
};

export default DataExportService;
