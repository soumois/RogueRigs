---
id: task-025-03-numbers-perf
description: "Integration test with the final Niagara asset and a -game S2 measure at 400 robots compared with the previous damage text."
parent: task-025-damage-numbers
kind: task
status: todo
---
# Damage numbers check

- goal: the new numbers are correct and cheap.
- agent: cpp
- depends_on: task-025-02-bp-numbers
- inputs: [[stress-opt-m1]], [[sys-stress]], task-025-02 result.
- expected_output: integration test (hits produce merged entries, crits flagged); S2 -game at 100/400 robots before/after numbers in stress-opt-m1 or a short new node; full run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-025-damage-numbers]] — parent global task.
