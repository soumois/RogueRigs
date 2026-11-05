---
id: sys-objectives
description: "Run flow as built: URRRunFlowSubsystem alert bar, console drop and pause, ARRConsole hack, ARRBonusChest drop and reward, ARRShuttle extraction, run end, Blueprint events."
parent: systems-world-index
kind: doc
---
# Objectives and run flow

- responsibility: the level run flow (alert bar, mandatory objective pause, extraction shuttle, run end) and the command console objective.
- main_classes (`Source/RogueRigs/Objectives/`): split by class in the child nodes under Relations (run flow, console and chest, shuttle, events).
- entry_points: subsystem `GetAlert`, `GetAlertSeconds`, `GetPhase`, `HasPendingObjective`, `IsExtractionReady`, `GetConsole`, `GetKillCount`, `DoKillsAddAlert` (false until the first horde front is over, task-454-01), `SetAlert`, `SkipToConsole`, `CompleteConsoleHack`, `SkipToExtraction`, `GetBonusChest`, `IsBonusChestTriggered`, `GetShuttle`, `IsRunEnded`, `WasRunSuccessful`, `EndRun`, `RestartLevel`, `SetRunFlowEnabled`, `SetRandomSeed`, `AddCarriedOre`, `GetCarriedOre`, `GetSecuredOre`; delegates `OnAlertChanged(Alert, bPaused)`, `OnObjectiveChanged(Console, State, MarkedRocksLeft, MarkedRocksTotal)`, `OnObjectiveProgressChanged(Progress, bHacking)`, `OnConsoleDropped(Console)`, `OnBonusChestDropped(BonusChest)`, `OnExtractionReady()`, `OnShuttleLanded(Shuttle)`, `OnShuttleCountdown(Remaining, ShuttleLocation)` (every countdown frame and at landing), `OnRunEnded(bSuccess)`, `OnOreChanged(CarriedOre, SecuredOre)`. Shuttle getters, delegates `OnShuttleCountdownChanged`, `OnShuttleFinished`. Console `ForceComplete` (cheats only: `SetAlert`, `SkipToConsole`, `CompleteConsoleHack`, `SkipToExtraction` and `ForceComplete` are in Blueprint category `Cheats` and do nothing in shipping, [[sys-cheats]]), getters, delegates `OnConsoleStateChanged`, `OnConsoleProgressChanged`, `OnConsoleMarkedRocksChanged`.
- data: tunables above (provisional values from [[level-flow]] M1 and [[objective-console]]); no table.
- depends_on: [[sys-grid]], [[sys-drone]], [[sys-flow-field]], [[sys-player]], [[sys-enemies]], [[sys-cheats]] (alert multiplier).
- implements: [[level-flow]] (M1 sequence), [[objective-console]], [[extraction]] (M1 end screens), alert counter of [[difficulty]].
- gotchas: every distance above is a cell count of the 67 uu grid; a cell size change needs them rescaled ([[arena-grid]]). Threat counter not built ([[sys-difficulty]]). One console and one bonus chest per level; no tier pool. The chest drops in other tests too when their alert passes 0.6 before full. The flow is active in every game world with a grid and a miner, including other tests (alert never reaches 0.4 there). Ore carried is counted and secured on extraction ([[sys-objectives-ore-counter]]); meta-progression not built. The bare test world has no controller: those tests prove the end state; `Integration.ConsoleExtractionWin` / `ShuttleTimeoutLoss` prove the real pause, the HUD texts and the end screen. Tests: `RogueRigs.RunFlow.*`, `RogueRigs.Console.*`, `RogueRigs.Extraction.*` ([[sys-automation-tests]]).

## Relations
- [[sys-objectives-run-flow]] — URRRunFlowSubsystem and FRRRunFlowConfig: alert bar, objective pause, extraction phases and tunables. Open to change the run flow.
- [[sys-objectives-console-chest]] — ARRConsole and ARRBonusChest: marked rocks, hack zone, capture visual, landing clearance. Open to change console or chest behaviour.
- [[sys-objectives-shuttle]] — ARRShuttle: landing cell, countdown and extraction. Open to change the shuttle.
- [[sys-objectives-ore-counter]] — carried and secured ore, extraction and loss, OnOreChanged. Open to change ore counting.
- [[sys-objectives-events]] — Blueprint events and HUD locator functions fed by the run flow. Open to wire objective visuals.
- [[systems-index]] — parent index.
- [[objective-console]] — gameplay spec of the console.
- [[level-flow]] — the run sequence implemented.
- [[sys-drone]] — the hack state the console drives.
- [[objective-bonus-chest]] — gameplay spec of the bonus chest.
- [[sys-hud]] — the HUD events fed by the run flow.
- [[sys-difficulty]] — the counter ownership.
- [[sys-automation-tests]] — tests of the run flow and console.
- [[sys-ground-telegraph]] — draws the console and chest hack zone.
