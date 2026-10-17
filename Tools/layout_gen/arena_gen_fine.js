#!/usr/bin/env node
// Direct 240 x 240 layout generator (67 uu cells): the layout-generator-recipe run natively on the fine grid
// instead of 80 x 80 upscaled 3 x. Deterministic: same params + seed => byte-identical output (seeded LCG, position hash).
//   node Tools/layout_gen/arena_gen_fine.js --params <file> --seed <n> --name <layout name> --out <dir> [--brief <brief.json>]
// With --brief: seeds are tried from --seed upward (params.search.max_seeds) until the recipe checks and the brief
// bounds pass; the chosen seed is printed. Writes <out>/<name>.json and, with --brief, <out>/<name>.brief.json.
// Steps (vault: layout-generator-recipe, "Direct 240 generation"): outline, notching, spawn clearing, main network of
// wander walks, hidden pockets, far-cell filler, cleanup (regions, rock islands, 3-cell passage repair), heal veins 2 x 2,
// barrels, seed search. Every tunable value lives in the params file.
'use strict';
const fs = require('fs');
const path = require('path');

// ---- CLI ----
function arg(name, def) { const i = process.argv.indexOf('--' + name); return i >= 0 ? process.argv[i + 1] : def; }
const paramsFile = arg('params', path.join(__dirname, 'params', 'fine_default.json'));
const seed0 = parseInt(arg('seed', '1'), 10);
const name = arg('name', 'arena_fine_gen');
const outDir = arg('out', null);
const briefFile = arg('brief', null);
if (!outDir || !Number.isInteger(seed0) || !/^[a-z0-9_]+$/.test(name)) {
  console.log('usage: node Tools/layout_gen/arena_gen_fine.js --params <file> --seed <n> --name <snake_case> --out <dir> [--brief <brief.json>]');
  process.exit(2);
}
const P = JSON.parse(fs.readFileSync(paramsFile, 'utf8'));
const B = briefFile ? JSON.parse(fs.readFileSync(briefFile, 'utf8')) : null;

// cell types
const VOID = 0, ROCK = 1, FLOOR = 2, HEAL = 3, BARREL = 4, SPAWN = 5;
const TYPE_NAME = ['void', 'rock', null, 'heal_ore', 'barrel', 'spawn'];
const D4 = [[1, 0], [-1, 0], [0, 1], [0, -1]];

