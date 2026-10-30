---
id: sys-run
description: "URRRunSubsystem: run level index, layout list, miner snapshot at level end and its re-application after the map travel. Open to change run state."
parent: systems-world-index
kind: doc
---
# Run state

- responsibility: state of a run across its levels ([[run-structure]]): current level 1..N, layout of each level, snapshot of the miner taken at level end and re-applied to the next level's miner. No travel, no screen: the transition flow ([[task-206-03-transition-flow]]) calls it.
- main_classes (`Source/RogueRigs/Run/`): `URRRunSubsystem` (`UGameInstanceSubsystem`, survives `OpenLevel`); `FRRRunMinerSnapshot` and `FRRRunPickedCard` (`RRRunSnapshot.h`); `URRRunSettings` (`UCLASS(Config = Game, DefaultConfig)` plain `UObject`, same choice as `URRIconSettings` in [[sys-upgrades]]); debug commands `RRRunDebugCommands.cpp` (not in shipping).
- entry_points:
  - `ARRMiner::BeginPlay` ends with `HandleMinerBeginPlay(Miner)` (first miner of a world only): no run in progress → `BeginRun(grid LayoutName)` (PIE or -game on any arena map starts level 1; the -game default map is now the main menu L_MainMenu, whose New Run calls `ResetRun` then opens `URRRunSettings::NewRunMapName`, [[sys-main-menu]]); a pending snapshot captured in another world → `ApplySnapshotToMiner`, once.
  - Flow calls (BlueprintCallable): `BeginRun(Level1LayoutName)`, `CaptureLevelEndSnapshot(Miner)` (after `URRRunFlowSubsystem::EndRun(true)`), `AdvanceToNextLevel()` (false on the last level), `ResetRun()` (loss, run won, restart, main menu New Run: next map begins level 1, secured ore bank cleared).
  - Reads (BlueprintPure): `IsRunInProgress`, `GetLevelIndex` (0 without run), `GetLevelCount`, `IsLastLevel`, `GetLayoutNameForLevel(Index)`, `GetCurrentLayoutName`, `GetLastSnapshot`, `HasPendingSnapshot`, `GetRunSecuredOre`.
  - Delegates (BlueprintAssignable): `OnRunStarted(LevelIndex, LayoutName)`, `OnLevelSnapshotCaptured(Snapshot)` (recap), `OnLevelAdvanced(NewLevelIndex, LayoutName)` (the layout of the new level is already requested, [[sys-run-layout]]), `OnLayoutReady(LevelIndex, LayoutName, bSuccess)`, `OnSnapshotApplied(Miner, LevelIndex)`, `OnRunReset()`.
  - Snapshot: picked cards in order (`TargetWeapon` replaced by its exact class), owned weapon classes, miner level and XP, Health and MaxHealth, recap ore (`OreSecuredThisLevel`, `RunSecuredOre`, `CarriedOreAtCapture`).
  - Re-application, in order: `URRProgressionComponent::RestoreProgress` (no level-up, [[sys-progression]]); every card replayed through `URRUpgradeComponent::ApplyUpgrade` (a NewWeapon card acquires its weapon, which replays the earlier all-weapons cards; a weapon card targets the owned weapon of its class), so GE stacks match the previous level; snapshot weapons still missing → `AddWeapon`; Health base set to the snapshot value, clamped to [1, MaxHealth].
  - Ore (decision of this task): carried as secured ore. `EndRun(true)` moves the carried count into `URROreBankSubsystem` ([[sys-objectives-ore-counter]]), which keeps it across the travel; each level starts with 0 carried. Carried ore still held at capture is logged and not carried. The bank is cleared at every run end (task-340-01): `ResetRun` calls `URROreBankSubsystem::ResetSecuredOre` ([[sys-objectives-ore-counter]]); `BeginRun` does not reset it, so ore secured on a level stays spendable on the next ones. Log: `URRRunSubsystem: run reset; the next map begins level 1, secured ore N.` (N = bank after the clear, 0). `OnOreChanged` is not broadcast by the reset.
  - Intermission writes (task-316-01, [[sys-run-shop]]): `AppendCardToPendingSnapshot(Miner, Card)` adds a card bought after the capture to the pending snapshot (replayed with the others, Health/MaxHealth refreshed); `RecordIntermissionOreSpent(Amount)` lowers the level-start and run-start ore baselines so the recap keeps counting secured ore.
  - Layout provider: `RequestLayout`, `IsLayoutReady`, `GetPreparedLayout`, `SetLayoutProvider`: [[sys-run-layout]].
  - Transition (task-206-03, [[sys-run-transition]]): `URRRunTransitionSubsystem` calls capture and advance on a level 1-3 extraction; `GetLevelSecuredOre()` (pure, task-317-01) gives the level 4 stats screen its ore; restart (`URRRunFlowSubsystem::RestartLevel`, `ARRGameMode::RestartLevel`) calls `ResetRun`, so a loss or a won run restarts at level 1.
  - Widget binding API (stats, shop and loading screens, controller class properties): [[sys-run-transition]].
  - Debug: `rr.Run.Status` (also transition phase, run won); `rr.Run.Stats.Next`; `rr.Run.Continue`; `rr.Run.NextLevel`; `rr.Run.EndLevel`; `rr.Run.Restart`; shop: `rr.Shop.Buy <Slot>`, `rr.Shop.Reroll` ([[sys-cheats]], [[sys-run-shop]]).
