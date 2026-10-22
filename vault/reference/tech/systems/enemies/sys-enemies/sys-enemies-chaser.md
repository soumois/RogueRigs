---
id: sys-enemies-chaser
description: "ARRChaser as built: melee states Chase, WindUp, Attack, Recover, Dead; cadence, capsule, tunables; baked run and attack clips via Custom Primitive Data 0 and 1."
parent: sys-enemies
kind: doc
---
# Chaser (`ARRChaser`)

- responsibility: the melee rusher's decisions.
- main_classes: `ARRChaser` (Blueprintable, `RRChaser.h/.cpp`), `ERRChaserState` Chase, WindUp, Attack, Recover, Dead.
- behavior: Chase (movement on) → WindUp when the target is within `Stats.AttackRange` (`IsTargetWithin`): movement off, `UpdateTelegraphBlink(WindUpFlashRate)` (12 Hz) for `Stats.WindUp` → Attack (one frame): if still in range, `ApplyDamageSpecTo(miner ASC, DamageEffectClass, Stats.Damage)` (`URRGE_DamageFlat`, × `AttackPower`) → Recover (movement off) until 1 / `AttackRate` after the attack → Chase. Leaving WindUp shows the meshes again. Arrival distance = `AttackRange` × `ArrivalRangeRatio` (0.9 → 108 uu). Measured cadence in contact: 0.1 s, then every 1.117 s. Capsule 24 × 62.
- animation (task-157-03; writer in the base since task-284-05, [[sys-enemies-base]] animation clips; inherited by [[sys-enemies-shield-chaser]]): `BodyMesh` (base `AnimatedMeshName`), CPD 0 clip id, CPD 1 start (world seconds), events only. Clips: 30 fps (`AnimFrameRate`), `RunClipId` 0 = run loop 16 frames (`RunClipDuration` 0.533 s; base `LoopClipId` 0), `AttackClipId` 1 = attack 15 frames (`AttackClipDuration` 0.5 s); no rest clip (`RestClipId` -1), so movement changes write nothing. Events: run loop from a random phase (base `PlayLoopClip`, spread `LoopPhaseSpread` 2.0 s, task-164-01; covers 0.762 s at RunRate 0.7) at BeginPlay and pool reuse (base), when the attack clip ends (base `OnAnimClipEnded`) and when Chase is entered while the attack clip still plays (relocation, `IsTimedClipPlaying` → `PlayMovementClip`); `PlayAttackClip` = base `PlayClip(AttackClipId, 0.5 s)` on entering WindUp. The material scales only the run clip by its RunRate (0.7, task-165-01). Checked in game (task-164) with `getall StaticMeshComponent CustomPrimitiveDataInternal`. Death and hits change nothing. Behaviour identical to before the move (task-284-05).
- entry_points: `GetState`, `GetLastAttackTime`, `GetAttackCount`.
- data: `StatsRowName` `Chaser` (CSV in [[sys-enemies]]); `DamageEffectClass`, `WindUpFlashRate`; `ArrivalRangeRatio` lives on the base ([[sys-enemies-base]]).
- depends_on: [[sys-enemies-base]], [[sys-damage]].
- implements: [[enemy-chaser]].
- gotchas: the flash hides meshes (no material yet). A hit refused by i-frames still counts as an attack (Recover runs).

## Relations
- [[sys-enemies]] — parent index.
- [[bp-chaser]] — its Blueprint.
