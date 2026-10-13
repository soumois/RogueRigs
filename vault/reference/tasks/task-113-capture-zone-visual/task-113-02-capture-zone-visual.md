---
id: task-113-02-capture-zone-visual
description: "The old diffuse growing circle of the console and chest is removed from their Blueprints."
parent: task-113-capture-zone-visual
kind: task
status: done
---
# Remove the old diffuse circle

- goal: The old diffuse growing circle of the console and chest is removed from their Blueprints.
- agent: bp
- depends_on: task-113-01-capture-zone-visual, task-112-02-ground-telegraph
- inputs: bp-objective-console (vault/reference/tech/blueprints/bp-objective-console.md), bp-bonus-chest (vault/reference/tech/blueprints/bp-bonus-chest.md), objective-console (vault/reference/gdd/run/objective-console.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md)
- expected_output: EXACT LIST from task-113-01 (commits 5db7481, ddf56c3; sys-objectives): in BP_ObjectiveConsole remove the HackRing StaticMeshComponent (engine Plane, MI_GroundRingTeal) and the SetRelativeScale3D(HackRing) call in its OnHackProgress graph; in BP_BonusChest remove the HackRing StaticMeshComponent (engine Plane, MI_GroundRingGold) and the same SetRelativeScale3D call in its OnHackProgress graph; KEEP the SetScalarParameterValueOnMaterials(Mesh, GlowAmount, ...) calls and the chest OnRewardGranted glow burst (they light the mesh). The material asset MI_GroundRingTeal / MI_GroundRingGold may be left in place (state whether anything else uses them; do not delete assets). Original text: remove the old diffuse growing circle effect (the exact component, mesh, particle or material that task-113-01 reports) from BP_ObjectiveConsole and BP_BonusChest and any of their events that only drove it, so the C++ ground telegraph is the only zone visual; keep every other visual (gauge, marked rocks outline, mesh). Compile 0 errors; save; update bp-objective-console and bp-bonus-chest; commit. Rules for Blueprint work: editor OPEN, MCP via JSON-RPC to http://127.0.0.1:8000/mcp with small outputs, a few large write_graph_dsl batches, stray events deleted by connection count, set_properties values as a JSON string, bOverride_* flags true for SizeBox values, dirty-asset trick before saving (check git status shows the uasset changed), no screenshot calls (nobody sees the screen), commit after each group by name (never git add -A; untracked .claude/agents/3d.md, art.md, Tools/art/ and Art/ may belong to someone else), 40-turn limit, no python3 (use node).
- done_when: old circle removed from both Blueprints, compiled 0 errors, saved, docs updated, lint clean, committed.
- editor_state: open
- token_ceiling: 40000
- model: sonnet
- tokens: input 60, cache write 44093, cache read 841806, output 14861
- runs: 1
- milestone: M2
- result: HackRing (engine Plane) and its SetRelativeScale3D call removed from BP_ObjectiveConsole and BP_BonusChest; glow calls kept; MI_GroundRingTeal and MI_GroundRingGold have no referencers (left in place); not seen in PIE. Commits 75d1513, 7c24452, f983540.

## Relations
- [[task-113-capture-zone-visual]] — parent global task.
