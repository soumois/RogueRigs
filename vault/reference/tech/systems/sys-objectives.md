---
id: sys-objectives
description: "Run flow as built: URRRunFlowSubsystem alert bar, console drop and pause, ARRConsole hack, ARRShuttle extraction countdown, run end and restart, Blueprint events list."
parent: systems-index
kind: doc
---
# Objectives and run flow

- responsibility: the level run flow (alert bar, mandatory objective pause, extraction shuttle, run end) and the command console objective.
- main_classes (`Source/RogueRigs/Objectives/`):
  - `URRRunFlowSubsystem` (tickable world subsystem, Game/PIE, not ticked while paused): `AlertSeconds` += dt × cheat multiplier; kills add `KillAlertSeconds` (`NotifyEnemyKilled`, called by `ARREnemy::HandleDeath`; a self-instigated death such as the kamikaze fuse is ignored). Nothing grows while the console is pending. At `ConsoleAlertThreshold` the alert is clamped and the console drops on a random Floor cell, straight-line 12–20 cells from the miner's cell, reachable in the flow field, preferring cells with rocks to mark (`bPreferCellsWithMarkedRocks`, provisional choice of this task). Alert 1 and nothing pending: `OnExtractionReady` once (phase `ExtractionReady`), then the shuttle lands (below). Run end (`EndRun`, from the shuttle): refused if already ended or the miner is dead (game over owns it); cancels the miner level-up offer (`URRLevelUpComponent::CancelChoices`: screen closed, later screens blocked; the end screen wins, [[sys-level-up]]); player 0 controller `SetPause(true)`, input Game and UI, cursor shown; `OnRunEnded(bSuccess)` once (phase `Ended`). `RestartLevel` reopens the current map.
  - `FRRRunFlowConfig` (`RRRunFlowConfig.h`): `AlertFillSeconds` 240, `KillAlertSeconds` 0.5, `ConsoleAlertThreshold` 0.4, `ConsoleMinDistanceCells` 12, `ConsoleMaxDistanceCells` 20, `bPreferCellsWithMarkedRocks` true, `ConsoleClass` (none = native `ARRConsole`); extraction ([[extraction]]): `ShuttleMinDistanceCells` 25, `ShuttleCountdownSeconds` 45, `ShuttleBoardRadiusCells` 1 (provisional choice of task-016-02), `ShuttleClass` (none = native `ARRShuttle`; the BP agent sets the shuttle Blueprint). Edited on `BP_RRGameMode` (`ARRGameMode::RunFlowConfig`), read on the first tick; `SetConfig` overrides (tests).
  - `ARRConsole` (Blueprintable, not abstract): marks blocks of `MarkedCellTypes` (Rock, Ore, HealOre, SpecialOre; barrels excluded) within `MarkRadiusCells` 2 (Euclidean, cell centers) through `ARRArenaGrid::SetCellMarked` ([[sys-grid]]). States `ERRConsoleState`: Locked (marked rocks left, polled each tick), Idle, Hacking, Complete (final). Miner alive within `ZoneRadiusCells` 3 (2D): `ARRDrone::StartHack`; while linked progress += dt / `HackDuration` 20; otherwise `StopHack` and drain `DrainPerSecond` 0.05/s. Mesh: NoCollision by default.
  - `ARRShuttle` (Blueprintable, not abstract): spawned on a random reachable Floor cell at least `ShuttleMinDistanceCells` straight-line from the miner cell (none: farthest reachable floor cell, warning; no reachable cell: retried each tick). Countdown `ShuttleCountdownSeconds` (cheat `ShuttleCountdownOverride` > 0 replaces it at landing, [[sys-cheats]]). Each tick (not paused): miner alive within the board radius (2D) → extracted (win); else countdown −dt, at 0 → missed (loss). Dead miner freezes it. Robots keep spawning meanwhile ([[sys-spawner]] is independent). Mesh NoCollision.
- entry_points: subsystem `GetAlert`, `GetAlertSeconds`, `GetPhase`, `HasPendingObjective`, `IsExtractionReady`, `GetConsole`, `GetKillCount`, `SetAlert`, `SkipToConsole`, `CompleteConsoleHack`, `SkipToExtraction`, `GetShuttle`, `IsRunEnded`, `WasRunSuccessful`, `EndRun`, `RestartLevel`, `SetRunFlowEnabled`, `SetRandomSeed`; delegates `OnAlertChanged(Alert, bPaused)`, `OnObjectiveChanged(Console, State, MarkedRocksLeft, MarkedRocksTotal)`, `OnObjectiveProgressChanged(Progress, bHacking)`, `OnConsoleDropped(Console)`, `OnExtractionReady()`, `OnShuttleLanded(Shuttle)`, `OnShuttleCountdown(Remaining, ShuttleLocation)` (every countdown frame and at landing), `OnRunEnded(bSuccess)`. Shuttle getters, delegates `OnShuttleCountdownChanged`, `OnShuttleFinished`. Console `ForceComplete`, getters, delegates `OnConsoleStateChanged`, `OnConsoleProgressChanged`, `OnConsoleMarkedRocksChanged`.
- Blueprint events (BlueprintImplementableEvent) for the visuals:
  - `ARRConsole`: `OnMarkedRocksChanged(Remaining, Total)` (also at drop), `OnHackProgress(Progress, bHacking)`, `OnHackStateChanged(NewState)`.
  - `ARRShuttle`: `OnLanded()`, `OnCountdown(Remaining)` (every countdown frame), `OnExtracted()` (win), `OnMissed()` (loss).
  - `ARRArenaGrid`: `OnCellMarkedChanged(Cell, bMarked, CellCenter)`; and per-instance custom data float `MarkedCustomDataIndex` (0) = `MarkedCustomDataValue` (1) on the block HISMs, readable by a material through PerInstanceCustomData (pulsing white outline, [[color-code]]).
  - `URRHUDWidget`: `OnAlertChanged`, `OnObjectiveChanged`, `OnObjectiveProgressChanged`, `OnExtractionReady`, `OnShuttleCountdown`, `OnRunEnded`; call `RestartLevel` ([[sys-hud]]).
- data: tunables above (provisional values from [[level-flow]] M1 and [[objective-console]]); no table.
- depends_on: [[sys-grid]], [[sys-drone]], [[sys-flow-field]], [[sys-player]], [[sys-enemies]], [[sys-cheats]] (alert multiplier).
- implements: [[level-flow]] (M1 sequence), [[objective-console]], [[extraction]] (M1 end screens), alert counter of [[difficulty]].
- gotchas: threat counter not built ([[sys-difficulty]]). One console per level; no tier pool. The flow is active in every game world with a grid and a miner, including other tests (alert never reaches 0.4 there). Ore carried is not secured yet ([[ore-economy]] meta not built). The bare test world has no controller: those tests prove the end state; `Integration.ConsoleExtractionWin` / `ShuttleTimeoutLoss` prove the real pause, the HUD texts and the end screen. Tests: `RogueRigs.RunFlow.*`, `RogueRigs.Console.*`, `RogueRigs.Extraction.*` ([[sys-automation-tests]]).

## Relations
- [[systems-index]] — parent index.
- [[objective-console]] — gameplay spec of the console.
- [[level-flow]] — the run sequence implemented.
- [[sys-drone]] — the hack state the console drives.
- [[sys-hud]] — the HUD events fed by the run flow.
- [[sys-difficulty]] — the counter ownership.
- [[sys-automation-tests]] — tests of the run flow and console.
