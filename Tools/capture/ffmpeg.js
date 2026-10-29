// Rogue Rigs - shared ffmpeg helpers of Tools/capture (task-359-01): capture.js and sheet.js find ffmpeg the same way,
// so no capture run searches the disk for it. Node 18+, no dependencies.
//
// findFfmpeg(): 'ffmpeg' when it runs from PATH, else the first ffmpeg.exe of a WinGet package
//               ($LOCALAPPDATA/Microsoft/WinGet/Packages/*ffmpeg*/<version>/bin/ffmpeg.exe), else null.
// imageSize(ffmpeg, file): { w, h } read from the PNG header, or from the ffmpeg stream line for other formats; null when unknown.

const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');

let cached;

function findFfmpeg() {
  if (cached !== undefined) return cached;
  cached = null;
  const probe = spawnSync('ffmpeg', ['-version'], { stdio: 'ignore' });
  if (!probe.error && probe.status === 0) return (cached = 'ffmpeg');
  const winget = path.join(process.env.LOCALAPPDATA || '', 'Microsoft', 'WinGet', 'Packages');
  if (fs.existsSync(winget)) {
    for (const d of fs.readdirSync(winget).filter((x) => /ffmpeg/i.test(x))) {
      const sub = path.join(winget, d);
      for (const v of fs.readdirSync(sub)) {
        const exe = path.join(sub, v, 'bin', 'ffmpeg.exe');
        if (fs.existsSync(exe)) return (cached = exe);
      }
    }
  }
  return cached;
}

function imageSize(ffmpeg, file) {
  const fd = fs.openSync(file, 'r');
  const head = Buffer.alloc(24);
  fs.readSync(fd, head, 0, 24, 0);
  fs.closeSync(fd);
  if (head.toString('latin1', 1, 4) === 'PNG' && head.toString('latin1', 12, 16) === 'IHDR') {
    return { w: head.readUInt32BE(16), h: head.readUInt32BE(20) };
  }
  if (!ffmpeg) return null;
  const r = spawnSync(ffmpeg, ['-hide_banner', '-i', file], { encoding: 'utf8' });
  const m = (r.stderr || '').match(/Video:.*?(\d{2,5})x(\d{2,5})/);
  return m ? { w: Number(m[1]), h: Number(m[2]) } : null;
}

module.exports = { findFfmpeg, imageSize };
