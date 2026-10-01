#!/usr/bin/env node
// Rogue Rigs layout validator. Spec: vault node `level-layout-schema`.
//   node Tools/validate_layout.js <layout.json> [--brief <brief.json>]
// Exit 0 = valid. Errors and an ASCII preview go to stdout.

const fs = require('fs');

const BLOCK_TYPES = new Set(['rock', 'ore', 'heal_ore', 'special_ore', 'barrel']);
const MARKER_TYPES = new Set(['spawn']);
const VOID_TYPE = 'void'; // outside the asteroid: no floor, no block
const GLYPH = { rock: '#', ore: 'o', heal_ore: '+', special_ore: '*', barrel: 'B', spawn: 'S', void: ' ' };
const SPAWN_CLEAR_RADIUS = 2; // cells around the spawn that must stay empty (no block, no void)

const args = process.argv.slice(2);
const file = args[0];
const briefIdx = args.indexOf('--brief');
const briefFile = briefIdx >= 0 ? args[briefIdx + 1] : null;
if (!file) { console.log('usage: node Tools/validate_layout.js <layout.json> [--brief <brief.json>]'); process.exit(2); }

const errors = [];
const err = (m) => errors.push(m);
let grid = null, W = 0, H = 0, coverage = 0, reachable = 0;
const counts = {};

let L;
try { L = JSON.parse(fs.readFileSync(file, 'utf8')); }
catch (e) { console.log(`FAIL: cannot read or parse ${file}: ${e.message}`); process.exit(1); }

// ---- header ----
if (L.version !== 1) err('version must be 1');
if (typeof L.name !== 'string' || !/^[a-z0-9_]+$/.test(L.name)) err('name must be a snake_case string');
for (const k of ['width', 'height']) if (!Number.isInteger(L[k]) || L[k] < 8 || L[k] > 256) err(`${k} must be an integer in [8, 256]`);
if (!Array.isArray(L.cells)) err('cells must be an array');
const known = new Set(['version', 'name', 'width', 'height', 'cells']);
for (const k of Object.keys(L)) if (!known.has(k)) err(`unknown top-level field: ${k}`);
if (errors.length) report();

// ---- cells ----
W = L.width; H = L.height;
grid = Array.from({ length: H }, () => Array(W).fill(null));
let spawn = null;
L.cells.forEach((c, i) => {
  const where = `cells[${i}]`;
  if (typeof c !== 'object' || c === null) return err(`${where} is not an object`);
  for (const k of Object.keys(c)) if (!['x', 'y', 'type'].includes(k)) err(`${where}: unknown field ${k}`);
  if (!Number.isInteger(c.x) || !Number.isInteger(c.y)) return err(`${where}: x and y must be integers`);
  if (c.x < 0 || c.x >= W || c.y < 0 || c.y >= H) return err(`${where}: (${c.x},${c.y}) out of bounds ${W}x${H}`);
  if (!BLOCK_TYPES.has(c.type) && !MARKER_TYPES.has(c.type) && c.type !== VOID_TYPE) return err(`${where}: unknown type "${c.type}"`);
  if (grid[c.y][c.x]) return err(`${where}: duplicate cell (${c.x},${c.y})`);
  grid[c.y][c.x] = c.type;
  if (c.type === 'spawn') { if (spawn) err(`${where}: more than one spawn`); spawn = c; }
});
if (!spawn) err('exactly one spawn cell is required');
else {
  for (let dy = -SPAWN_CLEAR_RADIUS; dy <= SPAWN_CLEAR_RADIUS; dy++)
    for (let dx = -SPAWN_CLEAR_RADIUS; dx <= SPAWN_CLEAR_RADIUS; dx++) {
      const x = spawn.x + dx, y = spawn.y + dy;
      if (x < 0 || y < 0 || x >= W || y >= H) { err(`spawn too close to the arena edge (needs ${SPAWN_CLEAR_RADIUS} free cells)`); continue; }
      if ((dx || dy) && grid[y][x]) err(`cell (${x},${y}) near the spawn must be empty floor (found ${grid[y][x]})`);
    }
}

// ---- stats ----
let blocks = 0, voids = 0, floor = 0;
for (const row of grid) for (const t of row) {
  if (t) counts[t] = (counts[t] || 0) + 1;
  if (t === VOID_TYPE) voids++; else if (BLOCK_TYPES.has(t)) blocks++; else floor++;
}
const solid = W * H - voids; // asteroid cells
coverage = solid ? blocks / solid : 0;
// reachable floor: share of floor cells (non-void, non-block; spawn included) 4-connected to the spawn
const isFloor = (x, y) => x >= 0 && y >= 0 && x < W && y < H && grid[y][x] !== VOID_TYPE && !BLOCK_TYPES.has(grid[y][x]);
if (spawn && floor) {
  const seen = Array.from({ length: H }, () => Array(W).fill(false));
  const stack = [[spawn.x, spawn.y]]; seen[spawn.y][spawn.x] = true; let n = 0;
  while (stack.length) {
    const [x, y] = stack.pop(); n++;
    for (const [nx, ny] of [[x + 1, y], [x - 1, y], [x, y + 1], [x, y - 1]])
      if (isFloor(nx, ny) && !seen[ny][nx]) { seen[ny][nx] = true; stack.push([nx, ny]); }
  }
  reachable = n / floor;
}

// ---- brief checks (optional) ----
if (briefFile) {
  let B;
  try { B = JSON.parse(fs.readFileSync(briefFile, 'utf8')); } catch (e) { err(`cannot read brief: ${e.message}`); B = {}; }
  if (B.width && B.width !== W) err(`width ${W} differs from brief ${B.width}`);
  if (B.height && B.height !== H) err(`height ${H} differs from brief ${B.height}`);
  if (B.rock_coverage) {
    const [lo, hi] = B.rock_coverage;
    if (coverage < lo || coverage > hi) err(`block coverage ${(coverage * 100).toFixed(1)}% outside brief [${lo * 100}%, ${hi * 100}%]`);
  }
  if (B.reachable_floor_min !== undefined) {
    const m = B.reachable_floor_min;
    if (typeof m !== 'number' || m < 0 || m > 1) err('brief reachable_floor_min must be a number in [0, 1]');
    else if (reachable < m) err(`reachable floor ${(reachable * 100).toFixed(1)}% below brief minimum ${(m * 100).toFixed(1)}%`);
  }
  for (const [t, range] of Object.entries(B.counts || {})) {
    const n = counts[t] || 0;
    if (n < range[0] || n > range[1]) err(`${t} count ${n} outside brief [${range[0]}, ${range[1]}]`);
  }
}

report();

function report() {
  if (grid) {
    console.log(`layout ${L && L.name} ${W}x${H} — legend: # rock, o ore, + heal_ore, * special_ore, B barrel, S spawn, . floor, (space) void`);
    for (let y = 0; y < H; y++) console.log(grid[y].map((t) => (t ? GLYPH[t] : '.')).join(''));
    console.log(`counts: ${JSON.stringify(counts)}; block coverage ${(coverage * 100).toFixed(1)}% of non-void cells; reachable floor ${(reachable * 100).toFixed(1)}%`);
  }
  if (errors.length) { console.log(`FAIL: ${errors.length} error(s)`); for (const e of errors) console.log(` - ${e}`); process.exit(1); }
  console.log('OK'); process.exit(0);
}
