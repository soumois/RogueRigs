---
id: task-464-02-turret-laser-green
description: "Bp: Turret shots and their impact sparks in the same green look as the rifle laser"
parent: task-464-player-shot-colour
kind: task
status: done
---
# Player shot colour: bp

- goal: Turret shots and their impact sparks in the same green look as the rifle laser
- agent: bp
- depends_on: task-464-01-rifle-laser-green
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), turret (vault/reference/gdd/weapons/turret.md), task-464-01-rifle-laser-green (vault/reference/tasks/task-464-player-shot-colour/task-464-01-rifle-laser-green.md)
- expected_output: see done_when
- done_when: In a -game run turret shots and their impact sparks match the rifle laser look of task-464-01; turret behaviour and values unchanged.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-464
- usage: included in the executor run of task-464
- runs: 1
- milestone: M2
- started: 2026-11-06T09:06:28Z
- waiting_for: none
- result: Done. BP_TurretBullet ShotMesh OverrideMaterials = M_PlayerLaserShot and ImpactSparkSystem = NS_ShotImpactSparkGreen (the rifle assets of task-464-01), compiled, read back, saved. -game frame Saved/Captures/task464_02_check/turret.png: the turret bolt is green with a pale centre and darker rim, same look as the rifle shot. Sparks use the same system as the rifle (one green spark seen in 464-01), not seen separately on a turret hit. Turret behaviour and values untouched (no C++ or data change). The old white-gold sphere MuzzleFlash of BP_Turret still shows at the barrel: task-464-04.

## Relations
- [[task-464-player-shot-colour]] — parent global task.
