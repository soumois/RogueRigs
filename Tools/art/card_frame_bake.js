// Bakes the card frame layers at the exact card size, so Unreal draws plain images (no 9-slice Box brush).
// Node 18+, no dependencies (PNG codec from ui_icon_prepare.js).
//
// Usage:
//   node Tools/art/card_frame_bake.js [--scale 0.5] [--width 1000] [--height 1440]
// Bakes Art/UI/Cards/T_CardFrame_{Stat,Stat_Accents,Stat_Glow,Weapon}.png (1086 x 1448) to
// T_CardFrame_*_Baked.png: 9-slice compose with the corners scaled uniformly by s, edge bands stretched
// along their length only, centre stretched. Resampling is a premultiplied tent/area filter (no nearest).
// Then measures the opening safe rect (alpha < 128 from the centre, largest clear axis-aligned rect grown
// from the centre) per baked frame, and writes Art/Source/card-frames/rarity-examples/baked-control.png
// (5 stat rarities + weapon card at 1x = half the baked size, on #15171c, opening outlined 1 px on the
// first stat card and the weapon card). Prints everything as JSON.

const path = require('path');
const { decodePng, encodePng } = require('./ui_icon_prepare');

const REPO = path.resolve(__dirname, '..', '..');
const DIR = path.join(REPO, 'Art', 'UI', 'Cards');
const M_STAT = { l: 226, t: 173, r: 230, b: 185 };
const M_WEAPON = { l: 140, t: 180, r: 140, b: 190 };
const LAYERS = [
  ['T_CardFrame_Stat', M_STAT], ['T_CardFrame_Stat_Accents', M_STAT],
  ['T_CardFrame_Stat_Glow', M_STAT], ['T_CardFrame_Weapon', M_WEAPON],
];

function arg(name, def) {
  const i = process.argv.indexOf('--' + name);
  return i > 0 ? Number(process.argv[i + 1]) : def;
}

// Piecewise map of dest pixel centre -> source coordinate, with the local scale (src px per dst px).
function axisWeights(srcLen, dstLen, m0, m1, s) {
  const d0 = m0 * s, d1 = m1 * s;
  const midSrc = srcLen - m0 - m1, midDst = dstLen - d0 - d1;
  if (midDst <= 0) throw new Error('target too small for the corners at this scale');
  const out = [];
  for (let d = 0; d < dstLen; d++) {
    const c = d + 0.5;
    let u, k;
    if (c < d0) { k = 1 / s; u = c * k; }
    else if (c > dstLen - d1) { k = 1 / s; u = srcLen - (dstLen - c) * k; }
    else { k = midSrc / midDst; u = m0 + (c - d0) * k; }
    const rad = Math.max(1, k), list = [];
    let sum = 0;
    for (let i = Math.max(0, Math.floor(u - rad)); i <= Math.min(srcLen - 1, Math.ceil(u + rad)); i++) {
      const w = Math.max(0, 1 - Math.abs(i + 0.5 - u) / rad);
      if (w > 0) { list.push([i, w]); sum += w; }
    }
    out.push(list.map(([i, w]) => [i, w / sum]));
  }
  return out;
}

function bake(img, m, W, H, s) {
  const wx = axisWeights(img.width, W, m.l, m.r, s), wy = axisWeights(img.height, H, m.t, m.b, s);
  const { width: sw, data } = img;
  const tmp = new Float32Array(W * img.height * 4);
  for (let y = 0; y < img.height; y++) for (let x = 0; x < W; x++) {
    const acc = [0, 0, 0, 0];
    for (const [i, k] of wx[x]) {
      const p = (y * sw + i) * 4, a = data[p + 3] / 255;
      acc[0] += data[p] * a * k; acc[1] += data[p + 1] * a * k; acc[2] += data[p + 2] * a * k; acc[3] += a * k;
    }
    tmp.set(acc, (y * W + x) * 4);
  }
  const out = new Uint8Array(W * H * 4);
  for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) {
    const acc = [0, 0, 0, 0];
    for (const [j, k] of wy[y]) {
      const p = (j * W + x) * 4;
      for (let c = 0; c < 4; c++) acc[c] += tmp[p + c] * k;
    }
    const a = acc[3], d = (y * W + x) * 4;
    if (a > 0) for (let c = 0; c < 3; c++) out[d + c] = Math.round(Math.min(255, acc[c] / a));
    out[d + 3] = Math.round(Math.min(1, a) * 255);
  }
  return { width: W, height: H, data: out };
}

// Largest clear rect grown from the centre: all pixels alpha < 128; sides advance round-robin.
function opening(img) {
  const { width: W, height: H, data } = img;
  const clear = (x, y) => data[(y * W + x) * 4 + 3] < 128;
  let l = Math.floor(W / 2), r = l, t = Math.floor(H / 2), b = t;
  if (!clear(l, t)) throw new Error('centre is not clear');
  const free = { l: true, t: true, r: true, b: true };
  const strip = (side) => {
    if (side === 'l') { if (l <= 0) return false; for (let y = t; y <= b; y++) if (!clear(l - 1, y)) return false; l--; }
    if (side === 'r') { if (r >= W - 1) return false; for (let y = t; y <= b; y++) if (!clear(r + 1, y)) return false; r++; }
    if (side === 't') { if (t <= 0) return false; for (let x = l; x <= r; x++) if (!clear(x, t - 1)) return false; t--; }
    if (side === 'b') { if (b >= H - 1) return false; for (let x = l; x <= r; x++) if (!clear(x, b + 1)) return false; b++; }
    return true;
  };
  while (free.l || free.t || free.r || free.b) {
    for (const k of ['l', 't', 'r', 'b']) if (free[k] && !strip(k)) free[k] = false;
  }
  return { l, t, r, b, inset: { left: l, top: t, right: W - 1 - r, bottom: H - 1 - b } };
}

