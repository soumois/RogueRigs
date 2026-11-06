---
id: task-459-01-delay
description: "Cpp: The card screen opens about 1.8 s after a level-up; nothing else in the sequence changes."
parent: task-459-level-up-pause-delay
kind: task
status: in-progress
---
# One more second before the level-up cards: cpp

- goal: The card screen opens about 1.8 s after a level-up; nothing else in the sequence changes.
- agent: cpp
- depends_on: none
- inputs: sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), task-441-level-up-effect (vault/reference/tasks/task-441-level-up-effect.md)
- expected_output: see done_when
- done_when: Build ok; the delay measured in PIE or a test is about 1.8 s, chained level-ups wait once; LevelUp test family passes, figures in the result.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T04:09:56Z
- waiting_for: none
- result: open

## Relations
- [[task-459-level-up-pause-delay]] — parent global task.
