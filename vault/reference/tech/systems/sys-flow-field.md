---
id: sys-flow-field
description: "Flow field as built: IRRPathProvider, URRFlowFieldSubsystem (grid BFS from the miner, 8-neighbour), URREnemyMovementComponent following any provider; triggers, debug, BFS cost."
parent: systems-index
kind: doc
---
# Flow field and enemy movement

- responsibility: give every ground robot a direction toward the miner from one shared, swappable path provider ([[sys-enemy-ai]] pathfinding decision).
- main_classes (`Source/RogueRigs/FlowField/`):
  - `IRRPathProvider` (`RRPathProvider.h`, C++-only UInterface): `GetMoveDirection(From)` (unit, Z 0; zero at the goal or unreachable), `IsReachable(From)`, `GetPathDistance(From)` (uu, negative when unreachable), `GetGoalLocation(Out)`, `HasDirectPath(From, To)`. The last two let the movement component steer straight without knowing the backend.
  - `URRFlowFieldSubsystem` (`UTickableWorldSubsystem`, implements the interface): BFS from the goal cell over walkable cells (Floor, Spawn; blocks and void are not), 8-neighbour, a diagonal step only when both orthogonal cells are walkable (no corner cutting past blocks), every step costs 1. Per cell: distance (steps) and next-neighbour index (lowest distance, ties to the neighbour closest to the goal in a straight line). `GetMoveDirection` aims from the actual position at the next cell center, so the heading turns smoothly across a cell. `GetPathDistance` = steps × `CellSize`. `HasDirectPath` samples the segment every quarter cell on the grid (no physics trace).
  - `URREnemyMovementComponent` (actor component for `ACharacter` robots, ticks `TG_PrePhysics`): no input within `ArrivalDistance` (100 uu, provisional; the robot sets it from its range); straight at the goal within `DirectChaseRange` (400 uu, provisional) when `HasDirectPath`, also when the provider direction is zero (goal cell); else the provider direction; `AddMovementInput(Direction, 1)`. Provider: `SetPathProvider`, else `PathProviderClass` (editable, `MustImplement`; default the flow-field subsystem; a world subsystem or an owner component). `SetMovementEnabled`, `HasArrived`, `GetLastMoveDirection`.
- entry_points: subsystem `SetGrid`, `SetGoalActor` (defaults: first `ARRArenaGrid`, first living `ARRMiner`), `UpdateField`, `MarkDirty`, `GetCellDistance`, `GetNextCell`, `GetGoalCell`, `IsWalkableCell`, `GetLastComputeMilliseconds`, `GetComputeCount`. Console: `rr.FlowField.Debug 1` (arrows, goal box, red points on unreachable floor), `rr.FlowField.DebugRadius` (cells, 15).
- recompute triggers: the goal enters a new cell, the grid broadcasts `OnCellDestroyed`, the grid or goal changes, `MarkDirty`; at most once per frame (`GFrameCounter`), checked in Tick and lazily in every query.
- data: grid layout through [[sys-grid]] (`GetGridSize`, `GetCellType`).
- depends_on: [[sys-grid]], [[sys-player]] (miner as default goal).
- implements: [[enemy-chaser]] movement (next task), later [[enemy-kamikaze]] and [[enemy-heavy]].
- gotchas: measured BFS on `arena_m1` 80 × 80: mean 0.22 ms, worst 0.29 ms (20 runs, editor headless). Paths pass through cell centers and ignore capsule radius; CharacterMovement slides along block corners. A miner over void (falling) seeds the BFS from a non-walkable cell. Tests: [[sys-automation-tests]].

## Relations
- [[systems-index]] — parent index.
- [[sys-enemy-ai]] — the pathfinding decision this implements.
- [[sys-grid]] — the terrain the field is built on.
- [[sys-automation-tests]] — `RogueRigs.FlowField.*` tests.
