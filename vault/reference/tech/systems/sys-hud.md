---
id: sys-hud
description: "In-run HUD C++ base: URRHUDWidget display events (health, XP, cards, alert, objective, shuttle, run end, game over, pressure), button calls, controller creation."
parent: systems-index
kind: doc
---
# HUD

- responsibility: feed the UI layer from C++: the miner's health, XP and level-ups, and the game-over event. Layout, text and visuals live in the Blueprint subclass (BP agent, [[cpp-bp-split]]); no gameplay logic in the widget.
- main_classes: `URRHUDWidget : UUserWidget` (`Source/RogueRigs/UI/RRHUDWidget.h`, `Abstract`). `BlueprintImplementableEvent`s to implement in the Blueprint subclass: `OnHealthChanged(float Current, float Max)` (Health and MaxHealth of the miner), `OnXPChanged(float Current, float Needed, int32 Level)` and `OnLevelUp(int32 NewLevel)` (miner progression, [[sys-progression]]; drives the "LEVEL UP" text), `OnUpgradeOfferShown(TArray<FRRUpgradeCard> Cards, bool bCanReroll)` (show or refresh the card screen: opening, after a reroll, each chained screen; game paused, cursor shown) and `OnUpgradeOfferClosed()` (hide it, game resumes) ([[sys-level-up]]), and `OnGameOver()`. Run flow ([[sys-objectives]]): `OnAlertChanged(float Alert, bool bPaused)` (0..1; paused while the console is pending), `OnObjectiveChanged(bool bActive, ERRConsoleState State, int32 MarkedRocksLeft, int32 MarkedRocksTotal)` (bActive false = no console yet; the text lives in the Blueprint), `OnObjectiveProgressChanged(float Progress, bool bHacking)`, `OnExtractionReady()`, `OnShuttleCountdown(float Remaining, FVector ShuttleLocation)` (every countdown frame), `OnRunEnded(bool bSuccess)` (true = success screen, false = run-lost screen; game paused, Game and UI input, cursor shown, any card screen already closed). Turret ([[sys-turret]]): `OnTurretChargesChanged(int32 Charges, int32 MaxCharges, float RechargeDuration)` (0 = no recharge running; not called without a launcher) and pure `GetTurretRechargeProgress()` (0..1, per-frame bar). Pressure ([[sys-hud-pressure]]): `OnPlayerDamaged`, `OnHordeIncoming`, `OnAlertEventsChanged`. Guards: `OnHealthChanged` is never called with Max <= 0 and `OnXPChanged` never with Needed <= 0, so the Blueprint may divide by them.
- Blueprint calls (buttons): `ChooseUpgrade(int32 Index)` (0-based in `Cards`; bool), `RequestReroll()` (bool), `CanRerollUpgrade()` (pure); forwarded to the bound miner's `URRLevelUpComponent`. `RestartLevel()` (both end screens) forwards to `URRRunFlowSubsystem::RestartLevel` (reopens the map). Card fields: `DisplayName`, `TargetText`, `ValueText`, `Tier` ([[sys-upgrades]]).
- entry_points:
  - `ARRPlayerController::BeginPlay` (local player only: skipped when `HUDWidgetClass` is none, the controller is not local or has no `ULocalPlayer`, so headless tests create no widget) creates `HUDWidgetClass`, adds it to the viewport, calls `BindToGameMode(auth game mode as ARRGameMode)` and `BindToMiner(possessed miner)`. `ARRPlayerController::OnPossess` rebinds to the new miner. `GetHUDWidget()` returns it.
  - `BindToMiner(ARRMiner*)`: subscribes to the ASC value-change delegates of `Health` and `MaxHealth` (either change calls `OnHealthChanged` with both current values, skipped while MaxHealth is 0) to the miner `URRProgressionComponent` `OnXPChanged` / `OnLevelUp` and `URRLevelUpComponent` `OnUpgradeOfferShown` / `OnUpgradeOfferClosed` (forwarded to the display events), then pushes the current health and XP once (and an open card screen); also binds the miner's turret launcher (now, or on `OnWeaponAdded` when acquired later) and pushes its charges; null unbinds.
  - `BindToRunFlow(URRRunFlowSubsystem*)`: called by `ARRPlayerController::BeginPlay`; binds the six run-flow delegates and pushes the current alert, objective (and progress), shuttle countdown and run end once; null unbinds.
  - `BindToSpawner(ARREnemySpawner*)`: called by `ARRPlayerController::BeginPlay` with the first spawner of the world; horde banner and alert-bar icons ([[sys-hud-pressure]]); null unbinds.
  - `BindToGameMode(ARRGameMode*)`: binds `ARRGameMode::OnGameOver` to `OnGameOver` ([[sys-player]] game over); null unbinds. `NativeDestruct` removes every binding.
- data: editable on the controller Blueprint: `HUDWidgetClass` (category HUD, none by default; set to the HUD widget Blueprint).
- depends_on: [[sys-player]] (miner, controller, game mode), [[sys-gas-core]] (health attribute set), [[sys-progression]]; module `UMG`.
- implements: [[miner]] (M1 "Game Over" text, health display data), [[xp-level-up]] (XP bar and level-up data).
- gotchas: the game mode exists only on the server, so the game-over binding is single-player only. If the controller begins play before the miner applied its CSV base stats, MaxHealth is 0: the bind-time push is skipped (it caused a divide-by-zero in WBP_HUD at PIE start) and the base-stat change fires `OnHealthChanged` with the real values. The XP curve is loaded at component initialization, so the bind-time `OnXPChanged` has its need.

## Relations
- [[systems-index]] — parent index.
- [[sys-player]] — the controller that creates the widget, the miner and game mode it listens to.
- [[sys-progression]] — the XP and level events it forwards.
- [[sys-turret]] — the turret charge events it forwards.
- [[sys-level-up]] — the card offer events it forwards and the choice calls it relays.
- [[sys-objectives]] — the run flow events it forwards.
- [[sys-hud-pressure]] — child: damage vignette, horde banner and alert-bar icon events.
