---
id: sys-hud
description: "In-run HUD C++ base: URRHUDWidget display events (health, game over) fed by the miner ASC and the game mode, created by the player controller."
parent: systems-index
kind: doc
---
# HUD

- responsibility: feed the UI layer from C++: the miner's health and the game-over event. Layout, text and visuals live in the Blueprint subclass (BP agent, [[cpp-bp-split]]); no gameplay logic in the widget.
- main_classes: `URRHUDWidget : UUserWidget` (`Source/RogueRigs/UI/RRHUDWidget.h`, `Abstract`). `BlueprintImplementableEvent`s to implement in the Blueprint subclass: `OnHealthChanged(Current, Max)` (Health and MaxHealth of the miner) and `OnGameOver()`.
- entry_points:
  - `ARRPlayerController::BeginPlay` (local player only: skipped when `HUDWidgetClass` is none, the controller is not local or has no `ULocalPlayer`, so headless tests create no widget) creates `HUDWidgetClass`, adds it to the viewport, calls `BindToGameMode(auth game mode as ARRGameMode)` and `BindToMiner(possessed miner)`. `ARRPlayerController::OnPossess` rebinds to the new miner. `GetHUDWidget()` returns it.
  - `BindToMiner(ARRMiner*)`: subscribes to the ASC value-change delegates of `Health` and `MaxHealth` (either change calls `OnHealthChanged` with both current values) and calls `OnHealthChanged` once at bind time; null unbinds.
  - `BindToGameMode(ARRGameMode*)`: binds `ARRGameMode::OnGameOver` to `OnGameOver` ([[sys-player]] game over); null unbinds. `NativeDestruct` removes every binding.
- data: editable on the controller Blueprint: `HUDWidgetClass` (category HUD, none by default; set to the HUD widget Blueprint).
- depends_on: [[sys-player]] (miner, controller, game mode), [[sys-gas-core]] (health attribute set); module `UMG`.
- implements: [[miner]] (M1 "Game Over" text, health display data).
- gotchas: the game mode exists only on the server, so the game-over binding is single-player only. The first `OnHealthChanged` may report 0 / 0 if the controller begins play before the miner applied its CSV base stats; the base-stat change then fires it again with the real values.

## Relations
- [[systems-index]] — parent index.
- [[sys-player]] — the controller that creates the widget, the miner and game mode it listens to.
