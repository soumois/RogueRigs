// Rogue Rigs - one command for the -game captures of the game screens (task-549-01, owner 2026-11-13: "generalise the UI
// habits and scripts"). Tasks 540, 545, 546, 547 and 548 each rebuilt the same -ExecCmds by hand to reach a screen; the
// recipes live here once, and a before/after pair of runs is compared screen by screen.
// Node 18+, no dependencies; runs capture.js (one -game run per group) and sheet.js, ffmpeg found by ffmpeg.js.
//
// Usage: node Tools/capture/ui-screens.js --name <Name> [--screens a,b,...] [--res 1920x1080] [--compare <BeforeName>]
//          [--timeout 240] [--list]
//   --name     output folder Saved/Captures/<Name>/: <screen>.png per screen, one sub-folder per run (run.log, raw shots)
//   --screens  subset of the screens below (default all); a run is started for each group that holds a requested screen
//   --res      window size, passed with --force-res (default 1920x1080)
//   --compare  folder name of an earlier run (Saved/Captures/<BeforeName>/): writes compare-<screen>.png (before | after)
//              in <Name>/ and prints the PSNR in dB of each pair (inf = identical pixels; above ~35 dB the eye sees no change)
//   --list     prints the screens and their groups, starts nothing
// Exit code: 0 every requested screen captured (and compared), 1 a run or a comparison failed, 2 bad arguments.
//
// Screens are reached with the rr.* debug commands only (Tools/capture/commands.js lists them); the cursor is parked in a
// corner (rr.Debug.Cursor 0.97 0.97) so no button shows its hover. Cards of the shop are random per run: compare the frame,
// layout and texts, not the card faces. Times are real seconds after the map opened.

const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');
const { findFfmpeg } = require('./ffmpeg');

const REPO = path.resolve(__dirname, '..', '..');
const CAPTURES = path.join(REPO, 'Saved', 'Captures');
const WEAPONS = '/Game/RogueRigs/Blueprints/Weapons';
const shot = (at, prefix) => `rr.Debug.Delayed ${at} rr.Debug.Shots 0 1 1 ${prefix} 1`;

// One group = one -game run. shots: screen -> screenshot prefix (rr.Debug.Shots names files <prefix>_00.png).
const GROUPS = {
  menu: {
    map: 'none',
    shots: { mainmenu: 'uimm' },
    exec: ['rr.Debug.Delayed 5 rr.Debug.Cursor 0.97 0.97', shot(7, 'uimm'), 'rr.Debug.Delayed 9 Quit'],
  },
  pause: {
    // Three weapons so the pause grid shows both weapon columns and the horizontal divider (task-545).
    shots: { pause: 'uipm', options: 'uiop' },
    exec: ['rr.Cheat.Invulnerable 1', `rr.Debug.Delayed 3 rr.Debug.AddWeapon ${WEAPONS}/BP_Shotgun.BP_Shotgun_C`,
      `rr.Debug.Delayed 3.5 rr.Debug.AddWeapon ${WEAPONS}/BP_TurretLauncher.BP_TurretLauncher_C`,
      'rr.Debug.Delayed 5 rr.Debug.PauseMenu open', 'rr.Debug.Delayed 5.5 rr.Debug.Cursor 0.97 0.97', shot(7, 'uipm'),
      'rr.Debug.Delayed 8 rr.Options.Open graphics', 'rr.Debug.Delayed 8.5 rr.Debug.Cursor 0.97 0.97', shot(10, 'uiop'),
      'rr.Debug.Delayed 12 Quit'],
  },
  levelend: {
    // Stats screen after its entrance (about 1.4 s), then the shop with one card bought (SOLD slot shown, task-548).
    shots: { stats: 'uist', shop: 'uish' },
    exec: ['rr.Cheat.Invulnerable 1', 'rr.Debug.Delayed 4 rr.Cheat.GrantOre 70', 'rr.Debug.Delayed 5 rr.Run.EndLevel',
      'rr.Debug.Delayed 5.5 rr.Debug.Cursor 0.97 0.97', shot(9, 'uist'), 'rr.Debug.Delayed 10 rr.Run.Stats.Next',
      'rr.Debug.Delayed 12 rr.Shop.Buy 0', 'rr.Debug.Delayed 12.5 rr.Debug.Cursor 0.97 0.97', shot(14, 'uish'),
      'rr.Debug.Delayed 16 Quit'],
  },
  death: {
    shots: { death: 'uirl' },
    exec: ['rr.Cheat.Invulnerable 0', 'rr.Debug.Delayed 4 rr.Run.Lost.Die', 'rr.Debug.Delayed 6 rr.Debug.Cursor 0.97 0.97',
      shot(11, 'uirl'), 'rr.Debug.Delayed 13 Quit'],
  },
  runwon: {
    // Level 4 stats screen (WBP_Intermission with bRunWon, CASH OUT): three levels skipped, then the last one ended.
    shots: { runwon: 'uirw' },
    exec: ['rr.Cheat.Invulnerable 1', 'rr.Debug.Delayed 3 rr.Run.NextLevel', 'rr.Debug.Delayed 15 rr.Run.NextLevel',
      'rr.Debug.Delayed 27 rr.Run.NextLevel', 'rr.Debug.Delayed 39 rr.Run.EndLevel', 'rr.Debug.Delayed 39.5 rr.Debug.Cursor 0.97 0.97',
      shot(43, 'uirw'), 'rr.Debug.Delayed 45 Quit'],
  },
};
const SCREENS = Object.fromEntries(Object.entries(GROUPS).flatMap(([g, def]) => Object.keys(def.shots).map((s) => [s, g])));