function build(SEED) {
  const W = P.width, H = P.height, N = W * H;
  const CX = (W - 1) / 2, CY = (H - 1) / 2, SX = W >> 1, SY = H >> 1; // outline centre, spawn cell
  let s = SEED >>> 0;
  const rnd = () => { s = (s * 1664525 + 1013904223) >>> 0; return s / 4294967296; };
  const gauss = () => (rnd() + rnd() + rnd() - 1.5) * 1.15;
  const NS = (Math.imul(SEED, 7919) + P.noise_seed) | 0;
  function hash(x, y, k) {
    let h = (Math.imul(x, 374761393) + Math.imul(y, 668265263) + Math.imul(k, 2147483647 | 0) + NS) | 0;
    h = Math.imul(h ^ (h >>> 13), 1274126177); h ^= h >>> 16;
    return (h >>> 0) / 4294967296;
  }
  function vnoise(x, y, scale, k) { // value noise in [0, 1]
    const fx = x / scale, fy = y / scale, x0 = Math.floor(fx), y0 = Math.floor(fy);
    const tx = fx - x0, ty = fy - y0, sx = tx * tx * (3 - 2 * tx), sy = ty * ty * (3 - 2 * ty);
    const a = hash(x0, y0, k), b = hash(x0 + 1, y0, k), c = hash(x0, y0 + 1, k), d = hash(x0 + 1, y0 + 1, k);
    return (a + (b - a) * sx) * (1 - sy) + (c + (d - c) * sx) * sy;
  }
  const g = new Uint8Array(N);
  const inb = (x, y) => x >= 0 && y >= 0 && x < W && y < H;
  const A = (x, y) => inb(x, y) && g[y * W + x] !== VOID;
  const T = (x, y) => (inb(x, y) ? g[y * W + x] : VOID);
  const isOpen = (t) => t === FLOOR || t === SPAWN || t === BARREL; // passage rule (validator)
  const walkable = (t) => t === FLOOR || t === SPAWN;

  // 1. outline: polar radius + low-amplitude value noise, clamped edge_margin cells from the grid edge
  const O = P.outline, em = O.edge_margin;
  for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) {
    const dx = x - CX, dy = y - CY, th = Math.atan2(dy, dx), d = Math.hypot(dx, dy);
    let r = O.base;
    for (const [amp, f, ph] of O.harmonics) r += amp * Math.sin(f * th + ph);
    r += O.noise_amp * (2 * vnoise(x, y, O.noise_scale, 1) - 1);
    g[y * W + x] = d <= r && x >= em && x < W - em && y >= em && y < H - em ? ROCK : VOID;
  }
  // straight outline runs (cells with void on one side) longer than max_run
  function runs(maxRun) {
    const res = [];
    for (const [dx, dy] of D4) {
      const vert = dx !== 0, L = vert ? H : W, M = vert ? W : H;
      for (let a = 0; a < M; a++) {
        let run = [];
        for (let b = 0; b <= L; b++) {
          const x = vert ? a : b, y = vert ? b : a;
          if (b < L && A(x, y) && !A(x + dx, y + dy)) run.push([x, y]);
          else { if (run.length > maxRun) res.push({ run, dx, dy }); run = []; }
        }
      }
    }
    return res;
  }
  const NT = P.notch;
  for (let it = 0; it < NT.max_iter; it++) {
    const r = runs(NT.max_run); if (!r.length) break;
    const { run, dx, dy } = r[Math.floor(rnd() * r.length)];
    const m = Math.floor(run.length / 2) + Math.floor((rnd() - 0.5) * (run.length - NT.len_max));
    const n = NT.len_min + Math.floor(rnd() * (NT.len_max - NT.len_min + 1));
    for (let k = 0; k < n; k++) {
      const c = run[Math.max(0, Math.min(run.length - 1, m + k))];
      for (let dd = 0; dd < NT.depth; dd++) { const x = c[0] - dx * dd, y = c[1] - dy * dd; if (inb(x, y)) g[y * W + x] = VOID; }
    }
  }
  // drop any asteroid fragment cut off by notching (keep the piece holding the spawn)
  {
    const seen = new Uint8Array(N), st = [SY * W + SX]; seen[st[0]] = 1;
    while (st.length) { const i = st.pop(), x = i % W, y = (i - x) / W; for (const [a, b] of D4) { const X = x + a, Y = y + b, j = Y * W + X; if (A(X, Y) && !seen[j]) { seen[j] = 1; st.push(j); } } }
    for (let i = 0; i < N; i++) if (g[i] !== VOID && !seen[i]) g[i] = VOID;
  }
  // safe[i]: no void within void_margin (Chebyshev); the outline never changes after this point
  const WK = P.walk, vm = WK.void_margin;
  const safe = new Uint8Array(N);
  for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) {
    let ok = 1;
    for (let j = -vm; j <= vm && ok; j++) for (let i = -vm; i <= vm; i++) if (!A(x + i, y + j)) { ok = 0; break; }
    safe[y * W + x] = ok;
  }
  const isSafe = (x, y) => inb(x, y) && safe[y * W + x] === 1;
  let total = 0; for (let i = 0; i < N; i++) if (g[i] !== VOID) total++;

  // hidden pocket guard: cells within pockets.guard of a pocket never get carved by later walks
  const hid = new Uint8Array(N), hidGuard = new Uint8Array(N);
  function markHidden(cells) {
    const gd = P.pockets.guard;
    for (const i of cells) {
      hid[i] = 1; const x = i % W, y = (i - x) / W;
      for (let j = -gd; j <= gd; j++) for (let k = -gd; k <= gd; k++) if (inb(x + k, y + j)) hidGuard[(y + j) * W + x + k] = 1;
    }
  }
  // wander walk: returns carved cell indices (filtered: safe from void, not near a hidden pocket)
  const stamp = new Uint32Array(N); let walkId = 0;
  function walk(x, y, ang, len, rho0, turn) {
    walkId++; const cells = []; let rho = rho0, a = ang;
    const amp = WK.brush_noise_amp;
    for (let k = 0; k < len; k++) {
      rho = Math.max(WK.brush_min, Math.min(WK.brush_max, rho + gauss() * WK.brush_drift));
      const R = Math.ceil(rho + amp) + 1, rx = Math.round(x), ry = Math.round(y);
      for (let j = -R; j <= R; j++) for (let i = -R; i <= R; i++) {
        const cx = rx + i, cy = ry + j; if (!A(cx, cy)) continue;
        const id = cy * W + cx; if (stamp[id] === walkId) continue;
        if (Math.hypot(cx - x, cy - y) <= rho + amp * (2 * vnoise(cx, cy, WK.brush_noise_scale, 2) - 1)) { stamp[id] = walkId; cells.push(id); }
      }
      a += gauss() * turn + (rnd() < WK.sharp_chance ? (rnd() < 0.5 ? 1 : -1) * (WK.sharp_min + rnd() * (WK.sharp_max - WK.sharp_min)) : 0);
      x += Math.cos(a) * WK.step; y += Math.sin(a) * WK.step;
      if (!isSafe(Math.round(x), Math.round(y))) break;
    }
    return cells.filter((i) => safe[i] && !hidGuard[i]);
  }
  let floorN = 0;
  const apply = (cs) => { for (const i of cs) if (g[i] === ROCK) { g[i] = FLOOR; floorN++; } };

  // 2. spawn clearing: wobbly disc + the untouched zone square (2 zone_half + 1 cells)
  const SPn = P.spawn, zh = SPn.zone_half;
  for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) {
    const i = y * W + x;
    if (g[i] === ROCK && Math.hypot(x - SX, y - SY) <= SPn.disc_radius + SPn.disc_wobble * Math.sin(x * SPn.disc_freq_x + y * SPn.disc_freq_y)) { g[i] = FLOOR; floorN++; }
  }
  for (let y = SY - zh; y <= SY + zh; y++) for (let x = SX - zh; x <= SX + zh; x++) { const i = y * W + x; if (g[i] !== FLOOR) floorN++; g[i] = FLOOR; }
  const inZone = (x, y, h) => Math.abs(x - SX) <= h && Math.abs(y - SY) <= h;

  // 3. main network
  const MN = P.main;
  for (let guard = 0; floorN < MN.floor_share * total && guard < MN.max_walks; guard++) {
    const f = []; for (let i = 0; i < N; i++) if (g[i] === FLOOR) f.push(i);
    const st = f[Math.floor(rnd() * f.length)];
    apply(walk(st % W, Math.floor(st / W), rnd() * 6.283, MN.steps_min + Math.floor(rnd() * (MN.steps_max - MN.steps_min)),
      MN.rho0_min + rnd() * (MN.rho0_max - MN.rho0_min), WK.turn));
  }
  // reachable set from the spawn (walkable cells, 4-connected)
  function reach() {
    const rs = new Uint8Array(N), st = [SY * W + SX]; rs[st[0]] = 1; let n = 1;
    while (st.length) { const i = st.pop(), x = i % W, y = (i - x) / W; for (const [a, b] of D4) { const X = x + a, Y = y + b; if (!inb(X, Y)) continue; const j = Y * W + X; if (!rs[j] && walkable(g[j])) { rs[j] = 1; n++; st.push(j); } } }
    rs.count = n; return rs;
  }
  // BFS distance (4-connected, through any non-void cell) from a source mask
  function bfs(src) {
    const d = new Int32Array(N).fill(1 << 20), q = new Int32Array(N); let qh = 0, qt = 0;
    for (let i = 0; i < N; i++) if (src(i)) { d[i] = 0; q[qt++] = i; }
    while (qh < qt) { const i = q[qh++], x = i % W, y = (i - x) / W; for (const [a, b] of D4) { const X = x + a, Y = y + b; if (!A(X, Y)) continue; const j = Y * W + X; if (d[j] > d[i] + 1) { d[j] = d[i] + 1; q[qt++] = j; } } }
    return d;
  }

  // 4. hidden pockets
  const PK = P.pockets;
  let rs = reach();
  const mainD = bfs((i) => rs[i] === 1);
  let pockets = 0;
  for (let t = 0; t < PK.tries && pockets < PK.count; t++) {
    const x = PK.margin + rnd() * (W - 2 * PK.margin), y = PK.margin + rnd() * (H - 2 * PK.margin);
    const rx = Math.round(x), ry = Math.round(y);
    if (!inb(rx, ry) || g[ry * W + rx] !== ROCK) continue;
    const cs = walk(x, y, rnd() * 6.283, PK.steps_base + Math.floor(rnd() * PK.steps_rand), PK.rho0_min + rnd() * (PK.rho0_max - PK.rho0_min), WK.turn + PK.turn_add);
    if (cs.length < PK.min_cells) continue;
    let md = 1 << 20, ok = true;
    for (const i of cs) { if (g[i] !== ROCK || mainD[i] < PK.dist_min) { ok = false; break; } md = Math.min(md, mainD[i]); }
    if (!ok || md > PK.dist_max) continue;
    for (const i of cs) g[i] = FLOOR; floorN += cs.length; markHidden(cs); pockets++;
  }

  // 5. far-cell filler: carve toward any cell more than maxd from open floor
  const FL = P.filler;
  function filler() { // returns the number of walks carved
    let it = 0;
    for (; it < FL.max_iter; it++) {
      const d = bfs((i) => walkable(g[i]));
      const far = []; for (let i = 0; i < N; i++) if (g[i] !== VOID && d[i] > FL.maxd) far.push(i);
      if (!far.length) break;
      const c = far[Math.floor(rnd() * far.length)], cx = c % W, cy = (c - cx) / W;
      const r2 = reach(); let best = -1, bd = 1e9;
      for (let i = 0; i < N; i++) if (r2[i]) { const x = i % W, y = (i - x) / W, dd = Math.hypot(x - cx, y - cy); if (dd < bd) { bd = dd; best = i; } }
      const bx = best % W, by = (best - bx) / W;
      apply(walk(bx, by, Math.atan2(cy - by, cx - bx) + gauss() * FL.aim_noise, Math.ceil(bd) + FL.extra_min + Math.floor(rnd() * FL.extra_rand),
        FL.rho0_min + rnd() * (FL.rho0_max - FL.rho0_min), WK.turn * FL.turn_scale));
    }
    return it;
  }
  filler();

  // 6. cleanup
  const CL = P.cleanup;
  // unreachable walkable regions with their gap (rock cells) to the main network
  function regions() {
    const r = reach(), d = bfs((i) => r[i] === 1), lab = new Int32Array(N).fill(-1), out = [];
    for (let i0 = 0; i0 < N; i0++) {
      if (!walkable(g[i0]) || r[i0] || lab[i0] >= 0) continue;
      const cells = [i0]; lab[i0] = out.length; let md = 1 << 20;
      for (let h = 0; h < cells.length; h++) {
        const i = cells[h], x = i % W, y = (i - x) / W; md = Math.min(md, d[i]);
        for (const [a, b] of D4) { const X = x + a, Y = y + b; if (!inb(X, Y)) continue; const j = Y * W + X; if (walkable(g[j]) && !r[j] && lab[j] < 0) { lab[j] = out.length; cells.push(j); } }
      }
      out.push({ cells, gap: md - 1 });
    }
    return { reach: r, regs: out };
  }
  function refillRegions() {
    for (const rg of regions().regs) if (rg.cells.length < CL.min_region || rg.gap > CL.max_gap) for (const i of rg.cells) { g[i] = ROCK; hid[i] = 0; floorN--; }
  }
  function rockIslands() { // rock components of <= max_rock_island cells that touch no void become floor
    const lab = new Uint8Array(N);
    for (let i0 = 0; i0 < N; i0++) {
      if (g[i0] !== ROCK || lab[i0]) continue;
      const cells = [i0]; lab[i0] = 1; let voidTouch = false;
      for (let h = 0; h < cells.length && cells.length <= CL.max_rock_island; h++) {
        const i = cells[h], x = i % W, y = (i - x) / W;
        for (const [a, b] of D4) { const X = x + a, Y = y + b; if (!A(X, Y)) { voidTouch = true; continue; } const j = Y * W + X; if (g[j] === ROCK && !lab[j]) { lab[j] = 1; cells.push(j); } }
      }
      if (!voidTouch && cells.length <= CL.max_rock_island) for (const i of cells) { g[i] = FLOOR; floorN++; }
    }
  }
  // cells covered by an all-open k x k window (k = cleanup.passage)
  function coveredMap() {
    const k = CL.passage, sat = new Int32Array((W + 1) * (H + 1)), W1 = W + 1, cov = new Uint8Array(N);
    for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) sat[(y + 1) * W1 + x + 1] = sat[y * W1 + x + 1] + sat[(y + 1) * W1 + x] - sat[y * W1 + x] + (isOpen(g[y * W + x]) ? 1 : 0);
    for (let y = 0; y + k <= H; y++) for (let x = 0; x + k <= W; x++)
      if (sat[(y + k) * W1 + x + k] - sat[y * W1 + x + k] - sat[(y + k) * W1 + x] + sat[y * W1 + x] === k * k)
        for (let dy = 0; dy < k; dy++) for (let dx = 0; dx < k; dx++) cov[(y + dy) * W + x + dx] = 1;
    return cov;
  }
  function passageRepair() { // narrow floor slivers become rock until every open cell is covered
    let total = 0;
    for (let rounds = 0; rounds < CL.repair_rounds; rounds++) {
      const cov = coveredMap(); let n = 0;
      for (let i = 0; i < N; i++) if (g[i] === FLOOR && !cov[i]) { g[i] = ROCK; hid[i] = 0; floorN--; n++; }
      total += n; if (!n) break;
    }
    return total; // floor cells turned to rock
  }
    // thin outline peninsulas (inside void_margin, so no walk can reach them) farther than maxd from floor become void;
  // every remaining cell keeps a shorter path to floor, so the asteroid stays one piece
  function trimFar() {
    const d = bfs((i) => walkable(g[i]));
    for (let i = 0; i < N; i++) if (g[i] === ROCK && d[i] > FL.maxd && !safe[i]) { g[i] = VOID; total--; }
  }
  refillRegions(); filler(); rockIslands();
  for (let k = 0; k < CL.loops; k++) { const r = passageRepair(); refillRegions(); if (!filler() && !r) break; }
  passageRepair(); refillRegions(); trimFar(); // always end on a repair: the 3-cell rule is a hard rule, maxd is checked by the search
  g[SY * W + SX] = SPAWN;
  return { W, H, N, g, A, T, rnd, total, pockets, reach, regions, bfs, runs, coveredMap, isOpen, walkable, inZone, SX, SY };
}

