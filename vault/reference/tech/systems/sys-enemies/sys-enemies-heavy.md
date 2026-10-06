---
id: sys-enemies-heavy
description: "ARRHeavy as built: Chase, Telegraph, Slam, Recover, Dead; slam trigger and cadence, miner-only shockwave, corridor capsule, Blueprint events and getters, tunables."
parent: sys-enemies
kind: doc
---
# Heavy (`ARRHeavy`)

- responsibility: the heavy's decisions and its ground slam.
- main_classes: `ARRHeavy` (Blueprintable, `RRHeavy.h/.cpp`), `ERRHeavyState` Chase, Telegraph, Slam, Recover, Dead. Capsule 48 × 124 (2× chaser, radius ≤ 80 so it crosses one-cell corridors); the Blueprint mesh carries the 2× look.
- behavior: Chase (flow field, arrival = `AttackRange` × `ArrivalRangeRatio` 0.5 = 200 uu) → Telegraph when the miner is within `AttackRange` × `SlamTriggerRatio` (0.7 → 280 uu, `IsTargetWithin`) and the cadence allows it (previous slam + 1 / `AttackRate` − `WindUp`): stopped, `OnTelegraphStart`, committed for `WindUp` (0.6 s, task-032-01; was 0.8) → Slam (one frame): miner within `AttackRange` (400 uu since task-028-01, was 220; 2D centers) takes `ApplyDamageSpecTo(DamageEffectClass, Damage)` (`URRGE_DamageFlat`, × `AttackPower`); `OnSlam(Location = feet, Radius)` → Recover (stopped) `RecoverDuration` → `OnRecover` → Chase. The shockwave only looks for the miner: no terrain damage ([[destruction-rules]]), no robot damage. Knockback: base `ApplyKnockback` with CSV resistance 0.8 ([[sys-enemies-base]]).
- Blueprint-facing: events `OnTelegraphStart`, `OnSlam(Location, Radius)`, `OnRecover` (end of recovery; also on death during Telegraph or Recover, to reset the pose); pure `GetState`, `IsTelegraphing`, `GetTelegraphProgress` (0 → 1 over the telegraph, 0 outside: body tilt and glow), `GetSlamRadius`, `GetTelegraphDuration`, `GetLastSlamTime`, `GetSlamCount`.
- entry_points: the getters above, `GetSlamHitCount` (tests).
- data: CSV row `Heavy` ([[sys-enemies]]): AttackRange = slam radius, WindUp = telegraph, AttackRate 0.5 = one slam every 2 s (task-032-01; was 0.4, 2.5 s). `EditDefaultsOnly`: `DamageEffectClass` (`URRGE_DamageFlat`), `SlamTriggerRatio` 0.7, `RecoverDuration` 0.5 s, `TelegraphFlashRate` 0 (no mesh blink: the Blueprint glows); `ArrivalRangeRatio` 0.5. Alive cap 5: spawner entry `MaxAlive` ([[sys-spawner]]).
- depends_on: [[sys-enemies-base]], [[sys-damage]], [[sys-flow-field]].
- implements: [[enemy-heavy]].
- gotchas: measurements below were taken at radius 220 (trigger 154) before task-028-01; the radius tests were moved to 400 / 280 (inside 390, miss 410). Measured in contact (task-032-01): telegraphs every 2 s (3.217, 5.217, 7.217 s), slam 0.600 s later, 25 per hit; the telegraph started at 278 uu (trigger 280). Crosses the corridor x 36, rows 66–68 of `arena_m1` in 4.58 s (also with `BP_Heavy`). With `BP_Heavy` and `BP_Miner` (task-022-03): 25 per slam on a vulnerable miner, hit at 210 uu, miss at 230 uu, shotgun launch 120 uu/s vs 600 for `BP_Chaser`. SlamTriggerRatio, RecoverDuration and the arrival ratio are code choices, not gdd values. Tests: [[sys-automation-tests]].

## Relations
- [[sys-enemies]] — parent index.
- [[sys-enemies-base]] — the shared helpers it uses.
- [[sys-spawner]] — the per-class alive cap that limits heavies to 5.
