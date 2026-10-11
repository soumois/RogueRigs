---
id: sys-drone
description: "Drone C++ actor: orbit, arc dig (2.5-cell reach, up to 8 blocks), whole deposits mined as one, per-block events, hack, laser fan, MiningSpeed."
parent: systems-index
kind: doc
---
# Drone

- responsibility: the helper drone actor and its state behaviours: Orbit, Dig (arc mining, task-029-05), Hack. No dialogue yet.
- main_classes (in `Source/RogueRigs/Drone/`):
  - `ARRDrone : AActor` (`RRDrone.h`, `Abstract`, subclassed in Blueprint): root `UStaticMeshComponent` `Mesh` with profile `NoCollision` (no overlap events, does not affect navigation); child `UStaticMeshComponent` `LaserMesh` (absolute transform, `NoCollision`, no shadow, hidden outside Dig and Hack). Both meshes, and the laser's red material, are set in Blueprint; `LaserMesh` expects a cylinder along local Z with a centered pivot (the engine cylinder).
  - Laser fan (task-029-08): `LaserBeams` (transient pool, `VisibleInstanceOnly`), beam 0 = `LaserMesh`; `BeginPlay` fills it to `MaxArcBlocks` with runtime copies of `LaserMesh` (same mesh and materials, same world-space, no-collision, no-shadow setup), and it grows if an arc is larger. No Blueprint property to set.
  - Editable, orbit: `OrbitRadius` (150 uu), `OrbitPeriod` (2 s), `HoverHeight` (150 uu, also the dig and hack height), `FollowInterpSpeed` (10, provisional; 0 = snap; also used in Dig and Hack).
  - Editable, dig ([[drone]], provisional): `ArcReachCells` (2.5), `ArcAngleDegrees` (120, full cone), `MaxArcBlocks` (8); raised from 1.5 and 4 in task-060-01 (the GDD depth rule, about two rows). BP_Drone overrides neither. `DigHoverDistance` and `DigCancelDistanceCells` were removed in task-029-05.
  - Editable, hack: `HackHoverDistance` (100 uu, provisional), `HackLinkOffset` ((0, 0, 50), provisional).
  - Editable, laser: `LaserDiameter` (8 uu), `LaserMeshBaseHeight` (100 uu), `LaserMeshBaseDiameter` (100 uu); the base sizes are the mesh's size at scale 1.
  - `ERRDroneState` (`uint8`, BlueprintType): `Orbit`, `Dig`, `Hack`.
