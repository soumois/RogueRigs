// Rogue Rigs - base class of the in-run HUD widget: health, XP, level-up and game-over events fed by C++.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RRHUDWidget.generated.h"

class ARRGameMode;
class ARRMiner;
class URRProgressionComponent;
class UAbilitySystemComponent;
struct FOnAttributeChangeData;

/**
 * In-run HUD. Created by ARRPlayerController at BeginPlay (local player only) from its HUDWidgetClass. C++ binds it
 * to the miner's Health and MaxHealth attributes and to its URRProgressionComponent (BindToMiner), and to the game
 * mode's OnGameOver (BindToGameMode); the Blueprint subclass only implements the display events OnHealthChanged,
 * OnXPChanged, OnLevelUp and OnGameOver. OnHealthChanged is never called with Max <= 0 and OnXPChanged never with
 * Needed <= 0, so the Blueprint can divide by them. No gameplay logic.
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

	/** Display event: the run is over (the level restarts after the game mode's RestartDelay). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnGameOver();

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

	/** Reads Health and MaxHealth on the bound ASC and calls OnHealthChanged, unless MaxHealth <= 0 (stats not applied yet). */
	void BroadcastHealth();

	/** Removes the Health, MaxHealth and progression bindings. */
	void UnbindMiner();

	/** ASC of the bound miner. */
	TWeakObjectPtr<UAbilitySystemComponent> BoundASC;

	/** Progression component of the bound miner. */
	TWeakObjectPtr<URRProgressionComponent> BoundProgression;

	/** Game mode whose OnGameOver is bound. */
	TWeakObjectPtr<ARRGameMode> BoundGameMode;

	/** Handle of the Health change binding on BoundASC. */
	FDelegateHandle HealthChangedHandle;

	/** Handle of the MaxHealth change binding on BoundASC. */
	FDelegateHandle MaxHealthChangedHandle;
};