// Width of the first opaque run (alpha >= 128) at mid height, from the left edge.
function sideBand(img) {
  const y = Math.floor(img.height / 2);
  const op = (x) => img.data[(y * img.width + x) * 4 + 3] >= 128;
  let x0 = 0;
  while (x0 < img.width && !op(x0)) x0++;
  let w = 0;
  for (let x = x0; x < img.width && op(x); x++) w++;
  return w;
}

function down2(img) {
  const W = img.width / 2, H = img.height / 2, out = new Uint8Array(W * H * 4);
  for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) {
    const acc = [0, 0, 0, 0];
    for (let dy = 0; dy < 2; dy++) for (let dx = 0; dx < 2; dx++) {
      const p = ((2 * y + dy) * img.width + 2 * x + dx) * 4, a = img.data[p + 3] / 255;
      acc[0] += img.data[p] * a; acc[1] += img.data[p + 1] * a; acc[2] += img.data[p + 2] * a; acc[3] += a;
    }
    const d = (y * W + x) * 4;
    if (acc[3] > 0) for (let c = 0; c < 3; c++) out[d + c] = Math.round(acc[c] / acc[3]);
    out[d + 3] = Math.round(acc[3] / 4 * 255);
  }
  return { width: W, height: H, data: out };
}

// Alpha-over of img (tinted by rgb 0..1) onto canvas at (ox, oy).
function over(canvas, img, ox, oy, tint) {
  for (let y = 0; y < img.height; y++) for (let x = 0; x < img.width; x++) {
    const p = (y * img.width + x) * 4, a = img.data[p + 3] / 255;
    if (a <= 0) continue;
    const d = ((oy + y) * canvas.width + ox + x) * 4;
    for (let c = 0; c < 3; c++) canvas.data[d + c] = Math.round(img.data[p + c] * tint[c] * a + canvas.data[d + c] * (1 - a));
  }
}

function outline(canvas, ox, oy, o) {
  const put = (x, y) => {
    const d = ((oy + y) * canvas.width + ox + x) * 4;
    canvas.data[d] = 255; canvas.data[d + 1] = 0; canvas.data[d + 2] = 255; canvas.data[d + 3] = 255;
  };
  for (let x = o.l; x <= o.r; x++) { put(x, o.t); put(x, o.b); }
  for (let y = o.t; y <= o.b; y++) { put(o.l, y); put(o.r, y); }
}

function main() {
  const s = arg('scale', 0.5), W = arg('width', 1000), H = arg('height', 1440);
  const baked = {}, ops = {}, report = { scale: s, size: `${W}x${H}`, frames: {} };
  for (const [name, m] of LAYERS) {
    const src = decodePng(path.join(DIR, name + '.png'));
    if (src.width !== 1086 || src.height !== 1448) throw new Error(`${name}: expected 1086 x 1448`);
    const img = bake(src, m, W, H, s);
    encodePng(path.join(DIR, name + '_Baked.png'), img);
    baked[name] = img;
    const op = opening(img);
    ops[name] = op;
    report.frames[name] = {
      corners_1x: { left: m.l * s / 2, top: m.t * s / 2, right: m.r * s / 2, bottom: m.b * s / 2 },
      sideBand_baked_px: sideBand(img), sideBand_1x: sideBand(img) / 2,
      opening_inset_baked_px: op.inset,
      opening_inset_1x: Object.fromEntries(Object.entries(op.inset).map(([k, v]) => [k, v / 2])),
    };
  }
  // Control sheet at 1x.
  const cw = W / 2, ch = H / 2, gap = 20;
  const tints = [[.55, .55, .55], [.2, .8, .25], [.2, .45, 1], [.65, .25, .9], [1, .85, .1]];
  const sheetW = gap + 6 * (cw + gap), sheetH = ch + 2 * gap;
  const sheet = { width: sheetW, height: sheetH, data: new Uint8Array(sheetW * sheetH * 4) };
  for (let i = 0; i < sheetW * sheetH; i++) sheet.data.set([0x15, 0x17, 0x1c, 255], i * 4);
  const half = Object.fromEntries(Object.entries(baked).map(([k, v]) => [k, down2(v)]));
  tints.forEach((t, i) => {
    const ox = gap + i * (cw + gap), oy = gap;
    over(sheet, half.T_CardFrame_Stat, ox, oy, [1, 1, 1]);
    over(sheet, half.T_CardFrame_Stat_Glow, ox, oy, t);
    over(sheet, half.T_CardFrame_Stat_Accents, ox, oy, t);
  });
  over(sheet, half.T_CardFrame_Weapon, gap + 5 * (cw + gap), gap, [1, 1, 1]);
  const o = (n) => { const q = ops[n]; return { l: Math.round(q.l / 2), t: Math.round(q.t / 2), r: Math.round(q.r / 2), b: Math.round(q.b / 2) }; };
  outline(sheet, gap, gap, o('T_CardFrame_Stat'));
  outline(sheet, gap + 5 * (cw + gap), gap, o('T_CardFrame_Weapon'));
  encodePng(path.join(REPO, 'Art', 'Source', 'card-frames', 'rarity-examples', 'baked-control.png'), sheet);
  console.log(JSON.stringify(report, null, 1));
}

main();
