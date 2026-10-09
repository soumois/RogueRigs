---
id: task-063-01-perf-session
description: "Performance session on a quiet PC: -game stress at 67 uu and the flow-field budget tests, compared with the last results."
parent: task-063-perf-session
kind: task
status: done
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
- model: opus
- tokens: input 70, cache write 232351, cache read 1527200, output 19551
- runs: 1
- milestone: M2
- result: PC not quiet (Brave, Discord, Steam, Epic, Creative Cloud, uTorrent, NVIDIA overlay, other Claude sessions; CPU 53–61 % idle) and -game capped at 60 Hz by the external monitor, so frame times are not comparable with task-029-07; all runs pass the 25 ms p99, averages sit on the 16.67 ms cap; game thread at 100 robots chain 7.00 ms, dig 7.63 ms (6.22, 6.33 in task-029-02). FlowField.BudgetAt67uu 0/5 (rebuild 4.7–6.7 ms vs 1.8–2.6 documented, goal age 33–38 > 30): CPU load, not a regression (flow field code unchanged since task-029-07, test spawns no robots). DestroyedRockOpensPocket 5/5. Full run 193/195: BudgetAt67uu and PressureHordeBanner (stale since task-062-01, fixed in task-062-02). Clean numbers need the external monitor unplugged and background apps closed. Commit 86856cf.

## Relations
- [[task-063-perf-session]] — parent global task.