- entry_points:
  - Spawned by `ARRMiner::BeginPlay` from the miner's `DroneClass`, destroyed in `ARRMiner::EndPlay` ([[sys-player]]). `SetOwnerMiner(ARRMiner*)`: stops any dig or hack, sets the followed miner (tick prerequisite), orbit angle 0, snaps to the orbit point.
  - Getters: `GetOwnerMiner()`, `GetState()`, `GetMiningSpeed()` (miner ASC `MiningSpeed`; 1 if missing), `GetDigCells()` (dug cells, nearest first), `GetDigGrid()`, `GetDigArcDirection()`, `GetDigTargetPoint(Cell)` (center of the cell's face toward the miner, dominant axis, half block height; for per-block lasers), `GetHackTarget()`.
  - `SetState`: `Orbit` always accepted (stops dig or hack); `Dig` only with a miner and a block in the arc; `Hack` refused (use `StartHack`).
  - `RequestArcDig(Grid, ArcDirection)`: called every tick by the miner while it touches rock. Refused while hacking, without miner or grid. Selects the arc with `SelectArcCells` from the miner location; zero direction, or empty arc with no fallback cell = `ReleaseDig`, false. C++ overload `RequestArcDig(Grid, ArcDirection, TouchingCells)` (used by the miner, task-065-01): when the arc is empty, `SelectContactFallbackCells` digs the touching block whose direction from the miner is closest to the arc direction (ties within 0.01 cosine kept, at most `MaxArcBlocks`), so contact always digs at least one block. Cause it fixes: the miner steers the arc with any move input that has a positive dot with the wall direction (up to 90° off), while the cone is only ±60°; moving diagonally into a corner or along a wall pointed the cone between or past the touching blocks. Cells leaving the arc: `OnDigBlockStopped(Cell, false)`, damage kept. Cells entering: `OnDigBlockStarted(Cell)`. Enters Dig. The shake follows the veins, not the arc (`RefreshDigShake`, `ShakeCells`): every block of the dig targets shakes, the others stop.
  - `SelectArcCells(Grid, MinerLocation, ArcDirection, OutCells)` (C++ public, const): block cells whose center is within `ArcReachCells` × cell size of the front point (miner location + direction × capsule radius) and within `ArcAngleDegrees` / 2 of the direction seen from the miner location; sorted nearest to the front point, then smaller angle, then Y, X; at most `MaxArcBlocks`, so the front row is cut before the second. 2D, unrotated grid. Against a flat wall at 67 uu with a 34 uu capsule: the 3 wall blocks within 60°, plus the block behind the nearest, whose center sits exactly 1.5 cells from a touching front point; it is in only with no gap (character movement keeps about 0.1 uu, so 3 blocks in play; 4 when the miner stands at a cell center, 0.5 uu into the wall, as in the tests). Corners and diagonal pushes can give 4.
  - `ReleaseDig()`: leaves Dig (no effect otherwise).
  - `StartHack(Target)` / `StopHack(Target)`: driven by the objective every tick ([[sys-objectives]]); one target at a time; a dig in progress is stopped.
  - Dig tick: Orbit if miner or grid gone; cells no longer blocks (explosions) leave the arc (`OnDigBlockStopped(Cell, true)`); move toward the arc centre (mean of dug cell centers; Z = miner capsule bottom + `HoverHeight`); beam i from the drone to `GetDigTargetPoint(DigCells[i])`, unused beams hidden; then the targets are built: each dug cell expanded to its deposit with `GetDepositCells` ([[sys-grid]]; by deposit id since task-070-01, so a touching deposit is not pulled in), duplicates removed, and each target takes `ApplyTerrainDamage(Cell, DeltaSeconds × GetMiningSpeed(), Drone)` once per tick, however many of its vein blocks are in the arc; a dug cell no longer a block leaves the arc (`OnDigBlockStopped(Cell, true)`); then the shake is refreshed, so destroyed vein blocks stop shaking. Dig holds with an empty arc until the miner's next request or release.
  - Orbit tick: angle += 2π / `OrbitPeriod` per second; orbit point = miner + `OrbitRadius` × (cos, sin), Z = capsule bottom + `HoverHeight`; `VInterpTo` at `FollowInterpSpeed`. Hack tick: hover `HackHoverDistance` from the target toward the miner, laser to target + `HackLinkOffset`.
  - Leaving Dig (release, empty arc, `SetState(Orbit)`, hack, `SetOwnerMiner`, `EndPlay`): every dug cell stops shaking with `OnDigBlockStopped`, every beam hidden, arc cleared, orbit resumes from the drone's current angle. Damage stays on the blocks ([[sys-grid]]).
- events (BlueprintAssignable, for the laser and dig effects): `OnDigBlockStarted(FIntPoint Cell)`, `OnDigBlockStopped(FIntPoint Cell, bool bDestroyed)`. The render shake is done by the grid in C++.
- data: no table; tunables are `EditDefaultsOnly` defaults on the Blueprint subclass. Values from [[drone]].
- depends_on: [[sys-player]] (miner, capsule, dig trigger), [[sys-gas-core]] (`URRPlayerAttributeSet::MiningSpeed`; the drone has no ASC), [[sys-grid]] (block queries, damage, shake, face center).
- implements: [[drone]].
- gotchas:
  - Damage starts on the first Dig tick, while the drone still flies to the arc centre.
  - Each block of the arc is cut at the full rate (4 rock blocks in 0.5 s), as the GDD's cleared area per second requires.
  - Whole-deposit mining (task-034-01, generic since task-070-01, [[ore-blocks]]): touching one block of a deposit (a 2 × 2 Duskite deposit from the ore table, a layout heal vein, any size read from data) damages all its blocks at the full rate, so it breaks whole after one block dig time (1.5 s) and drops one chunk per block (Duskite: 4 of value 1, `RogueRigs.Ore.MineDeposit`); all its blocks shake together while dug, all stop on release or break (task-043-01, [[heal-ore]]); deposit blocks outside the arc get no laser and no events. No drone code depends on the size.
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
