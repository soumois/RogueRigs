---
id: sys-pause-menu
description: "Pause menu C++ as built: controller open/resume/exit, URRPauseMenuWidget stats struct and text formats, pause keys, refusal rules, rr.Debug.PauseMenu; open for pausing or stats."
parent: systems-ui-index
kind: doc
---
# Pause menu

- responsibility: pauses the run on a key, shows the miner stats screen (Resume, Exit), resumes or returns to the main menu. Display only; no stat is changed.
- main_classes: `ARRPlayerController` (`Player/RRPlayerController.h/.cpp`), `URRPauseMenuWidget` (Abstract `UUserWidget`, `UI/RRPauseMenuWidget.h/.cpp`), `UI/RRPauseMenuDebugCommands.cpp` (not shipping). Task-424-03.
- entry_points:
  - Controller: `OpenPauseMenu`, `ClosePauseMenu`, `ExitPauseMenuToMainMenu`, `TogglePauseMenu(Source)`, `IsPauseMenuOpen`, `CanOpenPauseMenu(OutReason)`, `GetPauseMenuWidget`.
  - Widget: `Resume()`, `Exit()`, `GetStats()` -> `FRRPauseMenuStats`, `RefreshStats()` (fires BlueprintImplementableEvent `OnStatsRefreshed(const FRRPauseMenuStats&)`), `FocusResume()`, statics `BuildStats(const ARRMiner*)` and `DescribeStats`. `BindWidgetOptional` `ResumeButton`, `ExitButton`, bound by the C++ base.
  - Console `rr.Debug.PauseMenu open|resume|exit` (`ECVF_Cheat`, controller of player 0 via `RRCheats::FindCheatWorld`, same functions; [[sys-cheats]]).
- data: `EditDefaultsOnly` on the controller: `PauseMenuWidgetClass` (`TSubclassOf<URRPauseMenuWidget>`, empty = pause without a screen, logged; set to [[wbp-pause-menu]] in [[bp-rr-player-controller]]), `PauseMenuWidgetZOrder` 40 (HUD 0, transitions 50), `PauseMenuAction` (optional IA, must have Trigger When Paused), `PauseMenuKeys` (default Escape, P, Gamepad_Special_Right). With `PauseMenuAction` unset, a transient Boolean `UInputAction` (`bTriggerWhenPaused`) is mapped to `PauseMenuKeys` in a transient context at `MappingContextPriority`+2, bound on Started to `HandlePauseMenu` -> `TogglePauseMenu("pause key")`. IMC_Miner is not edited. The main menu has its own controller: no action there.
- flow:
  - Open: `SetPause(true)`, widget created at Z 40, `RefreshStats`, Game and UI input mode, Resume focused, cursor shown.
  - Resume: widget removed, `SetPause(false)`, `RestoreGameInputMode`.
  - Exit: only while open; calls `URRRunFlowSubsystem::ReturnToMainMenu` ([[sys-objectives-run-flow]]): `ResetRun` then `OpenLevel(MainMenuMapName)`, no loading screen or fade ([[sys-main-menu]]). If refused the menu stays open with a warning.
  - Open refused (reason logged): no local player, menu open, exit requested, game already paused by another screen (level-up cards, end screens), transition or loading running, run ended, game over, miner dead.
- stats: `FRRPauseMenuStats`.
  - Miner (`bHasMiner`): MaxHealth, MoveSpeedBonus, PickupRadiusBonus, XPGainBonus, MiningSpeedBonus, Luck, each a number plus `*Text`; bonus = current / base - 1 read from the ASC.
  - Global weapon bonuses: GlobalDamage, FireRate, ReloadSpeed, CritChance, CritDamage = sum of the AllWeapons cards of `URRUpgradeComponent::GetPickedUpgrades`, shop buys included ([[sys-upgrades]]).
  - `Weapons` (`FRRPauseMenuWeaponStats`): owned weapons in acquisition order, up to MaxWeapons, locked slots absent. Fields: Weapon, Name (CSV DisplayName), bIsTurretLauncher, Damage, FireRate, ReloadSpeed, ReloadSeconds, CritChance, CritDamage, MagazineSize, Pierce, each with `*Text`.
  - Text formats: miner `100`, `+0%`, Luck `0`; global bonuses `+10%`, crit chance and damage in points `+2`; per weapon `12`, `3.0`, `1.8s`, `5%`, `150%`, `30`, `4`.
- logs (LogRogueRigs): `pause menu opened (<source>), game paused, widget <Class>`, `pause menu stats: ...`, `open refused`, `Resume (...)`, `Exit (...)`, `rr.Debug.PauseMenu <action>: accepted|refused`. The Exit line of `ReturnToMainMenu` still says "run won".
- depends_on: [[sys-player]] (controller), [[sys-upgrades]], [[sys-objectives-run-flow]], [[sys-main-menu]].
- implements: [[pause-menu]].
- gotchas:
  - Verified (task-424-05): -game captures at four resolutions with one weapon: menu opens, stats match the log, Resume and Exit through the console work (Exit reaches L_MainMenu). Not verified in game: real key presses, layouts with two to four weapons.
  - The HUD faintly shows through the backdrop (alpha 0.94, widget Z 40).

## Relations
- [[systems-ui-index]] — parent index.
- [[pause-menu]] — the design this implements.
- [[wbp-pause-menu]] — the widget Blueprint on `URRPauseMenuWidget`.
- [[bp-rr-player-controller]] — sets PauseMenuWidgetClass.
- [[sys-player]] — the controller class owner.
- [[sys-main-menu]] — where Exit lands.
- [[sys-cheats]] — rr.Debug.PauseMenu.
- [[sys-upgrades]] — the picked upgrades behind the global bonuses.
- [[sys-objectives-run-flow]] — ReturnToMainMenu.
- [[task-424-pause-menu-stats]] — the task that built it.
