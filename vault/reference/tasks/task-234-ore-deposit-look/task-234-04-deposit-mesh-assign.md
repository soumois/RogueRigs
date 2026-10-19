---
id: task-234-04-deposit-mesh-assign
description: "Import the Duskite deposit meshes and assign them with the violet material in BP_ArenaGrid."
parent: task-234-ore-deposit-look
kind: task
status: in-progress
---
# Import the Duskite deposit meshes and assign them with the violet material in BP_ArenaGrid

- goal: SM_DuskiteDeposit_01..03 imported under /Game/RogueRigs/Art/OreDeposit/ (FBX_SCALE_UNITS, flat normals, Nanite on), slot material MI_OreChunk_Bright, BP_ArenaGrid Duskite deposit mesh list set (property from 234-03).
- agent: bp
- depends_on: task-234-02-duskite-deposit-meshes, task-234-03-deposit-crystal-visual, task-234-07-heal-deposit-meshes
- inputs: vault/reference/tech/blueprints/world/bp-arena-grid.md, vault/reference/tech/blueprints/fx/bp-pickup-ore.md, vault/reference/agents/bp-mcp-lessons.md, vault/reference/agents/art-3d-lessons.md; the 234-02 report json
- expected_output: also SM_HealDeposit_01..03 (slots rock: a lavender rock material matching the slab; crystal: MI_HealCrystal_Bright) and the heal deposit list; assets saved, BP compiled and saved, bp-arena-grid doc updated.
- done_when: bounds read back match the report; BP compiled; committed.
- editor_state: open
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-234-ore-deposit-look]] — parent global task.
