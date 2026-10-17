// Procedural seamless crack-growth mask for the rock slab tops (task-230-05).
// Usage: node Tools/art/make_rock_crack_mask.js
// mask value m encodes growth order: a crack pixel shows for damage d when m > 1 - d.
// Background = 0. Each crack tree has a start threshold d0 and grows along its path: d = d0 + g * s
// (s = path length in px), mask = 1 - d. Branches start later than their parent at that point.
// Everything is drawn modulo the tile, so the texture wraps exactly in x and y. Deterministic (SEED).
const fs = require('fs');
const path = require('path');
const { decodePng, encodePng } = require('./ui_icon_prepare');
const SEED = 230052, N = 1024, G = 0.0026, MAXD = 0.85;
let st = SEED >>> 0;
const rnd = () => { st = (st + 0x6D2B79F5) >>> 0; let t = st; t = Math.imul(t ^ (t >>> 15), t | 1); t ^= t + Math.imul(t ^ (t >>> 7), t | 61); return ((t ^ (t >>> 14)) >>> 0) / 4294967296; };
const rr = (a, b) => a + (b - a) * rnd();
const mask = new Float32Array(N * N);
const wrap = v => ((v % N) + N) % N;
function stamp(x, y, r, v) {
  const R = Math.ceil(r + 1);
  for (let dy = -R; dy <= R; dy++) for (let dx = -R; dx <= R; dx++) {
    const dist = Math.hypot(dx + (Math.round(x) - x), dy + (Math.round(y) - y));
    const cov = Math.min(1, Math.max(0, r + 0.5 - dist));
    if (cov <= 0) continue;
    const X = wrap(Math.round(x) + dx), Y = wrap(Math.round(y) + dy), i = Y * N + X;
    const val = v * (cov < 1 ? 0.5 + 0.5 * cov : 1); // soft edge, keeps value order
    if (val > mask[i]) mask[i] = val;
  }
}
// Straight segments joined by sharp angular kinks (split stone). A crack stops when it runs into an
// existing crack (after a short free run), so branches meet and outline irregular plates.
function grow(x, y, ang, d0, depth, hw) {
  let s = 0;
  const maxLen = rr(170, 300) * (depth ? 0.7 : 1);
  while (s < maxLen) {
    const segLen = Math.floor(rr(22, 70));
    const dx = Math.cos(ang), dy = Math.sin(ang);
    for (let k = 0; k < segLen && s < maxLen; k++, s++) {
      const d = d0 + G * s;
      if (d > MAXD) return;
      if (s > 10 && mask[wrap(Math.round(y + dy * 4)) * N + wrap(Math.round(x + dx * 4))] > 0) return; // joined
      stamp(x, y, hw, 1 - d);
      x += dx; y += dy;
    }
    if (depth < 2 && rnd() < 0.35) { // branch at the kink
      const side = rnd() < 0.5 ? -1 : 1;
      grow(x, y, ang + side * rr(0.6, 1.4), d0 + G * s + rr(0.02, 0.1), depth + 1, hw);
    }
    ang += (rnd() < 0.5 ? -1 : 1) * rr(0.3, 1.0); // sharp kink
  }
}
// early starts: jittered 12 x 12 grid, d0 in [0, 0.05] so even d = 0.05 shows small cracks everywhere
const roots = [];
const K = 13, cs = N / K;
for (let j = 0; j < K; j++) for (let i = 0; i < K; i++)
  roots.push([(i + rnd()) * cs, (j + rnd()) * cs, rr(0, 0.05)]);
