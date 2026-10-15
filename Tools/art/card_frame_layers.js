// Builds the stat card frame layers from a flat-magenta frame image, and the rarity example sheet.
// Node 18+, no dependencies (PNG codec from ui_icon_prepare.js).
//
// Usage:
//   node Tools/art/card_frame_layers.js [--in <png>] [--out-dir Art/UI/Cards] [--sheet <png>]
// Defaults: --in Art/Source/card-frames/20261014-141713-card-frames-1.png,
//           --out-dir Art/UI/Cards, --sheet Art/Source/card-frames/rarity-examples/option-AC-mix.png
//
// Method:
//   1. key the magenta (corner background, distance + difference key, unmix, despill);
//   2. accent pixels = teal hue (130..215 deg), saturation > 0.25, value > 0.12 (soft weight);
//   3. layers, all the same canvas size as the input:
//      T_CardFrame_Stat.png          frame with the accents turned neutral grey (no tint in game);
//      T_CardFrame_Stat_Accents.png  accents as grey luminance (shading kept), alpha = accent weight;
//      T_CardFrame_Stat_Glow.png     white thin line on the inner edge of the frame + soft glow in the opening;
//   4. the sheet composites the three written layers as the game would: base, then Accents * tint,
//      then Glow * tint, over #15171c, for the five WBP_UpgradeChoice rarity colours, frames 300 px tall;
//   5. 9-slice margins are measured on the written Accents layer (accent coverage along each strip) and printed; they apply to the three layers (same canvas, same geometry).
const fs = require('fs');
const path = require('path');
const { decodePng, encodePng } = require('./ui_icon_prepare.js');
const REPO = path.resolve(__dirname, '..', '..');
const args = process.argv.slice(2);
const opt = (k, d) => { const i = args.indexOf('--' + k); return i >= 0 ? args[i + 1] : d; };
const IN = path.resolve(REPO, opt('in', 'Art/Source/card-frames/20261014-141713-card-frames-1.png'));
const OUTDIR = path.resolve(REPO, opt('out-dir', 'Art/UI/Cards'));
const SHEET = path.resolve(REPO, opt('sheet', 'Art/Source/card-frames/rarity-examples/option-AC-mix.png'));

