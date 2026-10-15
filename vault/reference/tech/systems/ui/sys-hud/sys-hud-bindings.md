---
id: sys-hud-bindings
description: "HUD Bind calls: BindToMiner, RunFlow, Spawner, GameMode, late binding for panels, controller BeginPlay order. Open to wire a HUD panel."
parent: sys-hud
kind: doc
---
# HUD — bindings

  - `ARRPlayerController::BeginPlay` (local player only: skipped when `HUDWidgetClass` is none, the controller is not local or has no `ULocalPlayer`, so headless tests create no widget) creates `HUDWidgetClass`, adds it to the viewport, calls `BindToGameMode(auth game mode as ARRGameMode)` and `BindToMiner(possessed miner)`. `ARRPlayerController::OnPossess` rebinds to the new miner. `GetHUDWidget()` returns it.
  - `BindToMiner(ARRMiner*)`: subscribes to the ASC value-change delegates of `Health` and `MaxHealth` (either change calls `OnHealthChanged` with both current values, skipped while MaxHealth is 0) to the miner `URRProgressionComponent` `OnXPChanged` / `OnLevelUp` and `URRLevelUpComponent` `OnUpgradeOfferShown` / `OnUpgradeOfferClosed` (forwarded to the display events), then pushes the current health and XP once (and an open card screen); also binds the miner's turret launcher (now, or on `OnWeaponAdded` when acquired later) and pushes its charges; null unbinds.
  - `BindToRunFlow(URRRunFlowSubsystem*)`: called by `ARRPlayerController::BeginPlay`; binds the six run-flow delegates and pushes the current alert, objective (and progress), shuttle countdown and run end once; null unbinds.
  - `BindToSpawner(ARREnemySpawner*)`: called by `ARRPlayerController::BeginPlay` with the first spawner of the world; horde banner and alert-bar icons ([[sys-hud-pressure]]); null unbinds.
  - Panels and late binding (task-094-02, [[sys-hud-events]]): every Bind call also hands its object once to each `URRHUDPanelWidget` of the HUD tree (`GetPanels`); a miner or spawner registering with the run flow after the HUD is bound when none is.
  - Ore counter panel (task-183-01, wired in task-183-04): override `BindToRunFlow`, bind `URRRunFlowSubsystem::OnOreChanged(CarriedOre, SecuredOre)` and show `GetCarriedOre` / `GetSecuredOre` once there (bind-time value); unbind `PreviousRunFlow`. A level restart is a new run flow (carried 0, secured kept by the game instance) ([[sys-objectives-ore-counter]]).
  - `BindToGameMode(ARRGameMode*)`: binds `ARRGameMode::OnGameOver` to `OnGameOver` ([[sys-player]] game over); null unbinds. `NativeDestruct` removes every binding.

## Relations
- [[sys-hud]] — parent hub: summary and verdict.
- [[sys-hud-pressure]] — referenced above.
- [[sys-hud-events]] — referenced above.
- [[sys-player]] — referenced above.
- [[sys-objectives-ore-counter]] — the ore event a panel binds in BindToRunFlow.
