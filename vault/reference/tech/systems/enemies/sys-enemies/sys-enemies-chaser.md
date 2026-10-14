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
- animation (task-157-03, inherited by [[sys-enemies-shield-chaser]]): the body mesh plays clips baked in a texture, sampled by its material's World Position Offset. Mesh: the `UStaticMeshComponent` named `AnimatedMeshName` (`EditDefaultsOnly`, `Chaser|Animation`, default `BodyMesh`, the component BP_Chaser and BP_ShieldChaser add), found once at BeginPlay and kept in `AnimatedMesh`; no such component (native class in tests) = no animation, nothing to bind in the Blueprint while the component keeps that name. Custom Primitive Data contract, indices 0 and 1 reserved for this animation on that mesh, written only by `ARRChaser::SetAnimClip` (one `SetCustomPrimitiveDataVector2f` call), only on events, never per tick: index 0 (`AnimClipDataIndex`) = clip id, float, `RunClipId` 0 = run (loop), `AttackClipId` 1 = attack; index 1 (`AnimStartDataIndex`) = clip start, world seconds (`GetWorld()->GetTimeSeconds()`, as float). The material plays t = Time − start with its `Time` node, the same world time (pauses and dilates with the game); game time and real time differ only when the game is paused or slowed. Clips: 30 fps (`AnimFrameRate`), run 16 frames (`RunClipDuration` 0.533 s), attack 15 frames (`AttackClipDuration` 0.5 s). Events: `PlayRunClip` (clip 0, start = now − `FMath::FRand()` × `RunPhaseSpread` (2.0 s, task-164-01), drawn per chaser at each run start so a crowd does not step in sync; the spread exceeds any run period, 0.533 s at rate 1, 0.762 s at rate 0.7; before task-164 it was one run period, so it covered only part of a slower loop. Checked in game with `getall StaticMeshComponent CustomPrimitiveDataInternal`: every BodyMesh gets its own start; ClipId and AnimStart are CPD-flagged parameters 0 and 1 in M_Chaser_Meshy_Anim) at BeginPlay and `ResetForNewLife` (pool reuse), when the attack clip ends, and when Chase is entered while the attack clip still plays (relocation); `PlayAttackClip` (clip 1, start = now) on entering WindUp, then one `FTimerHandle` (`AttackClipTimer`) calls `PlayRunClip` 0.5 s later. The material scales only the run clip by its RunRate parameter (default 0.7, task-165-01); the C++ phase spread 2.0 s covers any rate, and the C++ constant `RunClipDuration` 0.533 s is only used for the loop, not for the rate. Death and hits change nothing. Timing, damage, telegraph blink, knockback and pooling are unchanged.
- entry_points: `GetState`, `GetLastAttackTime`, `GetAttackCount`.
- data: `StatsRowName` `Chaser` (CSV in [[sys-enemies]]); `DamageEffectClass`, `WindUpFlashRate`; `ArrivalRangeRatio` lives on the base ([[sys-enemies-base]]).
- depends_on: [[sys-enemies-base]], [[sys-damage]].
- implements: [[enemy-chaser]].
- gotchas: the flash hides meshes (no material yet). A hit refused by i-frames still counts as an attack (Recover runs).

## Relations
- [[sys-enemies]] — parent index.
- [[bp-chaser]] — its Blueprint.
