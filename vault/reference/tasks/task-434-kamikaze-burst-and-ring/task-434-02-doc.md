---
id: task-434-02-doc
description: "Doc: Record the decision and what the Kamikaze row now plays."
parent: task-434-kamikaze-burst-and-ring
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-434-kamikaze-burst-and-ring]] — parent global task.
