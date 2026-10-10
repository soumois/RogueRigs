---
id: task-070-01-ore-in-rock
description: "Ore scattered through the rock: nuggets inside rock blocks so ore drips during the whole dig, as in DRGS."
parent: task-070-ore-in-rock
kind: task
status: in-progress
---
# Ore scattered in the rock

- goal: Ore scattered through the rock: nuggets inside rock blocks so ore drips during the whole dig, as in DRGS.
- agent: cpp
- depends_on: none
- inputs: ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), drgs-video-2026-10-06 (vault/reference/production/drgs-video-2026-10-06.md), sys-grid (vault/reference/tech/systems/sys-grid.md), sys-drone (vault/reference/tech/systems/sys-drone.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: owner decisions of 2026-10-10 are in ore-blocks, ore-economy, color-code and the note ore-in-rock-proposal (read them first). Build: an ore type table in data (row: ore id, share of rock blocks, group size, chunk value, colour); only gold: 8 %, 2 x 2, value 1. Deterministic placement of 2 x 2 groups at grid build (about 358 deposits on the current layout, ids per deposit, deposits may touch, never on heal veins, barrels or console-marked rocks; no start-zone protection: if a block of a deposit is destroyed at the start (explosion, arrival capsule) its chunks are thrown to the ground like any explosion drop, existing rule). Mining: touching any block mines the whole deposit (4 blocks, 1.5 s, shake together, 4 chunks, value 1 each), size read from the table so the same code serves 2 x 2 heal veins later (task-088-02). Respect the performance note ore-in-rock-plan (spawn queue at most 4 pickups per frame, no trail on the pickup). The existing 3 x 3 vein code path becomes the generic deposit of table-defined size. Tests; docs sys-grid, sys-drone, sys-pickups, sys-automation-tests.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; gold deposit count on the current layout logged and close to 358; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 90000
- model: opus
- tokens: input 32, cache write 42858, cache read 452866, output 12865
- runs: 1
- milestone: M2
- result: open

## Relations
- [[task-070-ore-in-rock]] — parent global task.
