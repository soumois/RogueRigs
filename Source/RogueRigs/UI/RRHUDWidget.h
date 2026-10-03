// Rogue Rigs - base class of the in-run HUD widget: health, XP, level-up, upgrade choice, alert bar, objective and game-over events fed by C++.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Objectives/RRConsole.h"
#include "Upgrades/RRUpgradeCard.h"
#include "RRHUDWidget.generated.h"

class ARRGameMode;
class ARRMiner;
class URRLevelUpComponent;
class URRProgressionComponent;
class URRRunFlowSubsystem;
class UAbilitySystemComponent;
struct FOnAttributeChangeData;

/**
 * In-run HUD. Created by ARRPlayerController at BeginPlay (local player only) from its HUDWidgetClass. C++ binds it
 * to the miner's Health and MaxHealth attributes, to its URRProgressionComponent and URRLevelUpComponent (BindToMiner),
 * and to the game mode's OnGameOver (BindToGameMode); the Blueprint subclass only implements the display events
 * OnHealthChanged, OnXPChanged, OnLevelUp, OnUpgradeOfferShown, OnUpgradeOfferClosed and OnGameOver, and calls
 * ChooseUpgrade / RequestReroll from its card and reroll buttons (forwarded to the level-up component).
 * OnHealthChanged is never called with Max <= 0 and OnXPChanged never with Needed <= 0, so the Blueprint can divide by
 * them. Run flow (BindToRunFlow, URRRunFlowSubsystem): OnAlertChanged, OnObjectiveChanged, OnObjectiveProgressChanged and
 * OnExtractionReady. No gameplay logic.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Listens to Miner's Health and MaxHealth changes and to its progression events (replaces a previous miner), then pushes
	 * the current values once (OnHealthChanged, OnXPChanged; each skipped while its maximum is still 0). Null unbinds.
	 */
	void BindToMiner(ARRMiner* Miner);

	/** Listens to GameMode's OnGameOver (replaces a previous game mode). Null unbinds. */
	void BindToGameMode(ARRGameMode* GameMode);

	/**
	 * Listens to the run flow's alert, objective, progress and extraction events (replaces a previous one), then pushes the
	 * current alert and objective once. Null unbinds.
	 */
	void BindToRunFlow(URRRunFlowSubsystem* RunFlow);

	/**
	 * Card button: picks card Index (0-based, order of OnUpgradeOfferShown's Cards) through the bound miner's
	 * URRLevelUpComponent::ChooseUpgrade. The next chained screen or OnUpgradeOfferClosed follows. False when refused.
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD|Upgrades")
	bool ChooseUpgrade(int32 Index);

	/** Reroll button: URRLevelUpComponent::RequestReroll; OnUpgradeOfferShown fires again with the new cards. False when refused. */
	UFUNCTION(BlueprintCallable, Category = "HUD|Upgrades")
	bool RequestReroll();

	/** True while the open choice screen can still be rerolled (reroll button enabled). */
	UFUNCTION(BlueprintPure, Category = "HUD|Upgrades")
	bool CanRerollUpgrade() const;

