// Cuts a flat-background icon image into a transparent, trimmed, centred UI icon.
// Node 18+, no dependencies (PNG codec on the built-in zlib), like lib.js.
//
// Usage:
//   node Tools/art/ui_icon_prepare.js --in <png> --name <icon_name> [--size 256] [--margin 0.1]
//        [--inner 40] [--outer 110] [--naive]
//   node Tools/art/ui_icon_prepare.js --check <png>...
//
// Output: Art/UI/Icons/T_Icon_<name>.png (8-bit RGBA).
// Method (background = flat magenta, e.g. #FF00FF):
//   1. background colour = mean of four 8 x 8 corner patches;
//   2. alpha = min(distance key, colour-difference key):
//      distance key  = smoothstep(inner, outer, |rgb - bg|);
//      difference key = 1 - (min(R,B) - G) / (min(bgR,bgB) - bgG) (catches light
//      added over magenta, e.g. a muzzle glow, that the distance key keeps opaque);
//   3. unmix: colour = (c - (1 - a) * bg) / a, then despill: the magenta excess
//      min(R,B) - G, when positive, is removed from R and B (no pink fringe);
//   4. trim to the alpha bounding box (alpha > 2), area-resample with premultiplied
//      alpha to fit size * (1 - 2 * margin), centre on a transparent canvas.
// --naive: distance key only, no unmix or despill (used to measure the halo it leaves).
// --check: prints size, alpha, corners, content bbox and margins, magenta-like pixels.

const fs = require('fs');
const path = require('path');
const zlib = require('zlib');
const L = require('./lib');

L.setUsage('node Tools/art/ui_icon_prepare.js --in <png> --name <icon_name> [--size 256] [--margin 0.1] [--inner 40] [--outer 110] [--naive]\n'
  + '       node Tools/art/ui_icon_prepare.js --check <png>...');

// ---------- PNG codec (8-bit, non-interlaced; grey, grey+alpha, RGB, RGBA, palette) ----------

const CRC_TABLE = (() => {
  const t = new Uint32Array(256);
  for (let n = 0; n < 256; n++) {
    let c = n;
    for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
    t[n] = c >>> 0;
  }
  return t;
})();

function crc32(buf) {
  let c = 0xffffffff;
  for (let i = 0; i < buf.length; i++) c = CRC_TABLE[(c ^ buf[i]) & 0xff] ^ (c >>> 8);
  return (c ^ 0xffffffff) >>> 0;
}

