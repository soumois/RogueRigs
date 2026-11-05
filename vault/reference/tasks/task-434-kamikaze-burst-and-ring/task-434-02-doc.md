---
id: task-434-02-doc
description: "Doc: Record the decision and what the Kamikaze row now plays."
parent: task-434-kamikaze-burst-and-ring
kind: task
status: done
---
# Kamikaze explosion plays the barrel burst and ring: doc

- goal: Record the decision and what the Kamikaze row now plays.
- agent: doc
- depends_on: task-434-01-row-fx
- inputs: bp-explosion-fx-barrel (vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), bp-barrel-explosion (vault/reference/tech/blueprints/fx/bp-barrel-explosion.md), bp-explosion-fx (vault/reference/tech/blueprints/fx/bp-explosion-fx.md)
- expected_output: see done_when
- done_when: The "not decided" line of bp-explosion-fx-barrel and the "kamikaze" mentions of unset burst and ring in sys-explosions are corrected, lint is clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-434
- usage: included in the executor run of task-434
- runs: 1
- milestone: M2
- started: 2026-11-05T16:27:24Z
- waiting_for: none
- result: Updated bp-explosion-fx-barrel (decision replaces the not decided line), sys-explosions (row, lifetime, warm-up, kamikaze mentions), bp-barrel-explosion, bp-explosion-fx. Lint clean.

## Relations
- [[task-434-kamikaze-burst-and-ring]] — parent global task.