const src = decodePng(IN);
const W = src.width, H = src.height, N = W * H;
const ss = (a, b, x) => { const t = Math.min(1, Math.max(0, (x - a) / (b - a))); return t * t * (3 - 2 * t); };
// ---- key
const bg = [0, 0, 0];
{
  let n = 0;
  for (const [cx, cy] of [[0, 0], [W - 8, 0], [0, H - 8], [W - 8, H - 8]])
    for (let y = cy; y < cy + 8; y++) for (let x = cx; x < cx + 8; x++) { const i = (y * W + x) * 4; for (let k = 0; k < 3; k++) bg[k] += src.data[i + k]; n++; }
  for (let k = 0; k < 3; k++) bg[k] /= n;
}
const bgSpill = Math.min(bg[0], bg[2]) - bg[1];
const rgb = new Float32Array(N * 3), al = new Float32Array(N);
for (let i = 0; i < N; i++) {
  const p = i * 4, r = src.data[p], g = src.data[p + 1], b = src.data[p + 2];
  let a = ss(40, 110, Math.hypot(r - bg[0], g - bg[1], b - bg[2]));
  a = Math.min(a, Math.min(1, Math.max(0, 1 - (Math.min(r, b) - g) / bgSpill)));
  let c = [r, g, b];
  if (a > 0) {
    if (a < 1) c = c.map((v, k) => Math.min(255, Math.max(0, (v - (1 - a) * bg[k]) / a)));
    const s = Math.min(c[0], c[2]) - c[1]; if (s > 0) { c[0] -= s; c[2] -= s; }
  }
  rgb[i * 3] = c[0]; rgb[i * 3 + 1] = c[1]; rgb[i * 3 + 2] = c[2]; al[i] = a;
}
function hsv(r, g, b) {
  r /= 255; g /= 255; b /= 255;
  const mx = Math.max(r, g, b), mn = Math.min(r, g, b), d = mx - mn;
  let h = 0;
  if (d > 0) { if (mx === r) h = ((g - b) / d + 6) % 6; else if (mx === g) h = (b - r) / d + 2; else h = (r - g) / d + 4; h *= 60; }
  return [h, mx ? d / mx : 0, mx];
}
const tw = new Float32Array(N);   // accent weight
for (let i = 0; i < N; i++) {
  if (al[i] < 0.01) continue;
  const [h, s, v] = hsv(rgb[i * 3], rgb[i * 3 + 1], rgb[i * 3 + 2]);
  tw[i] = ss(130, 150, h) * (1 - ss(195, 215, h)) * ss(0.25, 0.45, s) * ss(0.12, 0.25, v);
}
// ---- interior of the frame (flood fill the transparent opening)
const inside = new Uint8Array(N);
{
  const st = [Math.floor(H / 2) * W + Math.floor(W / 2)]; inside[st[0]] = 1;
  while (st.length) {
    const i = st.pop(), x = i % W, y = (i / W) | 0;
    for (const j of [x > 0 ? i - 1 : -1, x < W - 1 ? i + 1 : -1, y > 0 ? i - W : -1, y < H - 1 ? i + W : -1])
      if (j >= 0 && !inside[j] && al[j] < 0.5) { inside[j] = 1; st.push(j); }
  }
}
function boxBlur(a, r) {
  let cur = Float32Array.from(a); const tmp = new Float32Array(N);
  for (let pass = 0; pass < 3; pass++) {
    for (let y = 0; y < H; y++) {
      let s = 0; const o = y * W;
      for (let x = -r; x <= r; x++) s += cur[o + Math.min(W - 1, Math.max(0, x))];
      for (let x = 0; x < W; x++) { tmp[o + x] = s / (2 * r + 1); s += cur[o + Math.min(W - 1, x + r + 1)] - cur[o + Math.max(0, x - r)]; }
    }
    for (let x = 0; x < W; x++) {
      let s = 0;
      for (let y = -r; y <= r; y++) s += tmp[Math.min(H - 1, Math.max(0, y)) * W + x];
      for (let y = 0; y < H; y++) { cur[y * W + x] = s / (2 * r + 1); s += tmp[Math.min(H - 1, y + r + 1) * W + x] - tmp[Math.max(0, y - r) * W + x]; }
    }
  }
  return cur;
}
const glowField = boxBlur(Float32Array.from(inside, (v) => 1 - v), 33)   // radius 22 -> 33: soft part reaches 1.5x further;
const GLOW_GAIN = 3.4;
const nearLine = boxBlur(Float32Array.from(inside), 3);
// ---- layers
const base = new Uint8Array(N * 4), acc = new Uint8Array(N * 4), glow = new Uint8Array(N * 4);
for (let i = 0; i < N; i++) {
  const r = rgb[i * 3], g = rgb[i * 3 + 1], b = rgb[i * 3 + 2], a = al[i], w = tw[i], lum = 0.299 * r + 0.587 * g + 0.114 * b;
  const gr = lum * 1.15;
  base[i * 4] = Math.round(r + (gr - r) * w); base[i * 4 + 1] = Math.round(g + (gr - g) * w); base[i * 4 + 2] = Math.round(b + (gr - b) * w);
  base[i * 4 + 3] = Math.round(a * 255);
  const sh = Math.min(255, Math.round(lum * 1.7));          // shading kept, near white for bright accents
  acc[i * 4] = acc[i * 4 + 1] = acc[i * 4 + 2] = sh; acc[i * 4 + 3] = Math.round(w * a * 255);
  let ga = 0;
  if (inside[i]) ga = 1 - Math.exp(-GLOW_GAIN * Math.pow(Math.min(1, Math.max(0, glowField[i])), 1.15));   // about 3x the former intensity (was pow 1.3 * 0.9), saturating
  else if (a > 0.5 && nearLine[i] > 0.02) ga = Math.min(1, Math.max(0, nearLine[i]) * 2.5) * 0.95;
  glow[i * 4] = glow[i * 4 + 1] = glow[i * 4 + 2] = 255; glow[i * 4 + 3] = Math.round(Math.min(1, ga) * 255);
}
const names = ['T_CardFrame_Stat.png', 'T_CardFrame_Stat_Accents.png', 'T_CardFrame_Stat_Glow.png'];
[base, acc, glow].forEach((d, k) => encodePng(path.join(OUTDIR, names[k]), { width: W, height: H, data: d }));

