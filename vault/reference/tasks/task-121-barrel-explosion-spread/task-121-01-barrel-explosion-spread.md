---
id: task-121-01-barrel-explosion-spread
description: "The destruction of the blocks of a barrel explosion (radius 700 uu) is spread over several frames, about 2 ms of game-thread work per frame."
parent: task-121-barrel-explosion-spread
kind: task
status: in-progress
---
# Spread barrel explosion

- goal: The destruction of the blocks of a barrel explosion (radius 700 uu) is spread over several frames, about 2 ms of game-thread work per frame.
- agent: cpp
- depends_on: none
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), sys-grid-barrels (vault/reference/tech/systems/world/sys-grid/sys-grid-barrels.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), perf-stress-grid-pools-hud (vault/reference/tech/performance/perf-stress-grid-cells/perf-stress-grid-pools-hud.md), sys-tests-explosions-barrel (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-explosions-barrel.md)
- expected_output: Block removal and drops of an explosion queued and processed under a per-frame time budget (about 2 ms); damage to the miner and robots and the explosion visual unchanged; sys-grid-barrels and sys-explosions updated.
- done_when: build exits 0 with the editor closed; RogueRigs.Explosions.*, Barrel.*, BarrelFuse.* pass headless (expectations updated only where the spread changes timing); one test shows the work is split over frames; lint clean
- editor_state: editor closed (C++ build)
- token_ceiling: 60 turns, commit after each group
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: explosions call grid `QueueDestroyCellsInRadius`: barrels at once (chain timing kept), other blocks nearest first under `CellDestructionBudgetMs` 2 ms per frame (at least one per frame), dropped on rebuild and `EndPlay`; damage, FX, `OnExplosion` unchanged in the explosion frame. Build 0; Explosions.*, Barrel.*, BarrelFuse.* (BlocksInRadius and Radius700 wait for the queue), new Explosions.BlocksSpreadOverFrames (9 blocks over 9 frames), Ore.Placement, Integration.FineConsoleShuttlePlacement: 16/16 pass headless. No timing measured.

## Relations
- [[task-121-barrel-explosion-spread]] — parent global task.
