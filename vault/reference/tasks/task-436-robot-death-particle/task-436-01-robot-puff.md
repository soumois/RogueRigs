---
id: task-436-01-robot-puff
description: "Bp: Representative slice: the robot death puff has its new look."
parent: task-436-robot-death-particle
kind: task
status: done
---
# Better robot death particle: bp

- goal: Representative slice: the robot death puff has its new look.
- agent: bp
- depends_on: none
- inputs: bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), bp-barrel-explosion (vault/reference/tech/blueprints/fx/bp-barrel-explosion.md), Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png
- expected_output: see done_when
- done_when: System compiles with no error, one-shot behaviour and pool return kept, at most 20 sprites read back, material compiles with flat cel-shaded bands, colours as in the reference, red included, no pixel of the reference shipped.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-436
- usage: included in the executor run of task-436
- runs: 1
- milestone: M2
- started: 2026-11-05T17:37:13Z
- waiting_for: none
- result: NS_RobotDeathPuff rebuilt: 4 CPU sprite emitters (Lines 6, Core 1, Blobs 6, Puffs 5 = 18 sprites read back), system Once, loop 0.2 s, inactive Complete, last particle ends at about 0.42 s; compile state UpToDate, 0 errors; 3 new procedural materials M_CartoonBlastCore, Blob, Line compile with hard-edged flat bands, no texture and no pixel of the reference. BP_Kamikaze moved to NS_KamikazeDeathPuff, an untouched copy of the old puff, because it referenced the same system and A6 keeps its death unchanged. Look unverified in game (A7).

## Relations
- [[task-436-robot-death-particle]] — parent global task.
