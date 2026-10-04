---
id: sys-enemies-kamikaze
description: "ARRKamikaze as built: Chase, Armed, Dead; hover and core pulse visuals, fuse explosion without XP, explosion on death, tunables and measurements."
parent: sys-enemies
kind: doc
---
# Kamikaze (`ARRKamikaze`)

- responsibility: the charger that arms near the miner and explodes.
- main_classes: `ARRKamikaze` (Blueprintable, `RRKamikaze.h/.cpp`), `ERRKamikazeState` Chase, Armed, Dead. Components: `VisualRoot` (on the capsule) carrying `BodyMesh` and `CoreMesh` (no collision; meshes, eyes and logo plate set by the Blueprint).
- behavior: Chase (movement on, arrival `ArmRange` × `ArrivalRangeRatio` = 75 uu) → Armed when the target is within `ArmRange` (`IsTargetWithin`): movement off, committed → after `FuseTime` (`HasStateLasted`): queues one `ExplosionRowName` explosion ([[sys-explosions]], instigator and source = itself), adds `State.Dead` and dies without XP, so its own blast is refused and never counts as a kill. Killed before: `HandleDeath` queues the same explosion and the XP drops as usual; never exploded twice.
- visuals: `VisualRoot` `HoverHeight` above the capsule bottom with a sine bob; `CoreMesh` scale = BeginPlay scale × growth × (1 + `CorePulseAmplitude` × sin(phase)), phase at `CorePulseRate` (Armed: `ArmedPulseRate`), growth linear to `ArmedCoreScale` over `FuseTime` while armed.
- entry_points: `GetState`, `HasDetonated`, `GetArmTime`, `GetExplosionQueuedTime`, `GetCoreScaleFactor`.
- data (`EditDefaultsOnly`): `StatsRowName` `Kamikaze`, `ArmRange` 150 uu, `FuseTime` 0.5 s, `ArrivalRangeRatio` 0.5 (set in the constructor), `ExplosionRowName` `Kamikaze`, `HoverHeight` 40, `HoverBobAmplitude` 4, `HoverBobRate` 1.5 Hz, `CorePulseRate` 1.5 Hz, `ArmedPulseRate` 8 Hz, `CorePulseAmplitude` 0.15, `ArmedCoreScale` 1.8 (visual values are code defaults, not gdd values). Capsule 37.4 × 96.8 (1.1 × the miner).
- depends_on: [[sys-enemies-base]], [[sys-explosions]].
- implements: [[enemy-kamikaze]].
- gotchas: contact with the miner is at 71.4 uu, inside arrival and `ArmRange`, so it always arms first. Measured (300 uu/s): arms at 149.5 uu, explosion queued 0.500 s after arming, core peak about 2.00. Arm range and fuse are properties while its CSV columns stay 0 (F2, [[code-review-m1-gameplay]]).

## Relations
- [[sys-enemies]] — parent index.
- [[bp-kamikaze]] — its Blueprint.
