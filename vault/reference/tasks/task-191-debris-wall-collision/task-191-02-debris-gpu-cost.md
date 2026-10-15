---
id: task-191-02-debris-gpu-cost
description: "Measure the cost of the GPU debris with wall collision against task-182-03 (CPU, floor only), same method."
parent: task-191-debris-wall-collision
kind: task
status: done
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
- model: opus
- tokens: input 90, cache write 136600, cache read 2361395, output 29886
- runs: 1
- milestone: M3
- result: Runs done but not comparable frame for frame: the PC was about 1.8x slower on the GPU and capped at 60 Hz (control S0 200 without debris 16.67 ms vs 8.98); trace: GPU debris with walls about 0.7-1.3 ms per frame, mostly the global distance field update (0.85 ms) plus Niagara about 0.4 ms; new: NS_RobotDebris compiles at first use in editor -game (0.12-0.21 s + PSO wait, 395 ms frame in S7); S7 debris plays 200 above the 80 cap (counters overstated); section in perf-stress-chaser-meshy; commit 79bb0f2d

## Relations
- [[task-191-debris-wall-collision]] — parent global task.
