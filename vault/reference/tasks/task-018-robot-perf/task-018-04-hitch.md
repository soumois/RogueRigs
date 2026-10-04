---
id: task-018-04-hitch
description: "Remove or reduce the 45–50 ms hitch every 61 s found by the trace (garbage collection or other), measured in -game."
parent: task-018-robot-perf
kind: task
status: done
---
# Periodic hitch

- goal: no steady-play frame over 25 ms at 200–400 robots. The trace shows the 61 s hitch is the default full GC (29.5–32.6 ms: PyUtil::CollectGarbage 9–11 ms from the editor Python plugin, reachability 18–19 ms), the same at 200 and 400 robots: measure it first in a packaged Development build (or standalone without editor plugins), and change GC settings only if the hitch remains there.
- agent: cpp
- depends_on: task-018-03-light-movement
- inputs: task-018-01 result, [[perf-risks]], [[sys-projectiles]], [[sys-damage]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: fix chosen from the trace evidence: e.g. incremental or time-sliced GC settings in config, fewer transient UObjects (projectile and damage-number pools) or another cause; max frame over a 3-minute S2 run before/after; docs ([[perf-risks]], relevant system docs). If the cause cannot be fixed in the budget, report it with evidence and keep the code unchanged.
- done_when: build exits 0; all `RogueRigs.*` tests pass headless; before/after stress numbers (S0 and S2 at 100, 200, 400, same procedure as [[stress-final-m1]]) recorded; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 90000
- milestone: M1
- result: The 61 s hitch is editor-only: staged Development build (BuildCookRun cook+stage+pak; UAT exit 25 from GameFeatureData asset-manager errors, staged with a -skipcook rerun) has a GC frame of 10.8 ms at 200 and 16.4 ms at 400 robots (ConditionalCollectGarbage 3.6–5.3 ms, no Python hook) vs 40–44 ms in editor -game. No GC or code change (step 2 of the node). Staged game avg S0 100/200/400 3.7/5.0/6.4 ms, S2 4.1/6.0/9.8; max frame ≤ 16 ms in stepped runs. Run-StressTest.ps1 -Exe and trace-path fix; 55+5 s steps align with the 61 s GC (documented). No C++ or config changed, so no build or test run needed. Docs perf-trace-m1, perf-risks, sys-stress, build-toolchain. Commit 0ff5dfc, about 77k tokens.

## Relations
- [[task-018-robot-perf]] — parent global task.
