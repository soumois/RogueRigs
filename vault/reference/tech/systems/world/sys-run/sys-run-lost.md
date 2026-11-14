---
id: sys-run-lost
description: "URRRunLostSubsystem and URRRunLostWidget: death sequence, run-lost recap, Retry and Main Menu for a death or a missed shuttle; tunables, tests, debug commands."
parent: sys-run
kind: doc
---
# Run lost

- responsibility: the loss end of a run ([[death-screen]], task-484-03, commit 8af4aceb): death sequence, the event that shows the run-lost screen, its recap and its two actions.
- main_classes: `URRRunLostSubsystem` (`Run/RRRunLostSubsystem.h`, tickable world subsystem, Game and PIE; ticks only during the death sequence); `FRRRunLostRecap` (BlueprintType: `bMinerDied`, `ScreenTextId`, `Title`, `Subtitle`, `LevelReached`, `LevelCount`, `RobotsDestroyed`, `DuskiteHeld`, `LevelText` "Level N of M", `RobotsText`, `DuskiteText`); `URRRunLostWidget` (`UI/RRRunLostWidget.h`, Abstract UUserWidget, base of [[wbp-run-lost]]).
- entry_points:
  - Death: `URRRunFlowSubsystem::HandleMinerDeath` reads carried + secured ore, empties the carried ore, then `BeginDeathSequence(DuskiteHeld)`: level-up offer cancelled, `AWorldSettings::SetTimeDilation(DeathTimeDilation)`; after `DeathSequenceRealSeconds` of `UWorld::GetRealTimeSeconds` the dilation is restored, player 0's controller pauses (Game and UI, cursor) and `OnRunLost(Recap)` fires once. Controls are cut by `ARRGameMode::HandleMinerDeath` (DisableInput, `OnGameOver`); the old 3 s level restart is removed.
  - Missed shuttle: `URRRunFlowSubsystem::EndRun(false)` broadcasts `OnRunEnded(false)` then `ShowShuttleMissed(DuskiteHeld)`: `OnRunLost` at once, text id `run_lost_shuttle`.
  - Recap (`BuildRecap`): title and subtitle from `screen_texts.json` items `run_lost_death` / `run_lost_shuttle` ([[sys-text-content]]), fallback title "RUN LOST"; level from `URRRunSubsystem` (1 without a run); robots = `URRRunLogSubsystem::GetRunKillCount(World)` (kills of every level entry of the run plus the open level, same counts as the stats screens, [[sys-run-log]]), else the run flow `GetKillCount`.
  - `Retry()`: `URRRunTransitionSubsystem::StartNewRun(World, NewRunMapName)` (run reset, loading screen "DROPPING IN", level 1, [[sys-run-transition]]); without the game instance subsystem: `ResetRun` + `OpenLevel`. `ReturnToMainMenu()`: `URRRunFlowSubsystem::ReturnToMainMenu`. Each refuses a second press or a press before the screen.
  - `URRRunLostWidget`: `ShowRunLost(Recap)` (focus on the widget, `OnRunLostShown`), `Retry`, `ReturnToMainMenu`, `RequestSkipAppearance` (skips the entrance, `FinishAppearance`, then the optional notification `OnSkipAppearanceRequested`; true until finished), `FinishAppearance` (focus `RetryButton`), `SetLevelDesaturation(Amount)` (forwards to the entrance), optional bound `RetryButton`, `MainMenuButton`. Since task-547 the appearance is the shared [[sys-screen-entrance]] (`EntranceParts`: title group, RecapBlock, the two buttons, SkipButton), started after `OnRunLostShown`, ticked in `NativeTick`, restored in `NativeDestruct`.
  - `ARRPlayerController::HandleRunLost`: HUD hidden, `RunLostWidgetClass` created at `RunLostWidgetZOrder` 45, focus, cursor; removed when a loading starts. `URRHUDWidget` shows its end screen state for a death at `OnRunLost`, no longer at the lethal hit ([[sys-hud]]).
- data: `URRRunSettings` `DeathTimeDilation` 0.3, `DeathSequenceRealSeconds` 1.0 (Config Game, provisional, owner 2026-11-07).
- depends_on: [[sys-objectives-run-flow]], [[sys-run]], [[sys-run-transition]], [[sys-run-log]], [[sys-text-content]].
- implements: [[death-screen]].
- gotchas: the camera is not updated while paused unless the controller has `bShouldPerformFullTickWhenPaused` (set by WBP_RunLost Construct). Debug: `rr.Run.Lost.Die`, `rr.Run.Lost.Shuttle`, `rr.Run.Lost.Retry`, `rr.Run.Lost.MainMenu` (`Run/RRRunLostDebugCommands.cpp`). Tests: `RogueRigs.Player.GameOver` (sequence timing, recap), `RogueRigs.Integration.DeathRunLostScreen` (pause, HUD, Retry), `RogueRigs.Integration.ShuttleTimeoutLoss` (missed shuttle, Main Menu).

## Relations
- [[sys-run]] — parent: the run state the loss resets.
- [[wbp-run-lost]] — the Blueprint screen on URRRunLostWidget.
- [[sys-objectives-run-flow]] — calls it on a death and a missed shuttle.
- [[death-screen]] — the design it implements.
- [[sys-run-log]] — the run kill count of the recap.
- [[sys-screen-entrance]] — the shared appearance effect it drives.
