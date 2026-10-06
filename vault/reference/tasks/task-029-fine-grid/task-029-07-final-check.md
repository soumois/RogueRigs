---
id: task-029-07-final-check
description: "Integration tests on the fine grid and final stress at 67 uu (S0, S2, chain, dig) compared with stress-grid-cells."
parent: task-029-fine-grid
kind: task
status: in-progress
---
# Fine grid final check

- goal: the fine grid is covered and fast.
- agent: cpp
- depends_on: task-029-09-bp-fan-holes
- inputs: [[sys-automation-tests]], [[stress-grid-cells]], task-029 results.
- expected_output: integration tests with the real map and Blueprints (contact dig opens a passage robots then use, console and shuttle placement at fine scale); -game stress S0, S2, chain 30, dig at 67 uu with avg/p99/max; results added to stress-grid-cells; full run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean. Stress within budget or the gap reported.
- editor_state: closed
- token_ceiling: 100000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-029-fine-grid]] — parent global task.
