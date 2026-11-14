#!/usr/bin/env node
// Rogue Rigs - JSON-RPC client of the Unreal editor MCP server (http://127.0.0.1:8000/mcp) with the UMG helpers and the shared
// UI presets the executors rewrote in each task (task-549-01, owner 2026-11-13: "generalise the UI habits and scripts").
// Tasks 541, 545, 546, 547 and 548 each kept a hand-written client in a scratchpad (lib.js / mcp.js) when the mcp__unreal tools
// were not loaded; the same calls, brushes and anchors are here once. Node 18+ (global fetch), no dependencies.
// The editor must be open (ECONNREFUSED = closed: launch it, see build-toolchain). Tool names and argument shapes:
// Tools/unreal/mcp-tool-sheet.md.
//
// CLI:
//   node Tools/unreal/mcp-client.js list                               tools of the server (name and schema head)
//   node Tools/unreal/mcp-client.js call <toolset> <tool> ['<json>']   one call_tool; toolset = alias below or full name
//   node Tools/unreal/mcp-client.js batch <calls.json>                 [{ "toolset", "tool", "args", "label"?, "max"? }, ...]
//   node Tools/unreal/mcp-client.js brushes [WBP_Name ...]             audit: every textured brush of the widgets (default
//                                                                      the screens under /Game/RogueRigs/UI), grouped by texture
//   node Tools/unreal/mcp-client.js tree <WBP path or name> [depth]    widget tree with classes (GetWidgetDescription)
// Exit code: 0 done, 1 a call failed or the server is unreachable, 2 bad arguments.
//
// Library (require('<repo>/Tools/unreal/mcp-client.js')):
//   call(toolset, tool, args) -> text of the answer (throws on a tool error); json(toolset, tool, args) -> parsed returnValue
//   ref(path), wbp(name) -> '/Game/RogueRigs/UI/<name>.<name>' (paths kept as is), widget(W, name) -> '<W>:WidgetTree.<name>'
//   getProps(path, [names]) -> object, setProps(path, values), slotOf(W, name) -> slot path
//   brush(tex, { drawAs, size: [w, h], margin: [l, t, r, b], tint: [r, g, b, a] }), point(...), stretch(...) layouts
//   addWidget(W, cls, name, parent, index) -> slot path, addImage(W, name, parent, brush, layout, extra), compile(W), save(paths)
//   UI: the shared look values (frame, title bar, plate button states, bolt and bolted bars) as read on the screens in task-549.

const URL_ = process.env.RR_MCP_URL || 'http://127.0.0.1:8000/mcp';
const TOOLSETS = {
  umg: 'UMGToolSet.UMGToolSet',
  obj: 'editor_toolset.toolsets.object.ObjectTools',
  asset: 'editor_toolset.toolsets.asset.AssetTools',
  bp: 'editor_toolset.toolsets.blueprint.BlueprintTools',
  tex: 'editor_toolset.toolsets.texture.TextureTools',
  app: 'EditorToolset.EditorAppToolset',
  logs: 'EditorToolset.LogsToolset',
};
const UI_DIR = '/Game/RogueRigs/UI';

let sid = null;
let nextId = 1;
let ready = null;

async function rpc(method, params, notify) {
  const body = { jsonrpc: '2.0', method, params };
  if (!notify) body.id = nextId++;
  const headers = { 'content-type': 'application/json', accept: 'application/json, text/event-stream' };
  if (sid) headers['mcp-session-id'] = sid;
  const r = await fetch(URL_, { method: 'POST', headers, body: JSON.stringify(body) });
  const s = r.headers.get('mcp-session-id');
  if (s) sid = s;
  const t = await r.text();
  if (notify || !t) return null;
  const events = t.match(/data: (.*)/g); // streamable HTTP answers as server-sent events
  return events ? JSON.parse(events[events.length - 1].slice(6)) : JSON.parse(t);
}

