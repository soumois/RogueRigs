---
id: sys-layout-loader
description: "Asynchronous layout loader as built: worker parse, async grid asset load, grid built over frames, 0-100 LoadPercent and completion event for a loading screen."
parent: sys-grid
kind: doc
---
# Layout loader (asynchronous)

- responsibility: builds a level from a layout JSON without a blocking load ([[task-493-async-layout-loader]], commit b360ef09). Only the test map uses it (owner Q4, 2026-11-07); the game maps keep the grid's own synchronous path ([[sys-grid]] lifecycle).
- main_classes: `URRLayoutLoaderSubsystem` (UTickableWorldSubsystem, `Source/RogueRigs/Grid/RRLayoutLoaderSubsystem.h/.cpp`), `URRLayoutLoaderSettings` (`RRLayoutLoaderSettings.h`, config object like `URRRunSettings`), `ERRLayoutLoadPhase` (Idle, Loading, Building, Done, Failed), `FRRLayoutLoadStats`. Grid side in `ARRArenaGrid`: `SetExternalBuild`, `BeginIncrementalBuild`, `StepIncrementalBuild`, `GetIncrementalBuildProgress`.
- entry_points:
  - `StartLoad(LayoutPath, GridTransform)` (BlueprintCallable): path absolute, relative to the project folder, or a bare name in `Content/RogueRigs/Data/Levels/`. Refused while a load runs.
  - `LoadPercent` (BlueprintReadOnly) and `GetLoadPercent()` (BlueprintPure), `IsLoading()`, `GetPhase()`, `GetLoadedGrid()`.
  - `OnLoadCompleted(Grid, bSuccess)` (BlueprintAssignable): once per load, at 100 with the grid, or on failure with a null grid.
  - Debug: `rr.Level.LoadLayout <path> [keep] [stay]` (`Debug/RRLayoutLoaderDebugCommands.cpp`): loads at the map grid's transform, hides that grid at the end unless `keep`, moves the player to the new spawn cell unless `stay`.
- phases: 1. in parallel, the file read and parse on a worker task (`RRLayout::LoadLayoutFile`) and a streamable request for the grid Blueprint plus its hard package dependencies from the asset registry (meshes, materials, textures, pickup classes; 139 assets for BP_ArenaGrid; the class alone without registry data). 2. deferred spawn with `SetExternalBuild(true)` (OnConstruction and BeginPlay skip `LayoutName`), then `StepIncrementalBuild` each frame: units = layout and deposits, preparation, one per chunk for cells, corner preparation, one per chunk for rock pieces, piece log with colours and pulse, deposit crystals, barrel shadows (2 x chunks + 6, 456 on 240 x 240). `BuildInstances` runs the same units in one call, so both paths give the same grid.
- percentage: weighted sum of parse (0 or 1), assets (loaded / requested) and build (units done / total); never decreases; held below 100 until the grid is complete, then exactly 100 and the event fires. A loading screen binds `OnLoadCompleted` and reads `LoadPercent` each frame (no screen changed here, [[intermission]]).
- data: `URRLayoutLoaderSettings` (section `[/Script/RogueRigs.RRLayoutLoaderSettings]` of DefaultGame.ini, no entry yet): `GridClass` BP_ArenaGrid, `BuildBudgetMs` 5, `ParseWeight` 0.05, `AssetWeight` 0.35, `BuildWeight` 0.6, `bLogEachFrame` true. Values provisional, task-493-01.
- logs: one `RRLayoutLoader:` line per frame (percent, phase, frame ms, loader ms); at the end time, frames, longest frame and loader step, assets, and cell counts file/grid with `match yes|NO` (a cell turned into ore by an ore table deposit counts as its file rock).
- measures (task-493-01, L_ArenaM1 -game, cpp_1000 240 x 240): 1.57 s over 107 frames, build over 92 frames, longest loader step 9.63 ms, longest frame 26.65 ms; assets already in memory there.
- depends_on: [[sys-grid]], [[level-layout-schema]].
- implements: [[arena-grid]] (layout read at runtime).
- tests: `RogueRigs.LayoutLoader.FineLayoutMatchesSyncBuild` (same cells, instance counts, index maps and corner pieces as the synchronous build), `.GeneratedLayout` (a [[sys-layout-generator]] layout, seed 1000), `.MissingFileFails`.
- gotchas: one unit may exceed the budget (deposit placement or a dense chunk, up to about 10 ms). The OreTypes.csv table is still read on the game thread at the first unit (small file). Assets the map already holds load at once.

## Relations
- [[sys-grid]] — parent; the grid it spawns and builds.
- [[sys-layout-generator]] — writes the layouts it loads.
- [[level-layout-schema]] — the file format it reads.
- [[intermission]] — the loading screen that will read its percentage.
- [[task-493-async-layout-loader]] — the task that built it.
