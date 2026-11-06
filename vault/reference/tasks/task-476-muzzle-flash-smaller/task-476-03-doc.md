---
id: task-476-03-doc
description: "Doc: Docs record the new flash size."
parent: task-476-muzzle-flash-smaller
kind: task
status: todo
---
# Player muzzle flash a little smaller: doc

- goal: Docs record the new flash size.
- agent: doc
- depends_on: task-476-02-capture
- inputs: burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), task-464-player-shot-colour (vault/reference/tasks/task-464-player-shot-colour.md)
- expected_output: see done_when
- done_when: The bp-* and sys-* nodes of the muzzle flash give the new size and the turret compensation if any; lint clean; committed.
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
- [[task-476-muzzle-flash-smaller]] — parent global task.
