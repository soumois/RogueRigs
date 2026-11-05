---
id: task-433-04-doc
description: "Doc: Document the flash value, its slot and the materials that read it."
parent: task-433-robot-hit-flash
kind: task
status: done
---
# Robot hit flash: doc

- goal: Document the flash value, its slot and the materials that read it.
- agent: doc
- depends_on: task-433-03-flash-other-robots
- inputs: sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-damage (vault/reference/tech/systems/core/sys-damage.md)
- expected_output: see done_when
- done_when: The listed nodes and the bp nodes of the touched materials are updated, the slot use is written, lint is clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-433
- usage: included in the executor run of task-433
- runs: 1
- milestone: M2
- started: 2026-11-05T16:21:45Z
- waiting_for: none
- result: New bp-robot-hit-flash; updated sys-enemies-base (slot use 0 clip, 1 start, 2 flash), sys-damage, sys-tests-enemies, bp-robot-meshy-material, bp-robot-eye-material, bp-shield-chaser-v2, blueprints-enemies-index. Lint clean.

## Relations
- [[task-433-robot-hit-flash]] — parent global task.