function init() {
  if (!ready) {
    ready = (async () => {
      await rpc('initialize', { protocolVersion: '2025-03-26', capabilities: {}, clientInfo: { name: 'rr-mcp-client', version: '1' } });
      await rpc('notifications/initialized', {}, true);
    })();
  }
  return ready;
}

async function call(toolset, tool, args = {}) {
  await init();
  const res = await rpc('tools/call', { name: 'call_tool', arguments: { toolset_name: TOOLSETS[toolset] || toolset, tool_name: tool, arguments: args } });
  if (!res || res.error) throw new Error(`${tool}: ${JSON.stringify(res && res.error)}`);
  const text = (res.result.content || []).map((x) => x.text).join('\n');
  if (res.result.isError) throw new Error(`${tool}: ${text.slice(0, 2000)}`);
  return text;
}

// Parsed returnValue; a returnValue that is itself a JSON string (get_properties) is parsed too.
async function json(toolset, tool, args) {
  const text = await call(toolset, tool, args);
  let v = JSON.parse(text);
  v = v && v.returnValue !== undefined ? v.returnValue : v;
  if (typeof v === 'string') { try { v = JSON.parse(v); } catch { /* plain string */ } }
  return v;
}

const ref = (p) => ({ refPath: p });
const wbp = (name) => (name.startsWith('/') ? name : `${UI_DIR}/${name}.${name}`);
const widget = (W, name) => `${W}:WidgetTree.${name}`;

const getProps = (p, names) => json('obj', 'get_properties', { instance: ref(p), properties: names });
const setProps = (p, values) => call('obj', 'set_properties', { instance: ref(p), values: JSON.stringify(values) });

async function slotOf(W, name) {
  const d = await json('umg', 'GetWidgetDescription', { widgetBlueprint: ref(W), startWidget: ref(widget(W, name)), maxDepth: 0 });
  const s = d.widgets[0].slot;
  return s.refPath || s;
}

const colour = ([r, g, b, a = 1]) => ({ specifiedColor: { r, g, b, a }, colorUseRule: 'UseColor_Specified' });
function brush(tex, { drawAs = 'Image', size = [32, 32], margin = [0, 0, 0, 0], tint = [1, 1, 1, 1] } = {}) {
  const [left, top, right, bottom] = margin;
  return { tintColor: colour(tint), drawAs, tiling: 'NoTile', imageSize: { x: size[0], y: size[1] }, margin: { left, top, right, bottom },
    resourceObject: tex ? ref(tex) : 'None' };
}

// Canvas slot layouts (umg-layout rule): a point anchor with a size, or a stretch between two anchors with insets.
const point = (ax, ay, x, y, w, h, alx = 0.5, aly = 0.5) =>
  ({ offsets: { left: x, top: y, right: w, bottom: h }, anchors: { minimum: { x: ax, y: ay }, maximum: { x: ax, y: ay } }, alignment: { x: alx, y: aly } });
const stretch = (minX, minY, maxX, maxY, l = 0, t = 0, r = 0, b = 0, alx = 0, aly = 0) =>
  ({ offsets: { left: l, top: t, right: r, bottom: b }, anchors: { minimum: { x: minX, y: minY }, maximum: { x: maxX, y: maxY } }, alignment: { x: alx, y: aly } });

async function addWidget(W, cls, name, parent = 'Root', index) {
  const args = { widgetBlueprint: ref(W), widgetClass: ref(cls.includes('/') ? cls : `/Script/UMG.${cls}`), widgetDisplayName: name, parentWidget: ref(widget(W, parent)) };
  if (index !== undefined) args.childIndex = index;
  const v = await json('umg', 'AddWidget', args);
  return v && v.slot ? (v.slot.refPath || v.slot) : slotOf(W, name);
}

async function addImage(W, name, parent, imageBrush, layout, extra = {}) {
  const slot = await addWidget(W, 'Image', name, parent);
  await setProps(widget(W, name), { Brush: imageBrush, ...extra });
  await setProps(slot, { LayoutData: layout, bAutoSize: false });
  return slot;
}