// ---- 9-slice margins, measured on the written Accents layer
// Accent coverage along a strip: fraction of accent pixels (alpha > 60) in a band, per position.
// A corner group = the first run of coverage > 0.15 from the border, ended by a gap (>= 40 px under 0.05).
const A = decodePng(path.join(OUTDIR, names[1])).data;
function cover(len, band, at) { const c = new Float32Array(len); for (let t = 0; t < len; t++) { let n = 0; for (const u of band) if (at(t, u) > 60) n++; c[t] = n / band.length; } return c; }
function groupEnd(c) {   // end of the first group counted from index 0
  let i = 0; while (i < c.length && c[i] < 0.15) i++;
  let last = i, gap = 0;
  for (; i < c.length; i++) { if (c[i] > 0.15) { last = i; gap = 0; } else if (++gap >= 40) break; }
  return last + 1;
}
const rng = (a, b) => Array.from({ length: b - a }, (_, k) => a + k);
const accAt = (x, y) => A[(y * W + x) * 4 + 3];
const top = cover(W, rng(30, 100), (x, y) => accAt(x, y)), bot = cover(W, rng(H - 100, H - 30), (x, y) => accAt(x, y));
const lef = cover(H, rng(20, 90), (y, x) => accAt(x, y)), rig = cover(H, rng(W - 90, W - 20), (y, x) => accAt(x, y));
const rev = (c) => Float32Array.from(c).reverse();
const m = {
  left: Math.max(groupEnd(top), groupEnd(bot)), right: Math.max(groupEnd(rev(top)), groupEnd(rev(bot))),
  top: Math.max(groupEnd(lef), groupEnd(rig)), bottom: Math.max(groupEnd(rev(lef)), groupEnd(rev(rig))),
};
console.log('9-slice margins (px), same for the 3 layers:', JSON.stringify(m), 'canvas', W + 'x' + H);

// ---- sheet: composite the three written layers as the game does
const L = names.map((n) => decodePng(path.join(OUTDIR, n)).data);
const RAR = [[.55, .55, .55], [.2, .8, .25], [.2, .45, 1], [.65, .25, .9], [1, .85, .1]];
const BG = [0x15, 0x17, 0x1c], TH = 300, TW = Math.round(W * TH / H), GAP = 30, SW = GAP + 5 * (TW + GAP), SH = TH + 2 * GAP;
function over(o, c, a) { for (let k = 0; k < 3; k++) o[k] = o[k] * (1 - a) + c[k] * a; }
const sheet = new Uint8Array(SW * SH * 4);
for (let i = 0; i < SW * SH; i++) { sheet[i * 4] = BG[0]; sheet[i * 4 + 1] = BG[1]; sheet[i * 4 + 2] = BG[2]; sheet[i * 4 + 3] = 255; }
RAR.forEach((col, k) => {
  const full = new Float32Array(N * 3);
  for (let i = 0; i < N; i++) {
    const o = [BG[0], BG[1], BG[2]], p = i * 4;
    over(o, [L[0][p], L[0][p + 1], L[0][p + 2]], L[0][p + 3] / 255);
    over(o, [L[1][p] * col[0], L[1][p + 1] * col[1], L[1][p + 2] * col[2]], L[1][p + 3] / 255);
    over(o, [255 * col[0], 255 * col[1], 255 * col[2]], L[2][p + 3] / 255);
    full[i * 3] = o[0]; full[i * 3 + 1] = o[1]; full[i * 3 + 2] = o[2];
  }
  const sx = W / TW, sy = H / TH, ox = GAP + k * (TW + GAP);
  for (let y = 0; y < TH; y++) for (let x = 0; x < TW; x++) {
    let a = 0, b = 0, c = 0, n = 0;
    for (let yy = Math.floor(y * sy); yy < Math.min(H, Math.ceil((y + 1) * sy)); yy++)
      for (let xx = Math.floor(x * sx); xx < Math.min(W, Math.ceil((x + 1) * sx)); xx++) { const i = (yy * W + xx) * 3; a += full[i]; b += full[i + 1]; c += full[i + 2]; n++; }
    const p = ((GAP + y) * SW + ox + x) * 4; sheet[p] = a / n; sheet[p + 1] = b / n; sheet[p + 2] = c / n;
  }
});
encodePng(SHEET, { width: SW, height: SH, data: sheet });
console.log('sheet', SHEET, SW + 'x' + SH);
