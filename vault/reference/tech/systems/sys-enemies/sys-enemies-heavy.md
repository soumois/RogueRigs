---
id: sys-enemies-heavy
description: "ARRHeavy as built: Chase, Telegraph, Slam, Recover, Dead; slam trigger and cadence, miner-only shockwave, corridor capsule, Blueprint events and getters, tunables."
parent: sys-enemies
kind: doc
---
# Heavy (`ARRHeavy`)

- responsibility: the heavy's decisions and its ground slam.
- main_classes: `ARRHeavy` (Blueprintable, `RRHeavy.h/.cpp`), `ERRHeavyState` Chase, Telegraph, Slam, Recover, Dead. Capsule 48 × 124 (2× chaser, radius ≤ 80 so it crosses one-cell corridors); the Blueprint mesh carries the 2× look.
- behavior: Chase (flow field, arrival = `AttackRange` × `ArrivalRangeRatio` 0.5 = 110 uu) → Telegraph when the miner is within `AttackRange` × `SlamTriggerRatio` (0.7 → 154 uu, `IsTargetWithin`) and the cadence allows it (previous slam + 1 / `AttackRate` − `WindUp`): stopped, `OnTelegraphStart`, committed for `WindUp` (0.8 s) → Slam (one frame): miner within `AttackRange` (220 uu, 2D centers) takes `ApplyDamageSpecTo(DamageEffectClass, Damage)` (`URRGE_DamageFlat`, × `AttackPower`); `OnSlam(Location = feet, Radius)` → Recover (stopped) `RecoverDuration` → `OnRecover` → Chase. The shockwave only looks for the miner: no terrain damage ([[destruction-rules]]), no robot damage. Knockback: base `ApplyKnockback` with CSV resistance 0.8 ([[sys-enemies-base]]).
- Blueprint-facing: events `OnTelegraphStart`, `OnSlam(Location, Radius)`, `OnRecover` (end of recovery; also on death during Telegraph or Recover, to reset the pose); pure `GetState`, `IsTelegraphing`, `GetTelegraphProgress` (0 → 1 over the telegraph, 0 outside: body tilt and glow), `GetSlamRadius`, `GetTelegraphDuration`, `GetLastSlamTime`, `GetSlamCount`.
- entry_points: the getters above, `GetSlamHitCount` (tests).
- data: CSV row `Heavy` ([[sys-enemies]]): AttackRange = slam radius, WindUp = telegraph, AttackRate 0.4 = one slam every 2.5 s. `EditDefaultsOnly`: `DamageEffectClass` (`URRGE_DamageFlat`), `SlamTriggerRatio` 0.7, `RecoverDuration` 0.5 s, `TelegraphFlashRate` 0 (no mesh blink: the Blueprint glows); `ArrivalRangeRatio` 0.5. Alive cap 5: spawner entry `MaxAlive` ([[sys-spawner]]).
- depends_on: [[sys-enemies-base]], [[sys-damage]], [[sys-flow-field]].
- implements: [[enemy-heavy]].
- gotchas: measured in contact: telegraphs every 2.5 s, slam 0.800 s later, 25 per hit; the telegraph started at 151 uu. Crosses the corridor x 36, rows 66–68 of `arena_m1` in 4.58 s. SlamTriggerRatio, RecoverDuration and the arrival ratio are code choices, not gdd values. Tests: [[sys-automation-tests]].

## Relations
- [[sys-enemies]] — parent index.
- [[sys-enemies-base]] — the shared helpers it uses.
- [[sys-spawner]] — the per-class alive cap that limits heavies to 5.
