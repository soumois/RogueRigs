---
id: task-479-02-doc
description: "Doc: The level-up system doc gives the new delay."
parent: task-479-level-up-cards-delay
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-479
- usage: included in the executor run of task-479
- runs: 1
- milestone: M2
- started: 2026-11-06T20:53:52Z
- waiting_for: none
- result: sys-level-up gives ChoiceDelay 1.2 s with task-479 and the 72 frames (1.200 s) measure; lint clean; commit 5ee49665.

## Relations
- [[task-479-level-up-cards-delay]] — parent global task.
