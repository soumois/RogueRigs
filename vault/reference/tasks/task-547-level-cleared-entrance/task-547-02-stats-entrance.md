---
id: task-547-02-stats-entrance
description: "Bp: Wire the shared entrance to the stats screen and its level 4 variant"
parent: task-547-level-cleared-entrance
kind: task
status: todo
---
# Level cleared screen appears with the death screen entrance effect: bp

- goal: Wire the shared entrance to the stats screen and its level 4 variant
- agent: bp
- depends_on: task-547-01-shared-entrance
- inputs: intermission (vault/reference/gdd/run/intermission.md), death-screen (vault/reference/gdd/run/death-screen.md)
- expected_output: see done_when
- done_when: A1 to A3 hold.
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