function measure(o) {
  const { W, H, N, g, A } = o;
  let blocks = 0, floor = 0;
  for (let i = 0; i < N; i++) { const t = g[i]; if (t === ROCK || t === HEAL || t === BARREL) blocks++; else if (t !== VOID) floor++; }
  const { reach, regs } = o.regions();
  // pieces of the asteroid
  const seen = new Uint8Array(N); let pieces = 0;
  for (let i0 = 0; i0 < N; i0++) {
    if (g[i0] === VOID || seen[i0]) continue; pieces++; const st = [i0]; seen[i0] = 1;
    while (st.length) { const i = st.pop(), x = i % W, y = (i - x) / W; for (const [a, b] of D4) { const X = x + a, Y = y + b; if (!A(X, Y)) continue; const j = Y * W + X; if (!seen[j]) { seen[j] = 1; st.push(j); } } }
  }
  const d = o.bfs((i) => o.walkable(g[i])); let maxd = 0; for (let i = 0; i < N; i++) if (g[i] !== VOID) maxd = Math.max(maxd, d[i]);
  const cov = o.coveredMap(); let narrow = 0; for (let i = 0; i < N; i++) if (o.isOpen(g[i]) && !cov[i]) narrow++;
  const S = P.search;
  const goodPockets = regs.filter((r) => r.cells.length >= P.cleanup.min_region && r.gap >= 1 && r.gap <= P.cleanup.max_gap).length;
  const outline = Math.max(0, ...o.runs(0).map((r) => r.run.length));
  const counts = {}; for (let i = 0; i < N; i++) { const n = TYPE_NAME[g[i]]; if (n) counts[n] = (counts[n] || 0) + 1; }
  return { total: o.total, blocks, cov: blocks / o.total, reach: reach.count / floor, pieces, maxd, narrow, pockets: goodPockets, regions: regs.length, outline, counts, maxRun: S.max_outline_run };
}

