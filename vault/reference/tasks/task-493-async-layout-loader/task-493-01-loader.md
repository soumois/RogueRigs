---
id: task-493-01-loader
description: "Cpp: Asynchronous loader that loads the layout assets and builds the grid over frames, with a 0-100 percentage and a completion event."
parent: task-493-async-layout-loader
kind: task
status: done
---
# Asynchronous level loader from a layout JSON with a load percentage: cpp

- goal: Asynchronous loader that loads the layout assets and builds the grid over frames, with a 0-100 percentage and a completion event.
- agent: cpp
- depends_on: none
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md)
- expected_output: see done_when
- done_when: A1 to A5 of the global hold on arena_m1_fine and one task-492 layout; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-493
- usage: included in the executor run of task-493
- runs: 1
- milestone: M2
- started: 2026-11-07T06:47:27Z
- waiting_for: none
- result: b360ef09. URRLayoutLoaderSubsystem (Grid/RRLayoutLoaderSubsystem.h/.cpp, settings URRLayoutLoaderSettings): parse on a worker task + async load of BP_ArenaGrid and its 139 hard-dependency assets (asset registry), then deferred spawn (SetExternalBuild) and ARRArenaGrid incremental build in 2 x chunks + 6 units (456 on 240x240) under BuildBudgetMs 5 per frame; LoadPercent 0-100 (weights parse 0.05, assets 0.35, build 0.6, provisional), monotonic, 100 only at the end, OnLoadCompleted(Grid, bSuccess). BuildInstances refactored into the same units (one call). A1: arena_m1_fine and task-492 cpp_1000 read back = file counts; FineLayoutMatchesSyncBuild: same cells, instance counts, index maps, corner pieces as the sync path. A2: per-frame log percent; tests check monotonic and event at 100. A3: -game L_ArenaM1 cpp_1000 load 1.57 s over 107 frames, build over 92 frames, longest loader step 9.63 ms, longest frame 26.65 ms (Saved/Captures/task-493-loader/run.log). A4: map grid still logs built synchronously. A5: build ok; LayoutLoader 3/3, Dig, Ore, Barrel, BarrelFuse 30/30. Frame check: same camera, map grid vs loader grid identical (Saved/Captures/task-493-loader-same). Debug: rr.Level.LoadLayout <path> [keep] [stay]. ALERT executor over the standard budget (78 turns, 1.94 M units at closing): cause, 6 extra -game capture runs and 3 rebuilds to explain a floor look difference between frames, which came from the debug command disabling the map grid collision at load start (the player fell); fixed by hiding the map grid at load end.

## Relations
- [[task-493-async-layout-loader]] — parent global task.
