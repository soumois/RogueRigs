---
id: task-436-03-doc
description: "Doc: Document the new values of both puffs."
parent: task-436-robot-death-particle
kind: task
status: done
---
# Better robot death particle: doc

- goal: Document the new values of both puffs.
- agent: doc
- depends_on: task-436-02-heavy-puff
- inputs: bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), bp-heavy-death-puff (vault/reference/tech/blueprints/fx/bp-heavy-death-puff.md)
- expected_output: see done_when
- done_when: Both nodes state the current values; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-436
- usage: included in the executor run of task-436
- runs: 1
- milestone: M2
- started: 2026-11-05T17:50:21Z
- waiting_for: none
- result: bp-robot-death-puff and bp-heavy-death-puff state the new values; bp-kamikaze-death-puff created, bp-kamikaze and blueprints-fx-index updated (56596513); lint clean.

## Relations
- [[task-436-robot-death-particle]] — parent global task.
