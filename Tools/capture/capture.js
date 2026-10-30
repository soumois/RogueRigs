// Rogue Rigs - one-command -game capture with a short report (task-319 cost analysis, owner 2026-10-26).
// Starts one -game run with the given -ExecCmds, waits for it to end (only the process it started is stopped on timeout),
// copies the screenshots written by rr.Debug.Shots, builds a contact sheet with ffmpeg, and prints a report of about 20 lines
// instead of the 2000-line engine log. The full log stays in <out>/run.log for a targeted grep.
// Node 18+, no dependencies, like vault/scripts/lint.js. Runs alongside an open editor (as robot_shots.ps1).
//
// Usage: node Tools/capture/capture.js --name <Name> --exec "<ExecCmds>" [--prefix a,b] [--expect N] [--keep <regex>] [--ignore <regex>]
//          [--map /Game/...] [--res 1600x900] [--force-res] [--timeout 240] [--max-lines 40] [--cols 3] [--no-sheet]
//   --force-res  passes -ForceRes: keeps --res even when it is larger than the desktop (the engine otherwise falls back
//                to a smaller windowed size, 1600x900 on a 1920x1080 desktop; task-356-03)
//   --name    output folder Saved/Captures/<Name>/ (run.log, copied PNGs, contact_sheet.png)
//   --exec    the -ExecCmds string, comma separated, ending with "rr.Debug.Delayed <s> Quit" (the script adds no Quit)
//   --prefix  screenshot name prefix(es) given to rr.Debug.Shots; without it every PNG written during the run is taken
//   --expect  number of screenshots expected; the report flags a mismatch
//   --keep    regex (case-insensitive): extra LogRogueRigs lines to keep besides rr.* debug lines
//   --ignore  regex (case-insensitive): drop any log line that matches, whatever its category
//   --map     map to open (default /Game/RogueRigs/Maps/L_ArenaM1); "none" passes no map, so GameDefaultMap (L_MainMenu) opens
//             From Git Bash, MSYS rewrites an argument starting with /Game/ into <drive>:/.../Git/[usr/]Game/... before node
//             starts (task-402). The script repairs it back to /Game/... (match on the /Git/ or /Git/usr/ segment followed by
//             Game/, case-insensitive) and prints "map: repaired MSYS path -> /Game/..."; same repair on such prefixes inside --exec.
// Kept in the report: rr.* debug lines and --keep matches of LogRogueRigs, every Error/Fatal/ensure line, and Warning lines
// of game categories (other warnings are only counted). Repeated lines are merged ("x12"), long lines cut at 160 characters.
// Exit code: 0 run ended and screenshot count as expected, 1 timeout, crash or missing screenshots, 2 bad arguments.

const fs = require('fs');
const path = require('path');
const { spawn, spawnSync } = require('child_process');
const { findFfmpeg } = require('./ffmpeg'); // shared with sheet.js (task-359-01)

const REPO = path.resolve(__dirname, '..', '..');
const ENGINE = 'C:\\Program Files\\Epic Games\\UE_5.8\\Engine\\Binaries\\Win64\\UnrealEditor.exe';
const UPROJECT = path.join(REPO, 'RogueRigs.uproject');
// Engine startup noise seen in every -game run (experimental Toolset plugins fail to import their Python): counted, never shown.
const NOISE_CATEGORIES = ['LogPython', 'LogEditorDataStorageUI'];
const GAME_CATEGORIES = ['LogRogueRigs', 'LogBlueprint', 'LogBlueprintUserMessages', 'LogScript', 'LogPlayLevel'];

function usage(msg) {
  if (msg) console.error(`capture.js: ${msg}`);
  console.error('Usage: node Tools/capture/capture.js --name <Name> --exec "<ExecCmds>" [--prefix a,b] [--expect N] [--keep <regex>] [--ignore <regex>] [--map <path>] [--res WxH] [--force-res] [--timeout s] [--max-lines N] [--cols N] [--no-sheet]');
  process.exit(2);
}

