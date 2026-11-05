---
id: task-433-03-flash-other-robots
description: "Bp: Every other robot class flashes the same way, the kamikaze core excepted."
parent: task-433-robot-hit-flash
kind: task
status: done
---
# Robot hit flash: bp

- goal: Every other robot class flashes the same way, the kamikaze core excepted.
- agent: bp
- depends_on: task-433-02-flash-chaser-material
- inputs: readability-rules (vault/reference/gdd/art-audio/readability-rules.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-kamikaze (vault/reference/gdd/enemies/enemy-kamikaze.md), enemy-shooter (vault/reference/gdd/enemies/enemy-shooter.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md)
- expected_output: see done_when
- done_when: Materials of shield chaser, kamikaze body, shooter and heavy (v1 and v2 where the game spawns them) compiled with no error and read the flash slot; the kamikaze core material is untouched; any class that cannot be done is listed with the reason.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-433
- usage: included in the executor run of task-433
- runs: 1
- milestone: M2
- started: 2026-11-05T16:17:45Z
- waiting_for: none
- result: M_ShieldChaserV2_Anim and M_RobotEye get MF_RobotHitFlash (eye emissive goes through it); kamikaze, shooter and heavy bodies already through M_RobotMeshy. MI_ShieldChaserV2_Shield and _Emblem HitFlashStrength 0. MI_Kamikaze_Core untouched: CoreMesh never gets slot 2. Not done, not spawned: v1 BP_Chaser and BP_ShieldChaser materials. Test: 17/17 RogueRigs.Enemies, ShieldChaser. No compile error logged; look unverified.

## Relations
- [[task-433-robot-hit-flash]] — parent global task.
