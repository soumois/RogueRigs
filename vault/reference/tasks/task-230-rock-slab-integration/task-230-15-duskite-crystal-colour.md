---
id: task-230-15-duskite-crystal-colour
description: "Duskite ore crystals show their ore colour (violet) again instead of grey."
parent: task-230-rock-slab-integration
kind: task
status: done
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
- model: sonnet
- tokens: input 82, cache write 79966, cache read 2046824, output 721
- runs: 1
- milestone: M2
- result: Commit 2276a626. BP_ArenaGrid OreCrystalMaterial = MI_OreChunk_Bright (violet, both slots), same path as the working red heal crystals; grey came from the empty-override path (mesh slot materials), exact render cause not proven. Bright/dark two-tone lost on ore as on heal. Look to check in 230-17.

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
