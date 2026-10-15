---
id: task-182-03-debris-pool-cost
description: "Measure 80 debris systems of 4.9 s against task-181 (40 of 3.4 s) and record the cost per extra debris system."
parent: task-182-debris-pool-80-test
kind: task
status: in-progress
---
# Debris pool 80: cost measure

- goal: Measure 80 debris systems of 4.9 s (4.5 s + 0.4 s sink) against task-181 (40 of 3.4 s) and record the cost per extra debris system.
- agent: cpp
- depends_on: task-182-01-debris-lifetime-4-5s, task-182-02-debris-pool-80
- inputs: perf-stress-chaser-meshy (vault/reference/tech/performance/perf-stress-chaser-meshy.md), sys-stress (vault/reference/tech/systems/tools/sys-stress.md), sys-stress-run-modes (vault/reference/tech/systems/tools/sys-stress/sys-stress-run-modes.md), sys-stress-diagnostics (vault/reference/tech/systems/tools/sys-stress/sys-stress-diagnostics.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md)
- expected_output: Same method and PC profile as task-181: game mode caps 200 and 300 and S7 burst -Class Chaser 200 (one launch each, window covering 4.9 s); frame, game, draw, GPU avg/p99/max, puff and debris plays/skips, and memory (prewarmed debris components: used memory before/after the prewarm or Niagara memory stat). A section added to perf-stress-chaser-meshy: 40 x 3.4 s vs 80 x 4.9 s, skips, the cost per extra active debris system (ms and memory), and a rule of thumb for raising the cap
- done_when: Runs done, logs and CSV under Saved/Stress; section written with the comparison and the cost per extra system; report sent to the owner first; committed by path
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-182-debris-pool-80-test]] — parent global task.
