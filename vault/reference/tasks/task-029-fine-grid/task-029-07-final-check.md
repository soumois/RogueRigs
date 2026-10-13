---
id: task-029-07-final-check
description: "Integration tests on the fine grid and final stress at 67 uu (S0, S2, chain, dig) compared with stress-grid-cells."
parent: task-029-fine-grid
kind: task
status: done
---
# Fine grid final check

- goal: the fine grid is covered and fast.
- agent: cpp
- depends_on: task-029-09-bp-fan-holes
- inputs: [[sys-automation-tests]], [[perf-stress-grid-cells]], task-029 results.
- expected_output: integration tests with the real map and Blueprints (contact dig opens a passage robots then use, console and shuttle placement at fine scale); -game stress S0, S2, chain 30, dig at 67 uu with avg/p99/max; results added to stress-grid-cells; full run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean. Stress within budget or the gap reported.
- editor_state: closed
- token_ceiling: 100000
- model: opus
- tokens: input 148, cache write 178372, cache read 8101659, output 54246
- runs: 2
- milestone: M2
- result: stress at 67 uu within budget (400 robots: S0 9.97 / 12.98, S2 11.16 / 14.93 (one 59.7 ms frame), chain 30 10.08 / 13.70, dig 10.33 / 13.83 ms avg / p99), table in [[perf-stress-grid-cells]]; tests `Integration.FineContactDigOpensPassage` and `Integration.FineConsoleShuttlePlacement` added ([[sys-automation-tests]]); build 0, 158/158 headless, no pure virtual crash. Gaps: console and shuttle meshes overlap rock cells at some landings (no clearance rule); laser fan not screenshot in -game (no input driver).

## Relations
- [[task-029-fine-grid]] — parent global task.
