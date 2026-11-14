---
id: task-544-01-gauge-logic
description: "Cpp: Single "!" at half of C, kill gauge state and horde trigger replacing the fixed horde times"
parent: task-544-horde-kill-gauge
kind: task
status: in-progress
---
# Hordes: one "!" at half of C, then a kill gauge under the alert bar triggers each horde: cpp

- goal: Single "!" at half of C, kill gauge state and horde trigger replacing the fixed horde times
- agent: cpp
- depends_on: none
- inputs: level-flow (vault/reference/gdd/run/level-flow.md), enemies-spawn-rules (vault/reference/gdd/enemies/enemies-spawn-rules.md), difficulty (vault/reference/gdd/run/difficulty.md), sys-difficulty (vault/reference/tech/systems/core/sys-difficulty.md)
- expected_output: see done_when
- done_when: A1 to A6 and A8 hold in code and tests (overtime: kill weight x2 from 3:30, x3 from 4:30, no 30 s horde timer, +20 % base health per overtime horde for robots spawned after it) (gauge state exposed by events for the widget); kill counts in data.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-14T00:16:24Z
- waiting_for: none
- result: open

## Relations
- [[task-544-horde-kill-gauge]] — parent global task.
