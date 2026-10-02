---
id: task-009-05-spawner
description: "ARREnemySpawner: one robot every 2 s, at most 20 alive, on reachable floor cells just outside the camera view."
parent: task-009-chaser
kind: task
status: done
---
# Enemy spawner

- goal: robots keep coming from outside the screen.
- agent: cpp
- depends_on: task-009-04-enemy-chaser
- inputs: [[enemies-index]], [[difficulty]], `sys-flow-field`, `sys-enemies`, [[sys-grid]], [[sys-automation-tests]], [[cpp-standard]].
- expected_output: `ARREnemySpawner` (placed actor): editable enemy class, `SpawnInterval` 2 s, `MaxAlive` 20, `ViewMargin` (cells beyond the view, default 1), `MaxSpawnDistanceCells` (default 6 beyond the view edge), `bEnabled`; each interval picks a random floor cell reachable in the flow field, outside the camera's visible floor rectangle (projected frustum on the floor plane) by at least `ViewMargin`, within `MaxSpawnDistanceCells`; no spawn on a cell occupied by a pawn; tracks alive robots (death or destroy removes them). Console variable `rr.Spawner.Enabled 0/1`. New doc `sys-spawner` (template `system.md`). Tests `RogueRigs.Spawner.*`: with a fixed camera rectangle, every spawn is outside it and reachable; alive count never exceeds `MaxAlive`; a killed robot frees a slot.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 70000
- milestone: M1
- result: ARREnemySpawner (EnemyClass to set in BP, SpawnInterval 2, MaxAlive 20, ViewMargin 1, MaxSpawnDistanceCells 6, SpawnFloorClearance 2, rr.Spawner.Enabled); view rectangle from the player camera manager, else the miner camera component, or an override for tests; cells reachable in the flow field, 1.5 to 6.5 cells outside the view, no pawn on the cell. Tests SpawnsOutsideViewAndReachable, MaxAliveCap, KilledRobotFreesSlot. Build exit 0, 31/31 (orchestrator re-ran). New doc sys-spawner. Full grid scan per attempt (fine at 2 s). About 80k tokens.

## Relations
- [[task-009-chaser]] — parent global task.
