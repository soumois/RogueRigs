---
id: sys-objectives-run-flow
description: "URRRunFlowSubsystem and FRRRunFlowConfig: alert bar, objective pause, extraction phases and tunables. Open to change the run flow."
parent: sys-objectives
kind: doc
---
# Objectives — run flow

  - `URRRunFlowSubsystem` (tickable world subsystem, Game/PIE, not ticked while paused): `AlertSeconds` += dt × cheat multiplier; kills add `KillAlertSeconds` (`NotifyEnemyKilled`, called by `ARREnemy::HandleDeath`; a self-instigated death such as the kamikaze fuse is ignored). Nothing grows while the console is pending. At `ConsoleAlertThreshold` the alert is clamped and the console drops on a random Floor cell, straight-line 36–60 cells (67 uu) from the miner's cell, reachable in the flow field, whose whole landing footprint is reachable floor (below), keeping only cells whose zone holds the console class marked-block range ([[sys-objectives-console-chest]], task-304-01; switch `bPreferCellsWithMarkedRocks`, name kept, was a plain has-rocks preference). Alert 1 and nothing pending: `OnExtractionReady` once (phase `ExtractionReady`), then the shuttle lands (below). Run end (`EndRun`, from the shuttle): refused if already ended or the miner is dead (game over owns it); cancels the miner level-up offer (`URRLevelUpComponent::CancelChoices`: screen closed, later screens blocked; the end screen wins, [[sys-level-up]]); player 0 controller `SetPause(true)`, input Game and UI, cursor shown; `OnRunEnded(bSuccess)` once (phase `Ended`). On success, right after the pause (task-370-01, 2e1bde02): `Miner->HideForExtraction()` (miner and drone hidden, no collision) and `Shuttle->StartTakeoff()` (helicopter takeoff ticking while paused, [[sys-objectives-shuttle]]); the screen timing is unchanged. Run levels (task-206-03, [[sys-run-transition]]): a success on levels 1-3 of a run goes to `URRRunTransitionSubsystem::HandleLevelExtracted` (snapshot, advance, intermission) and `OnRunEnded` is NOT broadcast (no success end screen; phase still `Ended`); level 4 success and every loss broadcast it as before. `RestartLevel` calls `URRRunSubsystem::ResetRun`, then reopens the current map (level 1). During a transition loading the flow is held with `SetRunFlowEnabled(false)` until the pool prewarm is done (no alert, no console).
  - Run end listeners (task-211-01): `OnRunEnded` silences the subtitles for the rest of the world ([[sys-subtitles]]) and makes the HUD call `ShowEndScreen` (`URRHUDWidget::OnEndScreenShownDelegate` / `OnEndScreenShown`, the event the bp widgets bind to hide every HUD element but the end screen, [[sys-hud]]); the miner death does the same through `ARRGameMode::OnGameOver`.
  - `FRRRunFlowConfig` (`RRRunFlowConfig.h`): `AlertFillSeconds` 240, `KillAlertSeconds` 0.5, `ConsoleAlertThreshold` 0.4, `ConsoleMinDistanceCells` 36, `ConsoleMaxDistanceCells` 60 (task-029-04: 12–20 cells of 200 uu × 3), `bPreferCellsWithMarkedRocks` true, `ConsoleClass` (none = native `ARRConsole`); extraction ([[extraction]]): `ShuttleMinDistanceCells` 37 and `ShuttleMaxDistanceCells` 50 (task-049-01; was at least 75, no maximum; a max below the min counts as the min), `ShuttleCountdownSeconds` 45, `ShuttleBoardRadiusCells` 3 (1 cell of 200 uu, provisional choice of task-016-02), `ShuttleClass` (none = native `ARRShuttle`; the BP agent sets the shuttle Blueprint); bonus chest (task-056-01): `BonusChestAlertThreshold` 0.6 (0 = no chest), `BonusChestMinDistanceCells` 30, `BonusChestMaxDistanceCells` 45, `BonusChestClass` (none = native `ARRBonusChest`). Edited on `BP_RRGameMode` (`ARRGameMode::RunFlowConfig`), read on the first tick; `SetConfig` overrides (tests).

## Relations
- [[sys-objectives]] — parent hub: summary and verdict.
- [[sys-level-up]] — referenced above.
- [[extraction]] — referenced above.
- [[sys-objectives-shuttle]] — the takeoff EndRun starts.
- [[sys-run-transition]] — the intermission a level 1-3 success opens.
- [[sys-subtitles]] — silenced on OnRunEnded.
- [[sys-hud]] — the end screen event the HUD fires on OnRunEnded.
- [[sys-objectives-console-chest]] — the console marked-block drop range the picker applies.
