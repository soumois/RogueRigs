---
id: task-187-01-deposit-whole-on-explosion
description: "When an explosion destroys any block of an ore deposit, the whole deposit is destroyed and pops all its chunks: no partly destroyed deposit."
parent: task-187-deposit-whole-on-explosion
kind: task
status: done
---
# Ore deposits: destroyed whole by explosions

- goal: When an explosion destroys any block of an ore deposit, the whole deposit is destroyed and pops all its chunks: no partly destroyed deposit.
- agent: cpp
- depends_on: none
- inputs: ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), explosions (vault/reference/gdd/terrain/explosions.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), sys-flow-field-updates (vault/reference/tech/systems/world/sys-flow-field/sys-flow-field-updates.md), sys-pickups (vault/reference/tech/systems/world/sys-pickups.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-tests-explosions-barrel (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-explosions-barrel.md)
- expected_output: In the grid destruction path (Source/RogueRigs/Grid/), a block of a deposit destroyed by an explosion (any source: barrel, kamikaze, drone blast, start capsule) also destroys the deposit's other remaining blocks in the same call, each popping its chunks as today (DestroyDropCount, ChunkValue), one batched flow-field and render update; applies to every deposit (Duskite and heal veins, owner default); drone mining unchanged (already whole); ore-blocks updated with the rule (provisional, owner 2026-10-15), sys-grid-deposits updated
- done_when: Build passes with the editor closed; existing grid, explosion, dig and ore tests pass (update only an expectation this change breaks, e.g. a test that expects a partial deposit); a -game run with an explosion over part of a deposit logs the whole deposit destroyed and its 4 chunks (values, no screenshots; no new test); docs updated; committed by path
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 92, cache write 92220, cache read 2788805, output 27422
- runs: 1
- milestone: M2
- result: DestroyCell with Barrel/Kamikaze source on a deposit block destroys the deposit's other standing blocks (DestroySingleCell, no recursion, queued/destroyed skipped, drops and console counts per block); DestroyCellsInRadius returns real count; run: deposit 161 with 2 of 4 cells in radius broke whole, 4 chunks; 97/97 tests after 2 expectations updated; start capsule path and heal veins not exercised; commits 1531c7fd, 60b94290, 0f49dcae

## Relations
- [[task-187-deposit-whole-on-explosion]] — parent global task.