function usage(msg) {
  if (msg) console.error(`ui-screens.js: ${msg}`);
  console.error('Usage: node Tools/capture/ui-screens.js --name <Name> [--screens a,b] [--res 1920x1080] [--compare <BeforeName>] [--timeout s] [--list]');
  console.error(`Screens: ${Object.keys(SCREENS).join(', ')}`);
  process.exit(2);
}

function parseArgs(argv) {
  const out = {};
  for (let i = 0; i < argv.length; i++) {
    const a = argv[i];
    if (!a.startsWith('--')) usage(`unexpected argument ${a}`);
    if (a === '--list') { out.list = true; continue; }
    const v = argv[i + 1];
    if (v === undefined || v.startsWith('--')) usage(`missing value for ${a}`);
    out[a.slice(2)] = v;
    i++;
  }
  return out;
}

// PSNR of two same-size images in dB (ffmpeg psnr filter); null when ffmpeg is missing or fails.
function psnr(ffmpeg, a, b) {
  const r = spawnSync(ffmpeg, ['-hide_banner', '-i', a, '-i', b, '-lavfi', 'psnr', '-f', 'null', '-'], { encoding: 'utf8' });
  const m = (r.stderr || '').match(/average:(inf|[0-9.]+)/);
  return m ? m[1] : null;
}

function main() {
  const a = parseArgs(process.argv.slice(2));
  if (a.list) {
    for (const [g, def] of Object.entries(GROUPS)) console.log(`${g}: ${Object.keys(def.shots).join(', ')} (map ${def.map || 'L_ArenaM1'})`);
    return 0;
  }
  if (!a.name || !/^[A-Za-z0-9_.-]+$/.test(a.name)) usage('--name is required (letters, digits, _ . -)');
  const screens = a.screens ? a.screens.split(',').filter(Boolean) : Object.keys(SCREENS);
  for (const s of screens) if (!SCREENS[s]) usage(`unknown screen ${s}`);
  const res = a.res || '1920x1080';
  if (!/^\d+x\d+$/.test(res)) usage('--res must be WxH');
  const outDir = path.join(CAPTURES, a.name);
  fs.mkdirSync(outDir, { recursive: true });

  let failed = false;
  const groups = [...new Set(screens.map((s) => SCREENS[s]))];
  for (const g of groups) {
    const def = GROUPS[g];
    const runName = `${a.name}__${g}`; // capture.js wipes and fills Saved/Captures/<runName>/
    const prefixes = Object.values(def.shots);
    const args = [path.join(__dirname, 'capture.js'), '--name', runName, '--exec', def.exec.join(','), '--prefix', prefixes.join(','),
      '--expect', String(prefixes.length), '--res', res, '--force-res', '--no-sheet', '--timeout', a.timeout || '240', '--max-lines', '8'];
    if (def.map) args.push('--map', def.map);
    console.log(`run ${g} (${Object.keys(def.shots).join(', ')}) ...`);
    const r = spawnSync(process.execPath, args, { encoding: 'utf8' });
    const runDir = path.join(CAPTURES, runName);
    const dest = path.join(outDir, g);
    fs.rmSync(dest, { recursive: true, force: true });
    if (fs.existsSync(runDir)) fs.renameSync(runDir, dest);
    if (r.status !== 0) { failed = true; console.log(`  run ${g} exit ${r.status}: see ${path.join(dest, 'run.log')}`); }
    for (const [screen, prefix] of Object.entries(def.shots)) {
      if (!screens.includes(screen)) continue;
      const src = fs.existsSync(dest) ? fs.readdirSync(dest).find((f) => f.startsWith(prefix) && f.endsWith('.png')) : null;
      if (!src) { failed = true; console.log(`  ${screen}: no screenshot`); continue; }
      fs.copyFileSync(path.join(dest, src), path.join(outDir, `${screen}.png`));
      console.log(`  ${screen}: ${path.join(outDir, `${screen}.png`)}`);
    }
  }

  if (a.compare) {
    const beforeDir = path.join(CAPTURES, a.compare);
    const ffmpeg = findFfmpeg();
    for (const s of screens) {
      const before = path.join(beforeDir, `${s}.png`);
      const after = path.join(outDir, `${s}.png`);
      if (!fs.existsSync(before) || !fs.existsSync(after)) { failed = true; console.log(`compare ${s}: missing frame`); continue; }
      const sheet = path.join(outDir, `compare-${s}.png`);
      const r = spawnSync(process.execPath, [path.join(__dirname, 'sheet.js'), '--out', sheet, '--cols', '2', '--height', '540',
        `before ${s}=${before}`, `after ${s}=${after}`], { encoding: 'utf8' });
      if (r.status !== 0) failed = true;
      const db = ffmpeg ? psnr(ffmpeg, before, after) : null;
      console.log(`compare ${s}: PSNR ${db === null ? 'unknown' : `${db} dB`}, ${sheet}`);
    }
  }
  return failed ? 1 : 0;
}

process.exit(main());
