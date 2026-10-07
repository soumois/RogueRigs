---
id: sys-flow-field
description: "Flow field as built: IRRPathProvider, URRFlowFieldSubsystem (BFS from the miner, incremental openings, time-sliced goal rebuild, 0.3 ms budget), URREnemyMovementComponent; triggers, debug, costs."
parent: systems-index
kind: doc
---
# Flow field and enemy movement

- responsibility: give every ground robot a direction toward the miner from one shared, swappable path provider ([[sys-enemy-ai]] pathfinding decision).
- main_classes (`Source/RogueRigs/FlowField/`):
  - `IRRPathProvider` (`RRPathProvider.h`, C++-only UInterface): `GetMoveDirection(From)` (unit, Z 0; zero at the goal or unreachable), `IsReachable(From)`, `GetPathDistance(From)` (uu, negative when unreachable), `GetGoalLocation(Out)`, `HasDirectPath(From, To)`. The last two let the movement component steer straight without knowing the backend.
  - `URRFlowFieldSubsystem` (`UTickableWorldSubsystem`, implements the interface): BFS from the goal cell over walkable cells (Floor, Spawn; blocks and void are not), 8-neighbour, a diagonal step only when both orthogonal cells are walkable (no corner cutting past blocks), every step costs 1. Per cell: distance (steps) and next-neighbour index (lowest distance, ties to the neighbour closest to the goal in a straight line). `GetMoveDirection` aims from the actual position at the next cell center, so the heading turns smoothly across a cell. `GetPathDistance` = steps × `CellSize`. `HasDirectPath` samples the segment every quarter cell on the grid (no physics trace). Tie-break after straight-line closeness: neighbour order (+X, -X, +Y, -Y, then diagonals), so incremental and full results are identical.
  - `URREnemyMovementComponent` (actor component for `ACharacter` robots, ticks `TG_PrePhysics`): no input within `ArrivalDistance` (100 uu, provisional; the robot sets it from its range); straight at the goal within `DirectChaseRange` (400 uu, provisional) when `HasDirectPath`, also when the provider direction is zero (goal cell); else the provider direction; `AddMovementInput(Direction, 1)`. Provider: `SetPathProvider`, else `PathProviderClass` (editable, `MustImplement`; default the flow-field subsystem; a world subsystem or an owner component). `SetMovementEnabled`, `SetArrivalDistance`, `SetDirectionFilter` (owner reshapes each direction before input; the shooter never stepping away, task-038-01), `HasArrived`, `GetLastMoveDirection`.
- entry_points: subsystem `SetGrid`, `SetGoalActor` (defaults: `RRWorld::FindArenaGrid`, `RRWorld::FindLivingMiner`, [[sys-world-queries]]), `UpdateField`, `MarkDirty`, `GetCellDistance`, `GetNextCell`, `GetGoalCell`, `IsWalkableCell`, `GatherReachableCells(OutCells, Filter(Cell, Steps), bFloorOnly, ExcludedCells)` (task-020-02: the one placement scan, updates the field once, Y outer / X inner so seeded draws are stable; used by the console and shuttle cells, the spawner band, the stress spawn), `GatherPawnCells` (cells of every pawn, to exclude), `FlushPendingWork` (finish queued work now, no budget), `IsUpdatePending`, `GetLastComputeMilliseconds` (last complete computation; a sliced rebuild sums its slices), `GetComputeCount` (full and goal rebuilds), `GetOpenedCellCount`, `GetLastFrameWorkMilliseconds` / `GetWorkFrameCount` (flow time of the last frame with work; the stress sampler reads them). Console: `rr.FlowField.BudgetMs` (0.3, per-frame budget of the incremental work, <= 0 none; not a cheat), `rr.FlowField.Debug 1` (arrows, goal box, red points on unreachable floor), `rr.FlowField.DebugRadius` (cells, 15).
- updates (task-029-02; cells only open during play, so distances only decrease and a late field stays valid: every distance is a real path, every direction leads to a strictly closer cell):
  - full rebuild, synchronous: first computation, grid or goal actor changed (`SetGrid`, `SetGoalActor`, size), `MarkDirty` (fallback); at most once per frame, in Tick and lazily in every query. Drops queued work.
  - opened cell (`OnCellDestroyed`): queued; Tick marks it walkable, gives it its best neighbour + 1 and pushes it and its 8 neighbours (new edges, unblocked diagonals) on a min-heap; decrease-only relaxation, lowest distance first, directions re-chosen on pop and on equal-distance ties, so the result equals a full rebuild.
  - goal enters a new cell: BFS into a second buffer, sliced, swapped in when done; cells opened meanwhile are re-relaxed on the new field. Until then queries answer from the previous goal cell; a goal move during a rebuild starts the next one.
  - budget: one Tick pass per frame, openings first, then the goal rebuild; the clock is read every 32 steps and each part always advances, so constant digging cannot starve the goal. Queries never wait.
- consumers: `GatherReachableCells` (console, shuttle, spawner, stress) reads the last valid field: a cell opened a frame ago may be missing, Steps may be slightly long or measured from the previous goal cell; never an unreachable cell.
- data: grid layout through [[sys-grid]] (`GetGridSize`, `GetCellType`).
- depends_on: [[sys-grid]], [[sys-player]] (miner as default goal).
- implements: [[enemy-chaser]] movement ([[sys-enemies]]), later [[enemy-kamikaze]] and [[enemy-heavy]].
- gotchas: measured BFS on `arena_m1` 80 × 80: mean 0.22 ms, worst 0.29 ms (20 runs, editor headless). At 67 uu (240 × 240): full rebuild 1.8–2.6 ms; walking miner (600 uu/s) + dig, budget 0.3: per-frame work avg 0.27 ms, p95 0.31 ms, field goal at most 13 frames old (`BudgetAt67uu`); game runs in [[stress-grid-cells]]. Paths pass through cell centers and ignore capsule radius; CharacterMovement slides along block corners. A miner over void (falling) seeds the BFS from a non-walkable cell. Tests: [[sys-automation-tests]].

## Relations
- [[systems-index]] — parent index.
- [[sys-enemy-ai]] — the pathfinding decision this implements.
- [[sys-grid]] — the terrain the field is built on.
- [[sys-automation-tests]] — `RogueRigs.FlowField.*` tests.
- [[stress-grid-cells]] — 67 uu dig, chain and walk measurements of the incremental field.