- data: `URRRunSettings::NewRunMapName` (category `Run|Menu`, default `/Game/RogueRigs/Maps/L_ArenaM1`, map opened by the main menu New Run); `URRRunSettings::LevelLayouts`, C++ defaults `arena_m1_fine`, `arena_l2`, `arena_l3`, `arena_l4` (count = levels per run); override in `Config/DefaultGame.ini` `[/Script/RogueRigs.RRRunSettings]`. Nothing saved to disk.
- depends_on: [[sys-player]], [[sys-progression]], [[sys-upgrades]], [[sys-weapons]], [[sys-objectives-ore-counter]], [[sys-grid]] (`GetLayoutName`).
- implements: [[run-structure]] (level chain, carried state), [[intermission]] (recap data, PIE starts at level 1).
- gotchas: the snapshot is a `UPROPERTY` so Blueprint weapon classes stay loaded across the travel (they read None otherwise). On levels 2+ the grid builds from the prepared layout ([[sys-run-layout]]); level 1 uses the grid's own `LayoutName`. Both restarts call `ResetRun` since task-206-03, and the main menu New Run too (`URRRunTransitionSubsystem::StartNewRun`, called by `URRMainMenuWidget::StartNewRunInWorld`, task-379-01, [[sys-run-transition]]); all run ends go through it. Verified by a -game log of two `rr.Run.NextLevel` transitions (task-206-01); no automation test (owner rule).

## Relations
- [[systems-world-index]] — parent index.
- [[run-structure]] — the rules this state follows.
- [[intermission]] — the screen that reads the snapshot recap.
- [[sys-objectives-ore-counter]] — carried and secured ore.
- [[sys-progression]] — RestoreProgress used at re-application.
- [[sys-upgrades]] — picked cards replayed.
- [[sys-weapons]] — weapons re-acquired.
- [[sys-player]] — the miner that calls HandleMinerBeginPlay.
- [[sys-grid]] — LayoutName of level 1.
- [[sys-run-layout]] — Async layout provider of the run: RequestLayout off the game thread, OnLayoutReady, prepared layout kept across the travel, provider slot for the generator.
- [[task-206-02-async-layout]] — the task that added the layout provider.
- [[sys-run-transition]] — Level-to-level transition: intermission, Continue, loading hold until grid and pool prewarm are ready; widget binding API.
- [[task-206-03-transition-flow]] — calls the entry points.
- [[sys-run-shop]] — child: the intermission shop; adds bought cards to the snapshot.
- [[sys-main-menu]] — the main menu that resets the run and opens NewRunMapName.
