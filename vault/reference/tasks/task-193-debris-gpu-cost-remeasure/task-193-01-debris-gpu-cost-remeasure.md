---
id: task-193-01-debris-gpu-cost-remeasure
description: "Remeasure the GPU debris with wall collision with the PC in its usual state, against task-182-03, frame for frame."
parent: task-193-debris-gpu-cost-remeasure
kind: task
status: done
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
- model: opus
- tokens: input 124, cache write 213408, cache read 4612086, output 47876
- runs: 2
- milestone: M3
- result: Control passed after Armoury Crate Performance (8.65 ms, GPU 7.63); GPU debris with walls vs 182-03: game 200 frame 11.08 -> 14.15, game 300 12.96 -> 15.74 (p99 21.4, above the 181 limits), S7 200 max 25.9 -> 54.1; plays/skips real (S7 80/120); trace: render thread about 68 us per active GPU debris system (dynamic primitive upload, one pass per system), GPU +1.3-1.5 ms incl. global distance field 0.9 ms; first-use hitch gone in editor -game; section in perf-stress-chaser-meshy; commits 65a1e7aa, d1e06c83

## Relations
- [[task-193-debris-gpu-cost-remeasure]] — parent global task.
