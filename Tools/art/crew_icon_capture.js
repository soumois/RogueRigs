// Draws the crew board's `capture` icon: a 32x32 pixel-art camera, written as a 96x96 RGBA PNG on the flat #15171c
// background the other mascots use. Usage: node capture_icon.js <out.png>
const fs = require('fs')
const zlib = require('zlib')

const G = 32
const S = 3
const BG = [0x15, 0x17, 0x1c]
const px = Array.from({ length: G }, () => Array.from({ length: G }, () => BG))
const rect = (x0, y0, x1, y1, c) => {
  for (let y = y0; y <= y1; y++) for (let x = x0; x <= x1; x++) px[y][x] = c
}
const disc = (cx, cy, r, c) => {
  for (let y = 0; y < G; y++) for (let x = 0; x < G; x++) if (Math.hypot(x - cx, y - cy) <= r) px[y][x] = c
}
const OUT = [0x0b, 0x4f, 0x4a]
const TEAL = [0x14, 0xb8, 0xa6]
const SHADE = [0x0f, 0x8a, 0x7e]
const LIGHT = [0x7e, 0xe0, 0xd4]
const LENS = [0x0b, 0x22, 0x33]
const GLASS = [0x38, 0xbd, 0xf8]
const WHITE = [0xe5, 0xe9, 0xf0]
const YELLOW = [0xfa, 0xcc, 0x15]

rect(10, 5, 19, 9, OUT) // viewfinder bump
rect(11, 6, 18, 9, TEAL)
rect(3, 9, 28, 26, OUT) // body
rect(4, 10, 27, 25, TEAL)
rect(4, 22, 27, 25, SHADE)
rect(4, 10, 27, 10, LIGHT)
rect(22, 12, 25, 13, YELLOW) // flash
rect(5, 12, 7, 13, WHITE) // shutter button side
disc(15.5, 18, 7.2, OUT) // lens
disc(15.5, 18, 6.2, LIGHT)
disc(15.5, 18, 5.2, LENS)
disc(15.5, 18, 3.2, GLASS)
disc(15.5, 18, 1.6, LENS)
rect(13, 15, 14, 16, WHITE) // glint

const W = G * S
const raw = Buffer.alloc(W * (W * 4 + 1))
for (let y = 0; y < W; y++) {
  raw[y * (W * 4 + 1)] = 0
  for (let x = 0; x < W; x++) {
    const c = px[Math.floor(y / S)][Math.floor(x / S)]
    const i = y * (W * 4 + 1) + 1 + x * 4
    raw[i] = c[0]
    raw[i + 1] = c[1]
    raw[i + 2] = c[2]
    raw[i + 3] = 255
  }
}
const crcTable = Array.from({ length: 256 }, (_, n) => {
  let c = n
  for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1
  return c >>> 0
})
const crc = buf => {
  let c = 0xffffffff
  for (const b of buf) c = crcTable[(c ^ b) & 255] ^ (c >>> 8)
  return (c ^ 0xffffffff) >>> 0
}
const chunk = (kind, data) => {
  const len = Buffer.alloc(4)
  len.writeUInt32BE(data.length)
  const body = Buffer.concat([Buffer.from(kind, 'ascii'), data])
  const sum = Buffer.alloc(4)
  sum.writeUInt32BE(crc(body))
  return Buffer.concat([len, body, sum])
}
const ihdr = Buffer.alloc(13)
ihdr.writeUInt32BE(W, 0)
ihdr.writeUInt32BE(W, 4)
ihdr[8] = 8
ihdr[9] = 6
fs.writeFileSync(
  process.argv[2],
  Buffer.concat([Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]), chunk('IHDR', ihdr), chunk('IDAT', zlib.deflateSync(raw)), chunk('IEND', Buffer.alloc(0))]),
)
console.log(`wrote ${process.argv[2]} (${W}x${W})`)
