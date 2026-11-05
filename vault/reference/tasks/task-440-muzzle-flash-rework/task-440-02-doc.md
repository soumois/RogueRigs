---
id: task-440-02-doc
description: "Doc: Document the new values."
parent: task-440-muzzle-flash-rework
kind: task
status: done
---
# Muzzle flash rework: doc

- goal: Document the new values.
- agent: doc
- depends_on: task-440-01-flash-system
- inputs: bp-muzzle-flash (vault/reference/tech/blueprints/fx/bp-muzzle-flash.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md)
- expected_output: see done_when
- done_when: The node states the current emitters and material; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-440
- usage: included in the executor run of task-440
- runs: 1
- milestone: M2
- started: 2026-11-05T19:59:07Z
- waiting_for: none
- result: bp-muzzle-flash rewritten (emitters, sizes, material, shapes, verified, unverified); sys-weapons, bp-shotgun, blueprints-fx-index, sys-automation-tests, unreal-mcp-quirks updated; lint clean.

## Relations
- [[task-440-muzzle-flash-rework]] — parent global task.
