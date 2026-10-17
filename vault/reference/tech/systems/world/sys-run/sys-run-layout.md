---
id: sys-run-layout
description: "Async layout provider of the run: RequestLayout off the game thread, OnLayoutReady, prepared layout kept across the travel, provider slot for the generator."
parent: sys-run
kind: doc
---
# Run — layout provider

- responsibility: prepare the terrain of the next run level while the player is between levels ([[intermission]]), off the game thread, and keep it on the game instance until the next map's grid builds from it.
- main_classes: `IRRLayoutProvider` (`Source/RogueRigs/Run/RRLayoutProvider.h`, plain C++ interface: `BuildLayout(FRRLayoutRequest) -> TSharedRef<FRRLayoutData>`, called on a worker thread, must be thread-safe); `FRRJsonLayoutProvider` (today's provider: reads `<LayoutName>.json`, folder resolved on the game thread at construction); `FRRLayoutRequest` (LevelIndex, LayoutName; the generator adds its inputs here); `FRRLayoutData` and `RRLayout::LoadLayoutFile` (`Grid/RRLayoutData.h`, the parser shared with the grid, [[sys-grid]]).
- entry_points (`URRRunSubsystem`, [[sys-run]]):
  - `RequestLayout(LevelIndex)` (name from `GetLayoutNameForLevel`), `RequestLayoutByName(LevelIndex, Name)`: replace any earlier request; `AdvanceToNextLevel` calls `RequestLayout(new level)` before broadcasting `OnLevelAdvanced`. Level 1 is never requested.
  - `OnLayoutReady(LevelIndex, LayoutName, bSuccess)` (BlueprintAssignable, game thread).
  - `IsLayoutReady(Level)`, `IsLayoutPending(Level)` (BlueprintPure); `GetPreparedLayout(Level)` (C++, `TSharedPtr<const FRRLayoutData>`, null when pending, failed or another level); `WaitForPreparedLayout(Level)` (blocks on an in-flight request and delivers it at once; used by the grid at BeginPlay).
  - `SetLayoutProvider(Provider)` (C++; null restores the JSON provider): the slot of the deterministic C++ generator ([[arena-grid]]).
- threading: `UE::Tasks::Launch` runs `BuildLayout` on the task graph workers; the body posts `AsyncTask(GameThread)` with a request serial; the game thread takes the task result, stores it and broadcasts. A stale serial (new request, `BeginRun`, `ResetRun`) is dropped. `Deinitialize` waits for an in-flight task.
- data: no new data; layout names from `URRRunSettings::LevelLayouts`.
- depends_on: [[sys-grid]] (parser, cell types), [[level-layout-schema]].
- implements: [[intermission]] (next terrain prepared in the background).
- gotchas: the grid uses the prepared layout only in a game world, run in progress, level 2+, and sets its `LayoutName` to the prepared name; otherwise (direct PIE, editor, tests) it reads its own `LayoutName` synchronously as before. A failed layout logs an error and the grid falls back to its own `LayoutName`. `rr.Run.NextLevel` waits for `IsLayoutPending` false on the core ticker before `OpenLevel` (as the future Continue). Measured (task-206-02, -game -nullrhi): arena_l2 240 × 240 ready 110 ms after the request; L_ArenaM1 then built from it. No automation test (owner rule).

## Relations
- [[sys-run]] — parent: the run state that holds the provider.
- [[sys-grid]] — builds from the prepared layout.
- [[intermission]] — why the layout is prepared ahead.
- [[arena-grid]] — the future generator that fills the provider slot.
- [[level-layout-schema]] — the file format the JSON provider reads.
- [[task-206-03-transition-flow]] — waits for OnLayoutReady before the map open.
