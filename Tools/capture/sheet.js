// Rogue Rigs - labelled comparison sheet and crops from existing PNGs (task-359-01, owner 2026-10-29, Q6 of task-359).
// Replaces the ad-hoc ffmpeg commands of capture runs (before / after / mockup sheets, zoomed crops of one detail).
// ffmpeg is found by Tools/capture/ffmpeg.js (PATH, else the WinGet package): no ffmpeg path argument.
// Node 18+, no dependencies, like capture.js.
//
// Usage: node Tools/capture/sheet.js --out <png> [--cols N] [--height H] [--crop x,y,w,h] [--zoom Z] [--no-crops] <label>=<image> | <image> ...
//   <label>=<image>  one sheet cell, label drawn in a band above it; a bare <image> is labelled with its file name.
//                    Any format ffmpeg reads (png, jpg, webp mockups). Labels keep letters, digits, space and _ . - + x # ( ) /.
//   --out     sheet file (folders created). Cells in argument order, left to right, top to bottom.
//   --cols    cells per row (default: the image count, at most 3)
//   --height  cell image height in pixels (default 540 without --crop, the crop height of the first image with it), times --zoom
//   --crop    x,y,w,h applied to every image before scaling: four values all <= 1 are fractions of each frame (one crop fits
//             every resolution: 0.25,0.1,0.5,0.4), else pixels (clamped to the frame).
//   --zoom    scale factor of the cell height (default 1). With --crop, also the scale of each crop file.
//   --no-crops  with --crop, write the sheet only
// With --crop, each image's crop is also written next to the sheet: <out name>-<label>.png (crop size times --zoom).
// Output: 2 to 3 lines (sheet path, size, order; crop files). Exit code: 0 done, 1 ffmpeg missing or failed, 2 bad arguments.

const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');
const { findFfmpeg, imageSize } = require('./ffmpeg');

const REPO = path.resolve(__dirname, '..', '..');
const FONTS = ['C:/Windows/Fonts/arialbd.ttf', 'C:/Windows/Fonts/arial.ttf', 'C:/Windows/Fonts/segoeui.ttf'];
const BACKGROUND = '0x202020';

function usage(msg) {
  if (msg) console.error(`sheet.js: ${msg}`);
  console.error('Usage: node Tools/capture/sheet.js --out <png> [--cols N] [--height H] [--crop x,y,w,h] [--zoom Z] [--no-crops] <label>=<image> | <image> ...');
  process.exit(2);
}

function parseArgs(argv) {
  const flags = ['no-crops'];
  const out = { images: [] };
  for (let i = 0; i < argv.length; i++) {
    const a = argv[i];
    if (!a.startsWith('--')) { out.images.push(a); continue; }
    const name = a.slice(2);
    if (flags.includes(name)) { out[name] = true; continue; }
    const value = argv[i + 1];
    if (value === undefined || value.startsWith('--')) usage(`missing value for --${name}`);
    out[name] = value;
    i++;
  }
  return out;
}

