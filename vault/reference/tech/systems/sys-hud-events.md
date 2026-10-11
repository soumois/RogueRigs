---
id: sys-hud-events
description: "Gameplay events HUD panels bind (health, damage, weapon slots, ammo, alert-bar icons, horde relay, locator, XP) and the URRHUDPanelWidget base class."
parent: sys-hud
kind: doc
---
# HUD events and panel base

- responsibility: let each HUD sub-widget react to gameplay events fired by the system that owns the value, only on a real change (task-094-02, [[hud-refactor-missing-events]]); WBP_HUD becomes a placement root and the game binds only it.
- main_classes:
  - `ARRMiner`: `OnHealthChanged(float Current, float Max)` (ASC Health or MaxHealth change; once per change of the pair, never while Max is 0; first one at the base-stat application in BeginPlay); `OnDamaged(float Amount, float HealthFraction)` (Health went down; not a heal nor a MaxHealth clamp; payload of `OnPlayerDamaged`); `OnWeaponSlotChanged(int32 SlotIndex, const FRRWeaponSlotHUD& Slot)` (acquisition, ammo, reload start or end, turret charges or recharge start or end; compares every field but `ReloadProgress` and `RechargeProgress`; locked slots silent). Pure `GetCurrentHealth`, `GetMaxHealth`, `GetWeaponSlotHUD(Index)` for the bind-time read.
  - `ARRWeapon`: `OnAmmoChanged(ARRWeapon* Weapon, int32 Rounds, int32 MagazineSize)` (ASC Ammo or MagazineSize, rounded down; bound in `Acquire`, so the CSV values send the first one); pure `GetRounds`, `GetMagazineRounds`.
  - `URRRunFlowSubsystem`: `OnAlertBarEventsChanged(TArray<FRRAlertBarEvent>)` (recomputed on run-flow and horde events and in its tick, sent when `FRRAlertBarEvent::ListsDiffer` with `Config.AlertEventPositionTolerance` 0.002); `OnHordeIncoming(int32 HordeIndex, float SecondsUntil)` (relay of the registered spawner); `OnLocatorTargetChanged(ERRLocatorTarget, FVector)` (console drop, hack, shuttle landing, run end); `OnMinerRegistered`, `OnSpawnerRegistered`. `RegisterMiner` (end of the miner BeginPlay), `RegisterSpawner` (spawner BeginPlay, first one wins); pure `GetMiner`, `GetSpawner`, `GetAlertBarEvents`, `GetLocatorTarget`. Static `ComputeAlertBarEvents`, `EstimateBarPosition` (the HUD forwards to them).
  - `URRProgressionComponent::OnXPGained(float Amount)` (after `OnXPChanged`, XPGain applied).
  - Shared types moved to `UI/RRHUDTypes.h`: `ERRLocatorTarget`, `FRRObjectiveLocator`, `ERRAlertBarEventType`, `FRRAlertBarEvent`, `FRRWeaponSlotHUD` (`Make`, `HasSameState`).
  - `URRHUDPanelWidget : UUserWidget` (`UI/RRHUDPanelWidget.h`, Abstract): BlueprintNativeEvent hooks `BindToMiner(Miner, PreviousMiner)`, `BindToRunFlow`, `BindToSpawner`, `BindToGameMode`; pure `GetMiner`, `GetRunFlow`, `GetSpawner`, `GetGameMode`. Each hook runs once per distinct object (same object again ignored; null runs it once to clear).
- entry_points: `URRHUDWidget::BindToMiner / BindToRunFlow / BindToSpawner / BindToGameMode` hand their object to every panel of `GetPanels()` (`WidgetTree->ForEachWidget`, panels of the HUD tree only, not inside another panel). Late binding: the HUD listens to the run flow `OnMinerRegistered` / `OnSpawnerRegistered` and binds that object when none is bound, so a miner created after the HUD reaches the panels with its stats applied (fixes the MaxHealth = 0 timing of [[sys-hud]]).
- data: `FRRRunFlowConfig::AlertEventPositionTolerance` (0.002, game mode Blueprint).
- depends_on: [[sys-player]], [[sys-weapons]], [[sys-objectives]], [[sys-progression]].
- implements: [[hud-refactor-audit]] (event-driven sub-widgets).
- gotchas: per-frame values stay per frame (reload and recharge progress while `bReloading` / `bRecharging`, locator arrow placement while Target is not None, minimap, vignette decay). A hook override in Blueprint must unbind the previous object's events itself. The old `URRHUDWidget` display events and its NativeTick alert-bar refresh are unchanged until WBP_HUD is split. Tests `RogueRigs.HUD.Events*` ([[sys-automation-tests]]).

## Relations
- [[sys-hud]] — parent: the HUD root that binds the panels.
- [[hud-refactor-missing-events]] — the event list this implements.
- [[sys-hud-pressure]] — the older HUD pressure events these replace for panels.
- [[sys-automation-tests]] — the RogueRigs.HUD.Events* tests.
