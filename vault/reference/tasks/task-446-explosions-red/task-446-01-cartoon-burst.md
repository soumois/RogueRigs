---
id: task-446-01-cartoon-burst
description: "Bp: Representative slice: the burst and its material have the cartoon look, in the style of the robot death flash."
parent: task-446-explosions-red
kind: task
status: done
---
# Barrel and kamikaze explosions in cartoon style: bp

- goal: Representative slice: the burst and its material have the cartoon look, in the style of the robot death flash.
- agent: bp
- depends_on: none
- inputs: task-436-robot-death-particle (vault/reference/tasks/task-436-robot-death-particle.md), bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), bp-barrel-explosion (vault/reference/tech/blueprints/fx/bp-barrel-explosion.md), explosions (vault/reference/gdd/terrain/explosions.md), Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png
- expected_output: see done_when
- done_when: System and material compile with no error; at most 95 sprites read back; one-shot end within 1.5 s; still sized by User.Radius; no pixel of the reference shipped.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-446
- usage: included in the executor run of task-446
- runs: 1
- milestone: M2
- started: 2026-11-05T22:07:12Z
- waiting_for: none
- result: NS_BarrelExplosion rebuilt as a copy of NS_RobotDeathPuff sized by User.Radius (default 735): Core flash 1 (0.16 s, 0.8 R), Lines 20 (t 0.04 s, ring 0.8 R, 2 to 3 R/s), Blobs 14 (t 0.1 s, ring 0.85 R), dark red Puffs 16 (t 0.36 s, ring 0.8 R, colour 0.32, 0.045, 0.025) = 51 CPU sprites read back; system Once, loop 0.5 s, last particle ends by 0.96 s; translucency sort priority 1 so it draws above the sphere; same M_CartoonBlast materials, no texture, no pixel of the reference. Compile state UpToDate, 0 errors, 0 warnings. BurstSystem of BP_ExplosionFX_Barrel read back; unused M_BarrelExplosionSprite deleted. Commit 97dbee6e. Look unverified in game (A8).

## Relations
- [[task-446-explosions-red]] — parent global task.
