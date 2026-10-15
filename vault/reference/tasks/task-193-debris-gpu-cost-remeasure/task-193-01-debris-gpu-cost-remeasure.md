---
id: task-193-01-debris-gpu-cost-remeasure
description: "Remeasure the GPU debris with wall collision with the PC in its usual state, against task-182-03, frame for frame."
parent: task-193-debris-gpu-cost-remeasure
kind: task
status: in-progress
---
# Robot debris GPU and walls: remeasure

- goal: Remeasure the GPU debris with wall collision with the PC in its usual state, against task-182-03, frame for frame.
- agent: cpp
- depends_on: none
- inputs: perf-stress-chaser-meshy (vault/reference/tech/performance/perf-stress-chaser-meshy.md), sys-stress (vault/reference/tech/systems/tools/sys-stress.md), sys-stress-run-modes (vault/reference/tech/systems/tools/sys-stress/sys-stress-run-modes.md), sys-stress-diagnostics (vault/reference/tech/systems/tools/sys-stress/sys-stress-diagnostics.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md)
- expected_output: Before any run: a control S0 200 step (no deaths) must give about the task-181 numbers (frame about 9 ms, GPU about 8 ms, not capped at 60 Hz); if not, stop and report the PC state (60 Hz cap, GPU clocks, power plan, apps) instead of measuring. Then the same steps as task-182-03 and 191-02: game mode caps 200 and 300 and S7 burst -Class Chaser 200; frame, game, draw, GPU avg/p99/max, plays/skips; a section in perf-stress-chaser-meshy replacing the 191-02 comparison with a valid one and a verdict
- done_when: Control step at the usual speed; runs done; section written; report sent to the owner first; committed by path
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-193-debris-gpu-cost-remeasure]] — parent global task.