protected:
	/** Removes every binding. */
	virtual void NativeDestruct() override;

	/** Display event: the miner's health changed (also called once on binding). Max is always > 0. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnHealthChanged(float Current, float Max);

	/** Display event: the miner's XP changed (also called once on binding): XP in the level, XP to leave it (always > 0), level. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnXPChanged(float Current, float Needed, int32 Level);

	/** Display event: the miner reached NewLevel (once per level gained, before OnXPChanged). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnLevelUp(int32 NewLevel);

	/**
	 * Display event: show (or refresh) the choice screen with Cards (game paused, cursor visible). Called when a screen opens,
	 * after a reroll (bCanReroll false) and for each chained screen. Implement in Blueprint.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Upgrades")
	void OnUpgradeOfferShown(const TArray<FRRUpgradeCard>& Cards, bool bCanReroll);

	/** Display event: hide the choice screen (the last queued choice was made; the game resumes). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Upgrades")
	void OnUpgradeOfferClosed();

	/** Display event: the run is over (the level restarts after the game mode's RestartDelay). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnGameOver();

	/** Display event: the alert bar value (0..1) or its paused flag (a mandatory objective is pending) changed; also on binding. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Run")
	void OnAlertChanged(float Alert, bool bPaused);

	/**
	 * Display event: the objective changed (also on binding). bActive is false while no console exists (hide the objective
	 * panel); State Locked = destroy the marked rocks (MarkedRocksLeft of MarkedRocksTotal), Idle = go to the console, Hacking =
	 * stay in the zone, Complete = hacked. Implement in Blueprint (the text lives there).
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Run")
	void OnObjectiveChanged(bool bActive, ERRConsoleState State, int32 MarkedRocksLeft, int32 MarkedRocksTotal);

	/** Display event: the console hack progress (0..1) or hacking flag changed. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Run")
	void OnObjectiveProgressChanged(float Progress, bool bHacking);

	/** Display event: the alert bar is full and no objective is pending (extraction starts). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD|Run")
	void OnExtractionReady();

	/** Run flow OnAlertChanged callback. */
	UFUNCTION()
	void HandleAlertChanged(float Alert, bool bPaused);

	/** Run flow OnObjectiveChanged callback: calls OnObjectiveChanged with bActive = Console is valid. */
	UFUNCTION()
	void HandleObjectiveChanged(ARRConsole* Console, ERRConsoleState State, int32 MarkedRocksLeft, int32 MarkedRocksTotal);

	/** Run flow OnObjectiveProgressChanged callback. */
	UFUNCTION()
	void HandleObjectiveProgressChanged(float Progress, bool bHacking);

	/** Run flow OnExtractionReady callback. */
	UFUNCTION()
	void HandleExtractionReady();

	/** ASC callback for Health and MaxHealth: forwards the current values to OnHealthChanged. */
	void HandleHealthAttributeChanged(const FOnAttributeChangeData& ChangeData);

	/** Game mode OnGameOver callback: calls OnGameOver. */
	UFUNCTION()
	void HandleGameOver();

	/** Progression OnXPChanged callback: calls OnXPChanged unless Needed <= 0. */
	UFUNCTION()
	void HandleXPChanged(float Current, float Needed, int32 Level);

	/** Progression OnLevelUp callback: calls OnLevelUp. */
	UFUNCTION()
	void HandleLevelUp(int32 NewLevel);

	/** Level-up OnUpgradeOfferShown callback: calls OnUpgradeOfferShown. */
	UFUNCTION()
	void HandleUpgradeOfferShown(const TArray<FRRUpgradeCard>& Cards, bool bCanReroll);

	/** Level-up OnUpgradeOfferClosed callback: calls OnUpgradeOfferClosed. */
	UFUNCTION()
	void HandleUpgradeOfferClosed();

	/** Reads Health and MaxHealth on the bound ASC and calls OnHealthChanged, unless MaxHealth <= 0 (stats not applied yet). */
	void BroadcastHealth();

	/** Removes the Health, MaxHealth and progression bindings. */
	void UnbindMiner();

	/** ASC of the bound miner. */
	TWeakObjectPtr<UAbilitySystemComponent> BoundASC;

	/** Progression component of the bound miner. */
	TWeakObjectPtr<URRProgressionComponent> BoundProgression;

	/** Level-up flow of the bound miner. */
	TWeakObjectPtr<URRLevelUpComponent> BoundLevelUp;

	/** Game mode whose OnGameOver is bound. */
	TWeakObjectPtr<ARRGameMode> BoundGameMode;

	/** Run flow whose events are bound. */
	TWeakObjectPtr<URRRunFlowSubsystem> BoundRunFlow;

	/** Handle of the Health change binding on BoundASC. */
	FDelegateHandle HealthChangedHandle;

	/** Handle of the MaxHealth change binding on BoundASC. */
	FDelegateHandle MaxHealthChangedHandle;
};
