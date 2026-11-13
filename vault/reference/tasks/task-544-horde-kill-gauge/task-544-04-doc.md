---
id: task-544-04-doc
description: "Doc: Document the horde gauge in the system and widget nodes"
parent: task-544-horde-kill-gauge
kind: task
status: todo
---
# Hordes: one "!" at half of C, then a kill gauge under the alert bar triggers each horde: doc

- goal: Document the horde gauge in the system and widget nodes
- agent: doc
- depends_on: task-544-03-capture
- inputs: sys-difficulty (vault/reference/tech/systems/core/sys-difficulty.md), level-flow (vault/reference/gdd/run/level-flow.md)
- expected_output: see done_when
- done_when: sys and bp nodes describe the gauge, its data values and events; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-544-horde-kill-gauge]] — parent global task.
