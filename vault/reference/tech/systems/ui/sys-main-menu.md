---
id: sys-main-menu
description: "Main menu C++ as built: menu game mode, menu player controller, URRMainMenuWidget base (New Run, Exit), NewRunMapName setting, L_MainMenu start-up map, rr.Menu commands."
parent: systems-ui-index
kind: doc
---
# Main menu

- responsibility: the screen shown at game start outside the editor. Creates the menu widget, lets the player start a fresh run on `NewRunMapName` or quit. No pawn, no run logic of its own.
- main_classes (`Source/RogueRigs/Menu/`):
  - `ARRMainMenuGameMode` (`AGameModeBase`): constructor sets `DefaultPawnClass` none and `PlayerControllerClass` `ARRMainMenuPlayerController`. `CreateMainMenuWidget(APlayerController*)` creates `MainMenuWidgetClass` and adds it to the viewport (warns and returns null when the class is unset). `GetMainMenuWidgetClass` (BlueprintPure). `CreateLoadingScreenWidget(Controller)` (task-379-01: creates `LoadingScreenWidgetClass`, adds it at `LoadingScreenWidgetZOrder`; logs and returns null without a class), `GetLoadingScreenWidgetClass`. `EditDefaultsOnly`: `MainMenuWidgetClass` (`TSubclassOf<URRMainMenuWidget>`), `MainMenuWidgetZOrder` (int32, 0), `LoadingScreenWidgetClass` (`TSubclassOf<URRLoadingScreenWidget>`, WBP_Loading, set in the Blueprint child), `LoadingScreenWidgetZOrder` (int32, 10). Blueprint child: [[bp-main-menu-game-mode]].
  - `ARRMainMenuPlayerController`: shows the mouse cursor and click events; at `BeginPlay` (local player) calls the game mode `CreateMainMenuWidget`, then sets `FInputModeUIOnly` focused on the widget. `GetMainMenuWidget`, `GetLoadingScreenWidget`. Task-379-01: binds `URRRunTransitionSubsystem::OnLoadingStarted` at `BeginPlay` (unbound at `EndPlay`); `HandleLoadingStarted` disables the menu widget and sets it HitTestInvisible (no second New Run), creates the loading widget through the game mode, `ShowLoading(Level, Layout)`, UI-only input focused on it; logs `loading screen shown over the main menu for level N (layout L, progress not measured)`, or `New Run loading of level N without a loading screen (...)`.
  - `URRMainMenuWidget` (Abstract `UUserWidget`, focusable): `StartNewRun()` (BlueprintCallable: `StartNewRunInWorld`, which calls `URRRunTransitionSubsystem::StartNewRun(World, NewRunMapName)`: `ResetRun`, Loading phase for level 1, loading screen over the menu, paint wait, `OpenLevel`, arena held until ready, fade in, [[sys-run-transition]]; a second press is refused while a transition runs; without the transition subsystem it falls back to `ResetRun` + `OpenLevel` with a warning), `QuitGame()` (BlueprintCallable), `IsOptionsAvailable()` (BlueprintPure, true when WBP_Options loads) and `OpenOptions()` (task-510-04: `OptionsButton` bound, opens the options screen over the menu, [[sys-options]]). Statics `StartNewRunInWorld(UWorld*)` and `QuitGameInWorld(UWorld*)` are shared with the debug commands. Blueprint child: [[wbp-main-menu]].
  - `RRMainMenuDebugCommands.cpp` (not compiled in shipping): `rr.Menu.NewRun` and `rr.Menu.Exit` (`ECVF_Cheat`), acting on `RRCheats::FindCheatWorld()` ([[sys-cheats]]).
