#!/usr/bin/env node
// Rogue Rigs text content validator. Spec: vault node `text-content-schema` and its
// children `text-content-files`, `text-content-events`, `text-run-log-schema`.
//
//   node Tools/content/validate_text.js <content file> --log <log file> [--review <review file>]
//   node Tools/content/validate_text.js --selftest
// Content files: upgrade_texts, weapon_texts, barks, alerts, screen_texts (.json), item fields in text-content-files.
//
// Without --review, <log dir>/<run-id>.review.json is used when it exists.
// Exit 0 = everything valid. Otherwise exit 1 and one JSON line per problem on stdout:
//   {"item_id": <id or null>, "check": "<check name>", "message": "<text>"}
// Exit 2 = usage error. --selftest runs Tools/content/tests/ and exits 0 when every case
// gives its expected exit code and error lines.
//
// Read at run time from the vault (never duplicated here): bark triggers and alert events
// and weapon lore ids (text-content-events), weapon names (WeaponStats.csv DisplayName), stat display names (sys-upgrades), rule ids L1-L4 (lore) and
// T1-T9 (dialogue-tone), the id -> path map (_paths.md), and the T8 example corpus
// (every double-quoted string of 3+ words in EXAMPLE_NODES).
// Node only, no dependencies, like vault/scripts/lint.js.

const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');

const REPO = path.resolve(__dirname, '..', '..');
const VAULT = path.join(REPO, 'vault');
const TESTS = path.join(__dirname, 'tests');
const LOGS_DIR = path.join(REPO, 'Tools', 'content', 'logs');
const CONTENT_DIR = path.join(REPO, 'Content');
const WEAPON_STATS = path.join(CONTENT_DIR, 'RogueRigs', 'Data', 'Tables', 'WeaponStats.csv');

// Values owned by text-content-files (enums and word limits).
const FILES = {
  upgrade_texts: { keys: ['id', 'stat', 'flavor'], idKey: 'id', textKey: 'flavor', min: 3, max: 12 },
  weapon_texts: { keys: ['id', 'weapon', 'flavor'], idKey: 'id', textKey: 'flavor', min: 3, max: 12 },
  barks: { keys: ['line_id', 'speaker_id', 'trigger', 'tone', 'text'], idKey: 'line_id', textKey: 'text', min: 3, max: 8 },
  alerts: { keys: ['line_id', 'event', 'register', 'text'], idKey: 'line_id', textKey: 'text', min: 3, max: 12 },
  // Screen texts (end, intermission, loading screens): one item per screen, several text slots. textKey is the slot the log
  // output traces; min/max bound a review suggestion (any slot); fields hold the per-slot limits.
  // lines.caption: a lines entry whose trimmed text ends with ":" is a caption (label followed by a value shown beside it)
  // and takes the labels limit, 1 to 4 words (owner 2026-10-30, task-385); every other line keeps 3 to 12.
  screen_texts: { keys: ['id', 'title'], optional: ['subtitle', 'lines', 'labels'], idKey: 'id', textKey: 'title', min: 1, max: 12,
    fields: { title: { min: 1, max: 4 }, subtitle: { min: 3, max: 12 }, lines: { min: 3, max: 12, array: true, caption: { min: 1, max: 4 } },
      labels: { min: 1, max: 4, array: true } } },
};
const SCREEN_ID_RE = /^[a-z][a-z0-9]*(_[a-z0-9]+)+$/;
const SPEAKERS = new Set(['rook', 'vesna']);
const TONES = new Set(['cocky', 'deadpan', 'technical', 'propaganda', 'bureaucratic']);
const REGISTERS = new Set(['propaganda', 'bureaucratic']);
const RUN_ID_RE = /^[0-9]{8}-[0-9]{6}-(upgrade_texts|weapon_texts|barks|alerts|screen_texts)(-noretrieval)?$/;
// Nodes whose quoted example lines content must never equal (T8).
const EXAMPLE_NODES = ['dialogue-tone', 'crew-characters', 'company-voice', 'lore', 'fictional-films',
  'text-content-schema', 'text-content-files', 'text-run-log-schema'];

