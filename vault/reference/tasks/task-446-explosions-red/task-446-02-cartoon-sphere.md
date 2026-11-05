---
id: task-446-02-cartoon-sphere
description: "Bp: The expanding sphere of the explosion has the cartoon look and still shows the radius."
parent: task-446-explosions-red
kind: task
status: in-progress
---
# Barrel and kamikaze explosions in cartoon style: bp

- goal: The expanding sphere of the explosion has the cartoon look and still shows the radius.
- agent: bp
- depends_on: task-446-01-cartoon-burst
- inputs: bp-explosion-fx (vault/reference/tech/blueprints/fx/bp-explosion-fx.md), bp-explosion-fx-barrel (vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), explosions (vault/reference/gdd/terrain/explosions.md)
- expected_output: see done_when
- done_when: Compiles and saves with no error; both rows play one class; visible radius and hold read back unchanged; the fade still works.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T22:13:59Z
- waiting_for: none
- result: open

## Relations
- [[task-446-explosions-red]] — parent global task.
