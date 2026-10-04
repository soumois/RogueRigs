---
id: task-018-01-trace
description: "Unreal Insights CPU trace in -game at 400 robots (S0 and S2): game-thread cost breakdown per system and the cause of the 61 s hitch."
parent: task-018-robot-perf
kind: task
status: in-progress
---
# Insights trace

- goal: know where the frame time goes before changing code.
- agent: cpp
- depends_on: task-017-01-radius
- inputs: [[perf-risks]], [[stress-final-m1]], [[sys-stress]], [[build-toolchain]].
- expected_output: trace runs with `-trace=cpu,frame` (or the 5.8 equivalent) through `Scripts/Run-StressTest.ps1`; analysis with UnrealInsights or its command-line export (e.g. `-ExecOnAnalysisCompleteCmd` timer export to CSV) of the top game-thread timers at 400 robots (CharacterMovement, collision sweeps, RVO, flow field, GAS, ticks, HUD) and of the frames around a 61 s hitch; findings in a new node `perf-trace-m1` under performance, leads in [[perf-risks]] re-ranked if the trace contradicts them. Commit the script changes, not the trace files.
- done_when: traces captured on this PC; the breakdown table and the hitch cause (or the best evidence) are written with real numbers; lint clean.
- editor_state: closed
- token_ceiling: 90000
- milestone: M1
- result: open

## Relations
- [[task-018-robot-perf]] — parent global task.
