#!/usr/bin/env node
// Rogue Rigs style loop: the deterministic half of the Generate -> Evaluate -> Refine loop (Assignment 6).
// Spec: vault node `style-loop`. The model steps (generator, evaluator, refiner) are done by agents; this script
// holds everything that must NOT depend on a model: the format check, the pass threshold, the score file
// contract, the "only failing items may change" guard, the pass limit (circuit breaker) and the report.
//
//   node Tools/style/style_loop.js init   <runDir> --run-id <id> --source <stem>:<content file>:<log file> [...]
//                                         [--threshold 8] [--max-refinements 2]
//   node Tools/style/style_loop.js check  <runDir>
//   node Tools/style/style_loop.js gate   <runDir> --scores <score file>
//   node Tools/style/style_loop.js apply  <runDir> --stem <stem> --refined <content file> --log <log file>
//   node Tools/style/style_loop.js report <runDir>
//   node Tools/style/style_loop.js --selftest
//
// Exit 0 = ok. Exit 1 = contract violation (message on stderr). Exit 2 = usage error.
// `gate` prints one JSON line: {"round", "total", "failed", "action": "done" | "refine" | "escalate"}.
// Node only, no dependencies, like vault/scripts/lint.js.

const fs = require('fs');
const os = require('os');
const path = require('path');
const { spawnSync } = require('child_process');

const REPO = path.resolve(__dirname, '..', '..');
const VAULT = path.join(REPO, 'vault');
const VALIDATOR = path.join(REPO, 'Tools', 'content', 'validate_text.js');

// Text field and id field of each content file (values owned by the vault node text-content-files).
const FILES = {
  upgrade_texts: { id: 'id', text: 'flavor' },
  weapon_texts: { id: 'id', text: 'flavor' },
  barks: { id: 'line_id', text: 'text', also: ['tone'] },
  alerts: { id: 'line_id', text: 'text', also: ['register'] },
};
// Score dimensions the evaluator returns; `format` comes from the validator, never from a model.
const DIMENSIONS = ['voice', 'lore', 'reference'];
// Node holding each rule family (read at run time, never duplicated here): rule ids are the "- L1", "- T1", "- F1" bullets.
// Fields besides the text that a refinement may change: the label belongs to the line (a company line that turns
// bureaucratic carries the bureaucratic register). The validator checks the enum.
const RULE_NODES = { L: 'lore', T: 'dialogue-tone', F: 'style-guide-text' };

function fail(msg) { const e = new Error(msg); e.contract = true; throw e; }
const readJson = (f) => JSON.parse(fs.readFileSync(f, 'utf8'));
const writeJson = (f, d) => fs.writeFileSync(f, JSON.stringify(d, null, 1) + '\n');

function ruleIds() {
  const paths = {};
  for (const line of fs.readFileSync(path.join(VAULT, '_paths.md'), 'utf8').split(/\r?\n/)) {
    const m = line.match(/^(\S+)\s{2,}(\S+\.md)\s*$/);
    if (m) paths[m[1]] = m[2];
  }
  const ids = new Set();
  for (const node of Object.values(RULE_NODES)) {
    if (!paths[node]) continue;
    const text = fs.readFileSync(path.join(VAULT, paths[node]), 'utf8');
    for (const m of text.matchAll(/^- ([LTF]\d+)\b(?!.*\bremoved\b)/gm)) ids.add(m[1]);
  }
  return ids;
}

