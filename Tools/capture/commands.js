// Rogue Rigs - list of the rr.* console commands and variables, generated from the C++ source (task-359-01, owner 2026-10-29,
// Q5 of task-359). Replaces greps of the source in capture runs; the list cannot go stale since it reads the registrations.
// Node 18+, no dependencies, like capture.js.
//
// Usage: node Tools/capture/commands.js [filter]
//   filter  case-insensitive regex tested on the name, the help text and the system folder (e.g. shop, Cheat, rarity, ^rr\.Debug)
// Reads every .cpp/.h of Source/RogueRigs/ and finds the registrations TAutoConsoleVariable<T>, FAutoConsoleVariableRef,
// FAutoConsoleCommand* and IConsoleManager Register* whose name starts with "rr.". Grouped by system folder
// (first folder under Source/RogueRigs/). One line each: name, cmd or var with its default, help text, file:line.
// Exit code: 0 listed (even when the filter matches nothing), 2 bad arguments.

const fs = require('fs');
const path = require('path');

const REPO = path.resolve(__dirname, '..', '..');
const SOURCE = path.join(REPO, 'Source', 'RogueRigs');
const REGISTRATION = /\b(TAutoConsoleVariable\s*<[^>]*>|FAutoConsoleVariableRef|FAutoConsoleCommand\w*)\s+\w+\s*\(|\b(RegisterConsoleCommand|RegisterConsoleVariable\w*)\s*\(/g;

function listFiles(dir) {
  const out = [];
  for (const e of fs.readdirSync(dir, { withFileTypes: true })) {
    const p = path.join(dir, e.name);
    if (e.isDirectory()) out.push(...listFiles(p));
    else if (/\.(cpp|h)$/i.test(e.name)) out.push(p);
  }
  return out;
}

// Splits the call arguments that start after the opening parenthesis at `start`: top-level commas only,
// string and char literals and comments skipped. Returns the raw text of each argument.
function splitArgs(text, start) {
  const args = [];
  let depth = 0;
  let cur = '';
  for (let i = start; i < text.length; i++) {
    const ch = text[i];
    if (ch === '"' || ch === "'") {
      let j = i + 1;
      while (j < text.length && text[j] !== ch) j += text[j] === '\\' ? 2 : 1;
      cur += text.slice(i, j + 1);
      i = j;
    } else if (ch === '/' && text[i + 1] === '/') {
      i = text.indexOf('\n', i); if (i < 0) break;
    } else if (ch === '/' && text[i + 1] === '*') {
      i = text.indexOf('*/', i) + 1; if (i <= 0) break;
    } else if (ch === '(' || ch === '[' || ch === '{') { depth++; cur += ch; }
    else if (ch === ')' || ch === ']' || ch === '}') {
      if (depth === 0) { args.push(cur.trim()); return args; }
      depth--; cur += ch;
    } else if (ch === ',' && depth === 0) { args.push(cur.trim()); cur = ''; }
    else cur += ch;
  }
  return args;
}

// Concatenation of every string literal of an argument (TEXT("a") TEXT("b") -> "ab"), escapes resolved.
function literal(arg) {
  if (!arg) return '';
  const parts = [...arg.matchAll(/"((?:[^"\\]|\\.)*)"/g)].map((m) => m[1]);
  return parts.join('').replace(/\\n/g, ' ').replace(/\\(.)/g, '$1');
}

function scan() {
  const entries = [];
  for (const file of listFiles(SOURCE)) {
    const text = fs.readFileSync(file, 'utf8');
    if (!text.includes('rr.')) continue;
    for (const m of text.matchAll(REGISTRATION)) {
      const args = splitArgs(text, m.index + m[0].length);
      const name = literal(args[0]);
      if (!name.startsWith('rr.')) continue;
      const isVar = /Variable/.test(m[1] || m[2]);
      // TAutoConsoleVariable(name, default, help), FAutoConsoleVariableRef(name, ref, help), commands (name, help, delegate).
      const help = literal(isVar ? args[2] : args[1]).trim();
      const def = isVar && /TAutoConsoleVariable/.test(m[1] || '') ? (literal(args[1]) || args[1]).replace(/(\d)\.?f$/, '$1') : null;
      const relFile = path.relative(SOURCE, file).replace(/\\/g, '/');
      const line = text.slice(0, text.indexOf(args[0], m.index)).split('\n').length;
      entries.push({ name, isVar, def, help, file: relFile, line, system: relFile.includes('/') ? relFile.split('/')[0] : '(root)' });
    }
  }
  return entries;
}

function main() {
  const argv = process.argv.slice(2);
  if (argv.length > 1 || argv.some((x) => x.startsWith('--'))) {
    console.error('Usage: node Tools/capture/commands.js [filter]');
    process.exit(2);
  }
  let filter = null;
  if (argv[0]) {
    try { filter = new RegExp(argv[0], 'i'); } catch (e) { console.error(`commands.js: bad filter: ${e.message}`); process.exit(2); }
  }
  const all = scan();
  const shown = all.filter((e) => !filter || filter.test(e.name) || filter.test(e.help) || filter.test(e.system));
  const systems = [...new Set(shown.map((e) => e.system))].sort();
  for (const s of systems) {
    const list = shown.filter((e) => e.system === s).sort((x, y) => x.name.localeCompare(y.name));
    console.log(`${s} (${list.length})`);
    for (const e of list) {
      const kind = e.isVar ? `var${e.def !== null ? ` = ${e.def}` : ''}` : 'cmd';
      // Most help texts already start with "<name> <args>:"; others get the name in front.
      const body = e.help.startsWith(e.name) ? e.help : `${e.name}: ${e.help || '(no help text)'}`;
      console.log(`  [${kind}] ${body} (${e.file}:${e.line})`);
    }
  }
  const vars = all.filter((e) => e.isVar).length;
  console.log(`${shown.length} of ${all.length} rr.* entries (${all.length - vars} commands, ${vars} variables) from Source/RogueRigs/${filter ? `, filter /${argv[0]}/i` : ''}`);
}

main();