function parseArgs(argv) {
  const flags = ['no-sheet', 'force-res'];
  const out = {};
  for (let i = 0; i < argv.length; i++) {
    const a = argv[i];
    if (!a.startsWith('--')) usage(`unexpected argument ${a}`);
    const name = a.slice(2);
    if (flags.includes(name)) { out[name] = true; continue; }
    const value = argv[i + 1];
    if (value === undefined || value.startsWith('--')) usage(`missing value for --${name}`);
    out[name] = value;
    i++;
  }
  return out;
}

// Digits and engine object ids differ between otherwise identical lines: normalise them to merge repeats.
function normalise(text) {
  return text.replace(/_UAID_[0-9A-F]+_\d+/g, '_UAID').replace(/_C_\d+/g, '_C').replace(/-?\d+(\.\d+)?/g, '#');
}

function cut(text, n = 160) {
  return text.length > n ? `${text.slice(0, n - 3)}...` : text;
}

// "[2026.10.26-17.42.55:581][707]LogRogueRigs: Warning: text" -> { cat, sev, text }
function parseLine(raw) {
  const m = raw.match(/^(?:\[[^\]]*\]){0,2}(Log[A-Za-z0-9_]+): (?:(Error|Warning|Fatal|Display|Verbose|VeryVerbose): )?(.*)$/);
  if (!m) return { cat: '', sev: '', text: raw };
  return { cat: m[1], sev: m[2] || '', text: m[3] };
}

function findScreenshots(startMs, prefixes) {
  const root = path.join(REPO, 'Saved', 'Screenshots');
  const found = [];
  const walk = (dir) => {
    if (!fs.existsSync(dir)) return;
    for (const e of fs.readdirSync(dir, { withFileTypes: true })) {
      const p = path.join(dir, e.name);
      if (e.isDirectory()) { walk(p); continue; }
      if (!/\.png$/i.test(e.name)) continue;
      if (fs.statSync(p).mtimeMs < startMs - 1000) continue;
      if (prefixes.length && !prefixes.some((x) => e.name.startsWith(x))) continue;
      found.push(p);
    }
  };
  walk(root);
  return found.sort((a, b) => path.basename(a).localeCompare(path.basename(b)));
}

function makeSheet(files, outFile, cols) {
  const ffmpeg = findFfmpeg();
  if (!ffmpeg) return 'ffmpeg not found';
  const n = files.length;
  const c = Math.max(1, Math.min(cols, n));
  const rows = Math.ceil(n / c);
  const cellW = Math.floor(1920 / c);
  const cellH = Math.floor((cellW * 9) / 16);
  const list = path.join(path.dirname(outFile), 'sheet_list.txt');
  fs.writeFileSync(list, files.map((f) => `file '${f.replace(/\\/g, '/')}'\nduration 1`).join('\n') + '\n');
  const vf = `scale=${cellW}:${cellH}:force_original_aspect_ratio=decrease,pad=${cellW}:${cellH}:(ow-iw)/2:(oh-ih)/2,tile=${c}x${rows}`;
  const r = spawnSync(ffmpeg, ['-y', '-loglevel', 'error', '-f', 'concat', '-safe', '0', '-i', list, '-vf', vf, '-frames:v', '1', outFile], { encoding: 'utf8' });
  fs.rmSync(list, { force: true });
  return r.status === 0 ? '' : `ffmpeg failed: ${cut((r.stderr || '').trim().split('\n').pop() || 'unknown error', 120)}`;
}

