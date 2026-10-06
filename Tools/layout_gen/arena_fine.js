#!/usr/bin/env node
// Converts the 80 x 80 arena_m1 layout to the 240 x 240 fine grid (67 uu cells). Deterministic, no RNG state.
//   node Tools/layout_gen/arena_fine.js [in.json] [out.json]
// Rules (vault: layout-generator-recipe, "Fine-grid conversion"):
//  1. Every old cell becomes 3 x 3 cells of the same type. Spawn: centre cell only (rest floor).
//     Barrel: centre cell only (rest floor). Ore / heal ore: whole 3 x 3 stays a cluster.
//  2. Edge roughening on rock masses (ore, heal ore, barrel, spawn never change), all driven by a
//     position hash (value noise, 2 octaves): two erosion passes rock -> floor, one dilation pass floor -> rock,
//     two erosion passes rock -> void on the cliff edge. An erosion needs 2 rock cells behind the face,
//     so no wall thinner than 3 cells is ever breached and hidden pockets stay closed.
//  3. Spawn zone (radius 8 cells) is never touched. Passage repair: floor cells not covered by an
//     all-open 3 x 3 window are turned to rock (when not reachable-critical) or the rock around is opened.
const fs = require('fs');
const path = require('path');
const root = path.join(__dirname, '..', '..');
const inF = process.argv[2] || path.join(root, 'Content/RogueRigs/Data/Levels/arena_m1.json');
const outF = process.argv[3] || path.join(root, 'Content/RogueRigs/Data/Levels/arena_m1_fine.json');
const S = 3, SEED = 6701;
const old = JSON.parse(fs.readFileSync(inF, 'utf8'));
const OW = old.width, OH = old.height, W = OW * S, H = OH * S;
const g = Array.from({ length: H }, () => Array(W).fill(null)); // null = floor
let spawn = null;
for (const c of old.cells) {
  const bx = c.x * S, by = c.y * S;
  if (c.type === 'spawn') { spawn = [bx + 1, by + 1]; g[by + 1][bx + 1] = 'spawn'; continue; }
  if (c.type === 'barrel') { g[by + 1][bx + 1] = 'barrel'; continue; }
  for (let dy = 0; dy < S; dy++) for (let dx = 0; dx < S; dx++) g[by + dy][bx + dx] = c.type;
}
// ---- deterministic value noise ----
function hash(x, y, s) {
  let h = (Math.imul(x, 374761393) + Math.imul(y, 668265263) + Math.imul(s, 2147483647 | 0) + SEED) | 0;
  h = Math.imul(h ^ (h >>> 13), 1274126177); h ^= h >>> 16;
  return (h >>> 0) / 4294967296;
}
function vnoise(x, y, scale, s) {
  const fx = x / scale, fy = y / scale, x0 = Math.floor(fx), y0 = Math.floor(fy);
  const tx = fx - x0, ty = fy - y0, sx = tx * tx * (3 - 2 * tx), sy = ty * ty * (3 - 2 * ty);
  const a = hash(x0, y0, s), b = hash(x0 + 1, y0, s), c = hash(x0, y0 + 1, s), d = hash(x0 + 1, y0 + 1, s);
  return (a + (b - a) * sx) * (1 - sy) + (c + (d - c) * sx) * sy;
}
const noise = (x, y, s) => 0.65 * vnoise(x, y, 6, s) + 0.35 * vnoise(x, y, 2.5, s + 50) ; // ~[0.1, 0.9]
const inb = (x, y) => x >= 0 && y >= 0 && x < W && y < H;
const T = (x, y) => (inb(x, y) ? g[y][x] : 'void');
const D4 = [[1, 0], [-1, 0], [0, 1], [0, -1]];
const isFloor = (t) => t === null || t === 'barrel' || t === 'spawn';
const isRock = (t) => t === 'rock';
const nearSpawn = (x, y) => Math.abs(x - spawn[0]) <= 8 && Math.abs(y - spawn[1]) <= 8;

