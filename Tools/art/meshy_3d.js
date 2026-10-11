#!/usr/bin/env node
// Rogue Rigs 3D agent: owner-validated image -> raw mesh through the Meshy image-to-3D API.
// Spec: vault nodes `agent-3d` and `asset-pipeline`.
//
//   node Tools/art/meshy_3d.js --asset <name> --image vault/assets/<file>
//        [--mode smart-topology|standard] [--polycount N] [--pose t-pose|a-pose]
//        [--no-texture] [--dry-run]
//   node Tools/art/meshy_3d.js --asset <name> --resume <task-id>
//
// Gate: --image must be inside vault/assets/ (only owner-validated images are copied there).
// --resume polls an existing task and downloads it without paying again.
// smart-topology = ai_model meshy-t2, polycount 100..15000 (default 4000);
// standard = ai_model latest with remesh, polycount 100..300000 (default 30000).
// Output: Art/Meshes/<asset>/raw/<run-id>.fbx|.glb (+ base color texture when textured)
// and the run log Art/Meshes/<asset>/raw/<run-id>.json.
// Exit 0 = downloaded; 1 = task failed; 2 = usage error. Key: MESHY_API_KEY. Paid.

const path = require('path');
const fs = require('fs');
const L = require('./lib');

L.setUsage('usage: node Tools/art/meshy_3d.js --asset <name> --image vault/assets/<file> '
  + '[--mode smart-topology|standard] [--polycount N] [--pose t-pose|a-pose] [--no-texture] [--dry-run]\n'
  + '       node Tools/art/meshy_3d.js --asset <name> --resume <task-id>');

const BASE = process.env.MESHY_BASE_URL || 'https://api.meshy.ai';
const ENDPOINT = `${BASE}/openapi/v1/image-to-3d`;
const MODES = {
  'smart-topology': { min: 100, max: 15000, def: 4000 },
  standard: { min: 100, max: 300000, def: 30000 },
};
const FORMATS = ['fbx', 'glb'];

function buildBody(a) {
  const mode = a.mode || 'smart-topology';
  const m = MODES[mode];
  if (!m) L.usage(`--mode must be one of ${Object.keys(MODES).join(', ')}`);
  const polycount = Number(a.polycount || m.def);
  if (!Number.isInteger(polycount) || polycount < m.min || polycount > m.max) L.usage(`--polycount must be ${m.min}..${m.max} for ${mode}`);
  if (a.pose && !['t-pose', 'a-pose'].includes(a.pose)) L.usage('--pose must be t-pose or a-pose');

  const body = {
    target_polycount: polycount,
    should_texture: !a['no-texture'],
    enable_pbr: false,
    target_formats: FORMATS,
  };
  if (mode === 'smart-topology') { body.model_type = 'smart-topology'; body.ai_model = 'meshy-t2'; }
  else { body.model_type = 'standard'; body.ai_model = 'latest'; body.should_remesh = true; body.topology = 'triangle'; }
  if (a.pose) body.pose_mode = a.pose;
  return { body, mode };
}

function checkValidated(image) {
  const abs = path.resolve(image);
  const assets = path.join(L.REPO, 'vault', 'assets') + path.sep;
  if (!abs.toLowerCase().startsWith(assets.toLowerCase())) L.usage('--image must be an owner-validated image inside vault/assets/');
  if (!fs.existsSync(abs)) L.usage(`image not found: ${image}`);
  return abs;
}

async function waitAndDownload(taskId, key, outDir, id, log) {
  const task = await L.poll(async () => {
    const t = await L.httpJson('GET', `${ENDPOINT}/${encodeURIComponent(taskId)}`, { key });
    if (['SUCCEEDED', 'FAILED', 'CANCELED'].includes(t.status)) return t;
    return undefined;
  }, { firstMs: 5000, maxMs: 20000, timeoutMs: 30 * 60 * 1000, label: taskId });
  log.status = task.status;
  log.credits = task.consumed_credits ?? null;
  if (task.status !== 'SUCCEEDED') {
    log.error = (task.task_error && task.task_error.message) || task.status;
    return false;
  }
  for (const fmt of FORMATS) {
    const url = task.model_urls && task.model_urls[fmt];
    if (!url) continue;
    const dest = path.join(outDir, `${id}.${fmt}`);
    await L.download(url, dest);
    log.files.push(L.rel(dest));
  }
  const tex = (task.texture_urls || [])[0];
  if (tex && tex.base_color) {
    const dest = path.join(outDir, `${id}-base-color.png`);
    await L.download(tex.base_color, dest);
    log.files.push(L.rel(dest));
  }
  return true;
}

async function main() {
  const a = L.parseArgs(process.argv.slice(2), { flags: ['dry-run', 'no-texture'] });
  const asset = L.checkAssetName(a.asset);
  const outDir = path.join(L.REPO, 'Art', 'Meshes', asset, 'raw');

  if (a.resume) {
    const key = L.requireKey('MESHY_API_KEY');
    const id = L.runId(asset);
    const log = { run_id: id, asset, resumed_task_id: a.resume, task_id: a.resume, files: [] };
    const ok = await waitAndDownload(a.resume, key, outDir, id, log);
    L.writeJson(path.join(outDir, `${id}.json`), log);
    console.log(JSON.stringify({ run_id: id, status: log.status, files: log.files }));
    return ok ? 0 : 1;
  }

  if (!a.image) L.usage('--image is required (or --resume)');
  const image = checkValidated(a.image);
  const { body, mode } = buildBody(a);
  const id = L.runId(asset);

  if (a['dry-run']) {
    console.log(JSON.stringify({ dry_run: true, run_id: id, endpoint: ENDPOINT, body: { image_url: `<data uri of ${L.rel(image)}>`, ...body } }, null, 2));
    return 0;
  }

  const key = L.requireKey('MESHY_API_KEY');
  const created = await L.httpJson('POST', ENDPOINT, { key, body: { image_url: L.dataUri(image), ...body } });
  const taskId = created.result;
  if (!taskId) throw new Error(`Meshy returned no task id: ${JSON.stringify(created).slice(0, 300)}`);
  const log = { run_id: id, asset, mode, image: L.rel(image), request: body, task_id: taskId, files: [] };
  const logFile = path.join(outDir, `${id}.json`);
  L.writeJson(logFile, log); // task id saved first: an interrupted run is resumed with --resume
  const ok = await waitAndDownload(taskId, key, outDir, id, log);
  L.writeJson(logFile, log);
  console.log(JSON.stringify({ run_id: id, task_id: taskId, status: log.status, files: log.files, log: L.rel(logFile) }));
  return ok ? 0 : 1;
}

main().then((code) => process.exit(code)).catch((e) => { console.error(`error: ${e.message}`); process.exit(1); });
