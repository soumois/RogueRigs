#!/usr/bin/env node
// Rogue Rigs art agent: candidate images through the Kie.ai image API (GPT Image 2).
// Spec: vault node `agent-art`; prompt method: `art-prompts`.
//
//   node Tools/art/kie_image.js --asset <name> --prompt-file <txt> [--ref <png|url>]...
//        [--aspect 1:1|16:9|9:16|4:3|3:4] [--variants N]
//        [--dry-run]
//
// With --ref the image-to-image model is used (references attached, max 16); without,
// text-to-image. A correction prompt ("Keep this exact image: ...") must pass the image
// it corrects as --ref.
// Output: Art/Source/<asset>/<run-id>-<n>.png and the run log Art/Source/<asset>/<run-id>.json
// (prompt verbatim, refs, model, task ids, credits, result URLs, files).
// Exit 0 = every variant downloaded; 1 = a task failed (the log says which); 2 = usage error.
// Resolution is capped at 1K (owner 2026-10-11): Kie prices by resolution, and 1K is
// enough for review and for Meshy input. Key: KIE_API_KEY. Paid: each variant is one task.

const fs = require('fs');
const path = require('path');
const L = require('./lib');

L.setUsage('usage: node Tools/art/kie_image.js --asset <name> --prompt-file <txt> [--ref <img>]... '
  + '[--aspect 1:1] [--variants 1] [--dry-run]');

const BASE = process.env.KIE_BASE_URL || 'https://api.kie.ai';
const MODEL_T2I = 'gpt-image-2-text-to-image';
const MODEL_I2I = 'gpt-image-2-image-to-image';
const ASPECTS = ['1:1', '16:9', '9:16', '4:3', '3:4'];
const RESOLUTIONS = ['1K']; // cap, owner 2026-10-11
const MAX_VARIANTS = 4;

async function uploadRef(ref, key) {
  if (/^https?:\/\//.test(ref)) return ref;
  if (!fs.existsSync(ref)) L.usage(`reference not found: ${ref}`);
  const res = await L.httpJson('POST', `${BASE}/api/v1/file-base64-upload`, {
    key,
    body: { base64Data: L.dataUri(ref), uploadPath: 'rogue-rigs/refs', fileName: path.basename(ref) },
  });
  const url = res.data && res.data.downloadUrl;
  if (!url) throw new Error(`upload of ${ref} returned no downloadUrl: ${JSON.stringify(res).slice(0, 300)}`);
  return url;
}

async function runTask(body, key) {
  const created = await L.httpJson('POST', `${BASE}/api/v1/jobs/createTask`, { key, body });
  const taskId = created.data && created.data.taskId;
  if (!taskId) throw new Error(`createTask returned no taskId: ${JSON.stringify(created).slice(0, 300)}`);
  const data = await L.poll(async () => {
    const r = await L.httpJson('GET', `${BASE}/api/v1/jobs/recordInfo?taskId=${encodeURIComponent(taskId)}`, { key });
    const d = r.data || {};
    if (d.state === 'success' || d.state === 'fail') return d;
    return undefined;
  }, { label: taskId });
  if (data.state === 'fail') return { taskId, state: 'fail', error: data.failMsg || 'unknown', urls: [] };
  let result = data.resultJson;
  if (typeof result === 'string') result = JSON.parse(result);
  return { taskId, state: 'success', credits: data.creditsConsumed ?? null, urls: (result && result.resultUrls) || [] };
}

async function main() {
  const a = L.parseArgs(process.argv.slice(2), { flags: ['dry-run'], multi: ['ref'] });
  const asset = L.checkAssetName(a.asset);
  if (!a['prompt-file'] || !fs.existsSync(a['prompt-file'])) L.usage('--prompt-file is required and must exist');
  const prompt = fs.readFileSync(a['prompt-file'], 'utf8').trim();
  if (!prompt) L.usage('prompt file is empty');
  const aspect = a.aspect || '1:1';
  const resolution = a.resolution || '1K';
  const variants = Number(a.variants || 1);
  if (!ASPECTS.includes(aspect)) L.usage(`--aspect must be one of ${ASPECTS.join(', ')}`);
  if (!RESOLUTIONS.includes(resolution)) L.usage(`--resolution is capped at ${RESOLUTIONS.join(', ')}`);
  if (!Number.isInteger(variants) || variants < 1 || variants > MAX_VARIANTS) L.usage(`--variants must be 1..${MAX_VARIANTS}`);
  if (a.ref.length > 16) L.usage('at most 16 --ref images');

  const model = a.ref.length ? MODEL_I2I : MODEL_T2I;
  const id = L.runId(asset);
  const outDir = path.join(L.REPO, 'Art', 'Source', asset);
  const log = {
    run_id: id, asset, model, aspect_ratio: aspect, resolution, variants,
    prompt_file: L.rel(path.resolve(a['prompt-file'])), prompt,
    refs: a.ref.map((r) => (/^https?:/.test(r) ? r : L.rel(path.resolve(r)))),
    tasks: [], files: [],
  };

  if (a['dry-run']) {
    const input = { prompt, aspect_ratio: aspect, resolution };
    if (a.ref.length) input.input_urls = a.ref.map((r) => `<upload of ${r}>`);
    console.log(JSON.stringify({ dry_run: true, run_id: id, requests: variants, body: { model, input } }, null, 2));
    return 0;
  }

  const key = L.requireKey('KIE_API_KEY');
  const urls = [];
  for (const r of a.ref) urls.push(await uploadRef(r, key));
  const input = { prompt, aspect_ratio: aspect, resolution };
  if (urls.length) input.input_urls = urls;

  const results = await Promise.all(
    Array.from({ length: variants }, () => runTask({ model, input }, key).catch((e) => ({ state: 'fail', error: e.message, urls: [] }))),
  );
  const logFile = path.join(outDir, `${id}.json`);
  for (const r of results) {
    log.tasks.push({ task_id: r.taskId || null, state: r.state, credits: r.credits ?? null, error: r.error || null, result_urls: r.urls });
  }
  L.writeJson(logFile, log); // written before downloading: paid URLs are never lost
  let n = 0;
  for (const r of results) {
    for (const u of r.urls) {
      n++;
      const dest = path.join(outDir, `${id}-${n}.png`);
      await L.download(u, dest);
      log.files.push(L.rel(dest));
    }
  }
  L.writeJson(logFile, log);
  const failed = log.tasks.filter((t) => t.state !== 'success').length;
  console.log(JSON.stringify({ run_id: id, log: L.rel(logFile), files: log.files, failed }));
  return failed ? 1 : 0;
}

main().then((code) => process.exit(code)).catch((e) => { console.error(`error: ${e.message}`); process.exit(1); });
