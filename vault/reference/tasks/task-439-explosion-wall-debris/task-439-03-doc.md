---
id: task-439-03-doc
description: "Doc: Document the debris effect and its bound."
parent: task-439-explosion-wall-debris
kind: task
status: done
---
# Wall debris thrown away from explosions: doc

- goal: Document the debris effect and its bound.
- agent: doc
- depends_on: task-439-02-debris-system
- inputs: blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md)
- expected_output: see done_when
- done_when: New bp node linked from the FX index, code doc updated; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-439
- usage: included in the executor run of task-439
- runs: 1
- milestone: M2
- started: 2026-11-05T19:37:28Z
- waiting_for: none
- result: bp-explosion-wall-debris created and linked from the FX index; sys-explosions and sys-actor-pool updated; lint clean.

## Relations
- [[task-439-explosion-wall-debris]] — parent global task.
