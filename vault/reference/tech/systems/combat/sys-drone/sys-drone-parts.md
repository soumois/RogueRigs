---
id: sys-drone-parts
description: "Drone animated parts: base yaw rate limit, laser arm pitch and beam start at its emitter, antenna spin; components, values, check. Open to tune them."
parent: sys-drone
kind: doc
---
# Drone — animated parts

- responsibility: the three animated parts of `ARRDrone` (task-253-01, owner 2026-10-20, all values provisional, category `Drone|Parts`, visual only, no collision change): base yaw, laser arm pitch, antenna spin; and where the beam starts.
- main_classes: `ARRDrone` (`Source/RogueRigs/Drone/RRDrone.h/.cpp`). Components: root `Mesh` = the base (SM_DroneBase, assigned in BP_Drone by a later bp subtask; the grey sphere until then); children of `Mesh`, relative transform, `NoCollision`, no shadow: `LaserPivot` (`UStaticMeshComponent`, SM_DroneLaser, relative location (1.606, 0, -17.248)) and `AntennaMesh` (SM_DroneAntenna, (-4.257, -0.837, 28.752)); offsets in Unreal axes from [[bp-drone-materials]]. `LaserMesh` (the beam) keeps its absolute transform.
- entry_points:
  - Base yaw (`UpdateAttitude`, [[sys-drone-dig]] Alive): the actor yaw is the base yaw; same point-of-interest list. In Dig and Hack the yaw turns toward the target at the constant `BaseYawTurnRate` (120 deg/s), never instantly; for the other interests the task-249-01 delay (`LookYawInterpSpeed` 6) is kept and capped by the same 120 deg/s (decision: the delay looked fine, the cap only acts above a 20 degree error, so a switch of interest no longer whips the base around). Tilt (pitch, roll) unchanged, on the whole actor.
  - Laser arm (`UpdateParts`, after `UpdateAttitude`, every tick with DeltaSeconds > 0, with or without a miner): aim point `GetLaserAimPoint` = `GetDigImpactPoint()` in Dig, hack target + `HackLinkOffset` in Hack, none in Orbit (rest 0). `ComputeLaserPitchToward`: in the base frame from the pivot, pitch P turns the arm's down axis to (sin P, 0, -cos P); P puts the pivot-to-emitter line on the point (out-of-plane error left to the base yaw). Clamped to [`LaserPitchMinDegrees` -20, `LaserPitchMaxDegrees` 80] (0 = straight down, positive = toward the base's front; the dig aim about 90 uu ahead and 60-80 uu below needs about 50-60), turned at `LaserPitchTurnRate` (240 deg/s) at most; `LaserPivot` relative rotation (P, 0, 0).
  - Beam start: `GetLaserEmitterLocation()` (BlueprintPure) = `LaserEmitterLocal` ((-0.054, 0.143, -19), local to `LaserPivot`) through the pivot's world transform, valid with no mesh. Dig and Hack draw `LaserMesh` from it (was the drone centre); `UpdateParts` re-places a visible beam from the moved emitter to the stored end `LaserBeamEnd`. Still hidden outside Dig and Hack.
  - Antenna: `AntennaMesh` relative yaw += `AntennaSpinSpeed` (90 deg/s) × dt, always (Orbit, Dig, Hack).
- data: `EditDefaultsOnly` defaults only; BP_Drone overrides none.
- depends_on: [[sys-drone-dig]] (attitude, dig impact point, hack link).
- implements: [[drone]] (Parts line).
- gotchas:
  - BP_Drone scales `Mesh` to 0.4 for the grey sphere: the children inherit it until the bp subtask assigns SM_DroneBase at scale 1 (the offsets are in the base mesh's units).
  - With the base turning at 120 deg/s, the drone can need up to 1.5 s to face a dig target; a 0.5 s rock arc may break before it does (the beam always goes from the emitter to the impact point, even off the arm's axis).
  - Measured by `Dig.DroneParts` ([[sys-tests-dig-ore]]).

## Relations
- [[sys-drone]] — parent hub: the drone actor.
- [[sys-drone-dig]] — the attitude and dig tick the parts follow.
- [[bp-drone-materials]] — meshes, bounds and attachment offsets.
- [[drone]] — gameplay spec.
- [[sys-tests-dig-ore]] — the headless check.
