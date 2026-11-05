---
id: task-446-04-doc
description: "Doc: Document the new look and remove the statements that no longer hold."
parent: task-446-explosions-red
kind: task
status: done
---
# Barrel and kamikaze explosions in cartoon style: doc

- goal: Document the new look and remove the statements that no longer hold.
- agent: doc
- depends_on: task-446-03-cartoon-ring
- inputs: bp-barrel-explosion (vault/reference/tech/blueprints/fx/bp-barrel-explosion.md), bp-explosion-fx (vault/reference/tech/blueprints/fx/bp-explosion-fx.md), bp-explosion-fx-barrel (vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md), bp-explosion-shockwave (vault/reference/tech/blueprints/fx/bp-explosion-shockwave.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md)
- expected_output: see done_when
- done_when: Nodes state the current look and colours; the task-434 record untouched; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-446
- usage: included in the executor run of task-446
- runs: 1
- milestone: M2
- started: 2026-11-05T22:20:14Z
- waiting_for: none
- result: Updated bp-barrel-explosion (new copy of the robot flash, 51 sprites, timings, sort priority, deleted M_BarrelExplosionSprite), bp-explosion-fx-barrel (cartoon sphere material M_CartoonExplosionSphere and its bands and fade), bp-explosion-fx (child overrides the sphere material), bp-explosion-shockwave (cel ring, parameters left, removed ZoneColor, ProgressColor, FillOpacity), sys-explosions (cartoon look line, no C++ change, relations). Task-434 record untouched. Lint clean (0 problems). Commit e41525d6.

## Relations
- [[task-446-explosions-red]] — parent global task.
