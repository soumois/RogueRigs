---
id: sys-drone-parts
description: "Drone animated parts: base yaw rate, laser arm pitch and beam start, antenna spin, two speed-driven pod thrusters; components, values, check. Open to tune them."
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
  - Thrusters (task-254-02, owner 2026-10-20, all values provisional, category `Drone|Thrusters`, visual only): two `UNiagaraComponent` `ThrusterLeft` and `ThrusterRight`, constructor subobjects attached to `Mesh` (relative transform, so they follow the base yaw and tilt), `bAutoActivate` false (C++ activates), `NoCollision`, never spawned per frame. Relative transforms `ThrusterLeftOffset` ((0, -27, -17), identity rotation, exhaust along -Z) and `ThrusterRightOffset` ((0, 27, -17)), applied in the constructor and in `OnConstruction` (so a Blueprint override applies); guessed from the pod bounds of [[bp-drone-materials]] (pods at y ±26 to ±35, bottoms at z -15.6), to fine-tune on the real meshes in task-254-03. The pods are mirror-symmetric in Y (and the Unreal Y of the Blender values is mirrored), so which sign is "left" does not matter. Both play `ThrusterSystem` (default `/Game/RogueRigs/FX/Drone/NS_DroneThruster` through a `ConstructorHelpers::FObjectFinder`, like `DigImpactSystem`; None = no effect; [[bp-drone-thruster]]). `UpdateThrusters`, last in `Tick`, every state: raw speed = |location − previous location| / DeltaSeconds (the actor's own world speed, miner motion included), smoothed exponentially (alpha = 1 − exp(−dt / `ThrusterSpeedSmoothing`), 0.1 s; 0 = raw), written to `User.Speed` (cached FName) of both components; an inactive component is activated (Orbit, Dig and Hack alike; idle glow at 0). `SetOwnerMiner` resets the measure (speed 0, previous location retaken next tick) so its snap onto the patrol point is not read as speed. `GetThrusterSpeed()` (BlueprintPure) returns the smoothed value. Without rendering (`FApp::CanEverRender()` false, `-nullrhi`) the speed is still measured but no activation is attempted, so the thrusters are never active there. The system clamps the speed at 700 uu/s (`PatrolFarSpeed`).
- data: `EditDefaultsOnly` defaults only; BP_Drone overrides none.
- depends_on: [[sys-drone-dig]] (attitude, dig impact point, hack link).
- implements: [[drone]] (Parts line).
- gotchas:
  - BP_Drone scales `Mesh` to 0.4 for the grey sphere: the children inherit it until the bp subtask assigns SM_DroneBase at scale 1 (the offsets are in the base mesh's units).
  - With the base turning at 120 deg/s, the drone can need up to 1.5 s to face a dig target; a 0.65 s rock arc (0.5 s before task-260-01) may break before it does (the beam always goes from the emitter to the impact point, even off the arm's axis).
  - Measured by `Dig.DroneParts` ([[sys-tests-dig-ore]]). The thrusters have no test (no new test asked; Niagara never runs under `-nullrhi`).
  - Shadows (task-263-01): `ARRDrone::PostInitializeComponents` calls `RRShadows::DisableCastShadows`: base, laser arm, antenna, beam, thrusters and any BP_Drone component cast no shadow (only the grid rocks cast, [[lights-no-shadows]]).
  - Blob shadow (task-264-02, `Drone|Blob Shadow`, provisional): `BlobShadow` (`URRBlobShadowComponent`, [[sys-blob-shadow]]) attached to `Mesh` with absolute location, rotation and scale; `UpdateBlobShadow`, after `UpdateThrusters` in `Tick`, puts it on the miner floor (capsule bottom) + 3 uu under the drone; diameter `BlobShadowDiameter` (70 uu, the base width) × `GetBlobShadowHeightScale`: 1 up to `BlobShadowFullSizeHeight` (130 uu above the floor), linear to `BlobShadowHighScale` (0.75) at `BlobShadowHighHeight` (250). No miner = not moved.
  - Thrusters: a teleport other than `SetOwnerMiner` (the patrol snap beyond `EscortSnapDistance`) reads as a one-frame speed spike; the smoothing and the system's 700 uu/s clamp keep it short.

## Relations
- [[sys-drone]] — parent hub: the drone actor.
- [[sys-drone-dig]] — the attitude and dig tick the parts follow.
- [[bp-drone-materials]] — meshes, bounds and attachment offsets.
- [[drone]] — gameplay spec.
- [[sys-tests-dig-ore]] — the headless check.
- [[sys-blob-shadow]] — the floor disc under the drone.
- [[bp-drone-thruster]] — NS_DroneThruster, the system both thrusters play (User.Speed).
