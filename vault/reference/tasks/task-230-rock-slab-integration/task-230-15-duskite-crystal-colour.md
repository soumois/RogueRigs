---
id: task-230-15-duskite-crystal-colour
description: "Duskite ore crystals show their ore colour (violet) again instead of grey."
parent: task-230-rock-slab-integration
kind: task
status: in-progress
---
# Duskite ore crystals show their ore colour (violet) again instead of grey

- goal: Duskite crystal clusters read violet as before task-230 (owner 2026-10-18, after the 230-12 check: they render grey, hard to tell from rock; heal crystals are fine). Find why (OreCrystalMaterial null keeps the SM_OreChunk_01 materials; the ore colour used to come from OreColorCustomDataIndex floats) and fix it in the asset or report the C++ change needed.
- agent: bp
- depends_on: task-230-14-barrel-chain-crash
- inputs: vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md, vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md, vault/reference/tech/blueprints/fx/bp-pickup-ore.md, vault/reference/tech/blueprints/world/bp-arena-grid.md, vault/reference/gdd/art-audio/color-code.md, vault/reference/agents/bp-mcp-lessons.md
- expected_output: cause, fix (material or BP_ArenaGrid property), doc nodes updated.
- done_when: assets saved; docs updated; committed; the look is checked in task-230-17.
- editor_state: open
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
