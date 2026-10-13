---
id: task-121-barrel-explosion-spread
description: "Global task: Block destruction of a barrel explosion (radius 700 uu) is spread over several frames, about 2 ms of game-thread work per frame."
parent: tasks-done-index
kind: task
status: done
---
# Spread barrel explosion

- request: Etalement de l'explosion du baril sur plusieurs frames (8 ms sur le game thread) (owner, 2026-10-13)
- goal: The destruction of the blocks of a barrel explosion (radius 700 uu) is spread over several frames, about 2 ms of game-thread work per frame.
- acceptance: build exits 0 with the editor closed; RogueRigs.Explosions.*, Barrel.*, BarrelFuse.* pass headless (expectations updated only where the spread changes timing); one test shows the work is split over frames; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. Block destruction queued, nearest first, budget CellDestructionBudgetMs 2 ms per frame; barrels fuse at once; 16 tests pass incl Ore.Placement and FineConsoleShuttlePlacement; commits 4fdc0b0 ac20c4e

## Subtasks
1. task-121-01-barrel-explosion-spread — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-121-01-barrel-explosion-spread]] — step 1, cpp.
