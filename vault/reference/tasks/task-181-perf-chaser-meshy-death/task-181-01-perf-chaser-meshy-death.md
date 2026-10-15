---
id: task-181-01-perf-chaser-meshy-death
description: "Measure the frame cost of the Meshy chaser (animated material) and its death effects (puff, debris) with the stress tool, compared with the M1 tables."
parent: task-181-perf-chaser-meshy-death
kind: task
status: todo
---
# Performance test: Meshy chaser, animation and death effects

- goal: Measure the frame cost of the Meshy chaser (animated material) and its death effects (puff, debris) with the stress tool, compared with the M1 tables.
- agent: cpp
- depends_on: none
- inputs: sys-stress (vault/reference/tech/systems/tools/sys-stress.md), sys-stress-run-modes (vault/reference/tech/systems/tools/sys-stress/sys-stress-run-modes.md), sys-stress-diagnostics (vault/reference/tech/systems/tools/sys-stress/sys-stress-diagnostics.md), perf-stress-test-m1 (vault/reference/tech/performance/perf-stress-test-m1.md), perf-stress-final-m1 (vault/reference/tech/performance/perf-stress-final-m1.md), perf-stress-grid-cells (vault/reference/tech/performance/perf-stress-grid-cells.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md)
- expected_output: open (scenarios to be confirmed by the owner); a new perf node under performance-index with the tables, the comparison with perf-stress-final-m1 and perf-stress-grid-cells, and a verdict
- done_when: open (set when the owner confirms the plan)
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-181-perf-chaser-meshy-death]] — parent global task.
