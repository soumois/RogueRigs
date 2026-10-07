---
id: sys-drone
description: "Drone C++ actor: orbit, contact arc dig (up to 4 blocks, whole ore veins, per-block events), hack, laser fan, MiningSpeed from the miner ASC."
parent: systems-index
kind: doc
---
# Drone

- responsibility: the helper drone actor and its state behaviours: Orbit, Dig (arc mining, task-029-05), Hack. No dialogue yet.
- main_classes (in `Source/RogueRigs/Drone/`):
  - `ARRDrone : AActor` (`RRDrone.h`, `Abstract`, subclassed in Blueprint): root `UStaticMeshComponent` `Mesh` with profile `NoCollision` (no overlap events, does not affect navigation); child `UStaticMeshComponent` `LaserMesh` (absolute transform, `NoCollision`, no shadow, hidden outside Dig and Hack). Both meshes, and the laser's red material, are set in Blueprint; `LaserMesh` expects a cylinder along local Z with a centered pivot (the engine cylinder).
  - Laser fan (task-029-08): `LaserBeams` (transient pool, `VisibleInstanceOnly`), beam 0 = `LaserMesh`; `BeginPlay` fills it to `MaxArcBlocks` with runtime copies of `LaserMesh` (same mesh and materials, same world-space, no-collision, no-shadow setup), and it grows if an arc is larger. No Blueprint property to set.
  - Editable, orbit: `OrbitRadius` (150 uu), `OrbitPeriod` (2 s), `HoverHeight` (150 uu, also the dig and hack height), `FollowInterpSpeed` (10, provisional; 0 = snap; also used in Dig and Hack).
  - Editable, dig ([[drone]], provisional): `ArcReachCells` (1.5), `ArcAngleDegrees` (120, full cone), `MaxArcBlocks` (4). `DigHoverDistance` and `DigCancelDistanceCells` were removed in task-029-05.
  - Editable, hack: `HackHoverDistance` (100 uu, provisional), `HackLinkOffset` ((0, 0, 50), provisional).
  - Editable, laser: `LaserDiameter` (8 uu), `LaserMeshBaseHeight` (100 uu), `LaserMeshBaseDiameter` (100 uu); the base sizes are the mesh's size at scale 1.
  - `ERRDroneState` (`uint8`, BlueprintType): `Orbit`, `Dig`, `Hack`.
