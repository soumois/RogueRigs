---
id: task-091-01-robot-pool
description: "Robots come from the pool, missing ones created over at most 20 frames, wave spikes measured before and after."
parent: task-091-robot-pool
kind: task
status: in-progress
---
# Robots from the actor pool

- goal: Robots come from the pool: reuse on spawn and death, missing ones created over at most 20 frames; wave spawn spikes measured before and after.
- agent: cpp
- depends_on: task-090-01-actor-pool
- inputs: sys-spawner (vault/reference/tech/systems/sys-spawner.md), sys-spawner-director (vault/reference/tech/systems/sys-spawner-director.md), sys-enemies-base (vault/reference/tech/systems/sys-enemies/sys-enemies-base.md), sys-enemies-movement (vault/reference/tech/systems/sys-enemies/sys-enemies-movement.md), sys-enemy-ai (vault/reference/tech/systems/sys-enemy-ai.md), enemies-index (vault/reference/gdd/enemies/enemies-index.md), sys-stress (vault/reference/tech/systems/sys-stress.md), stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), perf-trace-real-game (vault/reference/tech/performance/perf-trace-real-game.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: migrate robot spawning (ARREnemySpawner single spawns, packs and hordes) to the actor pool of task-090 with MaxFrames 20: dead robots (after the death effect) are released instead of destroyed; a robot taken from the pool is fully reset (health and attributes, AI state, movement, knockback, shield, rigid parts and death-shrink state, GAS, ignore lists, kamikaze arming, heavy slam state, relocation timers) through the reset hook; recycling by relocation is unchanged (it already moves, not destroys). A robot requested but not yet ready appears up to 20 frames later: the spawner counts pending ones against MaxAlive so the cap is never exceeded; spawn candidates are chosen off screen as today. Gameplay identical (stats, packs, caps, class weights). BEFORE numbers first, same session, owner normal PC profile: `rr.Stress.Run 250 30 60 1 game` twice (sys-stress, perf-trace-real-game and stress-grid-cells for the exact commands and the spike lines); AFTER the same; report wave-spawn spikes (frames over 25 ms tagged wave spawn), game-thread avg/p99, pool counters. Existing tests that count live robots or destroyed actors may be updated only where this change breaks them; add tests for reset completeness (a recycled robot behaves like a fresh one: stats, no stale state), cap with pending robots, spread within 20 frames. Docs sys-spawner, sys-enemies-base, sys-actor-pool, stress-grid-cells (dated section), sys-automation-tests. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; before/after numbers in stress-grid-cells; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 90000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-091-robot-pool]] — parent global task.