const rel = (p) => path.relative(REPO, path.resolve(p)).split(path.sep).join('/');
const isInside = (p, dir) => { const r = path.relative(dir, path.resolve(p)); return r !== '' && !r.startsWith('..') && !path.isAbsolute(r); };
const isObj = (v) => v !== null && typeof v === 'object' && !Array.isArray(v);
const typeOf = (v) => (v === null ? 'null' : Array.isArray(v) ? 'array' : typeof v);

if (process.argv.includes('--selftest')) selftest();
else main();

// ---------------------------------------------------------------- vault data
function loadVault() {
  const pathsText = fs.readFileSync(path.join(VAULT, '_paths.md'), 'utf8').replace(/\r\n/g, '\n');
  const paths = {};
  for (const m of pathsText.matchAll(/^([a-z0-9_][a-z0-9_.-]*)  (\S+\.md)$/gm)) paths[m[1]] = m[2];
  const cache = {};
  const node = (id) => {
    if (!(id in cache)) {
      const p = paths[id] ? path.join(VAULT, paths[id]) : null;
      cache[id] = p && fs.existsSync(p) ? fs.readFileSync(p, 'utf8').replace(/\r\n/g, '\n') : null;
    }
    return cache[id];
  };
  const need = (id) => { const t = node(id); if (t === null) throw new Error(`vault node "${id}" not found through _paths.md`); return t; };

  const section = (text, title) => { const i = text.indexOf(`## ${title}`); if (i < 0) return ''; const j = text.indexOf('\n## ', i + 3); return text.slice(i, j < 0 ? undefined : j); };
  const tableIds = (s) => new Set([...s.matchAll(/^\| `([a-z_]+)` \|/gm)].map((m) => m[1]));
  const events = need('text-content-events');
  const triggers = tableIds(section(events, 'Bark triggers'));
  const alertEvents = tableIds(section(events, 'Alert events'));
  const weaponIds = tableIds(section(events, 'Weapon lore ids'));

  const sm = need('sys-upgrades').match(/stat names ([^;]+);/);
  if (!sm) throw new Error('sys-upgrades: "stat names ...;" list not found');
  const stats = new Set(sm[1].split(',').map((s) => s.trim()));

  const rules = new Set();
  for (const id of ['lore', 'dialogue-tone']) for (const m of need(id).matchAll(/^- ([LT][0-9]+) /gm)) rules.add(m[1]);

  const examples = new Set();
  for (const id of EXAMPLE_NODES) {
    const t = node(id);
    if (t === null) continue;
    for (const m of t.matchAll(/"([^"\n]+)"|“([^”\n]+)”/g)) {
      const s = (m[1] || m[2]).trim();
      if (s.split(/\s+/).length >= 3) examples.add(s.toLowerCase());
    }
  }
  // Weapon display names: the DisplayName column of WeaponStats.csv (sys-weapons).
  const csv = fs.readFileSync(WEAPON_STATS, 'utf8').replace(/\r\n/g, '\n').split('\n').filter((l) => l.trim() !== '');
  const col = csv.length ? csv[0].split(',').indexOf('DisplayName') : -1;
  if (col < 0) throw new Error('WeaponStats.csv: DisplayName column not found');
  const weapons = new Set(csv.slice(1).map((l) => (l.split(',')[col] || '').trim()).filter((w) => w !== ''));
  return { paths, node, triggers, alertEvents, stats, weapons, weaponIds, rules, examples };
}

