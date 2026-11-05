---
id: task-446-02-doc
description: "Doc: Document the colours and remove the "no red" statements that no longer hold."
parent: task-446-explosions-red
kind: task
status: todo
---
# Red in the barrel and kamikaze explosions: doc

- goal: Document the colours and remove the "no red" statements that no longer hold.
- agent: doc
- depends_on: task-446-01-burst-colours
- inputs: bp-barrel-explosion (vault/reference/tech/blueprints/fx/bp-barrel-explosion.md), bp-explosion-shockwave (vault/reference/tech/blueprints/fx/bp-explosion-shockwave.md), bp-explosion-fx-barrel (vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md)
- expected_output: see done_when
- done_when: Nodes state the current colours; the task-434 record untouched; lint clean.
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
- [[task-446-explosions-red]] — parent global task.