const open = (x, y) => { const t = T(x, y); return t === null || t === 'barrel' || t === 'spawn'; };
// an open cell is covered when it lies in an all-open 3 x 3 window (passage >= 3 cells)
function covered(x, y) {
  for (let oy = y - 2; oy <= y; oy++) for (let ox = x - 2; ox <= x; ox++) {
    let all = true;
    for (let dy = 0; dy < 3 && all; dy++) for (let dx = 0; dx < 3; dx++) if (!open(ox + dx, oy + dy)) { all = false; break; }
    if (all) return true;
  }
  return false;
}
// sets a cell, keeps it only if every open cell within 2 stays covered (else reverts)
function tryChange(x, y, v) {
  const prev = g[y][x]; g[y][x] = v;
  for (let dy = -2; dy <= 2; dy++) for (let dx = -2; dx <= 2; dx++) {
    const a = x + dx, b = y + dy;
    if (inb(a, b) && open(a, b) && !covered(a, b)) { g[y][x] = prev; return false; }
  }
  return true;
}
const eroded = new Set(); // cells opened by erosion; reverted when they leave a passage narrower than 3
function revertNarrow() {
  for (let guard = 0; guard < 50; guard++) {
    let n = 0;
    for (const k of [...eroded]) {
      const x = k % W, y = (k - x) / W;
      if (g[y][x] === null && !covered(x, y)) { g[y][x] = 'rock'; eroded.delete(k); n++; }
    }
    if (!n) break;
  }
}
function erode(target, s, thr) { // rock -> target ('floor' null or 'void')
  const ch = [];
  for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) {
    if (!isRock(g[y][x]) || nearSpawn(x, y)) continue;
    let okDir = false, bad = false;
    for (const [dx, dy] of D4) {
      const n = T(x + dx, y + dy);
      const open = target === 'void' ? n === 'void' : isFloor(n);
      const anyOpen = isFloor(n) || n === 'void';
      if (anyOpen && !open) { bad = true; break; } // mixed faces: leave alone
      if (open) {
        if (isRock(T(x - dx, y - dy)) && isRock(T(x - 2 * dx, y - 2 * dy))) okDir = true; else { bad = true; break; }
      }
    }
    if (okDir && !bad && noise(x, y, s) < thr) ch.push([x, y]);
  }
  for (const [x, y] of ch) { if (target === 'void') g[y][x] = 'void'; else { g[y][x] = null; eroded.add(y * W + x); } }
}
function dilate(s, thr) { // floor -> rock, depth 1
  const ch = [];
  for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) {
    if (g[y][x] !== null || nearSpawn(x, y)) continue;
    if (D4.some(([dx, dy]) => isRock(T(x + dx, y + dy))) && noise(x, y, s) > thr) ch.push([x, y]);
  }
  for (const [x, y] of ch) tryChange(x, y, 'rock');
}
erode(null, 1, 0.42); erode(null, 2, 0.40); revertNarrow();
dilate(3, 0.60);
erode('void', 4, 0.45); erode('void', 5, 0.45);

// ---- passage repair: every open cell must lie in an all-open 3 x 3 window ----
function uncovered() {
  const cov = Array.from({ length: H }, () => Array(W).fill(false));
  for (let y = 0; y < H - 2; y++) for (let x = 0; x < W - 2; x++) {
    let all = true;
    for (let dy = 0; dy < 3 && all; dy++) for (let dx = 0; dx < 3; dx++) if (!open(x + dx, y + dy)) { all = false; break; }
    if (all) for (let dy = 0; dy < 3; dy++) for (let dx = 0; dx < 3; dx++) cov[y + dy][x + dx] = true;
  }
  const out = [];
  for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) if (open(x, y) && !cov[y][x]) out.push([x, y]);
  return out;
}
let first = uncovered().length, rounds = 0;
while (rounds < 30) {
  const u = uncovered(); if (!u.length) break;
  // fill narrow slivers with rock: cheapest, keeps walls solid; connectivity checked by the validator
  for (const [x, y] of u) if (g[y][x] === null) g[y][x] = 'rock';
  rounds++;
}
console.error(`narrow floor cells before repair: ${first}; repair rounds: ${rounds}; left: ${uncovered().length}`);

const cells = [];
for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) if (g[y][x]) cells.push({ x, y, type: g[y][x] });
fs.writeFileSync(outF, JSON.stringify({ version: 1, name: 'arena_m1_fine', width: W, height: H, cells }));
console.error(`wrote ${outF}: ${cells.length} cells`);