// Returns { width, height, data: Uint8Array RGBA }.
function decodePng(file) {
  const buf = fs.readFileSync(file);
  if (buf.readUInt32BE(0) !== 0x89504e47) throw new Error(`${file}: not a PNG`);
  let pos = 8, width = 0, height = 0, depth = 0, type = 0, interlace = 0, palette = null, trns = null;
  const idat = [];
  while (pos < buf.length) {
    const len = buf.readUInt32BE(pos);
    const kind = buf.toString('ascii', pos + 4, pos + 8);
    const body = buf.subarray(pos + 8, pos + 8 + len);
    if (kind === 'IHDR') {
      width = body.readUInt32BE(0); height = body.readUInt32BE(4);
      depth = body[8]; type = body[9]; interlace = body[12];
    } else if (kind === 'PLTE') palette = body;
    else if (kind === 'tRNS') trns = body;
    else if (kind === 'IDAT') idat.push(body);
    else if (kind === 'IEND') break;
    pos += 12 + len;
  }
  if (depth !== 8 || interlace !== 0) throw new Error(`${file}: only 8-bit non-interlaced PNG is supported`);
  const channels = { 0: 1, 2: 3, 3: 1, 4: 2, 6: 4 }[type];
  if (!channels) throw new Error(`${file}: unsupported colour type ${type}`);
  const raw = zlib.inflateSync(Buffer.concat(idat));
  const stride = width * channels;
  const px = new Uint8Array(stride * height);
  for (let y = 0; y < height; y++) {
    const f = raw[y * (stride + 1)];
    const src = y * (stride + 1) + 1, dst = y * stride, prev = dst - stride;
    for (let x = 0; x < stride; x++) {
      const a = x >= channels ? px[dst + x - channels] : 0;
      const b = y > 0 ? px[prev + x] : 0;
      const c = y > 0 && x >= channels ? px[prev + x - channels] : 0;
      let v = raw[src + x];
      if (f === 1) v += a;
      else if (f === 2) v += b;
      else if (f === 3) v += (a + b) >> 1;
      else if (f === 4) {
        const p = a + b - c, pa = Math.abs(p - a), pb = Math.abs(p - b), pc = Math.abs(p - c);
        v += pa <= pb && pa <= pc ? a : pb <= pc ? b : c;
      }
      px[dst + x] = v & 0xff;
    }
  }
  const data = new Uint8Array(width * height * 4);
  for (let i = 0; i < width * height; i++) {
    const s = i * channels, d = i * 4;
    if (type === 0) { data[d] = data[d + 1] = data[d + 2] = px[s]; data[d + 3] = 255; }
    else if (type === 4) { data[d] = data[d + 1] = data[d + 2] = px[s]; data[d + 3] = px[s + 1]; }
    else if (type === 2) { data[d] = px[s]; data[d + 1] = px[s + 1]; data[d + 2] = px[s + 2]; data[d + 3] = 255; }
    else if (type === 6) { data.set(px.subarray(s, s + 4), d); }
    else {
      const k = px[s];
      data[d] = palette[k * 3]; data[d + 1] = palette[k * 3 + 1]; data[d + 2] = palette[k * 3 + 2];
      data[d + 3] = trns && k < trns.length ? trns[k] : 255;
    }
  }
  return { width, height, data };
}

function chunk(kind, body) {
  const head = Buffer.alloc(8);
  head.writeUInt32BE(body.length, 0);
  head.write(kind, 4, 'ascii');
  const crc = Buffer.alloc(4);
  crc.writeUInt32BE(crc32(Buffer.concat([head.subarray(4), body])), 0);
  return Buffer.concat([head, body, crc]);
}

// Writes 8-bit RGBA, filter 0 per row (small images, size is not a concern).
function encodePng(file, { width, height, data }) {
  const ihdr = Buffer.alloc(13);
  ihdr.writeUInt32BE(width, 0); ihdr.writeUInt32BE(height, 4);
  ihdr[8] = 8; ihdr[9] = 6; ihdr[10] = 0; ihdr[11] = 0; ihdr[12] = 0;
  const raw = Buffer.alloc((width * 4 + 1) * height);
  for (let y = 0; y < height; y++) {
    raw[y * (width * 4 + 1)] = 0;
    Buffer.from(data.buffer, data.byteOffset + y * width * 4, width * 4).copy(raw, y * (width * 4 + 1) + 1);
  }
  const sig = Buffer.from([0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a]);
  fs.mkdirSync(path.dirname(file), { recursive: true });
  fs.writeFileSync(file, Buffer.concat([sig, chunk('IHDR', ihdr),
    chunk('IDAT', zlib.deflateSync(raw, { level: 9 })), chunk('IEND', Buffer.alloc(0))]));
}

// ---------- Cut-out ----------

function cornerBackground({ width, height, data }) {
  const n = 8, sum = [0, 0, 0];
  let count = 0;
  for (const [cx, cy] of [[0, 0], [width - n, 0], [0, height - n], [width - n, height - n]]) {
    for (let y = cy; y < cy + n; y++) {
      for (let x = cx; x < cx + n; x++) {
        const i = (y * width + x) * 4;
        sum[0] += data[i]; sum[1] += data[i + 1]; sum[2] += data[i + 2]; count++;
      }
    }
  }
  return sum.map((s) => s / count);
}