- entry_points:
  - Spawned by `ARRMiner::BeginPlay` from the miner's `DroneClass`, destroyed in `ARRMiner::EndPlay` ([[sys-player]]). `SetOwnerMiner(ARRMiner*)`: stops any dig or hack, sets the followed miner (tick prerequisite), orbit angle 0, snaps to the orbit point.
  - Getters: `GetOwnerMiner()`, `GetState()`, `GetMiningSpeed()` (miner ASC `MiningSpeed`; 1 if missing), `GetDigCells()` (dug cells, nearest first), `GetDigGrid()`, `GetDigArcDirection()`, `GetDigTargetPoint(Cell)` (center of the cell's face toward the miner, dominant axis, half block height; for per-block lasers), `GetHackTarget()`.
  - `SetState`: `Orbit` always accepted (stops dig or hack); `Dig` only with a miner and a block in the arc; `Hack` refused (use `StartHack`).
  - `RequestArcDig(Grid, ArcDirection)`: called every tick by the miner while it touches rock. Refused while hacking, without miner or grid. Selects the arc with `SelectArcCells` from the miner location; empty arc or zero direction = `ReleaseDig`, false. Cells leaving the arc: shake off, `OnDigBlockStopped(Cell, false)`, damage kept. Cells entering: `SetCellShaking(Cell, true)`, `OnDigBlockStarted(Cell)`. Enters Dig.
  - `SelectArcCells(Grid, MinerLocation, ArcDirection, OutCells)` (C++ public, const): block cells whose center is within `ArcReachCells` × cell size of the front point (miner location + direction × capsule radius) and within `ArcAngleDegrees` / 2 of the direction seen from the miner location; sorted nearest to the front point, then smaller angle, then Y, X; at most `MaxArcBlocks`. 2D, unrotated grid. Against a flat wall at 67 uu with a 34 uu capsule: the 3 wall blocks within 60°, plus the block behind the nearest, whose center sits exactly 1.5 cells from a touching front point; it is in only with no gap (character movement keeps about 0.1 uu, so 3 blocks in play; 4 when the miner stands at a cell center, 0.5 uu into the wall, as in the tests). Corners and diagonal pushes can give 4.
  - `ReleaseDig()`: leaves Dig (no effect otherwise).
  - `StartHack(Target)` / `StopHack(Target)`: driven by the objective every tick ([[sys-objectives]]); one target at a time; a dig in progress is stopped.
  - Dig tick: Orbit if miner or grid gone; cells no longer blocks (explosions) leave the arc (`OnDigBlockStopped(Cell, true)`); move toward the arc centre (mean of dug cell centers; Z = miner capsule bottom + `HoverHeight`); beam i from the drone to `GetDigTargetPoint(DigCells[i])`, unused beams hidden; then the targets are built: each dug cell expanded to its vein with `GetVeinCells` ([[sys-grid]]; Ore and HealOre only, task-034-01), duplicates removed, and each target takes `ApplyTerrainDamage(Cell, DeltaSeconds × GetMiningSpeed(), Drone)` once per tick, however many of its vein blocks are in the arc; a dug cell no longer a block leaves the arc (`OnDigBlockStopped(Cell, true)`). Dig holds with an empty arc until the miner's next request or release.
  - Orbit tick: angle += 2π / `OrbitPeriod` per second; orbit point = miner + `OrbitRadius` × (cos, sin), Z = capsule bottom + `HoverHeight`; `VInterpTo` at `FollowInterpSpeed`. Hack tick: hover `HackHoverDistance` from the target toward the miner, laser to target + `HackLinkOffset`.
  - Leaving Dig (release, empty arc, `SetState(Orbit)`, hack, `SetOwnerMiner`, `EndPlay`): every dug cell stops shaking with `OnDigBlockStopped`, every beam hidden, arc cleared, orbit resumes from the drone's current angle. Damage stays on the blocks ([[sys-grid]]).
- events (BlueprintAssignable, for the laser and dig effects): `OnDigBlockStarted(FIntPoint Cell)`, `OnDigBlockStopped(FIntPoint Cell, bool bDestroyed)`. The render shake is done by the grid in C++.
- data: no table; tunables are `EditDefaultsOnly` defaults on the Blueprint subclass. Values from [[drone]].
- depends_on: [[sys-player]] (miner, capsule, dig trigger), [[sys-gas-core]] (`URRPlayerAttributeSet::MiningSpeed`; the drone has no ASC), [[sys-grid]] (block queries, damage, shake, face center).
- implements: [[drone]].
- gotchas:
  - Damage starts on the first Dig tick, while the drone still flies to the arc centre.
  - Each block of the arc is cut at the full rate (4 rock blocks in 0.5 s), as the GDD's cleared area per second requires.
  - Whole-vein mining (task-034-01, [[ore-blocks]]): touching one block of a 3 × 3 ore or heal vein damages all 9 blocks at the full rate, so the vein breaks whole after one block dig time (1.5 s) and drops 9 chunks; vein blocks outside the arc get no laser and no shake.
  - Arc direction is world-space; the miner's facing does not steer it.
  - Tick order: the miner requests, then the drone ticks (prerequisite), so a request is served the same frame.
  - The laser is red ([[drone]]); readability against the shooter's red targeting laser is still to check. Can the drone be hit: open ([[drone]]); collision is off.

## Relations
- [[systems-index]] — parent index.
- [[drone]] — gameplay spec the actor implements.
- [[sys-player]] — the miner that spawns, owns, destroys the drone and triggers digs.
- [[sys-gas-core]] — where `MiningSpeed` lives.
- [[sys-grid]] — the terrain the drone damages and shakes.
- [[sys-automation-tests]] — automation tests of the arc dig, its limits, speeds and release.
- [[sys-objectives]] — the console that drives the hack.
