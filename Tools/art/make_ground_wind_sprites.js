// Ground wind sprites of the extractor (task-466-01), redrawn after the owner sheet
// Art/Source/extractor-fx/reference/owner-ground-wind-sheet-2026-11-06.webp (its checker is painted, so nothing is cut from it).
// Usage: node Tools/art/make_ground_wind_sprites.js [outDir]   (default Art/Source/extractor-fx/ground-wind)
// Writes four RGBA PNGs with real alpha, warm whitish-beige, tinted further by the particle colour:
//   T_GroundWind_Ring.png    1024  flattened swirl ring (laid flat on the floor), soft painted edges
//   T_GroundWind_Disc.png     512  round radial dust disc, soft
//   T_GroundWind_Streaks.png 1024  2x2 atlas of curved wind streaks, long axis along V (texture Y), crisp
//   T_GroundWind_Puffs.png   1024  2x2 atlas of cartoon dust puffs with debris dots, crisp, flat two-tone shading
// RGB is filled everywhere (no dark fringe when filtered); the shape lives in alpha. Deterministic (SEED).
const fs = require('fs');
const path = require('path');
const { encodePng } = require('./ui_icon_prepare');

const OUT = process.argv[2] || path.join(__dirname, '..', '..', 'Art', 'Source', 'extractor-fx', 'ground-wind');
fs.mkdirSync(OUT, { recursive: true });

let st = 466010 >>> 0;
const rnd = () => { st = (st + 0x6D2B79F5) >>> 0; let t = st; t = Math.imul(t ^ (t >>> 15), t | 1); t ^= t + Math.imul(t ^ (t >>> 7), t | 61); return ((t ^ (t >>> 14)) >>> 0) / 4294967296; };
const rr = (a, b) => a + (b - a) * rnd();
const clamp01 = v => Math.max(0, Math.min(1, v));
const smooth = (a, b, x) => { const t = clamp01((x - a) / (b - a)); return t * t * (3 - 2 * t); };
const mix = (a, b, t) => a + (b - a) * t;

// sRGB 0..1 tones taken from the sheet: light top, base beige, shaded underside.
const LIGHT = [0.97, 0.94, 0.89], BASE = [0.88, 0.81, 0.74], SHADE = [0.70, 0.62, 0.56];

function canvas(n) { return { n, rgb: new Float32Array(n * n * 3).fill(0), a: new Float32Array(n * n), w: new Float32Array(n * n) }; }
// Paint a colour with coverage cov (0..1) over pixel i: alpha union, colour weighted by coverage.
function paint(c, i, col, cov) {
  if (cov <= 0) return;
  const k = cov;
  c.rgb[i * 3] = mix(c.rgb[i * 3], col[0], k); c.rgb[i * 3 + 1] = mix(c.rgb[i * 3 + 1], col[1], k); c.rgb[i * 3 + 2] = mix(c.rgb[i * 3 + 2], col[2], k);
  c.w[i] = 1;
}
function save(c, name) {
  const n = c.n, data = Buffer.alloc(n * n * 4);
  for (let i = 0; i < n * n; i++) {
    const has = c.w[i] > 0;
    for (let k = 0; k < 3; k++) data[i * 4 + k] = Math.round(255 * clamp01(has ? c.rgb[i * 3 + k] : BASE[k]));
    data[i * 4 + 3] = Math.round(255 * clamp01(c.a[i]));
  }
  encodePng(path.join(OUT, name), { width: n, height: n, data });
  console.log('wrote', path.join(OUT, name));
}

