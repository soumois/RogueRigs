---
id: task-438-03-doc
description: "Doc: Document the burst and its bound."
parent: task-438-dig-debris
kind: task
status: done
---
# Debris when digging: doc

- goal: Document the burst and its bound.
- agent: doc
- depends_on: task-438-02-debris-system
- inputs: blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), sys-drone-dig (vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md)
- expected_output: see done_when
- done_when: New bp node linked from the FX index, code doc updated; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-438
- usage: included in the executor run of task-438
- runs: 1
- milestone: M2
- started: 2026-11-05T19:17:19Z
- waiting_for: none
- result: New bp-dig-debris linked from blueprints-fx-index; sys-drone-dig, sys-actor-pool, sys-grid and sys-tests-dig-ore updated. Lint clean.

## Relations
- [[task-438-dig-debris]] — parent global task.
