---
id: sys-drone
description: "Drone C++ actor: spawned by the miner; orbit, dig and hack states; dig hover, laser mesh, cancel rule, hack link; MiningSpeed from the miner's ASC."
parent: systems-index
kind: doc
---
# Drone

- responsibility: the helper drone actor and its state behaviours: Orbit, Dig, Hack. No dialogue yet.
- main_classes (in `Source/RogueRigs/Drone/`):
  - `ARRDrone : AActor` (`RRDrone.h`, `Abstract`, subclassed in Blueprint): root `UStaticMeshComponent` `Mesh` with profile `NoCollision` (no overlap events, does not affect navigation); child `UStaticMeshComponent` `LaserMesh` (absolute transform, `NoCollision`, no shadow, hidden outside Dig). Both meshes, and the laser's red material, are set in Blueprint; `LaserMesh` expects a cylinder along local Z with a centered pivot (the engine cylinder).
  - Editable, orbit: `OrbitRadius` (150 uu), `OrbitPeriod` (2 s), `HoverHeight` (150 uu, also the dig height), `FollowInterpSpeed` (10, provisional; 0 = snap; also used in Dig).
  - Editable, dig: `DigHoverDistance` (60 uu, provisional), `DigCancelDistanceCells` (4.5 cells of 67 uu = the former 1.5 cells of 200 uu, task-029-04; arc digging in task-029-05 may replace it).
  - Editable, hack: `HackHoverDistance` (100 uu, provisional), `HackLinkOffset` ((0, 0, 50), provisional).
  - Editable, laser: `LaserDiameter` (8 uu), `LaserMeshBaseHeight` (100 uu), `LaserMeshBaseDiameter` (100 uu); the base sizes are the mesh's size at scale 1.
  - `ERRDroneState` (`uint8`, BlueprintType): `Orbit`, `Dig`, `Hack`.
- entry_points:
  - Spawned by `ARRMiner::BeginPlay` from the miner's editable `DroneClass` (owner and instigator = miner, kept in the miner's `Drone` UPROPERTY); destroyed in `ARRMiner::EndPlay` ([[sys-player]]). `DroneClass` unset = no drone, warning logged.
  - `SetOwnerMiner(ARRMiner*)`: stops any dig, sets the followed miner, adds it as tick prerequisite, resets the orbit angle to 0 and snaps the drone to the orbit point.
  - `GetOwnerMiner()`, `GetState()`, `GetDigCell()`, `GetMiningSpeed()` (miner ASC `MiningSpeed`; 1 if no miner, ASC or attribute).
  - `SetState(ERRDroneState)`: `Orbit` always accepted (stops a dig or hack); `Dig` accepted only with a miner and a dig target that still holds a block (set by `RequestDig`), refused while hacking; `Hack` refused (use `StartHack`). Refusals return false and log a warning.
  - `StartHack(AActor* Target)`: called every tick by the objective while the miner is in its zone ([[sys-objectives]]); stops a dig, enters Hack; same target = true; another target while hacking = false (one at a time). `StopHack(Target)`: back to Orbit if Target is the hack target. `GetHackTarget()`. `RequestDig` returns false while hacking (no dig and hack at once).
  - Hack tick: back to Orbit if the miner or target is gone; moves to the target location + `HackHoverDistance` toward the miner (Z = miner floor + `HoverHeight`); `LaserMesh` from the drone to target + `HackLinkOffset`. Progress lives on the objective.
  - `RequestDig(ARRArenaGrid* Grid, FIntPoint Cell, FIntPoint MinerSide)`: called every tick by the miner while it pushes into a block ([[sys-player]]). `MinerSide` = unit grid step from the block toward the miner. Same cell: refreshes the side only. Other cell: releases the previous block (stops its shake, damage kept) and targets the new one. Enters Dig and starts the block shaking (`SetCellShaking(Cell, true)`). False when the cell has no block or there is no miner.
  - Orbit tick: angle += 2π / `OrbitPeriod` per second (wrapped to [0, 2π)); orbit point = miner location + `OrbitRadius` × (cos, sin) in XY, Z = miner capsule bottom + `HoverHeight`; the drone moves toward it with `FMath::VInterpTo` at `FollowInterpSpeed`.
  - Dig tick, in order: (1) cancel to Orbit if the miner, grid or block is gone, or the miner's 2D distance to the block center exceeds `DigCancelDistanceCells` × grid `CellSize`; (2) move toward the hover point = the cut face center (`ARRArenaGrid::GetCellFaceCenter(Cell, MinerSide)`) pushed `DigHoverDistance` outward, Z = miner capsule bottom + `HoverHeight`; (3) place the laser from the drone to the face center (location = midpoint, Z axis along the beam, scale XY = `LaserDiameter` / `LaserMeshBaseDiameter`, Z = length / `LaserMeshBaseHeight`) and show it; (4) `ApplyTerrainDamage(Cell, DeltaSeconds × GetMiningSpeed(), Drone)`; back to Orbit when it destroys the block.
  - Leaving Dig (cancel, destruction, `SetState(Orbit)`, `SetOwnerMiner`, `EndPlay`): `SetCellShaking(Cell, false)`, laser hidden, target cleared, orbit angle set to the drone's current angle around the miner so the orbit resumes where it is. Damage stays on the block ([[sys-grid]]).
- data: no table; tunables are `EditDefaultsOnly` defaults on the Blueprint subclass. Values from [[drone]].
- depends_on: [[sys-player]] (miner, capsule), [[sys-gas-core]] (`URRPlayerAttributeSet::MiningSpeed`; the drone has no ASC), [[sys-grid]] (block queries, damage, shake, face center).
- implements: [[drone]].
- gotchas:
  - The drone trails its target point slightly (interpolation toward a moving target); raise `FollowInterpSpeed` or set 0 for an exact circle.
  - Orbit angle 0 is world +X; it does not follow the miner's facing.
  - Damage starts on the first Dig tick, while the drone is still flying to the hover point; the laser follows the drone.
  - Block destruction is detected by polling `IsBlockCell` each Dig tick (covers explosions too); `OnCellDestroyed` is not bound.
  - The laser is red ([[drone]]); its material is a Blueprint asset. Readability against the shooter's red targeting laser is still to check.
  - Can the drone be hit: open ([[drone]]); collision is off for now.

## Relations
- [[systems-index]] — parent index.
- [[drone]] — gameplay spec the actor implements.
- [[sys-player]] — the miner that spawns, owns, destroys the drone and triggers digs.
- [[sys-gas-core]] — where `MiningSpeed` lives.
- [[sys-grid]] — the terrain the drone damages and shakes.
- [[sys-automation-tests]] — automation tests of the dig and its cancel rule.
- [[sys-objectives]] — the console that drives the hack.
