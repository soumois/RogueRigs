---
id: task-009-03-flow-field
description: "Grid BFS flow field toward the miner, a path-provider interface, and a generic enemy movement component that follows any provider."
parent: task-009-chaser
kind: task
status: todo
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
- result: open

## Relations
- [[task-009-chaser]] — parent global task.
