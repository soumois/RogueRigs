---
id: task-547-01-shared-entrance
description: "Cpp: Extract the death screen entrance into one shared effect, death screen unchanged"
parent: task-547-level-cleared-entrance
kind: task
status: todo
---
# Level cleared screen appears with the death screen entrance effect: cpp

- goal: Extract the death screen entrance into one shared effect, death screen unchanged
- agent: cpp
- depends_on: none
- inputs: death-screen (vault/reference/gdd/run/death-screen.md), intermission (vault/reference/gdd/run/intermission.md)
- expected_output: see done_when
- done_when: A4 and A5 hold; the effect can drive another screen with its title, frame and button parts.
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
- [[task-547-level-cleared-entrance]] — parent global task.
