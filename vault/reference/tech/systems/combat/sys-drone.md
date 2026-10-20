---
id: sys-drone
description: "Drone C++ actor: escort (Orbit) with height band, look and tilt, arc dig (up to 8 blocks), whole deposits, per-block events, hack, single beam, MiningSpeed."
parent: systems-combat-index
kind: doc
---
# Drone

- responsibility: the helper drone actor and its state behaviours: Orbit (escort movement since task-247-01), Dig (arc mining, task-029-05), Hack. No dialogue yet.
- main_classes (in `Source/RogueRigs/Drone/`):
  - `ARRDrone : AActor` (`RRDrone.h`, `Abstract`, subclassed in Blueprint): root `UStaticMeshComponent` `Mesh` with profile `NoCollision` (no overlap events, does not affect navigation); child `UStaticMeshComponent` `LaserMesh` (absolute transform, `NoCollision`, no shadow, hidden outside Dig and Hack). Both meshes, and the laser's red material, are set in Blueprint; `LaserMesh` expects a cylinder along local Z with a centered pivot (the engine cylinder). Child `UNiagaraComponent` `DigImpactComponent` (task-233-02, absolute transform, inactive outside Dig) plays `DigImpactSystem` (NS_DigImpact) at the beam end; editable `DigImpactFadeTime` (0.15 s) and chip colours `RockChipColor`, `OreChipColor`, `HealOreChipColor`, `BarrelChipColor` ([[sys-drone-dig]]).
  - Single dig beam (task-232-01, owner 2026-10-19; replaces the task-029-08 laser fan): Dig draws only `LaserMesh`, from the drone to `GetDigImpactPoint()` ([[sys-drone-dig]]). The `LaserBeams` pool, `EnsureLaserBeams`, `HideLaserBeams` and the `BeginPlay` override are removed (the pool was transient and no Blueprint graph read it).
  - Editable, Orbit = escort (task-247-01, owner 2026-10-20, all provisional; `OrbitRadius`, `OrbitPeriod` and `OrbitAngle` removed, BP_Drone kept their defaults): `EscortDistance` (125 uu), `EscortRestAngleDegrees` (100, the shoulder) and `EscortMoveAngleDegrees` (145, behind and to the side) from the heading, `bEscortOnRightSide` (true), `EscortHeadingTurnRate` (360 deg/s), `EscortHeadingMinSpeed` (50 uu/s), `EscortBlendSpeed` (4), `EscortFigureEightPeriod` (6 s), `EscortFigureEightTangential` (30 uu), `EscortFigureEightRadial` (10 uu), `EscortSpringFrequency` (1.5 Hz), `EscortDampingRatio` (0.75), `EscortVelocityFeedForward` (0.8), `EscortSnapDistance` (2000 uu), `HoverHeight` (150 uu, the hack height only since task-249-01), `EscortBobAmplitude` (9 uu since task-249-01, was 6), `EscortBobPeriod` (2.4 s), `FollowInterpSpeed` (10; 0 = snap; Dig and Hack only). Behaviour in [[sys-drone-dig]] (Orbit tick).
  - Editable, walls (task-248-01, owner 2026-10-20, all provisional, category `Drone|Walls`): `EscortAlternateAngleDegrees` (45), `EscortWideAngleDegrees` (90), `EscortSlotChangeDelay` (0.3 s), `EscortFallbackExtraHeight` (40 uu), `DroneWallClearance` (20 uu); dig hover `DigHoverFaceDistance` (90 uu) and `DigHoverHeight` (130 uu). BP_Drone overrides none. Behaviour in [[sys-drone-dig]].
  - Editable, alive (task-249-01, owner 2026-10-20, all provisional, category `Drone|Alive`, visual only): height `EscortCalmHeight` (158 uu), `EscortAlertHeight` (174 uu), band `EscortMinHeight` (130) / `EscortMaxHeight` (190), second bob `EscortBobSecondAmplitude` (4 uu) / `EscortBobSecondPeriod` (0.9 s), `EscortAlertRadius` (700 uu), `EscortAlertBlendSpeed` (1.5); look `InterestRefreshInterval` (0.25 s = 4 Hz), `InterestEnemyRadius` (900 uu), `InterestSiteRadius` (500 uu), `LookYawInterpSpeed` (6); tilt `TiltDegreesPerAcceleration` (0.015 deg per uu/s², 15° at the miner's 1000 uu/s²), `MaxTiltDegrees` (15), `TiltInterpSpeed` (6), `AccelerationSmoothingSpeed` (10); gestures `IdleGestureMinInterval` (4 s) / `IdleGestureMaxInterval` (8 s), `ScanSweepDegrees` (35) / `ScanSweepDuration` (1.6 s), `NodPitchDegrees` (12) / `NodDuration` (1.2 s), `DipDepth` (12 uu) / `DipDuration` (0.9 s). BP_Drone overrides none. Behaviour in [[sys-drone-dig]] (Alive).
  - Editable, dig ([[drone]], provisional): `ArcReachCells` (2.5), `ArcAngleDegrees` (120, full cone), `MaxArcBlocks` (8); raised from 1.5 and 4 in task-060-01 (the GDD depth rule, about two rows). BP_Drone overrides neither. `DigHoverDistance` and `DigCancelDistanceCells` were removed in task-029-05.
  - Editable, hack: `HackHoverDistance` (100 uu, provisional), `HackLinkOffset` ((0, 0, 50), provisional).
  - Editable, laser: `LaserDiameter` (8 uu), `LaserMeshBaseHeight` (100 uu), `LaserMeshBaseDiameter` (100 uu); the base sizes are the mesh's size at scale 1.
  - `ERRDroneState` (`uint8`, BlueprintType): `Orbit`, `Dig`, `Hack`.
- entry_points:
  - Spawned by `ARRMiner::BeginPlay` from the miner's `DroneClass`, destroyed in `ARRMiner::EndPlay` ([[sys-player]]). `SetOwnerMiner(ARRMiner*)`: stops any dig or hack, sets the followed miner (tick prerequisite), escort heading = the miner's yaw, blend, phases and spring velocity 0, snaps to the rest escort slot.
  - Getters: `GetOwnerMiner()`, `GetState()`, `GetMiningSpeed()` (miner ASC `MiningSpeed`; 1 if missing), `GetDigCells()` (dug cells, nearest first), `GetDigGrid()`, `GetDigArcDirection()`, `GetDigTargetPoint(Cell)` (center of the cell's face toward the miner, dominant axis, half block height), `GetDigImpactPoint()` (end of the single dig beam: mean of the target points on the nearest wall face; zero outside Dig), `GetHackTarget()`.
  - `SetState`: `Orbit` always accepted (stops dig or hack); `Dig` only with a miner and a block in the arc; `Hack` refused (use `StartHack`).
  - `ReleaseDig()`: leaves Dig (no effect otherwise).
  - `StartHack(Target)` / `StopHack(Target)`: driven by the objective every tick ([[sys-objectives]]); one target at a time; a dig in progress is stopped.
- events (BlueprintAssignable, for dig effects): `OnDigBlockStarted(FIntPoint Cell)`, `OnDigBlockStopped(FIntPoint Cell, bool bDestroyed)`. The render shake is done by the grid in C++.
- data: no table; tunables are `EditDefaultsOnly` defaults on the Blueprint subclass. Values from [[drone]].
- depends_on: [[sys-player]] (miner, capsule, dig trigger), [[sys-gas-core]] (`URRPlayerAttributeSet::MiningSpeed`; the drone has no ASC), [[sys-grid]] (block queries, damage, shake, face center).
- implements: [[drone]].
- gotchas:
  - Damage starts on the first Dig tick, while the drone still flies to its dig hover point.
  - Each block of the arc is cut at the full rate (4 rock blocks in 0.5 s), as the GDD's cleared area per second requires.
  - Whole-deposit mining (task-034-01, generic since task-070-01, [[ore-blocks]]): touching one block of a deposit (a 2 × 2 Duskite deposit from the ore table, a layout heal vein, any size read from data) damages all its blocks at the full rate, so it breaks whole after one block dig time (1.5 s) and drops one chunk per block (Duskite: 4 of value 1, `RogueRigs.Ore.MineDeposit`); all its blocks shake together while dug, all stop on release or break (task-043-01, [[heal-ore]]); deposit blocks outside the arc get no laser and no events. No drone code depends on the size.
  - Arc direction is world-space; the miner's facing does not steer it.
  - Tick order: the miner requests, then the drone ticks (prerequisite), so a request is served the same frame.
  - Escort heading follows the miner's velocity, not its aim (the aim can spin with the mouse); at rest it keeps the last move direction. The drone has no collision; since task-248-01 its slot and hover points are chosen clear of blocks on the grid (line of sight on the grid, not collision traces), and a move into a block cell is cut on the blocked axis (in `RogueRigs.Dig.DroneAvoidsWalls` that guard never had to act). Only the drone's centre cell is checked: its 40 uu sphere can still overlap a wall edge by up to its radius minus `DroneWallClearance`.
  - Alive (task-249-01): the actor rotation is now set every tick (yaw toward the point of interest, pitch and roll from the miner's acceleration); `LaserMesh` and `DigImpactComponent` use absolute transforms, so they ignore it. The enemy lookup iterates `ARREnemy` actors (`TActorIterator`, pooled idle robots skipped by `IsDead`) at 4 Hz only, never per tick ([[sys-world-queries]] caching rule). Gestures draw from the drone's own `FRandomStream` (seeded from its unique id), never from the global FMath stream that `Ore.HealVein` seeds.
  - The laser is red ([[drone]]); readability against the shooter's red targeting laser is still to check. Can the drone be hit: open ([[drone]]); collision is off.

## Relations
- [[sys-drone-dig]] — Drone dig mode: RequestArcDig, SelectArcCells, dig tick, orbit tick and leaving Dig. Open to change arc mining behaviour.
- [[systems-index]] — parent index.
- [[drone]] — gameplay spec the actor implements.
- [[sys-player]] — the miner that spawns, owns, destroys the drone and triggers digs.
- [[sys-gas-core]] — where `MiningSpeed` lives.
- [[sys-grid]] — the terrain the drone damages and shakes.
- [[sys-automation-tests]] — automation tests of the arc dig, its limits, speeds and release.
- [[sys-objectives]] — the console that drives the hack.
