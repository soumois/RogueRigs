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
  - `ARRMiner::BeginPlay` ends with `HandleMinerBeginPlay(Miner)` (first miner of a world only): no run in progress → `BeginRun(grid LayoutName)` (PIE or -game on any map starts level 1, no menu); a pending snapshot captured in another world → `ApplySnapshotToMiner`, once.
  - Flow calls (BlueprintCallable): `BeginRun(Level1LayoutName)`, `CaptureLevelEndSnapshot(Miner)` (after `URRRunFlowSubsystem::EndRun(true)`), `AdvanceToNextLevel()` (false on the last level), `ResetRun()` (loss, run won, restart: next map begins level 1).
  - Reads (BlueprintPure): `IsRunInProgress`, `GetLevelIndex` (0 without run), `GetLevelCount`, `IsLastLevel`, `GetLayoutNameForLevel(Index)`, `GetCurrentLayoutName`, `GetLastSnapshot`, `HasPendingSnapshot`, `GetRunSecuredOre`.
  - Delegates (BlueprintAssignable): `OnRunStarted(LevelIndex, LayoutName)`, `OnLevelSnapshotCaptured(Snapshot)` (recap), `OnLevelAdvanced(NewLevelIndex, LayoutName)` (the async layout provider starts here, [[task-206-02-async-layout]]), `OnSnapshotApplied(Miner, LevelIndex)`, `OnRunReset()`.
  - Snapshot: picked cards in order (`TargetWeapon` replaced by its exact class), owned weapon classes, miner level and XP, Health and MaxHealth, recap ore (`OreSecuredThisLevel`, `RunSecuredOre`, `CarriedOreAtCapture`).
  - Re-application, in order: `URRProgressionComponent::RestoreProgress` (no level-up, [[sys-progression]]); every card replayed through `URRUpgradeComponent::ApplyUpgrade` (a NewWeapon card acquires its weapon, which replays the earlier all-weapons cards; a weapon card targets the owned weapon of its class), so GE stacks match the previous level; snapshot weapons still missing → `AddWeapon`; Health base set to the snapshot value, clamped to [1, MaxHealth].
  - Ore (decision of this task): carried as secured ore. `EndRun(true)` moves the carried count into `URROreBankSubsystem` ([[sys-objectives-ore-counter]]), which keeps it across the travel; each level starts with 0 carried. Carried ore still held at capture is logged and not carried.
  - Debug: `rr.Run.Status`; `rr.Run.NextLevel` (EndRun(true), capture, advance, reopen the current map).
- data: `URRRunSettings::LevelLayouts`, C++ defaults `arena_m1_fine`, `arena_l2`, `arena_l3`, `arena_l4` (count = levels per run); override in `Config/DefaultGame.ini` `[/Script/RogueRigs.RRRunSettings]`. Nothing saved to disk.
- depends_on: [[sys-player]], [[sys-progression]], [[sys-upgrades]], [[sys-weapons]], [[sys-objectives-ore-counter]], [[sys-grid]] (`GetLayoutName`).
- implements: [[run-structure]] (level chain, carried state), [[intermission]] (recap data, PIE starts at level 1).
- gotchas: the snapshot is a `UPROPERTY` so Blueprint weapon classes stay loaded across the travel (they read None otherwise). The grid still loads its own `LayoutName`: `GetCurrentLayoutName` is not consumed until task-206-03. The existing death restart (`ARRGameMode::RestartLevel`) does not call `ResetRun` yet: the level index is kept. Verified by a -game log of two `rr.Run.NextLevel` transitions (task-206-01); no automation test (owner rule).

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
- [[task-206-02-async-layout]] — listens to OnLevelAdvanced.
- [[task-206-03-transition-flow]] — calls the entry points.
