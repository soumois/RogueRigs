---
id: task-463-01-aim-assist
description: "Cpp: Burst rifle bullets get the light aim assist decided in the global node; other shots unchanged."
parent: task-463-aim-assist
kind: task
status: done
---
# Light aim assist on miner shots: cpp

- goal: Burst rifle bullets get the light aim assist decided in the global node; other shots unchanged.
- agent: cpp
- depends_on: none
- inputs: miner (vault/reference/gdd/player/miner.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), barrel (vault/reference/gdd/terrain/barrel.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-auto-fire (vault/reference/tech/systems/combat/sys-auto-fire.md)
- expected_output: see done_when
- done_when: Build ok; in PIE or a test: a near-miss bullet is deflected at most 5 degrees onto a robot, a bullet already on a robot or a barrel is not, a robot behind rock or outside the cone is not targeted; the angle is read from data; weapon test families pass, figures in the result.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-463
- usage: included in the executor run of task-463
- runs: 1
- milestone: M2
- started: 2026-11-06T05:57:03Z
- waiting_for: none
- result: Commit d7210d03. New Weapons/RRAimAssist.h/.cpp (PredictFirstHit, FindAssistedDirection), ARRWeapon::GetAimAssistedRotation called per bullet in ARRWeapon::FireOnce after the spread; WeaponStats.csv column AimAssistDeg (BurstRifle 5, Shotgun 0, TurretLauncher 0), copied in ApplyStatsRow. Straight shot swept with the real projectile capsule and responses (miner axis to Muzzle, then Range): robot or barrel first = unchanged; else smallest-turn living robot within Range and the cone whose turned shot meets a robot first; flat aim at its current position. Build ok; RogueRigs.Weapons 16/16 pass incl. new RogueRigs.Weapons.AimAssist: near miss 3.79 deg -> 0.00 deg (turn <= 5), fired bullet hits the still robot (1000 -> 990); shot on robot, outside cone, beyond range, behind rock, AimAssistDeg 0 and barrel-beside-robot unchanged. Feel unverified in game.

## Relations
- [[task-463-aim-assist]] — parent global task.