// ---------------------------------------------------------------- validation
function main() {
  const args = process.argv.slice(2);
  const opt = (name) => { const i = args.indexOf(name); return i >= 0 ? args[i + 1] : undefined; };
  const contentFile = args[0] && !args[0].startsWith('--') ? args[0] : null;
  const logFile = opt('--log');
  let reviewFile = opt('--review');
  if (!contentFile || !logFile) {
    console.error('usage: node Tools/content/validate_text.js <content file> --log <log file> [--review <review file>]\n       node Tools/content/validate_text.js --selftest');
    process.exit(2);
  }

  const errors = [];
  const err = (item_id, check, message) => errors.push({ item_id, check, message });
  const finish = () => {
    for (const e of errors) console.log(JSON.stringify(e));
    if (errors.length) process.exit(1);
    console.error(`valid: ${rel(contentFile)}`);
    process.exit(0);
  };
  const readJson = (file, what) => {
    let raw;
    try { raw = fs.readFileSync(file, 'utf8'); } catch (e) { err(null, 'file', `${what} ${rel(file)}: cannot read`); return undefined; }
    if (raw.charCodeAt(0) === 0xfeff) { err(null, 'json', `${what} ${rel(file)}: starts with a BOM; write UTF-8 without BOM`); return undefined; }
    try { return JSON.parse(raw); } catch (e) { err(null, 'json', `${what} ${rel(file)}: ${e.message}`); return undefined; }
  };
  const exactKeys = (obj, keys, item_id, where, optional = []) => {
    if (!isObj(obj)) { err(item_id, 'keys', `${where} must be an object`); return false; }
    for (const k of keys) if (!(k in obj)) err(item_id, 'keys', `${where}: missing key "${k}"`);
    for (const k of Object.keys(obj)) if (!keys.includes(k) && !optional.includes(k)) err(item_id, 'keys', `${where}: extra key "${k}"`);
    return true;
  };

  let V;
  try { V = loadVault(); } catch (e) { err(null, 'vault', e.message); finish(); }

  // Text rules of text-content-schema, shared by content texts and review suggestions.
  const checkText = (item_id, field, s, spec) => {
    if (typeof s !== 'string') { err(item_id, 'type', `${field} must be a string, got ${typeOf(s)}`); return; }
    if (/[\r\n]/.test(s)) err(item_id, 'text-one-line', `${field} must be one line`);
    if (s !== s.trim()) err(item_id, 'text-space', `${field} has a leading or trailing space`);
    if (s.includes('  ')) err(item_id, 'text-space', `${field} has a double space`);
    if (/[^\x20-\x7e]/.test(s)) err(item_id, 'text-ascii', `${field} has a character outside printable ASCII (T7)`);
    const words = s.trim() === '' ? 0 : s.trim().split(/ +/).length;
    if (words < spec.min || words > spec.max) err(item_id, spec === FILES.barks ? 'T1-word-count' : 'word-count', `${field} has ${words} words, allowed ${spec.min} to ${spec.max}`);
    if (V.examples.has(s.trim().toLowerCase())) err(item_id, 'T8-example-reuse', `${field} equals a vault example line`);
  };

  // ---- content file ----
  const C = readJson(contentFile, 'content file');
  if (C === undefined) finish();
  let stem = null;
  let noRetrieval = false;
  if (exactKeys(C, ['schema_version', 'run_id', 'items'], null, 'content envelope')) {
    if (C.schema_version !== 1) err(null, 'envelope', 'schema_version must be the integer 1');
    const m = typeof C.run_id === 'string' ? C.run_id.match(RUN_ID_RE) : null;
    if (!m) err(null, 'run-id', `run_id ${JSON.stringify(C.run_id)} does not match ${RUN_ID_RE.source}`);
    else { stem = m[1]; noRetrieval = !!m[2]; }
    if (!Array.isArray(C.items) || C.items.length === 0) err(null, 'envelope', 'items must be a non-empty array');
  }
  const fileStem = path.basename(contentFile).replace(/\.json$/, '');
  if (!FILES[fileStem]) { err(null, 'file-name', `content file name must be upgrade_texts.json, weapon_texts.json, barks.json, alerts.json or screen_texts.json, got ${path.basename(contentFile)}`); finish(); }
  if (stem && stem !== fileStem) err(null, 'run-id', `run_id stem "${stem}" differs from the file name "${fileStem}"`);
  if (noRetrieval && (isInside(contentFile, CONTENT_DIR) || !isInside(contentFile, LOGS_DIR)))
    err(null, 'noretrieval-location', `a -noretrieval content file must live under Tools/content/logs/, never under Content/ (${rel(contentFile)})`);

  const spec = FILES[fileStem];
  const items = Array.isArray(C.items) ? C.items : [];
  const texts = {};              // item id -> content text
  const seenTexts = new Map();   // lowercase text -> item id
  const seenStats = new Set();
  const seenWeapons = new Set();
  const screenSlots = {};        // screen_texts item id -> every slot text (number check)
  items.forEach((it, i) => {
    const id = isObj(it) && typeof it[spec.idKey] === 'string' ? it[spec.idKey] : `#${i}`;
    if (!exactKeys(it, spec.keys, id, `item ${id}`, spec.optional || [])) return;
    for (const k of spec.keys) if (typeof it[k] !== 'string') err(id, 'type', `${k} must be a string, got ${typeOf(it[k])}`);
    if (id in texts) err(id, 'unique-id', `duplicate ${spec.idKey}`);
    texts[id] = it[spec.textKey];
    // Every text of the item: [field label, value, limits]; one field for the four line files, every slot for screen texts.
    const itemTexts = [];
    if (spec.fields) {
      for (const [k, lim] of Object.entries(spec.fields)) {
        if (!(k in it)) continue;
        if (!lim.array) { itemTexts.push([k, it[k], lim]); continue; }
        if (!Array.isArray(it[k]) || it[k].length === 0) { err(id, 'type', `${k} must be a non-empty array of strings, got ${typeOf(it[k])}`); continue; }
        // A caption entry (trimmed text ending with ":") takes the slot's caption limit when it has one.
        it[k].forEach((v, j) => itemTexts.push([`${k}[${j}]`, v, lim.caption && typeof v === 'string' && v.trim().endsWith(':') ? lim.caption : lim]));
      }
    } else itemTexts.push([spec.textKey, it[spec.textKey], spec]);
    for (const [field, value, lim] of itemTexts) {
      checkText(id, field, value, lim);
      if (typeof value === 'string') {
        const low = value.toLowerCase();
        if (seenTexts.has(low)) err(id, 'unique-text', `same text as ${seenTexts.get(low)}`);
        else seenTexts.set(low, id);
      }
    }
    if (fileStem === 'screen_texts') {
      screenSlots[id] = itemTexts.map((t) => t[1]);
      if (typeof it.id === 'string' && !SCREEN_ID_RE.test(it.id)) err(id, 'id-pattern', 'id must be lowercase snake_case <screen>_<name> (e.g. end_run_won)');
    } else if (fileStem === 'upgrade_texts') {
      if (!V.stats.has(it.stat)) err(id, 'stat', `stat ${JSON.stringify(it.stat)} is not a display stat name of sys-upgrades`);
      else if (seenStats.has(it.stat)) err(id, 'stat', `stat "${it.stat}" appears more than once`);
      seenStats.add(it.stat);
      if (typeof it.stat === 'string' && it.id !== 'upg_' + it.stat.toLowerCase().replace(/ /g, '_')) err(id, 'id-pattern', `id must be "upg_${it.stat.toLowerCase().replace(/ /g, '_')}"`);
      if (/[0-9]/.test(it.flavor)) err(id, 'number-in-flavor', 'flavor must contain no digit (values live in UpgradeValues.csv)');
    } else if (fileStem === 'weapon_texts') {
      if (!V.weapons.has(it.weapon)) err(id, 'weapon', `weapon ${JSON.stringify(it.weapon)} is not a DisplayName of WeaponStats.csv`);
      else if (seenWeapons.has(it.weapon)) err(id, 'weapon', `weapon "${it.weapon}" appears more than once`);
      seenWeapons.add(it.weapon);
      if (typeof it.weapon === 'string' && it.id !== 'wpn_' + it.weapon.toLowerCase().replace(/ /g, '_')) err(id, 'id-pattern', `id must be "wpn_${it.weapon.toLowerCase().replace(/ /g, '_')}"`);
      else if (!V.weaponIds.has(it.id)) err(id, 'weapon-id', `id ${JSON.stringify(it.id)} is not a weapon lore id of text-content-events`);
      if (/[0-9]/.test(it.flavor)) err(id, 'number-in-flavor', 'flavor must contain no digit (values live in WeaponStats.csv)');
    } else if (fileStem === 'barks') {
      if (!SPEAKERS.has(it.speaker_id)) err(id, 'speaker', `speaker_id ${JSON.stringify(it.speaker_id)} is not rook or vesna`);
      if (!V.triggers.has(it.trigger)) err(id, 'trigger', `trigger ${JSON.stringify(it.trigger)} is not a bark trigger of text-content-events`);
      if (!TONES.has(it.tone)) err(id, 'tone', `tone ${JSON.stringify(it.tone)} is not one of ${[...TONES].join(', ')}`);
      if (!new RegExp(`^${it.speaker_id}_${it.trigger}_(0[1-9]|[1-9][0-9])$`).test(it.line_id)) err(id, 'id-pattern', 'line_id must be <speaker_id>_<trigger>_<NN>, NN from 01');
    } else {
      if (!V.alertEvents.has(it.event)) err(id, 'event', `event ${JSON.stringify(it.event)} is not an alert event of text-content-events`);
      if (!REGISTERS.has(it.register)) err(id, 'register', `register ${JSON.stringify(it.register)} is not propaganda or bureaucratic`);
      if (!new RegExp(`^kv_${it.event}_(0[1-9]|[1-9][0-9])$`).test(it.line_id)) err(id, 'id-pattern', 'line_id must be kv_<event>_<NN>, NN from 01');
    }
  });

  // ---- retrieval log ----
  const L = readJson(logFile, 'log file');
  const entries = {};            // item id -> {loop -> entry}
  if (L !== undefined && exactKeys(L, ['run_id', 'content_file', 'retrieval', 'entries'], null, 'log')) {
    if (L.run_id !== C.run_id) err(null, 'log-run-id', `log run_id ${JSON.stringify(L.run_id)} differs from the content run_id`);
    if (path.basename(logFile) !== `${L.run_id}.json`) err(null, 'log-file-name', `log file must be named <run-id>.json (${L.run_id}.json)`);
    if (L.content_file !== `${fileStem}.json`) err(null, 'log-content-file', `content_file must be "${fileStem}.json"`);
    if (typeof L.retrieval !== 'boolean') err(null, 'type', 'log retrieval must be a boolean');
    else if (L.retrieval === noRetrieval) err(null, 'log-retrieval', `retrieval must be ${!noRetrieval} for run id ${C.run_id}`);
    if (!Array.isArray(L.entries) || L.entries.length === 0) err(null, 'log', 'entries must be a non-empty array');
    (Array.isArray(L.entries) ? L.entries : []).forEach((e, i) => {
      const id = isObj(e) && typeof e.item_id === 'string' ? e.item_id : `entry#${i}`;
      if (!exactKeys(e, ['item_id', 'loop', 'query', 'node_ids', 'excerpts', 'output'], id, `log entry ${i}`)) return;
      if (typeof e.item_id !== 'string') err(id, 'type', 'item_id must be a string');
      else if (!(e.item_id in texts)) err(id, 'log-item', 'log entry for an id absent from the content file');
      if (![0, 1, 2].includes(e.loop)) err(id, 'log-loop', 'loop must be the integer 0, 1 or 2');
      else { entries[id] = entries[id] || {}; if (entries[id][e.loop]) err(id, 'log-loop', `two entries for loop ${e.loop}`); entries[id][e.loop] = e; }
      if (typeof e.query !== 'string' || e.query.trim() === '') err(id, 'log-query', 'query must be a non-empty string');
      if (typeof e.output !== 'string') err(id, 'type', 'output must be a string');
      const nodeIds = Array.isArray(e.node_ids) ? e.node_ids : (err(id, 'type', 'node_ids must be an array'), []);
      const excerpts = Array.isArray(e.excerpts) ? e.excerpts : (err(id, 'type', 'excerpts must be an array'), []);
      if (L.retrieval === true && (nodeIds.length === 0 || excerpts.length === 0)) err(id, 'log-retrieval', 'node_ids and excerpts must be non-empty when retrieval is true');
      if (L.retrieval === false && (nodeIds.length || excerpts.length)) err(id, 'log-retrieval', 'node_ids and excerpts must be empty when retrieval is false');
      if (new Set(nodeIds).size !== nodeIds.length) err(id, 'log-node-ids', 'node_ids are not unique');
      for (const n of nodeIds) if (typeof n !== 'string' || !V.paths[n]) err(id, 'log-node-ids', `node id ${JSON.stringify(n)} is not in vault/_paths.md`);
      for (const x of excerpts) {
        if (!exactKeys(x, ['node_id', 'text'], id, 'excerpt')) continue;
        if (!nodeIds.includes(x.node_id)) err(id, 'excerpt-node', `excerpt node_id ${JSON.stringify(x.node_id)} is not listed in node_ids`);
        if (typeof x.text !== 'string' || x.text.trim().split(/\s+/).length < 3) { err(id, 'excerpt-length', 'excerpt text must be a string of at least 3 words'); continue; }
        const body = typeof x.node_id === 'string' && V.paths[x.node_id] ? V.node(x.node_id) : null;
        if (body !== null && !body.includes(x.text.replace(/\r\n/g, '\n'))) err(id, 'excerpt-verbatim', `excerpt not found word for word in node ${x.node_id}: ${JSON.stringify(x.text)}`);
      }
    });
  }

  // ---- log <-> content consistency ----
  for (const [id, text] of Object.entries(texts)) {
    const byLoop = entries[id] || {};
    if (!byLoop[0]) { err(id, 'log-missing', 'no loop-0 log entry for this item'); continue; }
    const last = byLoop[Math.max(...Object.keys(byLoop).map(Number))];
    if (last.output !== text) err(id, 'log-output', `highest-loop log output differs from the content text (loop ${last.loop})`);
    if (fileStem !== 'upgrade_texts' && fileStem !== 'weapon_texts' && typeof text === 'string') {
      const excerptText = Object.values(byLoop).flatMap((e) => (Array.isArray(e.excerpts) ? e.excerpts : [])).map((x) => (isObj(x) ? String(x.text) : ''));
      const runs = new Set(excerptText.flatMap((t) => t.match(/[0-9]+/g) || []));
      // Screen texts: every slot of the item, not only the traced title.
      const all = (screenSlots[id] || [text]).filter((t) => typeof t === 'string');
      for (const t of all) for (const d of t.match(/[0-9]+/g) || []) if (!runs.has(d)) err(id, 'number-not-in-excerpt', `number "${d}" does not appear in an excerpt of this item's log entries`);
    }
  }

  // ---- critic review ----
  if (!reviewFile && L && typeof L.run_id === 'string') {
    const guess = path.join(path.dirname(logFile), `${L.run_id}.review.json`);
    if (fs.existsSync(guess)) reviewFile = guess;
  }
  const failed = {};             // loop -> Set(item ids failed in that round)
  if (reviewFile) {
    const R = readJson(reviewFile, 'review file');
    if (R !== undefined && exactKeys(R, ['run_id', 'rounds'], null, 'review')) {
      if (R.run_id !== C.run_id) err(null, 'review-run-id', `review run_id ${JSON.stringify(R.run_id)} differs from the content run_id`);
      if (path.basename(reviewFile) !== `${R.run_id}.review.json`) err(null, 'review-file-name', 'review file must be named <run-id>.review.json');
      const rounds = Array.isArray(R.rounds) ? R.rounds : (err(null, 'type', 'rounds must be an array'), []);
      if (rounds.length > 3) err(null, 'review-rounds', 'at most 3 rounds (loops 0, 1, 2)');
      rounds.forEach((r, i) => {
        if (!exactKeys(r, ['loop', 'reviews'], null, `round ${i}`)) return;
        if (r.loop !== i) err(null, 'review-rounds', `round ${i} must have loop ${i}, got ${JSON.stringify(r.loop)}`);
        const reviews = Array.isArray(r.reviews) ? r.reviews : (err(null, 'type', `round ${i}: reviews must be an array`), []);
        failed[i] = new Set();
        const seen = new Set();
        for (const v of reviews) {
          const id = isObj(v) && typeof v.item_id === 'string' ? v.item_id : null;
          if (!exactKeys(v, ['item_id', 'verdict', 'rule', 'reason', 'suggestion'], id, `round ${i} review`)) continue;
          if (id === null || !(id in texts)) { err(id, 'review-item', `round ${i}: review for an id absent from the content file`); continue; }
          if (seen.has(id)) err(id, 'review-item', `round ${i}: more than one review`);
          seen.add(id);
          if (v.verdict === 'pass') {
            if (v.rule !== null) err(id, 'review-pass', `round ${i}: a pass must have rule null`);
            if (typeof v.reason !== 'string') err(id, 'review-pass', `round ${i}: reason must be a string`);
            if (v.suggestion !== null) err(id, 'review-pass', `round ${i}: a pass must have suggestion null`);
          } else if (v.verdict === 'fail') {
            failed[i].add(id);
            if (!V.rules.has(v.rule)) err(id, 'review-rule', `round ${i}: rule ${JSON.stringify(v.rule)} is not an existing rule id (L1-L4, T1-T9)`);
            if (typeof v.reason !== 'string' || v.reason.trim() === '' || /[\r\n]/.test(v.reason)) err(id, 'review-reason', `round ${i}: a fail needs a one-line non-empty reason`);
            checkText(id, `round ${i} suggestion`, v.suggestion, spec);
          } else err(id, 'review-verdict', `round ${i}: verdict must be "pass" or "fail"`);
        }
        for (const id of Object.keys(texts)) if (!seen.has(id)) err(id, 'review-item', `round ${i}: no review for this item`);
      });
    }
  }

  // ---- correction loops ----
  for (const [id, byLoop] of Object.entries(entries)) {
    for (const k of Object.keys(byLoop).map(Number)) {
      if (k === 0) continue;
      if (!failed[k - 1]) err(id, 'log-correction', `loop ${k} entry without a review round ${k - 1} (pass --review)`);
      else if (!failed[k - 1].has(id)) err(id, 'log-correction', `loop ${k} entry but the item did not fail review round ${k - 1}`);
    }
  }
  for (const [loop, ids] of Object.entries(failed)) for (const id of ids) {
    const k = Number(loop) + 1;
    if (k > 2) err(id, 'failing-item', 'failed review round 2: no correction loop left, the owner rules');
    else if (!(entries[id] && entries[id][k])) err(id, 'failing-item', `failed review round ${loop} and has no loop ${k} correction`);
  }

  finish();
}

