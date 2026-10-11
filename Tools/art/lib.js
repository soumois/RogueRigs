// Shared helpers for the Rogue Rigs art tools (kie_image.js, meshy_3d.js).
// Node 18+ only (global fetch), no dependencies, like vault/scripts/lint.js.
// API keys are read from environment variables by name; they are never written
// to a log, a report or the console.

const fs = require('fs');
const path = require('path');

const REPO = path.resolve(__dirname, '..', '..');

// --name value | --flag | repeated --name a --name b (collected when listed in `multi`).
function parseArgs(argv, { flags = [], multi = [] } = {}) {
  const out = { _: [] };
  for (const m of multi) out[m] = [];
  for (let i = 0; i < argv.length; i++) {
    const a = argv[i];
    if (!a.startsWith('--')) { out._.push(a); continue; }
    const name = a.slice(2);
    if (flags.includes(name)) { out[name] = true; continue; }
    const value = argv[i + 1];
    if (value === undefined || value.startsWith('--')) usage(`missing value for --${name}`);
    i++;
    if (multi.includes(name)) out[name].push(value); else out[name] = value;
  }
  return out;
}

let usageText = '';
function setUsage(text) { usageText = text; }
function usage(message) {
  if (message) console.error(`error: ${message}`);
  console.error(usageText);
  process.exit(2);
}

function requireKey(envName) {
  const key = process.env[envName];
  if (!key) {
    console.error(`error: environment variable ${envName} is not set`);
    process.exit(2);
  }
  return key;
}

// Asset names become folder names: keep them boring.
function checkAssetName(name) {
  if (!name || !/^[a-z0-9][a-z0-9-]*$/.test(name)) usage('--asset must be kebab-case (a-z, 0-9, -)');
  return name;
}

function runId(suffix) {
  const d = new Date();
  const p = (n) => String(n).padStart(2, '0');
  return `${d.getFullYear()}${p(d.getMonth() + 1)}${p(d.getDate())}-${p(d.getHours())}${p(d.getMinutes())}${p(d.getSeconds())}-${suffix}`;
}

async function httpJson(method, url, { key, body } = {}) {
  const headers = { Accept: 'application/json' };
  if (key) headers.Authorization = `Bearer ${key}`;
  if (body !== undefined) headers['Content-Type'] = 'application/json';
  const res = await fetch(url, { method, headers, body: body === undefined ? undefined : JSON.stringify(body) });
  const text = await res.text();
  let json;
  try { json = JSON.parse(text); } catch { json = null; }
  if (!res.ok) throw new Error(`${method} ${url} -> HTTP ${res.status}: ${text.slice(0, 300)}`);
  if (json === null) throw new Error(`${method} ${url} -> response is not JSON: ${text.slice(0, 300)}`);
  return json;
}

const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

// Calls `check` until it returns a non-undefined value. Interval grows x1.25 up to maxMs.
async function poll(check, { firstMs = 3000, maxMs = 15000, timeoutMs = 15 * 60 * 1000, label = 'task' } = {}) {
  const start = Date.now();
  let wait = firstMs;
  for (;;) {
    const result = await check();
    if (result !== undefined) return result;
    if (Date.now() - start > timeoutMs) throw new Error(`${label}: timed out after ${Math.round(timeoutMs / 1000)} s`);
    await sleep(wait);
    wait = Math.min(Math.round(wait * 1.25), maxMs);
  }
}

// Provider URLs expire (Kie 14 days, Tripo 5 min, Meshy signed): download at once.
async function download(url, dest) {
  const res = await fetch(url);
  if (!res.ok) throw new Error(`download ${url} -> HTTP ${res.status}`);
  fs.mkdirSync(path.dirname(dest), { recursive: true });
  fs.writeFileSync(dest, Buffer.from(await res.arrayBuffer()));
  return dest;
}

function mimeOf(file) {
  const ext = path.extname(file).toLowerCase();
  if (ext === '.png') return 'image/png';
  if (ext === '.jpg' || ext === '.jpeg') return 'image/jpeg';
  if (ext === '.webp') return 'image/webp';
  usage(`unsupported image type: ${file} (png, jpg, webp)`);
}

function dataUri(file) {
  return `data:${mimeOf(file)};base64,${fs.readFileSync(file).toString('base64')}`;
}

function writeJson(file, data) {
  fs.mkdirSync(path.dirname(file), { recursive: true });
  fs.writeFileSync(file, JSON.stringify(data, null, 2) + '\n');
}

function rel(p) { return path.relative(REPO, p).split(path.sep).join('/'); }

module.exports = {
  REPO, parseArgs, setUsage, usage, requireKey, checkAssetName, runId,
  httpJson, poll, download, mimeOf, dataUri, writeJson, rel, sleep,
};