// "label=path" or "path" (a path that exists as given is never split, so a file name holding "=" still works).
function parseImage(arg) {
  let label = null;
  let file = arg;
  const eq = arg.indexOf('=');
  if (!fs.existsSync(arg) && eq > 0) { label = arg.slice(0, eq); file = arg.slice(eq + 1); }
  file = path.resolve(file);
  if (!fs.existsSync(file)) usage(`image not found: ${arg}`);
  if (label === null) label = path.basename(file, path.extname(file));
  // drawtext option syntax: keep a safe set (no quote, colon, comma, backslash, percent).
  label = label.replace(/:/g, ' -').replace(/[^A-Za-z0-9 _.\-+x#()/]/g, ' ').replace(/\s+/g, ' ').trim() || 'image';
  return { label, file };
}

function parseCrop(text) {
  const v = text.split(',').map(Number);
  if (v.length !== 4 || v.some((x) => !Number.isFinite(x) || x < 0)) usage(`--crop needs x,y,w,h, got ${text}`);
  return { v, fraction: v.every((x) => x <= 1) };
}

// Crop rectangle in pixels of one image, clamped to it, even sizes for the encoders.
function cropRect(crop, size) {
  const [x, y, w, h] = crop.fraction ? [crop.v[0] * size.w, crop.v[1] * size.h, crop.v[2] * size.w, crop.v[3] * size.h] : crop.v;
  const cx = Math.min(Math.max(0, Math.round(x)), size.w - 2);
  const cy = Math.min(Math.max(0, Math.round(y)), size.h - 2);
  const cw = Math.max(2, Math.min(Math.round(w), size.w - cx));
  const ch = Math.max(2, Math.min(Math.round(h), size.h - cy));
  return { x: cx, y: cy, w: cw, h: ch };
}

const even = (n) => Math.max(2, 2 * Math.round(n / 2));
const rel = (p) => { const r = path.relative(REPO, p); return (r.startsWith('..') ? p : r).replace(/\\/g, '/'); };

function runFfmpeg(ffmpeg, args) {
  const r = spawnSync(ffmpeg, ['-y', '-hide_banner', '-loglevel', 'error', ...args], { encoding: 'utf8' });
  if (r.status === 0) return '';
  const last = (r.stderr || r.error?.message || 'unknown error').trim().split('\n').pop();
  return last.length > 160 ? `${last.slice(0, 157)}...` : last;
}

function main() {
  const a = parseArgs(process.argv.slice(2));
  if (!a.out) usage('--out is required');
  if (a.images.length === 0) usage('give at least one <label>=<image>');
  const ffmpeg = findFfmpeg();
  if (!ffmpeg) { console.error('sheet.js: ffmpeg not found (PATH or WinGet package Gyan.FFmpeg)'); process.exit(1); }

  const cells = a.images.map(parseImage);
  const crop = a.crop ? parseCrop(a.crop) : null;
  const zoom = Number(a.zoom || 1);
  if (!(zoom > 0)) usage('--zoom must be > 0');
  for (const c of cells) {
    c.size = imageSize(ffmpeg, c.file);
    if (!c.size) usage(`cannot read the size of ${c.file}`);
    c.rect = crop ? cropRect(crop, c.size) : { x: 0, y: 0, w: c.size.w, h: c.size.h };
  }
  const n = cells.length;
  const cols = Math.max(1, Math.min(Number(a.cols || Math.min(n, 3)), n));
  const rows = Math.ceil(n / cols);
  const cellH = even((a.height ? Number(a.height) : crop ? cells[0].rect.h : 540) * zoom);
  for (const c of cells) c.scaledW = even((cellH * c.rect.w) / c.rect.h);
  const cellW = Math.max(...cells.map((c) => c.scaledW));
  const band = even(Math.max(24, cellH * 0.07));
  const fontSize = Math.round(band * 0.62);
  const font = FONTS.find((f) => fs.existsSync(f));
  const outFile = path.resolve(a.out);
  fs.mkdirSync(path.dirname(outFile), { recursive: true });

  // One chain per image: crop, scale to the common height, centre in a common cell, label band on top.
  const chains = cells.map((c, i) => {
    const parts = [];
    if (crop) parts.push(`crop=${c.rect.w}:${c.rect.h}:${c.rect.x}:${c.rect.y}`);
    parts.push(`scale=${c.scaledW}:${cellH}:flags=lanczos`, 'format=rgb24');
    parts.push(`pad=${cellW}:${cellH + band}:(ow-iw)/2:${band}:color=${BACKGROUND}`);
    if (font) {
      parts.push(`drawtext=fontfile='${font.replace(':', '\\:')}':text='${c.label}':expansion=none:fontcolor=white:fontsize=${fontSize}:x=(w-text_w)/2:y=(${band}-text_h)/2`);
    }
    return `[${i}:v]${parts.join(',')}[c${i}]`;
  });
  let graph = chains.join(';');
  if (n === 1) graph += ';[c0]null[out]';
  else {
    const layout = cells.map((_, i) => `${(i % cols) * cellW}_${Math.floor(i / cols) * (cellH + band)}`).join('|');
    graph += `;${cells.map((_, i) => `[c${i}]`).join('')}xstack=inputs=${n}:layout=${layout}:fill=${BACKGROUND}[out]`;
  }
  const inputs = cells.flatMap((c) => ['-i', c.file]);
  const err = runFfmpeg(ffmpeg, [...inputs, '-filter_complex', graph, '-map', '[out]', '-frames:v', '1', outFile]);
  if (err) { console.error(`sheet.js: ffmpeg failed on the sheet: ${err}`); process.exit(1); }
  console.log(`sheet: ${rel(outFile)} | ${n} image(s), ${cols} col(s) x ${rows} row(s), cell ${cellW}x${cellH + band} px${crop ? `, crop ${a.crop}${crop.fraction ? ' (fractions)' : ' (px)'}` : ''}${zoom !== 1 ? `, zoom ${zoom}` : ''}${font ? '' : ', no font: labels skipped'}`);
  console.log(`order: ${cells.map((c) => c.label).join(' | ')}`);

  if (crop && !a['no-crops']) {
    const stem = outFile.replace(/\.[^.\\/]+$/, '');
    const written = [];
    for (const c of cells) {
      const file = `${stem}-${c.label.replace(/[^A-Za-z0-9_.\-+#()]+/g, '_')}.png`;
      const vf = `crop=${c.rect.w}:${c.rect.h}:${c.rect.x}:${c.rect.y},scale=${even(c.rect.w * zoom)}:${even(c.rect.h * zoom)}:flags=lanczos`;
      const e = runFfmpeg(ffmpeg, ['-i', c.file, '-vf', vf, '-frames:v', '1', file]);
      if (e) { console.error(`sheet.js: ffmpeg failed on the crop of ${c.label}: ${e}`); process.exit(1); }
      written.push(`${path.basename(file)} ${even(c.rect.w * zoom)}x${even(c.rect.h * zoom)}`);
    }
    console.log(`crops: ${written.length} in ${rel(path.dirname(outFile))} (${written.join(', ')})`);
  }
}

main();