// later starts: 120 more, d0 in [0.05, 0.4]
for (let k = 0; k < 90; k++) roots.push([rr(0, N), rr(0, N), rr(0.05, 0.4)]);
for (const [x, y, d0] of roots) {
  // a crack grows both ways from its start point
  const a = rr(0, Math.PI * 2);
  grow(x, y, a, d0, 0, 1.6); grow(x, y, a + Math.PI, d0, 0, 1.6);
}
const out = path.join(__dirname, '..', '..', 'Art', 'Textures', 'T_RockCrackMask.png');
const pdir = path.join(__dirname, '..', '..', 'Art', 'Source', 'rock-crack-mask');
const toRgba = f => { const b = Buffer.alloc(N * N * 4); for (let i = 0; i < N * N; i++) { const v = Math.round(Math.min(1, Math.max(0, f(i))) * 255); b[i*4]=b[i*4+1]=b[i*4+2]=v; b[i*4+3]=255; } return b; };
encodePng(out, { width: N, height: N, data: toRgba(i => mask[i]) });
encodePng(path.join(pdir, 'mask.png'), { width: N, height: N, data: toRgba(i => mask[i]) });
// 16-bit-free check: 8-bit quantisation must keep order, report distinct levels
const top = decodePng(path.join(__dirname, '..', '..', 'Art', 'Textures', 'T_RockTop.png'));
const tch = top.data.length / (top.width * top.height);
const Ds = [0.05, 0.2, 0.4, 0.6, 0.9];
const render = (d, W, H, ox, oy) => {
  const b = Buffer.alloc(W * H * 4);
  for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) {
    const sx = (x + ox) % N, sy = (y + oy) % N, m = mask[sy * N + sx], o = (y * W + x) * 4;
    const ti = ((sy % top.height) * top.width + (sx % top.width)) * tch;
    const k = m > 1 - d ? 0.6 : 0; // pale crack over the top
    for (let c = 0; c < 3; c++) b[o + c] = Math.round(top.data[ti + c] * (1 - k) + 235 * k);
    b[o + 3] = 255;
  }
  return b;
};
const P = 512, strip = Buffer.alloc(P * Ds.length * P * 4);
Ds.forEach((d, k) => { const r = render(d, P, P, 0, 0); for (let y = 0; y < P; y++) r.copy(strip, (y * P * Ds.length + k * P) * 4, y * P * 4, (y + 1) * P * 4); });
encodePng(path.join(pdir, 'strip_d005_020_040_060_090.png'), { width: P * Ds.length, height: P, data: strip });
const full = render(0.6, N, N, 0, 0), T3 = N * 3, t3 = Buffer.alloc(T3 * T3 * 4);
for (let y = 0; y < T3; y++) for (let x = 0; x < T3; x++) full.copy(t3, (y * T3 + x) * 4, ((y % N) * N + (x % N)) * 4, ((y % N) * N + (x % N)) * 4 + 4);
encodePng(path.join(pdir, 'tiled3x3_d060.png'), { width: T3, height: T3, data: t3 });
// seam: mean abs step across the wrap vs inside, on the 8-bit quantised mask
const q = i => Math.round(mask[i] * 255);
let w = 0, inn = 0, nw = 0, ni = 0;
for (let y = 0; y < N; y++) { w += Math.abs(q(y*N) - q(y*N+N-1)); nw++; for (let x = 1; x < N; x++) { inn += Math.abs(q(y*N+x) - q(y*N+x-1)); ni++; } }
for (let x = 0; x < N; x++) { w += Math.abs(q(x) - q((N-1)*N+x)); nw++; }
const cov = {}; for (const d of [0.05, 0.1, 0.2, 0.4, 0.6, 0.9]) { let n = 0; for (let i = 0; i < N * N; i++) if (mask[i] > 1 - d) n++; cov[d] = +(n / (N * N)).toFixed(4); }
const rep = { resolution: N, seed: SEED, growthPerPx: G, minMaskValue: +mask.reduce((a, v) => (v > 0 && v < a ? v : a), 1).toFixed(3),
  seamWrapStep: +(w / nw).toFixed(3), seamInnerStep: +(inn / ni).toFixed(3), coverageByDamage: cov };
fs.writeFileSync(path.join(pdir, 'report.json'), JSON.stringify(rep, null, 1));
console.log(JSON.stringify(rep));
