---
id: task-479-02-doc
description: "Doc: The level-up system doc gives the new delay."
parent: task-479-level-up-cards-delay
kind: task
status: todo
---
# Level-up cards after 1.2 s instead of 1.8 s: doc

- goal: The level-up system doc gives the new delay.
- agent: doc
- depends_on: task-479-01-delay
- inputs: sys-level-up (vault/reference/tech/systems/core/sys-level-up.md)
- expected_output: see done_when
- done_when: sys-level-up gives 1.2 s with the task id; lint clean; committed.
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
- [[task-479-level-up-cards-delay]] — parent global task.
