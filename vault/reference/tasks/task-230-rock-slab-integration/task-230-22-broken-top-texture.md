---
id: task-230-22-broken-top-texture
description: "Broken rock pieces show the same textured top as intact pieces instead of flat lavender squares."
parent: task-230-rock-slab-integration
kind: task
status: in-progress
---
# Broken rock pieces show the same textured top as intact pieces instead of flat lavender squares

- goal: broken piece block tops draw the T_RockTop world-UV top, cracks and tint like intact tops (owner 2026-10-19, after the 230-19 captures (Saved/RockSlab/after3/): rs_s_sag_close_00 and rs_h_crystals_00 show flat plain lavender squares on broken cells). Broken meshes are off Nanite and carry vertex colours (top R 1, rim R 0.5, sides G, bottoms B, task-230-20); intact tops are R 0. Find why the broken branch draws them flat (top/side detection, broken flag, vertex colour reading, WPO tilt versus normal).
- agent: bp
- depends_on: task-230-21-rock-rim-tone-restore
- inputs: vault/reference/tech/blueprints/world/bp-rock-slab-material.md, vault/reference/agents/bp-mcp-lessons.md, Art/Meshes/rock-slab/SM_RockSlabBroken.report.json; captures Saved/RockSlab/after3/
- expected_output: cause with evidence, fix in M_RockSlab, doc updated.
- done_when: material compiles; saved; doc updated; committed; look checked in task-230-26.
- editor_state: open
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
