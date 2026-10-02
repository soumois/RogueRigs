---
id: task-009-03-flow-field
description: "Grid BFS flow field toward the miner, a path-provider interface, and a generic enemy movement component that follows any provider."
parent: task-009-chaser
kind: task
status: done
---
# Flow field and generic enemy movement

- goal: every ground robot gets its direction from one shared, swappable path provider.
- agent: cpp
- depends_on: task-009-01-player-damage
- inputs: [[sys-enemy-ai]], [[sys-grid]], [[perf-risks]], [[arena-grid]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `IRRPathProvider` (UInterface): `GetMoveDirection(FVector From) → FVector` (unit, Z 0; zero at goal or unreachable), `IsReachable(FVector)`, `GetPathDistance(FVector)`. `URRFlowFieldSubsystem` (world subsystem) implementing it on the grid: BFS (8-neighbour with no corner cutting past blocks, or 4-neighbour: document the choice) from the miner's cell over floor cells; recomputed when the miner enters a new cell or the grid broadcasts `OnCellDestroyed`, at most once per frame; per-cell distance and direction arrays; direction smoothed toward the next cell center. `URREnemyMovementComponent` (actor component for ACharacter robots): each tick asks the active provider (editable, default the flow-field subsystem) and calls `AddMovementInput`; editable arrival distance; when the robot has a direct clear line to the target within an editable range, steer straight to it. Debug: console variable `rr.FlowField.Debug 1` draws directions. New doc `sys-flow-field` (template `system.md`); [[sys-enemy-ai]] updated (main classes). Tests `RogueRigs.FlowField.*`: following directions from 5 reachable cells reaches the miner's cell; an enclosed pocket reports unreachable; destroying the separating rock makes it reachable; the miner moving one cell updates directions; BFS time for 80 × 80 logged.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 90000
- milestone: M1
- result: IRRPathProvider (direction, reachable, distance, goal, direct path by grid walk); URRFlowFieldSubsystem (8-neighbour BFS without corner cutting, recompute on goal cell change, OnCellDestroyed, MarkDirty, once per frame; 0.22 ms mean, 0.29 ms worst on 80 × 80; rr.FlowField.Debug); URREnemyMovementComponent (provider class, ArrivalDistance 100, DirectChaseRange 400, straight chase on direct path). Grid GetGridSize. 6 FlowField tests (test character reaches the miner from behind rocks in 4.1 s). Build exit 0, 24/24 (orchestrator re-ran). New doc sys-flow-field; sys-enemy-ai, sys-grid, sys-automation-tests, build-toolchain. About 92k tokens.

## Relations
- [[task-009-chaser]] — parent global task.
