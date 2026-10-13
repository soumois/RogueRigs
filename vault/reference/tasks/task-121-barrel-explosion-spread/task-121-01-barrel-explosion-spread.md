---
id: task-121-01-barrel-explosion-spread
description: "The destruction of the blocks of a barrel explosion (radius 700 uu) is spread over several frames, about 2 ms of game-thread work per frame."
parent: task-121-barrel-explosion-spread
kind: task
status: done
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
- model: opus
- tokens: input 84, cache write 157207, cache read 4541378, output 37763
- runs: 1
- milestone: M2
- result: Block destruction queued, nearest first, budget CellDestructionBudgetMs 2 ms per frame; barrels fuse at once; 16 tests pass incl Ore.Placement and FineConsoleShuttlePlacement; commits 4fdc0b0 ac20c4e

## Relations
- [[task-121-barrel-explosion-spread]] — parent global task.
