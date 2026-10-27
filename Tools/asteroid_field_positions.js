// task-334-04: deterministic positions of the BP_AsteroidField asteroids from the layout void cells.
// Usage: node Tools/asteroid_field_positions.js [layout name, default arena_m1_fine] [--json out.json]
// Cell (x, y) spans [x*67, (x+1)*67] in world X/Y (grid actor at the origin). Floor cell = any non-void cell.
const fs = require('fs');
const path = require('path');

const CELL = 67;
const SEED = 33404;
const COUNT = 12;
const MESH_HALF = [ // bounds half extents of SM_Asteroid_01..04 (uu at scale 1)
  [50, 37.877, 45.565], [50, 43.566, 43.980], [48.674, 50, 39.791], [50, 31.345, 30.670],
];
const TOP_LIMIT = -300;     // every asteroid top (any rotation) stays below this Z
const DEEPEST_TOP = -2500;  // deepest top
const D_MIN = 300, D_MAX = 3000; // distance from centre to the nearest floor cell, uu

function rng(seed) { // mulberry32
  let a = seed >>> 0;
  return () => { a = (a + 0x6D2B79F5) >>> 0; let t = a; t = Math.imul(t ^ (t >>> 15), t | 1); t ^= t + Math.imul(t ^ (t >>> 7), t | 61); return ((t ^ (t >>> 14)) >>> 0) / 4294967296; };
}

function loadLayout(name) {
  const file = path.join(__dirname, '..', 'Content', 'RogueRigs', 'Data', 'Levels', name + '.json');
  const l = JSON.parse(fs.readFileSync(file, 'utf8'));
  const void_ = new Uint8Array(l.width * l.height);
  for (const c of l.cells) if (c.type === 'void') void_[c.y * l.width + c.x] = 1;
  return { w: l.width, h: l.height, void_ };
}

function floorBoundary(L) { // floor cells touching a void cell or the grid border
  const pts = [];
  for (let y = 0; y < L.h; y++) for (let x = 0; x < L.w; x++) {
    if (L.void_[y * L.w + x]) continue;
    let edge = false;
    for (const [dx, dy] of [[1, 0], [-1, 0], [0, 1], [0, -1]]) {
      const nx = x + dx, ny = y + dy;
      if (nx < 0 || ny < 0 || nx >= L.w || ny >= L.h || L.void_[ny * L.w + nx]) edge = true;
    }
    if (edge) pts.push([x, y]);
  }
  return pts;
}

// distance (uu) from a world point to the nearest floor cell rectangle
function distToFloor(px, py, bnd) {
  let best = Infinity;
  for (const [x, y] of bnd) {
    const dx = Math.max(x * CELL - px, 0, px - (x + 1) * CELL);
    const dy = Math.max(y * CELL - py, 0, py - (y + 1) * CELL);
    const d = Math.hypot(dx, dy);
    if (d < best) best = d;
  }
  return best;
}

function cellAt(L, px, py) {
  const x = Math.floor(px / CELL), y = Math.floor(py / CELL);
  if (x < 0 || y < 0 || x >= L.w || y >= L.h) return null;
  return { x, y, void: !!L.void_[y * L.w + x] };
}

function compute(layoutName) {
  const L = loadLayout(layoutName);
  const bnd = floorBoundary(L);
  const rand = rng(SEED);
  const cx = (L.w * CELL) / 2, cy = (L.h * CELL) / 2;
  // scales: 2 to 12 (200 to 1200 uu across); larger = deeper
  const scales = [2, 2.5, 3, 3.5, 4.5, 5, 6, 7, 8, 9.5, 11, 12];
  const items = [];
  for (let k = 0; k < COUNT; k++) {
    const s = scales[k];
    const mesh = Math.floor(rand() * 4);
    const hx = MESH_HALF[mesh];
    const R = Math.hypot(hx[0], hx[1], hx[2]) * s; // bounding sphere radius: safe for any rotation
    // angular sector k around the arena centre, with jitter
    const sector = (2 * Math.PI) / COUNT;
    const sectorOrder = (k * 5) % COUNT; // sizes interleave around the edge instead of growing in a ring
    const a0 = sectorOrder * sector;
    let placed = null;
    for (let tries = 0; tries < 20000 && !placed; tries++) {
      const ang = a0 + rand() * sector * 0.8 + sector * 0.1;
      const r = rand() * Math.hypot(cx, cy) * 1.15;
      const px = cx + Math.cos(ang) * r, py = cy + Math.sin(ang) * r;
      const c = cellAt(L, px, py);
      if (!c || !c.void) continue;
      const dMin = Math.max(D_MIN, 0.6 * R + 100);
      const d = distToFloor(px, py, bnd);
      if (d < dMin || d > D_MAX) continue;
      // also keep a gap to the other asteroids
      if (items.some((o) => Math.hypot(o.x - px, o.y - py) < o.R + R + 150)) continue;
      placed = { x: px, y: py, d, ang };
    }
    if (!placed) throw new Error('no position for asteroid ' + k);
    // top Z: -300 (smallest) down to -2500 (largest), by size rank, plus jitter
    const t = (s - 2) / 10;
    let top = TOP_LIMIT - 100 - t * (-DEEPEST_TOP + TOP_LIMIT + 100 - 100) + (rand() - 0.5) * 120;
    top = Math.min(top, TOP_LIMIT - 60);
    top = Math.max(top, DEEPEST_TOP);
    const z = top - R; // so that z + R = top for any rotation
    // rotation (deg/s): random unit axis scaled to 1..5 deg/s; initial rotation random
    const mag = 1 + rand() * 4;
    let ax = [rand() - 0.5, rand() - 0.5, rand() - 0.5];
    const n = Math.hypot(...ax) || 1; ax = ax.map((v) => v / n);
    const rate = { pitch: +(ax[0] * mag).toFixed(2), yaw: +(ax[1] * mag).toFixed(2), roll: +(ax[2] * mag).toFixed(2) };
    const rot = { pitch: Math.round(rand() * 360 - 180), yaw: Math.round(rand() * 360 - 180), roll: Math.round(rand() * 360 - 180) };
    items.push({ index: k + 1, mesh: mesh + 1, scale: s, R: +R.toFixed(1), x: +placed.x.toFixed(1), y: +placed.y.toFixed(1), z: +z.toFixed(1), top: +(z + R).toFixed(1), distFloor: +placed.d.toFixed(1), rot, rate });
  }
  return { L, bnd, items };
}

if (require.main === module) {
  const name = process.argv[2] && !process.argv[2].startsWith('--') ? process.argv[2] : 'arena_m1_fine';
  const { L, bnd, items } = compute(name);
  const out = process.argv.indexOf('--json');
  if (out > 0) fs.writeFileSync(process.argv[out + 1], JSON.stringify(items, null, 1));
  console.log('layout', name, 'boundary floor cells', bnd.length);
  for (const i of items) console.log(JSON.stringify(i));
  // check against the other run layouts (info): centre over void there too?
  for (const other of ['arena_m1', 'arena_l2', 'arena_l3', 'arena_l4']) {
    if (other === name) continue;
    try {
      const O = loadLayout(other); const ob = floorBoundary(O);
      const res = items.map((i) => { const c = cellAt(O, i.x, i.y); return (c && c.void) || !c ? 'v' : 'F'; }).join('');
      console.log('info', other, O.w + 'x' + O.h, res);
    } catch (e) { console.log('info', other, e.message); }
  }
}
module.exports = { compute, loadLayout, floorBoundary, cellAt, distToFloor, CELL };
