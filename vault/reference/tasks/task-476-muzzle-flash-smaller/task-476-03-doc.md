---
id: task-476-03-doc
description: "Doc: Docs record the new flash size."
parent: task-476-muzzle-flash-smaller
kind: task
status: done
---
# Player muzzle flash a little smaller: doc

- goal: Docs record the new flash size.
- agent: doc
- depends_on: task-476-02-capture
- inputs: burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), task-464-player-shot-colour (vault/reference/tasks/task-464-player-shot-colour.md)
- expected_output: see done_when
- done_when: The bp-* and sys-* nodes of the muzzle flash give the new size and the turret compensation if any; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-476
- usage: included in the executor run of task-476
- runs: 1
- milestone: M2
- started: 2026-11-06T19:33:45Z
- waiting_for: none
- result: Done (beaa3d6f). bp-muzzle-flash: rifle sizes x0.7 (streak 11.2 x 210-252, shape 119-140, core 35-42, sparks 5.6-8.4 x 35-63 at 280-490), NS_MuzzleFlashTurret at the old sizes, why (sprite size ignores component scale), verified_476 frames, task-476 relation. bp-turret: OnTurretFired spawns NS_MuzzleFlashTurret, scale 0.6. sys-weapons: muzzle flash paragraph. Lint clean (warnings pre-existing).

## Relations
- [[task-476-muzzle-flash-smaller]] — parent global task.
