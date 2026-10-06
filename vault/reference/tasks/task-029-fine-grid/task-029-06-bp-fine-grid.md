---
id: task-029-06-bp-fine-grid
description: "Block meshes and materials scaled to 67 uu, cracks and break debris readable at the new size, drone arc dig visual, PIE."
parent: task-029-fine-grid
kind: task
status: in-progress
---
# Fine grid visuals

- goal: the fine grid looks organic.
- agent: bp
- depends_on: task-029-05-arc-mining
- inputs: [[bp-arena-grid]], [[bp-drone]], [[visual-style]], [[color-code]], [[unreal-mcp]], [[sys-cheats]]
- expected_output: block meshes and materials at 67 uu (slight random rotation or bevel per instance if available for an organic look), marked-rock outline still visible, drone laser fan over the arc, PIE: dig a wall by contact, explosions carve rounder holes, no log error; re-save L_ArenaM1 so the old template-component instances are dropped (grid chunks rebuild at load); docs.
- done_when: PIE check with no error; docs updated; lint clean.
- editor_state: open
- token_ceiling: 110000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-029-fine-grid]] — parent global task.
