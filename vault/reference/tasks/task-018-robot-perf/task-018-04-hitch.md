---
id: task-018-04-hitch
description: "Remove or reduce the 45–50 ms hitch every 61 s found by the trace (garbage collection or other), measured in -game."
parent: task-018-robot-perf
kind: task
status: todo
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
- result: open

## Relations
- [[task-018-robot-perf]] — parent global task.
