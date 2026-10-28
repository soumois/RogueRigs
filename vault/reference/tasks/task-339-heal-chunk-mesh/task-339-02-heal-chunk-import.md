---
id: task-339-02-heal-chunk-import
description: "BP: import SM_HealChunk_01..03 into Unreal and give their two slots new red glowing instances MI_HealChunk_Bright and MI_HealChunk_Dark."
parent: task-339-heal-chunk-mesh
kind: task
status: wait-answers
---
# Heal pickup chunks: import and materials

- goal: import `Art/Meshes/HealChunk/SM_HealChunk_01.fbx`, `_02`, `_03` into `/Game/RogueRigs/Art/HealChunk/` through Unreal MCP (no extra lightmap UV channel if avoidable, [[blender-asset-route]] quirks); Nanite off (M_XPNutGlow has no Nanite usage, see deposit_use in [[bp-pickup-ore]]). Create in the same folder `MI_HealChunk_Bright` and `MI_HealChunk_Dark` as duplicates of `/Game/RogueRigs/Art/OreChunk/MI_HealCrystal_Bright` and `MI_HealCrystal_Dark` (parent M_XPNutGlow, same values: Bright Color and GlowColor linear (1.0, 0.0, 0.05), GlowAmount 8; Dark (0.5, 0.0, 0.025), GlowAmount 4; PulseAmount 0.4), so the pickup can be tuned alone without changing the grid heal crystals (global `gdd_check` Q3, provisional). Set them as slot defaults on the three meshes: `crystal_bright` = MI_HealChunk_Bright, `crystal_dark` = MI_HealChunk_Dark. MI_HealCrystal_* and M_XPNutGlow are not modified.
- agent: bp
- depends_on: task-339-01-heal-chunk-mesh
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), bp-pickup-heal (vault/reference/tech/blueprints/fx/bp-pickup-heal.md), color-code (vault/reference/gdd/art-audio/color-code.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md); `Art/Meshes/HealChunk/SM_HealChunk_01..03.fbx` and their `.report.json`
- expected_output: SM_HealChunk_01..03, MI_HealChunk_Bright and MI_HealChunk_Dark in `/Game/RogueRigs/Art/HealChunk/`, saved by explicit path; change summary for the doc subtask (asset paths, bounds, tris, slot defaults, parameter values read back). No doc node written (doc subtask).
- done_when: mesh bounds read back (longest side about 30 cm, centred) and tris match the reports; each slot of the three meshes has its instance; instance parameters read back equal to the values above; MI_HealCrystal_* unchanged; materials compile; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T01:53:54Z
- waiting_for: owner opens the Unreal editor (MCP at 127.0.0.1:8000/mcp unreachable)
- result: open

## Relations
- [[task-339-heal-chunk-mesh]] — parent global task.