// 7. heal veins (vein x vein blocks) and barrels; returns false when the quotas cannot be met
function decorate(o) {
  const { W, H, N, g, rnd, SX, SY } = o; const HL = P.heal, BR = P.barrels, v = HL.vein;
  const { reach: rs } = o.regions();
  const T = (x, y) => o.T(x, y);
  const perim = []; // 4-neighbours of a v x v block (relative)
  for (let k = 0; k < v; k++) perim.push([k, -1], [k, v], [-1, k], [v, k]);
  const blockCells = (x, y) => { const c = []; for (let j = 0; j < v; j++) for (let i = 0; i < v; i++) c.push((y + j) * W + x + i); return c; };
  const healNear = (x, y, m) => { for (let j = -m; j < v + m; j++) for (let i = -m; i < v + m; i++) if (T(x + i, y + j) === HEAL) return true; return false; };
  function coveredLocal(x, y) { // open cell (x,y) lies in an all-open 3 x 3 window
    for (let oy = y - 2; oy <= y; oy++) for (let ox = x - 2; ox <= x; ox++) {
      let all = true;
      for (let dy = 0; dy < 3 && all; dy++) for (let dx = 0; dx < 3; dx++) if (!o.isOpen(T(ox + dx, oy + dy))) { all = false; break; }
      if (all) return true;
    }
    return false;
  }
  function validWall(x, y) {
    if (o.inZone(x, y, HL.exclude_half) || o.inZone(x + v - 1, y + v - 1, HL.exclude_half) || o.inZone(x, y + v - 1, HL.exclude_half) || o.inZone(x + v - 1, y, HL.exclude_half)) return false;
    for (const i of blockCells(x, y)) if (g[i] !== ROCK) return false;
    if (healNear(x, y, 1)) return false;
    let rock = 0, touch = false;
    for (const [a, b] of perim) { const t = T(x + a, y + b); if (t === ROCK) rock++; if (o.walkable(t) && rs[(y + b) * W + x + a]) touch = true; }
    return touch && rock >= HL.wall_min_rock;
  }
  function validPocket(x, y) {
    if (o.inZone(x, y, HL.exclude_half) || o.inZone(x + v - 1, y + v - 1, HL.exclude_half)) return false;
    const bc = blockCells(x, y);
    for (const i of bc) if (g[i] !== FLOOR || rs[i]) return false;
    if (healNear(x, y, 1)) return false;
    let rock = 0; for (const [a, b] of perim) if (T(x + a, y + b) === ROCK) rock++;
    if (rock < HL.pocket_min_rock) return false;
    for (const i of bc) g[i] = HEAL; // tentative: every open cell within 3 must stay covered
    let ok = true;
    for (let j = -3; j < v + 3 && ok; j++) for (let i = -3; i < v + 3; i++) if (o.isOpen(T(x + i, y + j)) && !coveredLocal(x + i, y + j)) { ok = false; break; }
    for (const i of bc) g[i] = FLOOR;
    return ok;
  }
  const picked = [];
  const farFrom = (list, x, y) => { let m = 1e9; for (const [a, b] of list) m = Math.min(m, Math.hypot(a - x, b - y)); return m; };
  function pick(cands, n, valid, list, place) {
    for (let k = 0; k < n; k++) {
      let best = null, bd = -1;
      for (let t = 0; t < P.heal.tries && cands.length; t++) {
        const c = cands[Math.floor(rnd() * cands.length)];
        if (!valid(c[0], c[1])) continue;
        const d = farFrom(list, c[0], c[1]); if (d > bd) { bd = d; best = c; }
      }
      if (!best) return false;
      place(best); list.push(best);
    }
    return true;
  }
  const wallC = [], pocketC = [];
  for (let y = 1; y < H - v; y++) for (let x = 1; x < W - v; x++) { if (validWall(x, y)) wallC.push([x, y]); if (validPocket(x, y)) pocketC.push([x, y]); }
  const placeHeal = ([x, y]) => { for (const i of blockCells(x, y)) g[i] = HEAL; };
  if (!pick(pocketC, HL.pocket_veins, validPocket, picked, placeHeal)) return false;
  if (!pick(wallC, HL.wall_veins, validWall, picked, placeHeal)) return false;

  // barrels: reachable floor cells against a rock wall (a barrel counts as open for the passage rule, so the
  // 3-cell windows are unchanged), away from the spawn and heal veins, never cutting the local floor ring; spread; some in pairs
  const rs2 = o.reach();
  const RING = [[-1, -1], [0, -1], [1, -1], [1, 0], [1, 1], [0, 1], [-1, 1], [-1, 0]];
  const validBarrel = (x, y) => {
    if (T(x, y) !== FLOOR || !rs2[y * W + x] || o.inZone(x, y, BR.spawn_exclude)) return false;
    if (!D4.some(([a, b]) => T(x + a, y + b) === ROCK)) return false;
    for (let j = -BR.heal_clear; j <= BR.heal_clear; j++) for (let i = -BR.heal_clear; i <= BR.heal_clear; i++) { const t = T(x + i, y + j); if (t === HEAL || (t === BARREL && Math.hypot(i, j) < BR.pair_dist_min)) return false; }
    // walkable ring cells must form one run, so no neighbour loses its link through this cell
    let segs = 0; for (let k = 0; k < 8; k++) { const p = RING[k], q = RING[(k + 7) % 8]; if (o.walkable(T(x + p[0], y + p[1])) && !o.walkable(T(x + q[0], y + q[1]))) segs++; }
    return segs === 1;
  };
  const bC = []; for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) if (validBarrel(x, y)) bC.push([x, y]);
  const nB = BR.min + Math.floor(rnd() * (BR.max - BR.min + 1));
  const nPairs = BR.pairs_min + Math.floor(rnd() * (BR.pairs_max - BR.pairs_min + 1));
  const barrels = [];
  const placeBarrel = ([x, y]) => { g[y * W + x] = BARREL; };
  const spread = (x, y) => validBarrel(x, y);
  for (let p = 0; p < nPairs; p++) {
    if (!pick(bC, 1, spread, barrels, placeBarrel)) return false;
    const [x0, y0] = barrels[barrels.length - 1]; const R = Math.ceil(BR.pair_dist_max); const opts = [];
    for (let j = -R; j <= R; j++) for (let i = -R; i <= R; i++) { const d = Math.hypot(i, j); if (d >= BR.pair_dist_min && d <= BR.pair_dist_max && validBarrel(x0 + i, y0 + j)) opts.push([x0 + i, y0 + j]); }
    if (!opts.length) return false;
    const c = opts[Math.floor(rnd() * opts.length)]; placeBarrel(c); barrels.push(c);
  }
  if (!pick(bC, nB - 2 * nPairs, spread, barrels, placeBarrel)) return false;
  return true;
}

