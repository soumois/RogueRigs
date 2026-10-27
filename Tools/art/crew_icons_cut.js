// Cuts the crew-icons sheet (5 x 2) into ten 96x96 icons, nearest-neighbour, flat #15171c background.
// Usage: node Tools/art/crew_icons_cut.js --in <sheet.png> [--out Art/Source/crew-icons/icons]
const fs = require('fs');
const path = require('path');
const { decodePng, encodePng } = require('./ui_icon_prepare');
const arg = (n, d) => { const i = process.argv.indexOf('--' + n); return i > 0 ? process.argv[i + 1] : d; };
const inFile = arg('in'); const outDir = arg('out', 'Art/Source/crew-icons/icons');
if (!inFile) { console.error('usage: --in <sheet.png> [--out dir]'); process.exit(1); }
const names = ['cpp', 'bp', 'art', '3d', 'level-design', 'text', 'style-guide', 'planner', 'executor', 'claude'];
const BG = [0x15, 0x17, 0x1c], SIZE = 96, COLS = 5, ROWS = 2;
const img = decodePng(inFile); const { width: W, height: H, data } = img;
// background estimate: mean of the four corners
let bg = [0, 0, 0]; for (const [x, y] of [[2, 2], [W - 3, 2], [2, H - 3], [W - 3, H - 3]]) for (let c = 0; c < 3; c++) bg[c] += data[(y * W + x) * 4 + c] / 4;
const dist = (i) => Math.hypot(data[i] - bg[0], data[i + 1] - bg[1], data[i + 2] - bg[2]);
const T = 40; const cw = W / COLS, ch = H / ROWS;
const boxes = names.map((_, k) => {
  const x0 = Math.floor((k % COLS) * cw), x1 = Math.floor(((k % COLS) + 1) * cw), y0 = Math.floor(Math.floor(k / COLS) * ch), y1 = Math.floor((Math.floor(k / COLS) + 1) * ch);
  let a = 1e9, b = 1e9, c = -1, d = -1;
  for (let y = y0; y < y1; y++) for (let x = x0; x < x1; x++) if (dist((y * W + x) * 4) > T) { a = Math.min(a, x); b = Math.min(b, y); c = Math.max(c, x); d = Math.max(d, y); }
  return { a, b, c, d };
});
const side = Math.ceil(Math.max(...boxes.map(o => Math.max(o.c - o.a + 1, o.d - o.b + 1))) * 1.2);
fs.mkdirSync(outDir, { recursive: true });
names.forEach((n, k) => {
  const o = boxes[k]; const cx = (o.a + o.c) / 2, cy = (o.b + o.d) / 2; const out = new Uint8Array(SIZE * SIZE * 4);
  for (let y = 0; y < SIZE; y++) for (let x = 0; x < SIZE; x++) {
    const sx = Math.floor(cx - side / 2 + (x + 0.5) * side / SIZE), sy = Math.floor(cy - side / 2 + (y + 0.5) * side / SIZE);
    const j = (y * SIZE + x) * 4; let px = BG;
    if (sx >= 0 && sy >= 0 && sx < W && sy < H) { const i = (sy * W + sx) * 4; if (dist(i) > 12) px = [data[i], data[i + 1], data[i + 2]]; }
    out[j] = px[0]; out[j + 1] = px[1]; out[j + 2] = px[2]; out[j + 3] = 255;
  }
  encodePng(path.join(outDir, n + ".png"), { width: SIZE, height: SIZE, data: out });
});
console.log('side', side, 'bg', bg.map(Math.round));
