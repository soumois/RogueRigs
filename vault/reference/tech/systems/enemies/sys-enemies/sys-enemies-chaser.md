---
id: sys-enemies-chaser
description: "ARRChaser as built: melee state machine Chase, WindUp, Attack, Recover, Dead; measured cadence, capsule and tunables."
parent: sys-enemies
kind: doc
---
# Chaser (`ARRChaser`)

- responsibility: the melee rusher's decisions.
- main_classes: `ARRChaser` (Blueprintable, `RRChaser.h/.cpp`), `ERRChaserState` Chase, WindUp, Attack, Recover, Dead.
- behavior: Chase (movement on) → WindUp when the target is within `Stats.AttackRange` (`IsTargetWithin`): movement off, `UpdateTelegraphBlink(WindUpFlashRate)` (12 Hz) for `Stats.WindUp` → Attack (one frame): if still in range, `ApplyDamageSpecTo(miner ASC, DamageEffectClass, Stats.Damage)` (`URRGE_DamageFlat`, × `AttackPower`) → Recover (movement off) until 1 / `AttackRate` after the attack → Chase. Leaving WindUp shows the meshes again. Arrival distance = `AttackRange` × `ArrivalRangeRatio` (0.9 → 108 uu). Measured cadence in contact: 0.1 s, then every 1.117 s. Capsule 24 × 62.
- entry_points: `GetState`, `GetLastAttackTime`, `GetAttackCount`.
- data: `StatsRowName` `Chaser` (CSV in [[sys-enemies]]); `DamageEffectClass`, `WindUpFlashRate`; `ArrivalRangeRatio` lives on the base ([[sys-enemies-base]]).
- depends_on: [[sys-enemies-base]], [[sys-damage]].
- implements: [[enemy-chaser]].
- gotchas: the flash hides meshes (no material yet). A hit refused by i-frames still counts as an attack (Recover runs).

## Relations
- [[sys-enemies]] — parent index.
- [[bp-chaser]] — its Blueprint.
