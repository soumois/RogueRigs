---
id: task-009-07-tests-perf
description: "Integration tests with the final Blueprints (chasers reach and hit the miner around rocks, die to the rifle) and a 50-chaser headless timing run."
parent: task-009-chaser
kind: task
status: todo
---
# Integration tests and first performance read

- goal: prove the whole loop with the final assets and get a first cost figure.
- agent: cpp
- depends_on: task-009-06-bp-chaser
- inputs: [[sys-automation-tests]], `sys-enemies`, `sys-flow-field`, `sys-spawner`, [[stress-test-m1]], [[perf-risks]], [[build-toolchain]].
- expected_output: tests `RogueRigs.Integration.*` with `BP_Miner`, `BP_Chaser`, `BP_ArenaGrid` (arena_m1): a chaser spawned 8 cells away along a winding path reaches attack range; it damages the miner; the miner's rifle kills a chaser coming straight at it; the miner dies after enough hits and game over fires once. Perf: 50 `BP_Chaser` chasing the miner for 600 frames headless; log average and max game-thread time per frame and flow-field recompute time; record the figures in [[perf-risks]] as a first read (not the S0 test). Full `RogueRigs` run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (orchestrator re-runs); figures recorded; lint clean.
- editor_state: closed
- token_ceiling: 80000
- milestone: M1
- result: open

## Relations
- [[task-009-chaser]] — parent global task.
