---
id: task-258-01-heal-deposit-vibration
description: "The heal deposit no longer vibrates by itself: only its upper part moves without contact, even in the editor; find the cause and fix it."
parent: task-258-heal-deposit-vibration
kind: task
status: in-progress
---
# Heal deposit vibration

- goal: The heal deposit no longer vibrates by itself: only its upper part moves without contact, even in the editor; find the cause and fix it.
- agent: cpp
- depends_on: none
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), sys-grid-rock-damage (vault/reference/tech/systems/world/sys-grid/sys-grid-rock-damage.md), sys-grid-rock-pieces (vault/reference/tech/systems/world/sys-grid/sys-grid-rock-pieces.md), rock-state-render-options (vault/reference/tech/systems/world/sys-grid/rock-state-render-options.md), bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md), bp-rock-slab-material (vault/reference/tech/blueprints/world/bp-rock-slab-material.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md)
- expected_output: Diagnosis (what moves, which shader or code reads which data) and fix, written in the sys docs. Owner clue: it also vibrates in the editor outside PIE, so a material or per-instance data cause is likely; a lead to check first: task-257 (2026-10-20) writes per-instance custom data indices 4 and 5 (pulse phase and rate) on the deposit crystal instanced meshes, and the rock material of the heal deposit (MI_RockSlab) or the rock slab shake may read the same indices; also check the pre-existing shake data. The fix keeps the dig shake and the pulse desync working.
- done_when: The heal deposit stays still when untouched (cause identified with numbers: which data index or node moves it), the dig shake still works, existing RogueRigs.Dig, RogueRigs.Ore, RogueRigs.PulseDesync, RogueRigs.Integration.FineContactDigOpensPassage and RogueRigs.RunFlow tests pass; docs updated; lint clean. If the cause is in a Blueprint or material asset, report it for a bp subtask instead of editing it.
- editor_state: closed (C++ build); the owner has no editor open
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-258-heal-deposit-vibration]] — parent global task.
