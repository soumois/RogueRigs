// Makes a square PNG seamlessly tileable and renders a 3 x 3 tiled preview.
// Usage: node Tools/art/make_tileable.js --in <raw.png> --out <tileable.png> --preview <tiled.png> [--size 1024] [--blur 0]
// Method (deterministic, no dependencies):
//   1. centre-crop to a square, area-resample to size;
//   2. shift by half a tile in x and y (the old borders, the only possible discontinuities, now
//      form a cross through the centre; the new borders are continuous by construction);
//   3. cross-fade the cross: blend the shifted image with the ORIGINAL (unshifted) image, whose
//      centre is seam free, using a weight w = tent(x) * tent(y)-style mask that is 1 at the
//      seam lines of the shifted image's cross and 0 outside a band of band*size px:
//      out = shifted * (1 - m) + original_centre_content * m, where m = max(mx, my),
//      mx = smoothstep over |x - size/2| < band, my likewise for y.
//   4. the output borders sit where m = 0 and are the half-shifted borders: continuous.
// Preview: 3 x 3 repeat of the result, downscaled by 3 (area) to stay at size.
const fs = require('fs');
const L = require('./lib');
const { decodePng, encodePng } = require('./ui_icon_prepare');
const a = L.parseArgs(process.argv.slice(2));
const size = parseInt(a.size || '1024', 10);
const band = 0.18;
const src = decodePng(a.in);
const ch = src.data.length / (src.width * src.height);
if (ch < 3) throw new Error('need RGB(A)');
const s = Math.min(src.width, src.height);
const ox = (src.width - s) >> 1, oy = (src.height - s) >> 1;
// RGB float, area resample of the centre crop
const img = new Float32Array(size * size * 3);
for (let y = 0; y < size; y++) for (let x = 0; x < size; x++) {
  const x0 = Math.floor(ox + x * s / size), x1 = Math.max(x0 + 1, Math.floor(ox + (x + 1) * s / size));
  const y0 = Math.floor(oy + y * s / size), y1 = Math.max(y0 + 1, Math.floor(oy + (y + 1) * s / size));
  const acc = [0, 0, 0]; let n = 0;
  for (let yy = y0; yy < y1; yy++) for (let xx = x0; xx < x1; xx++) {
    const i = (yy * src.width + xx) * ch; acc[0] += src.data[i]; acc[1] += src.data[i + 1]; acc[2] += src.data[i + 2]; n++;
  }
  const o = (y * size + x) * 3; img[o] = acc[0] / n; img[o + 1] = acc[1] / n; img[o + 2] = acc[2] / n;
}
// Large-scale tone correction: remove the low-frequency gradient across the frame so opposite
// edges match in mean colour (a linear ramp per axis, fitted from the edge means).
for (const axis of [0, 1]) for (let c = 0; c < 3; c++) {
  let a0 = 0, a1 = 0;
  for (let t = 0; t < size; t++) {
    const lo = axis === 0 ? (t * size) * 3 + c : t * 3 + c;
    const hi = axis === 0 ? (t * size + size - 1) * 3 + c : ((size - 1) * size + t) * 3 + c;
    a0 += img[lo]; a1 += img[hi];
  }
  const d = (a1 - a0) / size; // difference high edge minus low edge
  for (let y = 0; y < size; y++) for (let x = 0; x < size; x++) {
    const t = axis === 0 ? x : y;
    img[(y * size + x) * 3 + c] -= d * (t / (size - 1) - 0.5);
  }
}
const sm = (e0, e1, v) => { const t = Math.min(1, Math.max(0, (v - e0) / (e1 - e0))); return t * t * (3 - 2 * t); };
const h = size >> 1, bw = band * size;
const out = new Float32Array(size * size * 3);
for (let y = 0; y < size; y++) for (let x = 0; x < size; x++) {
  // shifted pixel
  const sx = (x + h) % size, sy = (y + h) % size;
  const mx = 1 - sm(bw * 0.15, bw, Math.abs(x - h));   // 1 at the cross line (x = h)
  const my = 1 - sm(bw * 0.15, bw, Math.abs(y - h));
  const m = Math.max(mx, my);
  // shifted image value is img[sx,sy]; at the cross it is discontinuous, so there use a
  // blend of the two sides: the unshifted image at (x, y) is continuous at the centre.
  const o = (y * size + x) * 3, p = (sy * size + sx) * 3, q = o;
  for (let c = 0; c < 3; c++) out[o + c] = img[p + c] * (1 - m) + img[q + c] * m;
}
const rgba = Buffer.alloc(size * size * 4);
for (let i = 0; i < size * size; i++) {
  for (let c = 0; c < 3; c++) rgba[i * 4 + c] = Math.max(0, Math.min(255, Math.round(out[i * 3 + c])));
  rgba[i * 4 + 3] = 255;
}
encodePng(a.out, { width: size, height: size, data: rgba });
// seam metric: mean abs step across the wrap vs mean abs step between neighbours inside
let wrap = 0, inner = 0, nw = 0, ni = 0;
for (let y = 0; y < size; y++) for (let c = 0; c < 3; c++) {
  wrap += Math.abs(out[(y * size) * 3 + c] - out[(y * size + size - 1) * 3 + c]); nw++;
  for (let x = 1; x < size; x++) { inner += Math.abs(out[(y * size + x) * 3 + c] - out[(y * size + x - 1) * 3 + c]); ni++; }
}
for (let x = 0; x < size; x++) for (let c = 0; c < 3; c++) {
  wrap += Math.abs(out[x * 3 + c] - out[((size - 1) * size + x) * 3 + c]); nw++;
}
console.log(`wrap step ${(wrap / nw).toFixed(2)} vs inner step ${(inner / ni).toFixed(2)} (levels of 255)`);
if (a.preview) {
  const ps = size, pr = Buffer.alloc(ps * ps * 4);
  for (let y = 0; y < ps; y++) for (let x = 0; x < ps; x++) {
    const acc = [0, 0, 0];
    for (let dy = 0; dy < 3; dy++) for (let dx = 0; dx < 3; dx++) {
      const X = (x * 3 + dx) % size, Y = (y * 3 + dy) % size;
      for (let c = 0; c < 3; c++) acc[c] += rgba[(Y * size + X) * 4 + c];
    }
    for (let c = 0; c < 3; c++) pr[(y * ps + x) * 4 + c] = Math.round(acc[c] / 9);
    pr[(y * ps + x) * 4 + 3] = 255;
  }
  // 3 x 3 at full resolution would be 3072 px: write it too, that shows the seams at 1:1
  const fs3 = size * 3, full = Buffer.alloc(fs3 * fs3 * 4);
  for (let y = 0; y < fs3; y++) for (let x = 0; x < fs3; x++) {
    const i = ((y % size) * size + (x % size)) * 4, o = (y * fs3 + x) * 4;
    full[o] = rgba[i]; full[o + 1] = rgba[i + 1]; full[o + 2] = rgba[i + 2]; full[o + 3] = 255;
  }
  encodePng(a.preview, { width: fs3, height: fs3, data: full });
}