- reached from the game: the run won end screen "Main Menu" button calls `URRRunFlowSubsystem::ReturnToMainMenu`, which resets the run and opens `URRRunSettings::MainMenuMapName` (default L_MainMenu) with no loading screen; the menu controller then runs its normal BeginPlay (widget, UI-only input, cursor). task-390-01, [[sys-objectives-run-flow]]; debug `rr.Run.MainMenu`. The pause menu Exit button takes the same path (task-424, [[sys-pause-menu]]).
- entry_points: `StartNewRun`, `QuitGame` (the WBP buttons call them); `rr.Menu.NewRun`, `rr.Menu.Exit` (console, `-ExecCmds`).
- data:
  - `URRRunSettings::NewRunMapName` (category `Run|Menu`, default `/Game/RogueRigs/Maps/L_ArenaM1`; override in `[/Script/RogueRigs.RRRunSettings]`, [[sys-run]]).
  - Packaged builds: `Config/DefaultGame.ini` `MapsToCook` lists L_MainMenu and L_ArenaM1 ([[build-toolchain]]); without it L_ArenaM1, reached only by the `NewRunMapName` string, is not cooked.
  - Map `/Game/RogueRigs/Maps/L_MainMenu`: empty level, World Settings `DefaultGameMode` = `BP_MainMenuGameMode_C`.
  - `Config/DefaultEngine.ini` `[/Script/Engine.UserInterfaceSettings]` `RenderFocusRule=Never` (task-540-01): Slate draws no focus rectangle after keyboard or gamepad navigation, on every screen (the engine default NavigationOnly drew a thin blue one).
  - `Config/DefaultEngine.ini`: `GameDefaultMap` = L_MainMenu (was L_ArenaM1). `EditorStartupMap` (L_ArenaM1) and `GlobalDefaultGameMode` (`BP_RRGameMode_C`) unchanged, so the editor and PIE on L_ArenaM1 still start a run directly.
- depends_on: [[sys-run]] (`ResetRun`, `URRRunSettings`), [[sys-cheats]] (`FindCheatWorld`).
- implements: [[main-menu]].
- gotchas:
  - `capture.js --map none` (task-338-05) launches on the configured start-up map (the menu); a launch passing L_ArenaM1 (capture.js default) opens the arena.
  - Verified (task-338-05, -game, no map argument, 1920x1080): capture `Saved/Captures/MainMenu/MainMenu_00.png` shows the key-art background, dark stepped gradient left, title, New Run orange (focus), Options grey, Exit white; log `main menu shown (WBP_MainMenu_C).`. `rr.Menu.NewRun` logged `run reset`, `Main menu New Run: run reset, opening /Game/RogueRigs/Maps/L_ArenaM1 (level 1)`, `run started, level 1/4 on layout arena_m1_fine`. `rr.Menu.Exit` logged `Main menu Exit: quitting the game.`, exit code 0.
  - Verified packaged (task-365-01): staged Development build, `Saved/StagedBuilds/Windows/RogueRigs.exe -windowed -log -ExecCmds=rr.Menu.NewRun`, no map argument. Log: `main menu shown (WBP_MainMenu_C).`, `Main menu New Run: run reset, opening /Game/RogueRigs/Maps/L_ArenaM1 (level 1).`, `LoadMap: /Game/RogueRigs/Maps/L_ArenaM1`, `URRRunSubsystem: run started, level 1/4 on layout arena_m1_fine.`, no TravelFailure. Cause of the earlier failure: only L_MainMenu was cooked.
  - Task-379 (New Run loading screen): capture subtask 04 was closed on the owner's word, no capture run. The owner saw the spinning crystal during New Run and said it takes a while to start spinning; whether that is lag is unknown.
  - Not seen: real mouse click on New Run in the packaged build (owner-only), cursor, Options click. Enter key seen in -game (task-540-02: real Down, Up, Enter key presses load L_ArenaM1).
  - Editor map authoring: a dirty current level blocks `load_level`; File > New Level opens a modal dialog that blocks MCP; duplicating L_ArenaM1 leaves a ghost level.

## Relations
- [[systems-ui-index]] — parent index.
- [[main-menu]] — the design this implements.
- [[wbp-main-menu]] — the widget Blueprint on `URRMainMenuWidget`.
- [[bp-main-menu-game-mode]] — the game mode Blueprint set on L_MainMenu.
- [[sys-run]] — run reset and the NewRunMapName and MainMenuMapName settings.
- [[sys-objectives-run-flow]] — ReturnToMainMenu, which opens this menu after a won run.
- [[sys-pause-menu]] — its Exit reaches this menu.
- [[sys-cheats]] — rr.Menu.NewRun and rr.Menu.Exit.
- [[task-338-main-menu-screen]] — the task that built it.
- [[build-toolchain]] — packaging command and the MapsToCook rule.
- [[task-379-new-run-loading-screen]] — the task that added the New Run loading screen.
- [[task-365-new-run-packaged-map]] — the task that fixed New Run in the packaged build.
- [[sys-options]] — the options screen its Options button opens.