function checks(m) {
  const S = P.search, fails = [];
  if (m.pieces !== S.pieces) fails.push('pieces ' + m.pieces);
  if (m.outline > S.max_outline_run) fails.push('outline run ' + m.outline);
  if (m.maxd > S.max_far) fails.push('far cell ' + m.maxd);
  if (m.total < S.asteroid_min || m.total > S.asteroid_max) fails.push('asteroid ' + m.total);
  if (m.reach < S.reach_min || m.reach > S.reach_max) fails.push('reach ' + m.reach.toFixed(3));
  if (m.pockets < S.min_pockets) fails.push('pockets ' + m.pockets);
  if (m.narrow) fails.push('narrow cells ' + m.narrow);
  if (B) {
    if (B.width && B.width !== P.width) fails.push('brief width');
    if (B.height && B.height !== P.height) fails.push('brief height');
    if (B.rock_coverage && (m.cov < B.rock_coverage[0] || m.cov > B.rock_coverage[1])) fails.push('coverage ' + m.cov.toFixed(3));
    if (B.reachable_floor_min !== undefined && m.reach < B.reachable_floor_min) fails.push('brief reach');
    for (const [t, [lo, hi]] of Object.entries(B.counts || {})) { const n = m.counts[t] || 0; if (n < lo || n > hi) fails.push(t + ' ' + n); }
  }
  return fails;
}