function smoothstep(e0, e1, x) {
  const t = Math.min(1, Math.max(0, (x - e0) / (e1 - e0)));
  return t * t * (3 - 2 * t);
}

// Returns float RGBA (colour 0..255 straight, alpha 0..1).
function cutOut(img, bg, { inner, outer, naive }) {
  const { width, height, data } = img;
  const out = new Float32Array(width * height * 4);
  const bgSpill = Math.min(bg[0], bg[2]) - bg[1];   // magenta excess of the background
  const useDiff = !naive && bgSpill > 64;
  for (let i = 0; i < width * height; i++) {
    const p = i * 4;
    const r = data[p], g = data[p + 1], b = data[p + 2];
    const dist = Math.hypot(r - bg[0], g - bg[1], b - bg[2]);
    let a = smoothstep(inner, outer, dist);
    if (useDiff) a = Math.min(a, Math.min(1, Math.max(0, 1 - (Math.min(r, b) - g) / bgSpill)));
    let c = [r, g, b];
    if (!naive && a > 0) {
      if (a < 1) c = c.map((v, k) => Math.min(255, Math.max(0, (v - (1 - a) * bg[k]) / a)));
      const spill = Math.min(c[0], c[2]) - c[1];
      if (spill > 0) { c[0] -= spill; c[2] -= spill; }
    }
    out[p] = c[0]; out[p + 1] = c[1]; out[p + 2] = c[2]; out[p + 3] = a;
  }
  return out;
}

function alphaBox(px, width, height, minAlpha) {
  let x0 = width, y0 = height, x1 = -1, y1 = -1;
  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++) {
      if (px[(y * width + x) * 4 + 3] > minAlpha) {
        if (x < x0) x0 = x; if (x > x1) x1 = x; if (y < y0) y0 = y; if (y > y1) y1 = y;
      }
    }
  }
  return x1 < 0 ? null : { x0, y0, x1, y1, w: x1 - x0 + 1, h: y1 - y0 + 1 };
}

// Area-resample weights from srcLen to dstLen: for each destination cell, the list of [src, coverage].
function areaWeights(srcLen, dstLen) {
  const scale = srcLen / dstLen, w = [];
  for (let d = 0; d < dstLen; d++) {
    const s0 = d * scale, s1 = (d + 1) * scale, list = [];
    for (let s = Math.floor(s0); s < Math.min(srcLen, Math.ceil(s1)); s++) {
      const cov = Math.min(s1, s + 1) - Math.max(s0, s);
      if (cov > 0) list.push([s, cov / scale]);
    }
    w.push(list);
  }
  return w;
}

// Premultiplied area resample of a crop of float RGBA.
function resample(px, width, box, dw, dh) {
  const wx = areaWeights(box.w, dw), wy = areaWeights(box.h, dh);
  const tmp = new Float32Array(dw * box.h * 4);
  for (let y = 0; y < box.h; y++) {
    for (let x = 0; x < dw; x++) {
      const acc = [0, 0, 0, 0];
      for (const [s, k] of wx[x]) {
        const p = ((box.y0 + y) * width + box.x0 + s) * 4, a = px[p + 3];
        acc[0] += px[p] * a * k; acc[1] += px[p + 1] * a * k; acc[2] += px[p + 2] * a * k; acc[3] += a * k;
      }
      tmp.set(acc, (y * dw + x) * 4);
    }
  }
  const out = new Float32Array(dw * dh * 4);
  for (let y = 0; y < dh; y++) {
    for (let x = 0; x < dw; x++) {
      const acc = [0, 0, 0, 0];
      for (const [s, k] of wy[y]) {
        const p = (s * dw + x) * 4;
        for (let c = 0; c < 4; c++) acc[c] += tmp[p + c] * k;
      }
      out.set(acc, (y * dw + x) * 4);   // premultiplied
    }
  }
  return out;
}

