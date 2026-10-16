// Rogue Rigs - base class of the HUD sub-widgets (panels placed in WBP_HUD): receives the miner, run flow, spawner, game mode and the HUD root itself from the HUD root once each.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RRHUDPanelWidget.generated.h"

class ARREnemySpawner;
class ARRGameMode;
class ARRMiner;
class URRHUDWidget;
class URRRunFlowSubsystem;

/**
 * HUD panel (sub-widget of the HUD root, e.g. WBP_HealthStatus, WBP_Weapons). The game creates and binds only the HUD root
 * (URRHUDWidget); the root finds every panel of this class in its widget tree and hands each one the objects it binds to:
 * BindToMiner, BindToRunFlow, BindToSpawner, BindToGameMode, and itself through BindToHUD. Each hook runs once per distinct object (a repeated bind with the
 * same object is ignored; a new object calls it again, null = unbind and clear the display). The Blueprint subclass binds the
 * C++ events it needs (ARRMiner::OnHealthChanged, URRRunFlowSubsystem::OnAlertBarEventsChanged...) inside the hook and reads the
 * current values there; it never polls in Tick. No gameplay logic.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRHUDPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Called by the HUD root: stores Miner and calls BindToMiner when it differs from the stored one. */
	void SetMiner(ARRMiner* Miner);

	/** Called by the HUD root: stores RunFlow and calls BindToRunFlow when it differs from the stored one. */
	void SetRunFlow(URRRunFlowSubsystem* RunFlow);

	/** Called by the HUD root: stores Spawner and calls BindToSpawner when it differs from the stored one. */
	void SetSpawner(ARREnemySpawner* Spawner);

	/** Called by the HUD root: stores GameMode and calls BindToGameMode when it differs from the stored one. */
	void SetGameMode(ARRGameMode* GameMode);

	/** Called by the HUD root (its initialization and construct): stores HUD and calls BindToHUD when it differs from the stored one. */
	void SetHUD(URRHUDWidget* HUD);

	/** HUD root this panel lives in (null before the root initialized). Its minimap and locator delegates are bound in BindToHUD. */
	UFUNCTION(BlueprintPure, Category = "HUD|Panel")
	URRHUDWidget* GetHUD() const { return BoundHUD.Get(); }

	/** Miner this panel is bound to (null before binding or after the miner was destroyed). */
	UFUNCTION(BlueprintPure, Category = "HUD|Panel")
	ARRMiner* GetMiner() const { return BoundMiner.Get(); }

	/** Run flow this panel is bound to (null before binding). */
	UFUNCTION(BlueprintPure, Category = "HUD|Panel")
	URRRunFlowSubsystem* GetRunFlow() const { return BoundRunFlow.Get(); }

	/** Enemy spawner this panel is bound to (null before binding or in a level without spawner). */
	UFUNCTION(BlueprintPure, Category = "HUD|Panel")
	ARREnemySpawner* GetSpawner() const { return BoundSpawner.Get(); }

	/** Game mode this panel is bound to (null before binding, or on a client). */
	UFUNCTION(BlueprintPure, Category = "HUD|Panel")
	ARRGameMode* GetGameMode() const { return BoundGameMode.Get(); }

	/** Number of BindToMiner calls received (tests: checks a panel is bound once). */
	int32 GetMinerBindCount() const { return MinerBindCount; }

	/** Number of BindToRunFlow calls received (tests). */
	int32 GetRunFlowBindCount() const { return RunFlowBindCount; }

	/** Number of BindToSpawner calls received (tests). */
	int32 GetSpawnerBindCount() const { return SpawnerBindCount; }

	/** Number of BindToHUD calls received (tests). */
	int32 GetHUDBindCount() const { return HUDBindCount; }

