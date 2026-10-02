// Rogue Rigs - base class of the in-run HUD widget: health and game-over events fed by C++.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RRHUDWidget.generated.h"

class ARRGameMode;
class ARRMiner;
class UAbilitySystemComponent;
struct FOnAttributeChangeData;

/**
 * In-run HUD. Created by ARRPlayerController at BeginPlay (local player only) from its HUDWidgetClass. C++ binds it
 * to the miner's Health and MaxHealth attributes (BindToMiner) and to the game mode's OnGameOver (BindToGameMode);
 * the Blueprint subclass only implements the display events OnHealthChanged and OnGameOver. No gameplay logic.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Listens to Miner's Health and MaxHealth changes (replaces a previous miner) and calls OnHealthChanged once with the current values. Null unbinds. */
	void BindToMiner(ARRMiner* Miner);

	/** Listens to GameMode's OnGameOver (replaces a previous game mode). Null unbinds. */
	void BindToGameMode(ARRGameMode* GameMode);

protected:
	/** Removes every binding. */
	virtual void NativeDestruct() override;

	/** Display event: the miner's health changed (also called once on binding). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnHealthChanged(float Current, float Max);

	/** Display event: the run is over (the level restarts after the game mode's RestartDelay). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnGameOver();

	/** ASC callback for Health and MaxHealth: forwards the current values to OnHealthChanged. */
	void HandleHealthAttributeChanged(const FOnAttributeChangeData& ChangeData);

	/** Game mode OnGameOver callback: calls OnGameOver. */
	UFUNCTION()
	void HandleGameOver();

	/** Reads Health and MaxHealth on the bound ASC and calls OnHealthChanged. */
	void BroadcastHealth();

	/** Removes the Health and MaxHealth bindings from the bound ASC. */
	void UnbindMiner();

	/** ASC of the bound miner. */
	TWeakObjectPtr<UAbilitySystemComponent> BoundASC;

	/** Game mode whose OnGameOver is bound. */
	TWeakObjectPtr<ARRGameMode> BoundGameMode;

	/** Handle of the Health change binding on BoundASC. */
	FDelegateHandle HealthChangedHandle;

	/** Handle of the MaxHealth change binding on BoundASC. */
	FDelegateHandle MaxHealthChangedHandle;
};
