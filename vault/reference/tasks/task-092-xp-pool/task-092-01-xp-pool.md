---
id: task-092-01-xp-pool
description: "XP cubes come from the pool with creation spread over at most 3 frames, so a mass kill no longer spikes the frame."
parent: task-092-xp-pool
kind: task
status: in-progress
---
# XP cubes from the actor pool

- goal: XP cubes come from the pool with creation spread over at most 3 frames, so a mass kill no longer spikes the frame.
- agent: cpp
- depends_on: task-091-01-robot-pool
- inputs: sys-pickups (vault/reference/tech/systems/sys-pickups.md), sys-enemies-base (vault/reference/tech/systems/sys-enemies/sys-enemies-base.md), sys-stress (vault/reference/tech/systems/sys-stress.md), stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), perf-trace-real-game (vault/reference/tech/performance/perf-trace-real-game.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: migrate ARREnemy::DropXP (XP cubes, XPCubeCount per class) and URRPickupSubsystem::RequestSpawn to the actor pool with MaxFrames 3; collected cubes are released instead of destroyed and reset through the hook (position, motion state, magnet latch, trail/Niagara state, value). Also move the ore and heal pickups that use the 4-per-frame spawn queue (task-070-01) onto the pool in the same way (the owner did not object: keep their values and the heal-refused-at-full-health rule intact), and remove the old queue if nothing else uses it. Pooled cubes are not counted as live pickups by tests or systems (document). Gameplay identical (cube counts, XP value, magnet, collect radius). BEFORE numbers first: `rr.Stress.Run 250 30 60 1 game` twice plus the S7 burst scenario if sys-stress describes one for mass kills; AFTER the same; report XP-burst spike frames (frames over 25 ms during mass deaths), game-thread avg/p99. Update only the tests this breaks; add tests: reuse after collect, 3-frame spread, no loss of value, heal refused at full health still holds. Docs sys-pickups, sys-actor-pool, stress-grid-cells, sys-automation-tests. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; before/after numbers in stress-grid-cells; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-092-xp-pool]] — parent global task.