function prepare(a) {
  const size = Number(a.size || 256), margin = Number(a.margin ?? 0.1);
  const opts = { inner: Number(a.inner || 40), outer: Number(a.outer || 110), naive: !!a.naive };
  const img = decodePng(a.in);
  const bg = cornerBackground(img);
  const cut = cutOut(img, bg, opts);
  const box = alphaBox(cut, img.width, img.height, 2 / 255);
  if (!box) throw new Error(`${a.in}: no content found`);
  const avail = Math.floor(size * (1 - 2 * margin));
  const scale = avail / Math.max(box.w, box.h);
  const dw = Math.max(1, Math.round(box.w * scale)), dh = Math.max(1, Math.round(box.h * scale));
  const small = resample(cut, img.width, box, dw, dh);
  const ox = Math.floor((size - dw) / 2), oy = Math.floor((size - dh) / 2);
  const data = new Uint8Array(size * size * 4);
  for (let y = 0; y < dh; y++) {
    for (let x = 0; x < dw; x++) {
      const s = (y * dw + x) * 4, d = ((oy + y) * size + ox + x) * 4, al = small[s + 3];
      if (al <= 0) continue;
      for (let c = 0; c < 3; c++) data[d + c] = Math.round(Math.min(255, small[s + c] / al));
      data[d + 3] = Math.round(Math.min(1, al) * 255);
    }
  }
  const outFile = a.out || path.join(L.REPO, 'Art', 'UI', 'Icons', `T_Icon_${a.name}.png`);
  encodePng(outFile, { width: size, height: size, data });
  console.log(`${path.relative(L.REPO, outFile)}: bg rgb(${bg.map((v) => v.toFixed(0)).join(',')}), `
    + `source content ${box.w}x${box.h} at (${box.x0},${box.y0}), scaled to ${dw}x${dh} at (${ox},${oy})`);
}

// ---------- Check ----------

function check(file) {
  const img = decodePng(file), { width, height, data } = img;
  let alphaPx = 0, magentaNear = 0, pinkSpill = 0;
  for (let i = 0; i < width * height; i++) {
    const p = i * 4, r = data[p], g = data[p + 1], b = data[p + 2], al = data[p + 3];
    if (al < 255) alphaPx++;
    if (al <= 10) continue;
    if (Math.hypot(r - 255, g, b - 255) < 120) magentaNear++;      // close to #FF00FF
    if (Math.min(r, b) - g > 24) pinkSpill++;                       // magenta excess (pink tint)
  }
  const corners = [[0, 0], [width - 1, 0], [0, height - 1], [width - 1, height - 1]]
    .map(([x, y]) => data[(y * width + x) * 4 + 3]);
  const box = alphaBox(data, width, height, 0);
  const m = box ? [box.x0, box.y0, width - 1 - box.x1, height - 1 - box.y1] : [];
  console.log(JSON.stringify({
    file: path.basename(file), size: `${width}x${height}`, alphaPixels: alphaPx, cornerAlpha: corners,
    bbox: box && `${box.w}x${box.h} at (${box.x0},${box.y0})`, margins_LTRB: m,
    clipped: box ? m.some((v) => v <= 0) : null, magentaNear_a10: magentaNear, pinkSpill_a10: pinkSpill,
  }));
}

function main() {
  const argv = process.argv.slice(2);
  if (argv[0] === '--check') { argv.slice(1).forEach(check); return; }
  const a = L.parseArgs(argv, { flags: ['naive'] });
  if (!a.in || !fs.existsSync(a.in)) L.usage('--in is required and must exist');
  if (!a.out && !a.name) L.usage('--name is required');
  if (a.name && !/^[a-z0-9][a-z0-9_]*$/.test(a.name)) L.usage('--name must be snake_case (a-z, 0-9, _)');
  prepare(a);
}

if (require.main === module) main();
module.exports = { decodePng, encodePng };