function run(seed) {
  const o = build(seed);
  const maxd = measure(o).maxd; // distance to floor is measured before heal veins and barrels (recipe)
  const decorated = decorate(o);
  const m = measure(o); m.maxd = maxd;
  const fails = decorated ? checks(m) : ['decoration quotas'];
  return { o, m, fails };
}

const maxSeeds = B ? P.search.max_seeds : 1;
let res = null;
for (let k = 0; k < maxSeeds; k++) {
  const seed = seed0 + k; const r = run(seed);
  const line = `seed ${seed}: asteroid ${r.m.total}, coverage ${r.m.cov.toFixed(3)}, reachable ${r.m.reach.toFixed(3)}, pockets ${r.m.pockets}, maxd ${r.m.maxd}, outline ${r.m.outline}, counts ${JSON.stringify(r.m.counts)}`;
  console.error(line + (r.fails.length ? ' -- FAIL: ' + r.fails.join(', ') : ' -- OK'));
  if (!B || !r.fails.length) { res = { seed, ...r }; break; }
}
if (!res) { console.log(`no seed in [${seed0}, ${seed0 + maxSeeds - 1}] passes`); process.exit(1); }
const { o } = res, cells = [];
for (let y = 0; y < o.H; y++) for (let x = 0; x < o.W; x++) { const t = TYPE_NAME[o.g[y * o.W + x]]; if (t) cells.push({ x, y, type: t }); }
fs.mkdirSync(outDir, { recursive: true });
const outF = path.join(outDir, name + '.json');
fs.writeFileSync(outF, JSON.stringify({ version: 1, name, width: o.W, height: o.H, cells }));
if (B) fs.writeFileSync(path.join(outDir, name + '.brief.json'), JSON.stringify(B, null, 1) + '\n');
console.log(`seed ${res.seed}`);
console.error(`wrote ${outF}: ${cells.length} cells`);
process.exit(B || !res.fails.length ? 0 : 1);
