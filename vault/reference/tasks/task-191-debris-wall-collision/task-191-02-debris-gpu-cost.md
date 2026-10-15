---
id: task-191-02-debris-gpu-cost
description: "Measure the cost of the GPU debris with wall collision against task-182-03 (CPU, floor only), same method."
parent: task-191-debris-wall-collision
kind: task
status: todo
---
# Robot debris GPU and walls: cost measure

- goal: Measure the cost of the GPU debris with wall collision against task-182-03 (CPU, floor only), same method.
- agent: cpp
- depends_on: task-191-01-debris-gpu-df-collision
- inputs: perf-stress-chaser-meshy (vault/reference/tech/performance/perf-stress-chaser-meshy.md), sys-stress (vault/reference/tech/systems/tools/sys-stress.md), sys-stress-run-modes (vault/reference/tech/systems/tools/sys-stress/sys-stress-run-modes.md), sys-stress-diagnostics (vault/reference/tech/systems/tools/sys-stress/sys-stress-diagnostics.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md)
- expected_output: Same launcher, PC profile and steps as task-182-03: game mode caps 200 and 300 and S7 burst -Class Chaser 200; frame, game, draw, GPU avg/p99/max and debris plays/skips; a section in perf-stress-chaser-meshy comparing CPU floor-only and GPU with walls, with a verdict
- done_when: Runs done, section written, report sent to the owner first, committed by path
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-191-debris-wall-collision]] — parent global task.
