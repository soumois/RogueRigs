// Cuts ONE mascot image into a 96x96 icon, nearest-neighbour, flat #15171c background, character centred.
// Usage: node Tools/art/crew_icon_one.js --in <img.png> --out <icon.png> [--height 68]
const { decodePng, encodePng } = require('./ui_icon_prepare');
const arg = (n, d) => { const i = process.argv.indexOf('--' + n); return i > 0 ? process.argv[i + 1] : d; };
const inFile = arg('in'), outFile = arg('out'), H96 = Number(arg('height', 68));
if (!inFile || !outFile) { console.error('usage: --in img.png --out icon.png [--height 68]'); process.exit(1); }
const BG = [0x15, 0x17, 0x1c], SIZE = 96;
const { width: W, height: H, data } = decodePng(inFile);
let bg = [0, 0, 0]; for (const [x, y] of [[2, 2], [W - 3, 2], [2, H - 3], [W - 3, H - 3]]) for (let c = 0; c < 3; c++) bg[c] += data[(y * W + x) * 4 + c] / 4;
const dist = (i) => Math.hypot(data[i] - bg[0], data[i + 1] - bg[1], data[i + 2] - bg[2]);
let a = 1e9, b = 1e9, c = -1, d = -1;
for (let y = 0; y < H; y++) for (let x = 0; x < W; x++) if (dist((y * W + x) * 4) > 40) { a = Math.min(a, x); b = Math.min(b, y); c = Math.max(c, x); d = Math.max(d, y); }
const side = (d - b + 1) * SIZE / H96, cx = (a + c) / 2, cy = (b + d) / 2;
const out = new Uint8Array(SIZE * SIZE * 4);
for (let y = 0; y < SIZE; y++) for (let x = 0; x < SIZE; x++) {
  const sx = Math.floor(cx - side / 2 + (x + 0.5) * side / SIZE), sy = Math.floor(cy - side / 2 + (y + 0.5) * side / SIZE);
  const j = (y * SIZE + x) * 4; let px = BG;
  if (sx >= 0 && sy >= 0 && sx < W && sy < H) { const i = (sy * W + sx) * 4; if (dist(i) > 12) px = [data[i], data[i + 1], data[i + 2]]; }
  out[j] = px[0]; out[j + 1] = px[1]; out[j + 2] = px[2]; out[j + 3] = 255;
}
encodePng(outFile, { width: SIZE, height: SIZE, data: out });
console.log('bbox', a, b, c, d, 'side', Math.round(side));
