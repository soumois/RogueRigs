---
id: task-063-01-perf-session
description: "Performance session on a quiet PC: -game stress at 67 uu and the flow-field budget tests, compared with the last results."
parent: task-063-perf-session
kind: task
status: in-progress
---
# Performance session

- goal: Performance session on a quiet PC: -game stress at 67 uu and the flow-field budget tests, compared with the last results.
- agent: cpp
- depends_on: none
- inputs: sys-stress (vault/reference/tech/systems/sys-stress.md), stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), sys-flow-field (vault/reference/tech/systems/sys-flow-field.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: with nothing else running (check tasklist for GPU-heavy programs and report them; Unreal is closed): build if needed; -game stress at 67 uu with the current defaults: S0 and S2 at 100 and 400 robots, chain of 30 explosions, fast digging (the same scenarios and commands as task-029-07) with avg / p99 / max ms; then RogueRigs.FlowField.BudgetAt67uu and FlowField.DestroyedRockOpensPocket run alone 5 times each, and one full headless run; compare with the task-029-07 table in stress-grid-cells and say whether the flow-field budget failures are load or a regression (if a regression, name the likely change among tasks 036, 041, 046 and 060 with evidence, do not fix it); no code change, no new test; results added to stress-grid-cells.
- done_when: stress table and budget test results reported and written in stress-grid-cells; lint clean.
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-063-perf-session]] — parent global task.
