---
id: sys-run-transition
description: "Level-to-level transition: intermission after an extraction, Continue, map travel, loading hold until grid and pool prewarm are ready; widget binding API."
parent: sys-run
kind: doc
---
# Run — level transition

- responsibility: chain the levels of a run ([[intermission]], [[run-structure]]): intermission on a level 1-3 extraction, Continue, map travel, gameplay held behind the loading screen until the level is ready, run won on level 4, run reset on restart.
- main_classes: `URRRunTransitionSubsystem` (`Source/RogueRigs/Run/RRRunTransitionSubsystem.h`, `UGameInstanceSubsystem`); `FRRIntermissionRecap`, `ERRRunTransitionPhase` (None, Intermission, Loading), `ERRRunLoadingStage` (None, WaitingForLayout, OpeningMap, PreparingLevel); widget bases `URRIntermissionWidget`, `URRLoadingScreenWidget` (`UI/`, Abstract); widgets created by `ARRPlayerController` ([[sys-hud-bindings]]).
- entry_points (sequence):
  1. `URRRunFlowSubsystem::EndRun(true)` (ore secured, game paused, Game and UI input) calls `HandleLevelExtracted(Miner)`. Run in progress, not the last level: `CaptureLevelEndSnapshot`, `AdvanceToNextLevel` (next layout requested at once, [[sys-run-layout]]), phase Intermission, `OnIntermissionStarted(Recap)`; `OnRunEnded` is not broadcast (no success screen). Last level: `IsRunWon` true, `OnRunEnded(true)` as before.
  2. `Continue()` (BlueprintCallable): phase Loading, `OnLoadingStarted(LevelIndex, LayoutName)`; layout pending: stage WaitingForLayout until `OnLayoutReady`; then stage OpeningMap, `OpenLevel(URRRunSettings::ArenaMapName, empty = current map)`; a failed layout logs an error and travels (grid fallback, provisional).
  3. `PostLoadMapWithWorld` (after every BeginPlay: grid built from the prepared layout, snapshot re-applied; before the first tick): stage PreparingLevel, logs the grid layout and size; run flow `SetRunFlowEnabled(false)`, spawner `SetSpawningHeld(true)`, controller ignores move, aim and turret input (`IsGameplayInputHeld`).
  4. Spawner `OnPoolPrewarmFinished` ([[sys-spawner-pool-prewarm]]; at once without spawner or prewarm; `LoadingHoldTimeoutSeconds` 10 safety, provisional): released, phase None, `OnLevelReady(LevelIndex, LayoutName)`; the alert bar starts.
- Binding API for the bp widgets (task-206-04, 206-05): controller class properties `IntermissionWidgetClass`, `LoadingScreenWidgetClass` (category HUD|Run Transition, Z-order `TransitionWidgetZOrder` 50 and 51). `URRIntermissionWidget`: implement `OnRecapShown(FRRIntermissionRecap)` {FinishedLevelIndex, NextLevelIndex, LevelCount, OreSecuredThisLevel, RunSecuredOre, NextLayoutName}; button calls `Continue()`; pure `GetRecap()`. `URRLoadingScreenWidget`: implement `OnLoadingShown(LevelIndex, LayoutName)` and `OnLoadingStageChanged(Stage)`; pure `GetLoadingStage()`. Subsystem (BlueprintAssignable): `OnIntermissionStarted`, `OnLoadingStarted`, `OnLoadingStageChanged`, `OnLevelReady`; pure `GetPhase`, `IsInIntermission`, `IsLevelLoading`, `GetLoadingStage`, `GetIntermissionRecap`, `GetLoadingLevelIndex`, `IsRunWon`.
- Loading across the travel: no MoviePlayer. The old world's controller shows the loading widget on `OnLoadingStarted` (OpenLevel only requests the travel, so it is drawn before the blocking load); the new world's controller shows a new one at BeginPlay while `IsLevelLoading`; both removed on `OnLevelReady`.
- Restart: `URRRunFlowSubsystem::RestartLevel` (end screens) and `ARRGameMode::RestartLevel` (death) call `ResetRun` first: next map begins level 1.
- data: `URRRunSettings::ArenaMapName` (empty), `LoadingHoldTimeoutSeconds` (10).
- depends_on: [[sys-run]], [[sys-run-layout]], [[sys-objectives-run-flow]], [[sys-spawner-pool-prewarm]], [[sys-hud-bindings]].
- implements: [[intermission]], [[run-structure]].
- gotchas: without `IntermissionWidgetClass` the intermission logs a warning and waits for `rr.Run.Continue`. Restart reopens the current map, which is the level 1 map only while all levels share L_ArenaM1. Direct PIE or -game: no transition, no hold. Debug: `rr.Run.Continue`, `rr.Run.NextLevel` (EndRun(true) then Continue), `rr.Run.Restart`, `rr.Run.Status` (phase, run won). Verified by a -game -nullrhi log (task-206-03, no widget classes set yet): levels 1 to 3 extracted into the intermission, arena_l2/l3/l4 ready about 110 ms after the request, map loaded 2.3 to 2.7 s after Continue, hold released 445 ms later on the prewarm (61 spawner ticks), level 4 extraction = run won, restart and deaths reset to level 1. Widgets not seen on screen. No automation test (owner rule).

## Relations
- [[sys-run]] — parent: the run state the transition drives.
- [[sys-run-layout]] — the layout Continue waits for.
- [[sys-objectives-run-flow]] — EndRun routes here; held during the loading.
- [[sys-spawner-pool-prewarm]] — the completion signal the hold waits for.
- [[sys-hud-bindings]] — the controller that creates the two widgets.
- [[intermission]] — the screen this implements.
- [[task-206-03-transition-flow]] — the task that built it.
