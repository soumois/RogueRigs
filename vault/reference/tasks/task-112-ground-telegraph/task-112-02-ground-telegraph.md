---
id: task-112-02-ground-telegraph
description: "M_GroundTelegraph draws sharp rings: a dotted zone circle and a solid growing circle, driven by the component parameters."
parent: task-112-ground-telegraph
kind: task
status: todo
---
# Ground telegraph material

- goal: M_GroundTelegraph draws sharp rings: a dotted zone circle and a solid growing circle, driven by the component parameters.
- agent: bp
- depends_on: task-112-01-ground-telegraph
- inputs: ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), color-code (vault/reference/gdd/art-audio/color-code.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md)
- expected_output: create the material asset at the path the C++ component reads (DefaultMaterialPath, /Game/RogueRigs/FX/M_GroundTelegraph unless task-112-01 documents another; see the doc node sys-ground-telegraph for the exact parameter names Radius, Progress, ZoneColor, ProgressColor, Opacity): Deferred Decal domain (DBuffer translucent colour or the nearest setting that works with the project), decal UVs mapped so the distance from the centre in units of the radius is available; zone ring: thin (about 1.5 % of the radius), SHARP edges with screen-space anti-aliasing (fwidth based smoothstep), dotted by an angular pattern (about 48 dashes), colour ZoneColor; progress ring: solid, same sharpness, radius = Radius x Progress, colour ProgressColor (white), also a thin disc fill at low opacity inside the zone is optional (state if you add it: the owner wants NO diffuse glow); everything outside the radius transparent; Opacity parameter; the three modes are driven by the colours the component sets. Read the material graph back to prove the node wiring; compile; save; update sys-ground-telegraph and ground-telegraph notes with the final parameter ranges; commit. If the editor MCP cannot create a material graph, say precisely which tools exist and stop: do not fake it. Rules for Blueprint work: editor OPEN, MCP via JSON-RPC to http://127.0.0.1:8000/mcp with small outputs, a few large write_graph_dsl batches, stray events deleted by connection count, set_properties values as a JSON string, bOverride_* flags true for SizeBox values, dirty-asset trick before saving (check git status shows the uasset changed), no screenshot calls (nobody sees the screen), commit after each group by name (never git add -A; untracked .claude/agents/3d.md, art.md, Tools/art/ and Art/ may belong to someone else), 40-turn limit, no python3 (use node).
- done_when: material created at the expected path, compiled with no errors, parameters match the component; docs updated; lint clean; committed.
- editor_state: open
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-112-ground-telegraph]] — parent global task.