// F4 (deterministic): a film quoted as "Like in <Title>:" must be in the catalogue of the vault node fictional-films.
function filmCatalogue() {
  const paths = {};
  for (const line of fs.readFileSync(path.join(VAULT, '_paths.md'), 'utf8').split(/\r?\n/)) {
    const m = line.match(/^(\S+)\s{2,}(\S+\.md)\s*$/);
    if (m) paths[m[1]] = m[2];
  }
  const text = fs.readFileSync(path.join(VAULT, paths['fictional-films']), 'utf8');
  const body = text.split(/^## Catalogue\s*$/m)[1] || '';
  return new Set([...body.matchAll(/^- (.+?)(?: \((?:owner|provisional)\))?\s*$/gm)].map((m) => m[1].toLowerCase()));
}
function filmFinding(text, catalogue) {
  const m = text.match(/\bLike in ([^:]+):/);
  if (!m || catalogue.has(m[1].trim().toLowerCase())) return null;
  return { rule: 'F4', message: `film "${m[1].trim()}" is not in the fictional-films catalogue` };
}

function loadLoop(runDir) {
  const f = path.join(runDir, 'loop.json');
  if (!fs.existsSync(f)) fail(`no loop.json in ${runDir}; run init first`);
  return readJson(f);
}
const saveLoop = (runDir, loop) => writeJson(path.join(runDir, 'loop.json'), loop);
const currentFile = (runDir, stem) => path.join(runDir, 'current', `${stem}.json`);
function loadCurrent(runDir, loop) {
  const out = {};
  for (const s of loop.sources) out[s.stem] = readJson(currentFile(runDir, s.stem));
  return out;
}

function runValidator(content, log) {
  const r = spawnSync(process.execPath, [VALIDATOR, content, '--log', log], { cwd: REPO, encoding: 'utf8' });
  const lines = (r.stdout || '').split('\n').filter(Boolean).map((l) => { try { return JSON.parse(l); } catch { return { item_id: null, check: 'unparsable', message: l }; } });
  return { status: r.status, lines };
}

// Validator check name -> rule id of the style guide (F = format rules of style-guide-text).
function mapCheck(check, stem) {
  let m = check.match(/^(T\d)\b/);
  if (m) return m[1];
  if (check === 'text-ascii') return 'T7';
  if (/word/i.test(check)) return stem === 'barks' ? 'T1' : 'F1';
  if (check === 'number-in-flavor') return 'F2';
  if (check === 'number-not-in-excerpt') return 'F3';
  return null;
}

// ---- commands ----------------------------------------------------------------------------------------------------
function init(runDir, opts) {
  if (fs.existsSync(path.join(runDir, 'loop.json'))) fail(`${runDir} is already initialised`);
  if (!opts.runId) fail('init needs --run-id');
  if (!opts.sources.length) fail('init needs at least one --source <stem>:<content>:<log>');
  fs.mkdirSync(path.join(runDir, 'current'), { recursive: true });
  const sources = opts.sources.map((spec) => {
    const [stem, content, log] = spec.split(':');
    if (!FILES[stem]) fail(`unknown content file stem "${stem}"`);
    if (!content || !log) fail(`source "${spec}" needs stem:content:log`);
    fs.copyFileSync(content, currentFile(runDir, stem));
    return { stem, content, log };
  });
  saveLoop(runDir, { run_id: opts.runId, threshold: opts.threshold, max_refinements: opts.maxRefinements,
    refinements_done: 0, round: 0, status: 'open', sources, pending_stems: [], applied: {}, lineage: [], rounds: [] });
}

function check(runDir) {
  const loop = loadLoop(runDir);
  // Format findings come from the baseline files, once. Later rounds: a refined item was validated by `apply`.
  if (loop.round > 0) return readJson(path.join(runDir, 'format.json')).findings;
  const findings = [];
  for (const s of loop.sources) {
    const r = runValidator(s.content, s.log);
    for (const l of r.lines) {
      if (l.item_id == null) fail(`file-level validator problem in ${s.stem}: ${l.check}: ${l.message}`);
      const rule = mapCheck(l.check, s.stem);
      if (!rule) fail(`validator problem with no style rule in ${s.stem}/${l.item_id}: ${l.check}: ${l.message}`);
      const item = readJson(currentFile(runDir, s.stem)).items.find((i) => i[FILES[s.stem].id] === l.item_id);
      findings.push({ file: s.stem, item_id: l.item_id, rule, message: `${l.check}: ${l.message}`, text: item ? item[FILES[s.stem].text] : null });
    }
  }
  writeJson(path.join(runDir, 'format.json'), { findings });
  return findings;
}

function formatFailed(runDir, loop, stem, item) {
  const f = path.join(runDir, 'format.json');
  if (!fs.existsSync(f)) return null;
  const hit = readJson(f).findings.find((x) => x.file === stem && x.item_id === item[FILES[stem].id] && x.text === item[FILES[stem].text]);
  return hit || null; // only while the item still holds the text the validator rejected
}

function gate(runDir, scoresFile) {
  const loop = loadLoop(runDir);
  if (loop.status !== 'open') fail(`loop is ${loop.status}; nothing to gate`);
  if (loop.pending_stems.length) fail(`refinement of ${loop.pending_stems.join(', ')} not applied yet`);
  const round = loop.round;
  const gateFile = path.join(runDir, `round${round}.gate.json`);
  if (fs.existsSync(gateFile)) fail(`round ${round} already gated`);
  const rules = ruleIds();
  const current = loadCurrent(runDir, loop);
  const scored = readJson(scoresFile);
  if (scored.round !== round) fail(`score file is for round ${scored.round}, loop is at round ${round}`);

  const catalogue = filmCatalogue();
  const byKey = new Map();
  for (const s of scored.items) {
    const k = `${s.file}/${s.item_id}`;
    if (byKey.has(k)) fail(`${k} scored twice`);
    byKey.set(k, s);
  }
  const fails = [];
  let total = 0;
  for (const [stem, env] of Object.entries(current)) {
    for (const item of env.items) {
      total++;
      const id = item[FILES[stem].id];
      const k = `${stem}/${id}`;
      const s = byKey.get(k);
      if (!s) fail(`${k} has no score`);
      byKey.delete(k);
      for (const d of DIMENSIONS) {
        if (!Number.isInteger(s.scores && s.scores[d]) || s.scores[d] < 0 || s.scores[d] > 10) fail(`${k}: score "${d}" must be an integer 0 to 10`);
      }
      if (typeof s.reason !== 'string' || !s.reason.trim()) fail(`${k}: every item needs a reason, even a pass`);
      if (!Array.isArray(s.rules)) fail(`${k}: "rules" must be an array`);
      for (const r of s.rules) if (!rules.has(r)) fail(`${k}: rule "${r}" does not exist`);
      let fmt = formatFailed(runDir, loop, stem, item);
      if (!fmt) fmt = filmFinding(item[FILES[stem].text], catalogue);
      const scores = { ...s.scores, format: fmt ? 0 : 10 };
      const low = Math.min(...Object.values(scores));
      if (low >= loop.threshold) continue;
      const rule = (fmt && !s.rules.length) ? fmt.rule : s.rules[0];
      if (!rule) fail(`${k}: scores below ${loop.threshold} but no rule cited`);
      if (!fmt && (typeof s.suggestion !== 'string' || !s.suggestion.trim())) fail(`${k}: a failing item needs a suggestion`);
      fails.push({ file: stem, item_id: id, text: item[FILES[stem].text], scores, rule,
        reason: fmt && !s.rules.length ? fmt.message : s.reason, suggestion: s.suggestion || null });
    }
  }
  if (byKey.size) fail(`score file has items not in the content: ${[...byKey.keys()].join(', ')}`);

  let action = 'done';
  if (fails.length) action = loop.refinements_done < loop.max_refinements ? 'refine' : 'escalate';
  writeJson(gateFile, { round, threshold: loop.threshold, total, passed: total - fails.length, failed: fails.length, action, fails });
  loop.rounds.push({ round, total, failed: fails.length, action });
  loop.round = round + 1;
  if (action === 'done') loop.status = 'done';
  if (action === 'escalate') {
    loop.status = 'escalated';
    const md = ['# Escalation', '',
      `The circuit breaker opened after ${loop.refinements_done} refinement(s): ${fails.length} item(s) still below ${loop.threshold}. The loop stops here.`, '',
      'Root cause to find before another run: is the agent wrong (update its definition)? is the criterion too strict? are the requirements vague or impossible to satisfy together?', ''];
    for (const f of fails) md.push(`- \`${f.file}/${f.item_id}\` "${f.text}": rule ${f.rule}, scores ${JSON.stringify(f.scores)}. ${f.reason}`);
    fs.writeFileSync(path.join(runDir, 'escalation.md'), md.join('\n') + '\n');
  }
  loop.pending_stems = action === 'refine' ? [...new Set(fails.map((f) => f.file))] : [];
  loop.applied[round] = [];
  saveLoop(runDir, loop);
  return { round, total, failed: fails.length, action };
}

function apply(runDir, stem, refinedFile, logFile) {
  const loop = loadLoop(runDir);
  if (!loop.pending_stems.includes(stem)) fail(`no refinement pending for ${stem}`);
  const round = loop.round - 1;
  const gated = readJson(path.join(runDir, `round${round}.gate.json`));
  const want = gated.fails.filter((f) => f.file === stem).map((f) => f.item_id).sort();
  const refined = readJson(refinedFile);
  const cur = readJson(currentFile(runDir, stem));
  const { id, text } = FILES[stem];
  const got = refined.items.map((i) => i[id]).sort();
  if (JSON.stringify(got) !== JSON.stringify(want)) fail(`refined file must hold exactly the failing items [${want.join(', ')}], found [${got.join(', ')}]`);
  for (const r of refined.items) {
    const before = cur.items.find((i) => i[id] === r[id]);
    for (const key of new Set([...Object.keys(before), ...Object.keys(r)])) {
      if (key !== text && !(FILES[stem].also || []).includes(key) && before[key] !== r[key]) fail(`${r[id]}: only "${text}" may change, "${key}" changed`);
    }
    if (before[text] === r[text]) fail(`${r[id]}: refined text is identical to the failing text`);
  }
  if (process.env.STYLE_LOOP_SKIP_VALIDATOR !== '1') {
    const v = runValidator(refinedFile, logFile);
    if (v.status !== 0) fail(`refined file does not validate: ${JSON.stringify(v.lines.slice(0, 3))}`);
  }
  for (const r of refined.items) {
    const i = cur.items.findIndex((x) => x[id] === r[id]);
    const f = gated.fails.find((x) => x.file === stem && x.item_id === r[id]);
    loop.lineage.push({ round, file: stem, item_id: r[id], run_id: refined.run_id, rule: f.rule, reason: f.reason, scores: f.scores, before: cur.items[i][text], after: r[text] });
    cur.items[i] = r;
  }
  writeJson(currentFile(runDir, stem), cur);
  loop.pending_stems = loop.pending_stems.filter((s) => s !== stem);
  loop.applied[round].push(stem);
  if (!loop.pending_stems.length) loop.refinements_done++;
  saveLoop(runDir, loop);
}

function report(runDir) {
  const loop = loadLoop(runDir);
  const md = [`# Style loop report — ${loop.run_id}`, '',
    `Threshold ${loop.threshold}/10 on every dimension (voice, lore, reference, format). Pass limit: ${loop.max_refinements} refinement(s). Final status: **${loop.status}**.`, '',
    '| Round | Items | Below threshold | Action |', '|---|---|---|---|'];
  for (const r of loop.rounds) md.push(`| ${r.round} | ${r.total} | ${r.failed} | ${r.action} |`);
  md.push('', '## Corrections', '', '| Item | Round | Rule | Scores before | Reason | Before | After |', '|---|---|---|---|---|---|---|');
  for (const l of loop.lineage) {
    md.push(`| ${l.file}/${l.item_id} | ${l.round} | ${l.rule} | ${Object.entries(l.scores).map(([k, v]) => `${k} ${v}`).join(', ')} | ${l.reason} | ${l.before} | ${l.after} |`);
  }
  if ((loop.owner_overrides || []).length) {
    md.push('', '## Owner overrides', '', 'The owner kept the original line, knowing the evaluator rated it below the threshold; the deviation is deliberate, not an oversight.', '',
      '| Item | Kept (owner) | Evaluator-approved rewrite replaced |', '|---|---|---|');
    for (const o of loop.owner_overrides) md.push(`| ${o.file}/${o.item_id} | ${o.kept} | ${o.evaluator_version} |`);
  }
  fs.writeFileSync(path.join(runDir, 'report.md'), md.join('\n') + '\n');
}

// ---- self test ---------------------------------------------------------------------------------------------------
function selftest() {
  process.env.STYLE_LOOP_SKIP_VALIDATOR = '1';
  const tmp = fs.mkdtempSync(path.join(os.tmpdir(), 'style-loop-'));
  const mk = (n) => path.join(tmp, n);
  const items = [
    { line_id: 'rook_level_up_01', speaker_id: 'rook', trigger: 'level_up', tone: 'cocky', text: 'Stronger already. Try keeping up.' },
    { line_id: 'vesna_level_up_01', speaker_id: 'vesna', trigger: 'level_up', tone: 'deadpan', text: 'Noted. Do not get used to it.' }];
  writeJson(mk('barks.json'), { schema_version: 1, run_id: '20261020-000000-barks-noretrieval', items });
  writeJson(mk('barks.log.json'), {});
  const run = mk('run');
  let n = 0;
  const expectFail = (label, fn, part) => {
    try { fn(); } catch (e) { if (e.contract && e.message.includes(part)) { n++; return; } throw new Error(`${label}: wrong error: ${e.message}`); }
    throw new Error(`${label}: expected a contract violation`);
  };
  const score = (round, a, b, ra, sa) => ({ round, items: [
    { file: 'barks', item_id: 'rook_level_up_01', scores: { voice: a, lore: 9, reference: 10 }, rules: ra, reason: 'r', suggestion: sa },
    { file: 'barks', item_id: 'vesna_level_up_01', scores: { voice: b, lore: 9, reference: 10 }, rules: [], reason: 'ok', suggestion: null }] });
  init(run, { runId: 't', threshold: 8, maxRefinements: 1, sources: [`barks:${mk('barks.json')}:${mk('barks.log.json')}`] });
  writeJson(mk('s0.json'), score(0, 4, 9, ['T2'], 'Stronger already. Keep up if you can, boss.'));
  expectFail('unknown rule', () => { writeJson(mk('bad.json'), score(0, 4, 9, ['T99'], 'x y z')); gate(run, mk('bad.json')); }, 'does not exist');
  expectFail('fail without suggestion', () => { writeJson(mk('bad.json'), score(0, 4, 9, ['T2'], null)); gate(run, mk('bad.json')); }, 'suggestion');
  expectFail('wrong round', () => { writeJson(mk('bad.json'), score(3, 4, 9, ['T2'], 'x y z')); gate(run, mk('bad.json')); }, 'round');
  let g = gate(run, mk('s0.json'));
  if (g.action !== 'refine' || g.failed !== 1) throw new Error('gate round 0 should refine 1 item'); n++;
  const refined = (extra) => ({ schema_version: 1, run_id: '20261020-000100-barks', items: [{ ...items[0], text: 'Stronger already. Keep up if you can, boss.', ...extra }] });
  expectFail('touching a passing item', () => { writeJson(mk('r.json'), { ...refined(), items: [...refined().items, { ...items[1], text: 'Changed.' }] }); apply(run, 'barks', mk('r.json'), mk('barks.log.json')); }, 'exactly the failing items');
  expectFail('changing another field', () => { writeJson(mk('r.json'), refined({ trigger: 'run_won' })); apply(run, 'barks', mk('r.json'), mk('barks.log.json')); }, 'may change');
  expectFail('identical text', () => { writeJson(mk('r.json'), refined({ text: items[0].text })); apply(run, 'barks', mk('r.json'), mk('barks.log.json')); }, 'identical');
  expectFail('gate before apply', () => gate(run, mk('s0.json')), 'not applied yet');
  writeJson(mk('r.json'), refined());
  apply(run, 'barks', mk('r.json'), mk('barks.log.json'));
  if (readJson(path.join(run, 'current', 'barks.json')).items[0].text !== refined().items[0].text) throw new Error('apply did not replace the text'); n++;
  g = gate(run, mk('s1.json') && (writeJson(mk('s1.json'), score(1, 9, 9, [], null)), mk('s1.json')));
  if (g.action !== 'done') throw new Error('gate round 1 should be done'); n++;
  // circuit breaker: max 0 refinements, a fail escalates
  const run2 = mk('run2');
  init(run2, { runId: 't2', threshold: 8, maxRefinements: 0, sources: [`barks:${mk('barks.json')}:${mk('barks.log.json')}`] });
  g = gate(run2, mk('s0.json'));
  if (g.action !== 'escalate' || !fs.existsSync(path.join(run2, 'escalation.md'))) throw new Error('circuit breaker did not open'); n++;
  report(run); n++;
  fs.rmSync(tmp, { recursive: true, force: true });
  console.log(`selftest ok: ${n} checks`);
}

// ---- cli ---------------------------------------------------------------------------------------------------------
function parse(argv) {
  const o = { sources: [], threshold: 8, maxRefinements: 2 };
  const pos = [];
  for (let i = 0; i < argv.length; i++) {
    const a = argv[i];
    if (a === '--run-id') o.runId = argv[++i];
    else if (a === '--source') o.sources.push(argv[++i]);
    else if (a === '--threshold') o.threshold = Number(argv[++i]);
    else if (a === '--max-refinements') o.maxRefinements = Number(argv[++i]);
    else if (a === '--scores') o.scores = argv[++i];
    else if (a === '--stem') o.stem = argv[++i];
    else if (a === '--refined') o.refined = argv[++i];
    else if (a === '--log') o.log = argv[++i];
    else pos.push(a);
  }
  o.pos = pos;
  return o;
}

function main() {
  const argv = process.argv.slice(2);
  if (argv[0] === '--selftest') return selftest();
  const [cmd, runDir] = argv;
  const o = parse(argv.slice(2));
  const usage = (m) => { console.error(m); process.exit(2); };
  if (!cmd || !runDir) usage('usage: style_loop.js init|check|gate|apply|report <runDir> ... | --selftest');
  if (cmd === 'init') init(runDir, o);
  else if (cmd === 'check') { const f = check(runDir); console.log(JSON.stringify({ format_findings: f.length })); }
  else if (cmd === 'gate') { if (!o.scores) usage('gate needs --scores'); console.log(JSON.stringify(gate(runDir, o.scores))); }
  else if (cmd === 'apply') { if (!o.stem || !o.refined || !o.log) usage('apply needs --stem --refined --log'); apply(runDir, o.stem, o.refined, o.log); }
  else if (cmd === 'report') report(runDir);
  else usage(`unknown command ${cmd}`);
}

try { main(); } catch (e) {
  if (e.contract) { console.error(`style_loop: ${e.message}`); process.exit(1); }
  throw e;
}
