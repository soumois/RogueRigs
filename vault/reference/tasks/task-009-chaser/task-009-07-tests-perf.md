---
id: task-009-07-tests-perf
description: "Integration tests with the final Blueprints (chasers reach and hit the miner around rocks, die to the rifle) and a 50-chaser headless timing run."
parent: task-009-chaser
kind: task
status: done
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
- result: 4 Integration tests: chaser from (34, 34) takes a 15-step winding path, hits at 6.6 s; rifle kills a chaser 1283 uu out at 1.27 s with 1 XP drop; 10 chaser hits kill the miner, game over once, restart armed 3 s; 50 chasers 600 frames. First perf read (headless, no rendering): world tick avg 2.5 ms, p95 3.3 ms, max 5.0 ms; flow-field recompute 0.32 to 0.36 ms; recorded in [[perf-risks]]. Build exit 0, 35/35 (orchestrator re-ran). No bug found. About 91k tokens.

## Relations
- [[task-009-chaser]] — parent global task.