function summariseLog(logPath, keepRe, ignoreRe, maxLines) {
  const lines = fs.existsSync(logPath) ? fs.readFileSync(logPath, 'utf8').split(/\r?\n/) : [];
  const kept = new Map();
  const otherWarnings = {};
  let errors = 0;
  let noise = 0;
  let fatal = false;
  lines.forEach((raw, i) => {
    if (!raw) return;
    const { cat, sev, text } = parseLine(raw);
    if (NOISE_CATEGORIES.includes(cat) || (ignoreRe && ignoreRe.test(raw))) { if (sev === 'Error') noise++; return; }
    const isGame = GAME_CATEGORIES.includes(cat);
    const isCrash = /Fatal error|appError called|Assertion failed|Ensure condition failed|Unhandled Exception/i.test(raw);
    let keep = false;
    if (sev === 'Error' || sev === 'Fatal' || isCrash) { keep = true; errors++; if (sev === 'Fatal' || isCrash) fatal = true; }
    else if (sev === 'Warning') { if (isGame) keep = true; else otherWarnings[cat] = (otherWarnings[cat] || 0) + 1; }
    else if (cat === 'LogRogueRigs' && (/^rr\./.test(text) || (keepRe && keepRe.test(text)))) keep = true;
    if (!keep) return;
    const key = `${cat}|${sev}|${normalise(text)}`;
    const prev = kept.get(key);
    if (prev) prev.count++;
    else kept.set(key, { line: i + 1, cat, sev, text, count: 1 });
  });
  const entries = [...kept.values()];
  const out = entries.slice(0, maxLines).map((e) => {
    const label = e.sev && e.sev !== 'Display' ? `${e.cat} ${e.sev}` : e.cat === 'LogRogueRigs' ? '' : e.cat;
    return `L${e.line} ${cut(`${label ? `${label}: ` : ''}${e.text}`)}${e.count > 1 ? ` x${e.count}` : ''}`;
  });
  if (entries.length > maxLines) out.push(`... ${entries.length - maxLines} more distinct kept lines in run.log`);
  const tail = lines.filter(Boolean).slice(-30).map((l) => cut(l));
  return { out, errors, noise, fatal, otherWarnings, tail, total: lines.length };
}