const compile = (W) => call('umg', 'CompileWidgetBlueprint', { widgetBlueprint: ref(W) });
const save = (paths) => call('asset', 'save_assets', { asset_paths: paths.map((p) => p.replace(/\.[^./]+$/, '')) });

// Shared look values, as every screen used them on 2026-11-14 (read back with the `brushes` command, task-549). Values, not
// decisions: a screen that needs another size passes its own; changing one here changes no existing widget.
const TEX = {
  frame: `${UI_DIR}/LevelEnd/T_LevelEnd_Frame.T_LevelEnd_Frame`,
  titleBar: `${UI_DIR}/Shop/T_Shop_TitleBar.T_Shop_TitleBar`,
  plate: `${UI_DIR}/Shop/T_Shop_ButtonContinue.T_Shop_ButtonContinue`,
  bolt: `${UI_DIR}/Shared/T_UI_Bolt.T_UI_Bolt`,
  barH: `${UI_DIR}/Shared/T_UI_BarH.T_UI_BarH`,
  barV: `${UI_DIR}/Shared/T_UI_BarV.T_UI_BarV`,
};
const UI = {
  TEX,
  // 9-slice panel frame of the pause, death, stats, options (and its confirm dialog) and loading screens.
  frameBrush: () => brush(TEX.frame, { drawAs: 'Box', size: [942, 458], margin: [0.0955, 0.1878, 0.0955, 0.1878] }),
  // Title bar pair beside a screen title: Image at its size (death and options 210 x 29, stats and shop 356 x 48), or the pause
  // Box variant (276 x 38, margins 0.14 / 0.18) stretched to its slot; the right bar is the left one with render scale X -1.
  titleBarBrush: (size = [356, 48], box = false) => (box
    ? brush(TEX.titleBar, { drawAs: 'Box', size, margin: [0.14, 0, 0.18, 0] })
    : brush(TEX.titleBar, { size })),
  // Cream plate button: Box, side margins 0.155; size 640 x 163 (menus, pause, death, options, level-up, alert), 416 x 106
  // (stats and shop Continue), 302 x 77 (options tabs). Hovered = Normal on attached buttons (sys-button-hover).
  plateStyle: (size = [640, 163], { hovered = 1.3, pressed = 0.7, disabled = [0.35, 0.35, 0.35, 1] } = {}) => {
    const b = (tint) => brush(TEX.plate, { drawAs: 'Box', size, margin: [0.155, 0, 0.155, 0], tint });
    return { normal: b([1, 1, 1, 1]), hovered: b([hovered, hovered, hovered, 1]), pressed: b([pressed, pressed, pressed, 1]), disabled: b(disabled) };
  },
  // Shared bolted bars (wbp-ui-bolted-bar): widget classes to AddWidget; bar 16 and bolt 34 slate units thick.
  boltedBarH: `${UI_DIR}/Shared/WBP_UI_BoltedBarH.WBP_UI_BoltedBarH_C`,
  boltedBarV: `${UI_DIR}/Shared/WBP_UI_BoltedBarV.WBP_UI_BoltedBarV_C`,
  boltSize: 34,
  barThickness: 16,
};

module.exports = { TOOLSETS, rpc, init, call, json, ref, wbp, widget, getProps, setProps, slotOf, brush, point, stretch, addWidget, addImage, compile, save, UI };

// ---------- CLI ----------
const SCREENS = ['WBP_MainMenu', 'WBP_PauseMenu', 'WBP_Options', 'WBP_Intermission', 'WBP_ShopScreen', 'WBP_ShopCard', 'WBP_RunLost',
  'WBP_Loading', 'WBP_UpgradeChoice', 'WBP_AlertObjectives', 'WBP_HUD', 'WBP_Weapons', 'WBP_HealthStatus', 'WBP_XPStatus'];

