---
id: task-018-01-trace
description: "Unreal Insights CPU trace in -game at 400 robots (S0 and S2): game-thread cost breakdown per system and the cause of the 61 s hitch."
parent: task-018-robot-perf
kind: task
status: done
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
- result: Insights traces at 400 robots (S0, S2), headless export. Game thread S0 11.1 / S2 13.2 ms; CharacterMovement TickComponent 5.9 / 7.0 ms (walking physics 4.65, move sweeps 2.15 with 849 sweeps, floor check 1.86, attached mesh update 0.81, RVO 0.33; robot-robot blocking hits 61 per frame, 0.05 ms); skeletal mesh ticks 0.6; flow-field steering 0.26; GAS and HUD negligible. 61 s hitch = default full GC 29.5–32.6 ms (PyUtil::CollectGarbage 9–11 ms from the editor Python plugin, reachability 18–19 ms), independent of robot count. Leads re-ranked: movement first, GC second, crowd collision fourth (−0.3 to −1 ms). Run-StressTest.ps1 -Trace options; Stress_<N>_Measure regions. New node perf-trace-m1; perf-risks, sys-stress. Commit f61a093, about 90k tokens.

## Relations
- [[task-018-robot-perf]] — parent global task.