// Git Bash (MSYS) path conversion turns "/Game/X" into "C:/Program Files/Git/Game/X" (or ".../Git/usr/Game/X"): turn it back
// into "/Game/X" (task-402). Matched on the Git[/usr]/Game/ segment, not on the install folder.
const MSYS_MAP_RE = /^[A-Za-z]:[\\/](?:.*?[\\/])?Git[\\/](?:usr[\\/])?Game[\\/](.*)$/i;
const MSYS_EXEC_RE = /[A-Za-z]:[\\/](?:[^,"]*?[\\/])?Git[\\/](?:usr[\\/])?Game[\\/]/gi;

function repairMsysMap(value) {
  const m = value.match(MSYS_MAP_RE);
  return m ? `/Game/${m[1].replace(/\\/g, '/')}` : value;
}

function repairMsysExec(value) {
  return value.replace(MSYS_EXEC_RE, '/Game/');
}

async function main() {
  const a = parseArgs(process.argv.slice(2));
  if (!a.name || !a.exec) usage('--name and --exec are required');
  if (!/^[A-Za-z0-9_.-]+$/.test(a.name)) usage('--name may only hold letters, digits, _ . -');
  if (a.map && a.map !== 'none') {
    const fixed = repairMsysMap(a.map);
    if (fixed !== a.map) { console.log(`map: repaired MSYS path -> ${fixed}`); a.map = fixed; }
  }
  const fixedExec = repairMsysExec(a.exec);
  if (fixedExec !== a.exec) { console.log(`exec: repaired MSYS path(s) -> ${fixedExec}`); a.exec = fixedExec; }
  const timeoutS = Number(a.timeout || 240);
  const maxLines = Number(a['max-lines'] || 40);
  const expect = a.expect === undefined ? null : Number(a.expect);
  const prefixes = a.prefix ? a.prefix.split(',').filter(Boolean) : [];
  const keepRe = a.keep ? new RegExp(a.keep, 'i') : null;
  const ignoreRe = a.ignore ? new RegExp(a.ignore, 'i') : null;
  const [resW, resH] = (a.res || '1600x900').split('x');
  // --map none: no map argument, the game opens GameDefaultMap (L_MainMenu since task-338-04).
  const map = a.map === 'none' ? null : (a.map || '/Game/RogueRigs/Maps/L_ArenaM1');
  const outDir = path.join(REPO, 'Saved', 'Captures', a.name);
  fs.rmSync(outDir, { recursive: true, force: true });
  fs.mkdirSync(outDir, { recursive: true });
  const logPath = path.join(outDir, 'run.log');

  const args = [UPROJECT, ...(map ? [map] : []), '-game', '-windowed', `-ResX=${resW}`, `-ResY=${resH}`, ...(a['force-res'] ? ['-ForceRes'] : []), '-nosplash', '-log', `-abslog=${logPath}`, `-ExecCmds=${a.exec}`];
  const startMs = Date.now();
  const child = spawn(ENGINE, args, { stdio: 'ignore', windowsHide: false });
  const result = await new Promise((resolve) => {
    let done = false;
    const timer = setTimeout(() => {
      if (done) return;
      done = true;
      child.kill(); // only the process this script started
      resolve({ code: null, timedOut: true });
    }, timeoutS * 1000);
    child.on('exit', (code) => { if (done) return; done = true; clearTimeout(timer); resolve({ code, timedOut: false }); });
    child.on('error', (e) => { if (done) return; done = true; clearTimeout(timer); resolve({ code: null, timedOut: false, error: e.message }); });
  });
  const seconds = ((Date.now() - startMs) / 1000).toFixed(0);

  const copied = [];
  for (const f of findScreenshots(startMs, prefixes)) {
    const dest = path.join(outDir, path.basename(f));
    fs.copyFileSync(f, dest);
    copied.push(dest);
  }
  const log = summariseLog(logPath, keepRe, ignoreRe, maxLines);

  let sheetNote = 'skipped';
  const sheet = path.join(outDir, 'contact_sheet.png');
  if (!a['no-sheet'] && copied.length > 0) {
    const err = makeSheet(copied, sheet, Number(a.cols || 3));
    sheetNote = err || path.relative(REPO, sheet).split(path.sep).join('/');
  } else if (copied.length === 0) sheetNote = 'no screenshot';

  const crashed = log.fatal || (result.code !== null && result.code !== 0);
  const shotsOk = expect === null || copied.length === expect;
  const problems = [];
  if (result.error) problems.push(`could not start: ${result.error}`);
  if (result.timedOut) problems.push(`timeout after ${timeoutS} s, process stopped`);
  if (crashed) problems.push(`exit code ${result.code}${log.fatal ? ', fatal error in log' : ''}`);
  if (!shotsOk) problems.push(`expected ${expect} screenshot(s), found ${copied.length}`);
  if (log.errors > 0) problems.push(`${log.errors} error line(s)`);

  const rel = (p) => path.relative(REPO, p).replace(/\\/g, '/');
  const warnTotal = Object.values(log.otherWarnings).reduce((x, y) => x + y, 0);
  const topWarn = Object.entries(log.otherWarnings).sort((x, y) => y[1] - x[1]).slice(0, 3).map(([k, v]) => `${k} ${v}`).join(', ');
  console.log(`capture ${a.name}: ${problems.length ? 'PROBLEM' : 'ok'} | ran ${seconds} s | exit ${result.code === null ? 'none' : result.code} | log ${log.total} lines -> ${rel(logPath)}`);
  if (problems.length) console.log(`problems: ${problems.join('; ')}`);
  console.log(`screenshots: ${copied.length}${expect === null ? '' : `/${expect}`} in ${rel(outDir)}${copied.length ? ` (${copied.map((f) => path.basename(f)).join(', ')})` : ''}`);
  console.log(`contact sheet: ${sheetNote}${copied.length > 1 && sheetNote.endsWith('.png') ? ' (tile order = the file order above, left to right, top to bottom)' : ''}`);
  if (warnTotal || log.noise) console.log(`not shown: ${warnTotal} other warning(s)${topWarn ? ` (${topWarn})` : ''}, ${log.noise} engine startup error line(s) (LogPython, LogEditorDataStorageUI)`);
  console.log(`kept log lines (${log.out.length}):`);
  log.out.forEach((l) => console.log(`  ${l}`));
  if (crashed || result.timedOut) {
    console.log('last log lines:');
    log.tail.forEach((l) => console.log(`  ${l}`));
  }
  process.exit(problems.some((p) => !/error line/.test(p)) ? 1 : 0);
}

// Exported for a check of the contact sheet on existing PNGs without a game run (task-359-01) and of the MSYS repair (task-402).
module.exports = { makeSheet, repairMsysMap, repairMsysExec };

if (require.main === module) main().catch((e) => { console.error(`capture.js: ${e.stack || e.message}`); process.exit(1); });