function brushLine(b) {
  if (!b || !b.resourceObject || !b.resourceObject.refPath) return null;
  const t = b.tintColor && b.tintColor.specifiedColor;
  const m = b.margin;
  const n = (v) => +Number(v).toFixed(3);
  return { tex: b.resourceObject.refPath.split('.').pop(),
    text: `${b.drawAs} ${n(b.imageSize.x)}x${n(b.imageSize.y)} margin ${[m.left, m.top, m.right, m.bottom].map(n).join('/')}${t ? ` tint ${[t.r, t.g, t.b, t.a].map((v) => n(v)).join(',')}` : ''}` };
}

async function brushes(names) {
  const rows = new Map();
  for (const name of names) {
    const W = wbp(name);
    const short = W.split('.').pop();
    const list = await json('umg', 'GetWidgets', { widgetBlueprint: ref(W) });
    for (const w of list.widgets) {
      const p = w.widget.refPath;
      const props = {};
      for (const k of ['Brush', 'Background', 'WidgetStyle']) {
        try { Object.assign(props, await getProps(p, [k])); } catch { /* property absent on this class */ }
      }
      const items = [];
      if (props.Brush) items.push(['', props.Brush]);
      if (props.Background) items.push(['background', props.Background]);
      if (props.WidgetStyle) for (const k of ['normal', 'hovered', 'pressed', 'disabled']) items.push([k, props.WidgetStyle[k]]);
      for (const [k, b] of items) {
        const l = brushLine(b);
        if (!l) continue;
        if (!rows.has(l.tex)) rows.set(l.tex, []);
        rows.get(l.tex).push(`${short}.${p.split('.').pop()}${k ? `.${k}` : ''}: ${l.text}`);
      }
    }
  }
  for (const [tex, list] of [...rows.entries()].sort((a, b) => b[1].length - a[1].length)) {
    const screens = new Set(list.map((x) => x.split('.')[0]));
    console.log(`${tex} (${list.length} brushes, ${screens.size} widgets: ${[...screens].join(', ')})`);
    for (const x of list) console.log(`  ${x}`);
  }
}

async function tree(name, depth) {
  const d = await json('umg', 'GetWidgetDescription', { widgetBlueprint: ref(wbp(name)), maxDepth: depth });
  console.log(JSON.stringify(d, null, 1).slice(0, 20000));
}

async function cli() {
  const [mode, ...rest] = process.argv.slice(2);
  if (mode === 'list') {
    await init();
    const r = await rpc('tools/list', {});
    for (const t of r.result.tools) console.log(`${t.name}: ${JSON.stringify(t.inputSchema).slice(0, 300)}`);
  } else if (mode === 'call' && rest.length >= 2) {
    console.log(await call(rest[0], rest[1], rest[2] ? JSON.parse(rest[2]) : {}));
  } else if (mode === 'batch' && rest[0]) {
    const calls = JSON.parse(require('fs').readFileSync(rest[0], 'utf8'));
    let failed = false;
    for (const c of calls) {
      let out;
      try { out = await call(c.toolset, c.tool, c.args || {}); } catch (e) { out = `ERROR ${e.message}`; failed = true; }
      console.log(`== ${c.tool} ${c.label || ''}\n${out.slice(0, c.max || 4000)}`);
    }
    if (failed) process.exitCode = 1;
  } else if (mode === 'brushes') {
    await brushes(rest.length ? rest : SCREENS);
  } else if (mode === 'tree' && rest[0]) {
    await tree(rest[0], rest[1] ? Number(rest[1]) : 3);
  } else {
    console.error('Usage: node Tools/unreal/mcp-client.js list | call <toolset> <tool> [json] | batch <calls.json> | brushes [WBP ...] | tree <WBP> [depth]');
    console.error(`Toolset aliases: ${Object.entries(TOOLSETS).map(([k, v]) => `${k}=${v}`).join(', ')}`);
    process.exit(2);
  }
}

if (require.main === module) {
  cli().catch((e) => {
    console.error(/ECONNREFUSED|fetch failed/.test(String(e && (e.cause || e.message))) ? 'mcp-client: editor closed (ECONNREFUSED): launch it and retry' : `mcp-client: ${e.message}`);
    process.exit(1);
  });
}