protected:
	/**
	 * Hook: bind the miner events this panel shows (OnHealthChanged, OnDamaged, OnWeaponSlotChanged, its progression...) and read
	 * their current values. Miner is the new miner (null = none: clear). Unbinding PreviousMiner's events is the hook's job
	 * (GetMiner already returns the new one); dynamic bindings on a destroyed miner vanish with it.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HUD|Panel")
	void BindToMiner(ARRMiner* Miner, ARRMiner* PreviousMiner);

	/** Hook: bind the run flow events this panel shows (alert, objective, alert-bar icons, horde relay, locator) and read the current values. */
	UFUNCTION(BlueprintNativeEvent, Category = "HUD|Panel")
	void BindToRunFlow(URRRunFlowSubsystem* RunFlow, URRRunFlowSubsystem* PreviousRunFlow);

	/** Hook: bind the spawner horde events (most panels use the run flow relays instead). */
	UFUNCTION(BlueprintNativeEvent, Category = "HUD|Panel")
	void BindToSpawner(ARREnemySpawner* Spawner, ARREnemySpawner* PreviousSpawner);

	/** Hook: bind ARRGameMode::OnGameOver (single player: the game mode exists on the server only). */
	UFUNCTION(BlueprintNativeEvent, Category = "HUD|Panel")
	void BindToGameMode(ARRGameMode* GameMode, ARRGameMode* PreviousGameMode);

	/** Native default: counts the call. Overrides call Super. */
	virtual void BindToMiner_Implementation(ARRMiner* Miner, ARRMiner* PreviousMiner);

	/**
	 * Native bind step, run by SetMiner just before the BindToMiner hook: a C++ panel binds its own miner events here. Kept apart
	 * from BindToMiner_Implementation because a Blueprint override of BindToMiner replaces the native implementation (it would run
	 * only if the graph called its parent). Same arguments as BindToMiner. Default: nothing.
	 */
	virtual void NativeBindToMiner(ARRMiner* Miner, ARRMiner* PreviousMiner) {}

	/** Native default: counts the call. Overrides call Super. */
	virtual void BindToRunFlow_Implementation(URRRunFlowSubsystem* RunFlow, URRRunFlowSubsystem* PreviousRunFlow);

	/** Native default: counts the call. Overrides call Super. */
	virtual void BindToSpawner_Implementation(ARREnemySpawner* Spawner, ARREnemySpawner* PreviousSpawner);

	/** Native default: nothing. */
	virtual void BindToGameMode_Implementation(ARRGameMode* GameMode, ARRGameMode* PreviousGameMode) {}

	/**
	 * Hook: the HUD root this panel lives in (once per root). Bind its delegates here (URRHUDWidget::OnMinimapTextureReady,
	 * OnMinimapViewChanged, OnLocatorViewChanged) and read the current values (GetMinimapTexture, GetMinimapState,
	 * GetObjectiveLocator): the texture may already exist. Replaces any GetAllWidgetsOfClass lookup of the root.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HUD|Panel")
	void BindToHUD(URRHUDWidget* HUD, URRHUDWidget* PreviousHUD);

	/** Native default: counts the call. Overrides call Super. */
	virtual void BindToHUD_Implementation(URRHUDWidget* HUD, URRHUDWidget* PreviousHUD);

	/** Miner handed by the HUD root. */
	TWeakObjectPtr<ARRMiner> BoundMiner;

	/** Run flow handed by the HUD root. */
	TWeakObjectPtr<URRRunFlowSubsystem> BoundRunFlow;

	/** Spawner handed by the HUD root. */
	TWeakObjectPtr<ARREnemySpawner> BoundSpawner;

	/** Game mode handed by the HUD root. */
	TWeakObjectPtr<ARRGameMode> BoundGameMode;

	/** HUD root handed by itself. */
	TWeakObjectPtr<URRHUDWidget> BoundHUD;

	/** True once SetMiner was called (a first null still runs the hook so the panel clears). */
	bool bMinerSet = false;

	/** True once SetRunFlow was called. */
	bool bRunFlowSet = false;

	/** True once SetSpawner was called. */
	bool bSpawnerSet = false;

	/** True once SetGameMode was called. */
	bool bGameModeSet = false;

	/** BindToMiner calls received. */
	int32 MinerBindCount = 0;

	/** BindToRunFlow calls received. */
	int32 RunFlowBindCount = 0;

	/** BindToSpawner calls received. */
	int32 SpawnerBindCount = 0;

	/** BindToHUD calls received. */
	int32 HUDBindCount = 0;
};
