---
id: hud-refactor-missing-events
description: "C++ events missing for the HUD split (name, payload, owner class) and the sub-widget binding hook; input of task 094-02."
parent: hud-refactor-audit
kind: note
---
# Missing C++ events (input of 094-02)

Already bindable by any widget (dynamic multicast): URRProgressionComponent OnXPChanged, OnLevelUp; URRLevelUpComponent OnUpgradeOfferShown, OnUpgradeOfferClosed; ARRGameMode OnGameOver; URRRunFlowSubsystem OnAlertChanged, OnObjectiveChanged, OnObjectiveProgressChanged, OnConsoleDropped, OnBonusChestDropped, OnExtractionReady, OnShuttleLanded, OnShuttleCountdown, OnRunEnded; ARRMiner OnWeaponAdded; URRAutoFireComponent OnShotFired, OnReloadStarted, OnReloadFinished; ARRTurretLauncher OnTurretChargesChanged; ARREnemySpawner OnHordeIncoming, OnHordeStarted.

Missing (dynamic multicast, so Blueprints can bind them):
1. ARRMiner::OnHealthChanged(float Current, float Max). Today only the HUD listens to the ASC attributes. Owner: ARRMiner.
2. ARRMiner::OnDamaged(float Amount, float HealthFraction). Same payload as the HUD OnPlayerDamaged; not fired for a MaxHealth clamp. Owner: ARRMiner.
3. ARRWeapon::OnAmmoChanged(ARRWeapon* Weapon, int32 Rounds, int32 MagazineSize), from the Ammo and MagazineSize attribute change on the weapon ASC (shot, refill, upgrade). Owner: ARRWeapon.
4. ARRMiner::OnWeaponSlotChanged(int32 SlotIndex, FRRWeaponSlotHUD Slot): one aggregate for the weapons widget (weapon acquired, ammo, reload start or end, charges), fed by event 3, the autofire reload events and OnTurretChargesChanged. Owner: ARRMiner.
5. URRRunFlowSubsystem::OnAlertBarEventsChanged(TArray<FRRAlertBarEvent> Events): move ComputeAlertBarEvents out of the HUD NativeTick; recompute on the alert and horde events, keep the position tolerance. Owner: URRRunFlowSubsystem.
6. URRRunFlowSubsystem::OnHordeIncoming(int32 HordeIndex, float SecondsUntil): relay of the spawner event, so the banner needs no spawner reference. Owner: URRRunFlowSubsystem.
7. URRRunFlowSubsystem::OnLocatorTargetChanged(ERRLocatorTarget Target, FVector WorldLocation): the arrow ticks only while Target is not None. Owner: URRRunFlowSubsystem.
8. Optional: URRProgressionComponent::OnXPGained(float Amount) for an XP pop; no current widget needs it.

Binding hook (not an event): sub-widgets still need the miner, run flow and spawner. Proposal: a C++ base class URRHUDPanelWidget with virtual BindToMiner, BindToRunFlow, BindToSpawner; URRHUDWidget calls them once on each child of that class in its widget tree, so code initialises only WBP_HUD.

Done in task-094-02 ([[sys-hud-events]]): all eight events and the binding hook, with these differences: `OnWeaponSlotChanged` passes the slot by const reference; the panel hooks also receive the previous object and a fourth hook `BindToGameMode` exists (game over); the run flow learns the spawner and miner through `RegisterSpawner` / `RegisterMiner` (`OnSpawnerRegistered` / `OnMinerRegistered`), which also gives the HUD its late binding. The HUD's own events and NativeTick are unchanged.

Stays per frame (no event): reload and recharge progress, locator arrow position and pulse, minimap scroll and markers, vignette decay after a hit.

## Relations
- [[hud-refactor-audit]] — parent, summary.
- [[hud-refactor-audit-core]] — features that use these events.
- [[hud-refactor-audit-run]] — features that use these events.
- [[sys-hud]] — the C++ base class that fires today's events.
- [[sys-hud-events]] — what 094-02 built.