// ---------------------------------------------------------------- self test
// Each case: tests/<case>/ holds <stem>.json, its log, an optional review, and
// expected.jsonl (the exact error lines; empty file = must pass with exit 0).
function selftest() {
  let ok = true;
  const cases = fs.readdirSync(TESTS, { withFileTypes: true }).filter((d) => d.isDirectory()).map((d) => d.name).sort();
  for (const c of cases) {
    const dir = path.join(TESTS, c);
    const files = fs.readdirSync(dir);
    const content = files.find((f) => FILES[f.replace(/\.json$/, '')] && f.endsWith('.json'));
    const log = files.find((f) => RUN_ID_RE.test(f.replace(/\.json$/, '')) && !f.endsWith('.review.json'));
    const review = files.find((f) => f.endsWith('.review.json'));
    const expected = fs.readFileSync(path.join(dir, 'expected.jsonl'), 'utf8').replace(/\r\n/g, '\n').split('\n').filter(Boolean);
    const args = [__filename, rel(path.join(dir, content)), '--log', rel(path.join(dir, log))];
    if (review) args.push('--review', rel(path.join(dir, review)));
    const r = spawnSync(process.execPath, args, { cwd: REPO, encoding: 'utf8' });
    const got = r.stdout.replace(/\r\n/g, '\n').split('\n').filter(Boolean);
    const wantExit = expected.length ? 1 : 0;
    const missing = expected.filter((l) => !got.includes(l));
    const extra = got.filter((l) => !expected.includes(l));
    const pass = r.status === wantExit && !missing.length && !extra.length;
    ok = ok && pass;
    console.log(`${pass ? 'PASS' : 'FAIL'} ${c}: exit ${r.status} (expected ${wantExit}), ${got.length} error line(s) (expected ${expected.length})`);
    for (const l of missing) console.log(`  missing: ${l}`);
    for (const l of extra) console.log(`  unexpected: ${l}`);
  }
  console.log(ok ? `selftest OK (${cases.length} cases)` : 'selftest FAILED');
  process.exit(ok ? 0 : 1);
}