// ---------- Ring: brushy spiral arcs around a centre, flat on the floor ----------
function ring() {
  const n = 1024, c = canvas(n), h = n / 2;
  for (let i = 0; i < n * n; i++) { c.rgb[i * 3] = BASE[0]; c.rgb[i * 3 + 1] = BASE[1]; c.rgb[i * 3 + 2] = BASE[2]; c.w[i] = 1; }
  // Open arcs only (style-guide round 1: no closed loop, it would read as a progress circle).
  // Major arcs are spread around the circle with clear gaps; each tail curls inward like the sheet swirl.
  // arc: start angle, sweep, radius at the head, inward curl at the tail, width, strength
  const arcs = [];
  const major = 4;
  for (let k = 0; k < major; k++) arcs.push({ a0: (k / major) * 2 * Math.PI + rr(-0.15, 0.15), sw: rr(0.9, 1.25), r0: [0.86, 0.74, 0.80, 0.66][k], curl: rr(0.10, 0.18), wd: rr(0.035, 0.055), s: rr(0.6, 0.85) });
  for (let k = 0; k < 6; k++) arcs.push({ a0: rr(0, 2 * Math.PI), sw: rr(0.5, 1.0), r0: rr(0.55, 0.92), curl: rr(0.04, 0.10), wd: rr(0.010, 0.020), s: rr(0.6, 0.9) });
  for (let y = 0; y < n; y++) for (let x = 0; x < n; x++) {
    const dx = (x + 0.5 - h) / h, dy = (y + 0.5 - h) / h, r = Math.hypot(dx, dy);
    if (r > 1) continue;
    const ang = Math.atan2(dy, dx);
    let a = 0, hl = 0;
    for (const q of arcs) {
      let t = ang - q.a0; t = ((t % (2 * Math.PI)) + 2 * Math.PI) % (2 * Math.PI);
      if (t > q.sw) continue;
      const u = t / q.sw;                                   // 0 tail .. 1 head
      const rc = q.r0 - q.curl * Math.pow(1 - u, 2.5);      // tail swirls inward
      const taper = Math.sin(Math.PI * Math.pow(u, 0.7));
      const wd = q.wd * (0.2 + 0.8 * taper);
      const d = (r - rc) / wd;
      const v = q.s * taper * Math.exp(-d * d * 1.6);       // soft painted edge across the stroke
      a = 1 - (1 - a) * (1 - v);
      hl = Math.max(hl, v * Math.exp(-d * d * 6));          // brighter core line
    }
    const i = y * n + x;
    c.a[i] = clamp01(a * smooth(1, 0.94, r));
    paint(c, i, LIGHT, clamp01(hl * 1.4));
  }
  save(c, 'T_GroundWind_Ring.png');
}

// ---------- Disc: soft round dust ----------
function disc() {
  const n = 512, c = canvas(n), h = n / 2;
  for (let y = 0; y < n; y++) for (let x = 0; x < n; x++) {
    const r = Math.hypot((x + 0.5 - h) / h, (y + 0.5 - h) / h), i = y * n + x;
    c.a[i] = 0.75 * (1 - smooth(0.15, 0.98, r));
    paint(c, i, BASE, 1); paint(c, i, SHADE, 0.35 * (1 - smooth(0, 0.6, r)));
  }
  save(c, 'T_GroundWind_Disc.png');
}

// Anti-aliased coverage from a signed distance in pixels (negative inside): crisp edge.
const cover = sd => clamp01(0.5 - sd);

// ---------- Streaks: crescent strokes, long axis along Y, 2x2 atlas ----------
function streaks() {
  const n = 1024, cell = 512, c = canvas(n);
  // Swooshes (style-guide round 1): rounded head at the top (+V), long hair-thin tail, hook at the head,
  // one or two thin parallel strands cut inside the tail. Crisp edges, flat bands.
  // per variant: length (fraction of cell), head half width (px), bend (px), hook (px), strands
  const V = [
    { L: 0.90, w: 16, bend: 80, hook: 50, strands: 2 },  // long
    { L: 0.90, w: 11, bend: 60, hook: 30, strands: 1 },  // long, thin
    { L: 0.60, w: 14, bend: 40, hook: 60, strands: 1 },  // short, strong hook
    { L: 0.60, w: 9,  bend: 35, hook: 35, strands: 0 },  // short, thin
  ];
  V.forEach((p, k) => {
    const ox = (k % 2) * cell, oy = Math.floor(k / 2) * cell, half = p.L * cell / 2;
    // s = -1 tail (bottom) .. +1 head (top); centre line bends, the head hooks sideways
    const cx = s => cell / 2 + p.bend * (s * s - 0.5) + p.hook * Math.pow(Math.max(0, s - 0.45) / 0.55, 2);
    // half width: round head over s in [0.75, 1], then a long taper to a hair at the tail
    const width = s => s > 0.75 ? p.w * Math.sqrt(Math.max(0, 1 - Math.pow((s - 0.75) / 0.25, 2)))
                                : p.w * (0.06 + 0.94 * Math.pow((s + 1) / 1.75, 1.1));
    for (let y = 0; y < cell; y++) for (let x = 0; x < cell; x++) {
      const s = -((y + 0.5 - cell / 2) / half);      // head at the top of the cell (+V up in texture = -Y)
      if (Math.abs(s) > 1.0) continue;
      const hw = width(s);
      const dxp = x + 0.5 - cx(s);
      const slope = (cx(Math.min(1, s + 0.01)) - cx(Math.max(-1, s - 0.01))) / (0.02 * half);
      const across = Math.abs(dxp) / Math.sqrt(1 + slope * slope);
      let cov = cover(across - hw);
      if (cov <= 0) continue;
      const side = dxp / Math.max(hw, 0.5);
      // strands: thin transparent gaps along the tail half, parallel to the centre line
      if (p.strands && s < 0.55 && hw > 2.5) {
        const gaps = p.strands === 2 ? [-0.35, 0.3] : [0.05];
        for (const g of gaps) cov = Math.min(cov, clamp01(Math.abs(side - g) * hw - 0.6));
      }
      const i = (oy + y) * n + (ox + x);
      c.a[i] = Math.max(c.a[i], cov * 0.95);
      paint(c, i, BASE, 1);
      if (side > 0.45) paint(c, i, SHADE, 0.8);
      if (side > -0.6 && side < 0.0) paint(c, i, LIGHT, 1);
    }
  });
  save(c, 'T_GroundWind_Streaks.png');
}

