---
id: sys-enemies-kamikaze
description: "ARRKamikaze as built: Chase, Armed (0.5 s core flare), Dead; hover and core visuals, flare events and getters, fuse explosion, instant death explosion, tunables."
parent: sys-enemies
kind: doc
---
# Kamikaze (`ARRKamikaze`)

- responsibility: the charger that arms near the miner and explodes.
- main_classes: `ARRKamikaze` (Blueprintable, `RRKamikaze.h/.cpp`), `ERRKamikazeState` Chase, Armed, Dead. Components: `VisualRoot` (on the capsule) carrying `BodyMesh` and `CoreMesh` (no collision; meshes, eyes and logo plate set by the Blueprint).
- behavior: Chase (movement on, arrival arm range × `ArrivalRangeRatio` = 75 uu) → Armed when the target is within the arm range (`IsTargetWithin`): movement off, committed → after the fuse (`HasStateLasted`): queues one `ExplosionRowName` explosion ([[sys-explosions]], instigator and source = itself), adds `State.Dead` and dies without XP, so its own blast is refused and never counts as a kill. Killed before: `HandleDeath` queues the same explosion and the XP drops as usual; never exploded twice.
- visuals: `VisualRoot` `HoverHeight` above the capsule bottom with a sine bob; `CoreMesh` scale = BeginPlay scale × growth × (1 + `CorePulseAmplitude` × sin(phase)), phase at `CorePulseRate` (Armed: `ArmedPulseRate`), growth linear to `ArmedCoreScale` over the fuse while armed.
- flare (task-027-01, [[enemy-kamikaze]]): the Armed phase is the flare; duration = fuse (CSV `WindUp`, 0.5 s). Arming calls the Blueprint event `OnFlareStart(Duration)` and the native `OnFlareStarted(Kamikaze)`; leaving Armed calls `OnFlareEnd(bDetonatedByFuse)` (true at the fuse explosion, false when killed during the flare). `IsFlaring`, `GetFlareDuration`, `GetFlareProgress` (0 to 1 over the fuse; 1 after the fuse explosion; 0 before arming or when killed earlier) are BlueprintPure, for the core light ramp. A death explosion stays instant, no flare. XP burst: 4 cubes ([[sys-pickups]]). Measured: flare start = arm time, explosion 0.500 s later, progress 0.967 on the last flare frame.
- entry_points: `GetState`, `HasDetonated`, `GetArmRange` (CSV `AttackRange`, 150 uu), `GetFuseTime` (CSV `WindUp`, 0.5 s), `GetArmTime`, `GetExplosionQueuedTime`, `GetCoreScaleFactor`.
- data: arm range and fuse in its CSV row (task-021-01, R8 of [[code-review-m1-deferred]]; values unchanged); `EditDefaultsOnly`: `StatsRowName` `Kamikaze`, `ArrivalRangeRatio` 0.5 (set in the constructor), `ExplosionRowName` `Kamikaze`, `HoverHeight` 40, `HoverBobAmplitude` 4, `HoverBobRate` 1.5 Hz, `CorePulseRate` 1.5 Hz, `ArmedPulseRate` 8 Hz, `CorePulseAmplitude` 0.15, `ArmedCoreScale` 1.8 (visual values are code defaults, not gdd values). Capsule 37.4 × 96.8 (1.1 × the miner).
- depends_on: [[sys-enemies-base]], [[sys-explosions]].
- implements: [[enemy-kamikaze]].
- gotchas: contact with the miner is at 71.4 uu, inside arrival and the arm range, so it always arms first. Measured (300 uu/s): arms at 149.5 uu, explosion queued 0.500 s after arming, core peak about 2.00.

## Relations
- [[sys-enemies]] — parent index.
- [[bp-kamikaze]] — its Blueprint.