// ---------- Puffs: overlapping circles, crisp silhouette, two flat tones, debris dots ----------
function puffs() {
  const n = 1024, cell = 512, c = canvas(n);
  const layouts = [];
  for (let k = 0; k < 4; k++) {
    const blobs = [], dots = [];
    const count = k === 3 ? 2 : 6 + Math.floor(rnd() * 3);
    const scale = k === 3 ? 0.45 : 1;
    // a base row of circles, a taller one near the middle, like the sheet clouds
    for (let b = 0; b < count; b++) {
      const t = count === 1 ? 0.5 : b / (count - 1);
      const r = scale * (k === 3 ? 40 : rr(48, 78) * (1 - 0.45 * Math.abs(t - 0.45) * 2 * 0.6));
      blobs.push({ x: cell / 2 + scale * (t - 0.5) * 300 + rr(-12, 12), y: cell * 0.58 - scale * rr(0, 50) * (1 - Math.abs(t - 0.45) * 1.6), r });
    }
    if (k !== 3) blobs.push({ x: cell / 2 + rr(-50, 10), y: cell * 0.47, r: rr(64, 74) });
    if (k !== 3) for (const side of [-1, 1]) blobs.push({ x: cell / 2 + side * rr(165, 185), y: cell * 0.62, r: rr(26, 34) });
    const nd = k === 3 ? 7 : 5 + Math.floor(rnd() * 3);
    for (let d = 0; d < nd; d++) {
      const a = rr(0, 2 * Math.PI), dist = (k === 3 ? rr(60, 200) : rr(190, 235));
      dots.push({ x: cell / 2 + Math.cos(a) * dist, y: cell / 2 + Math.sin(a) * dist * 0.75, r: rr(6, 15) });
    }
    layouts.push({ blobs, dots });
  }
  layouts.forEach((L, k) => {
    const ox = (k % 2) * cell, oy = Math.floor(k / 2) * cell;
    for (let y = 0; y < cell; y++) for (let x = 0; x < cell; x++) {
      const px = x + 0.5, py = y + 0.5;
      let sd = 1e9, inner = 1e9, lifted = 1e9;
      for (const b of L.blobs) {
        const d = Math.hypot(px - b.x, py - b.y) - b.r; sd = Math.min(sd, d);
        // highlight: same circles shifted up-left and shrunk -> flat light band on top
        inner = Math.min(inner, Math.hypot(px - (b.x - b.r * 0.18), py - (b.y - b.r * 0.22)) - b.r * 0.78);
        // shade: the lower crescent of each circle, outside the same circles lifted up
        lifted = Math.min(lifted, Math.hypot(px - (b.x + b.r * 0.08), py - (b.y - b.r * 0.30)) - b.r);
      }
      let dotSd = 1e9;
      for (const d of L.dots) dotSd = Math.min(dotSd, Math.hypot(px - d.x, py - d.y) - d.r);
      const cov = Math.max(cover(sd), cover(dotSd));
      if (cov <= 0) continue;
      const i = (oy + y) * n + (ox + x);
      c.a[i] = cov;
      if (cover(dotSd) > cover(sd)) { paint(c, i, SHADE, 1); continue; }
      // flat bands: shaded lower crescents, base, light upper-left caps
      paint(c, i, BASE, 1);
      paint(c, i, SHADE, 1 - cover(lifted));
      paint(c, i, LIGHT, cover(inner) * cover(lifted));
    }
  });
  save(c, 'T_GroundWind_Puffs.png');
}

ring(); disc(); streaks(); puffs();
