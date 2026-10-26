// Rogue Rigs - C++ base of the end-of-level stats screen (WBP_Intermission, task-206-04, task-317-01): level recap and the Next
// level button.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "RRIntermissionWidget.generated.h"

class URRIntermissionShopSubsystem;

/**
 * End-of-level stats screen (intermission gdd, task-317-01). Created by ARRPlayerController (IntermissionWidgetClass) on
 * URRRunTransitionSubsystem::OnIntermissionStarted, game paused, and removed on OnShopScreenStarted, OnRunWonScreenStarted or
 * OnLoadingStarted. Layout and text live in the Blueprint subclass: implement OnRecapShown (recap: ore, kills per robot type,
 * damage per weapon, bRunWon); the Next level button calls NextLevel (levels 1-3: the cards shop screen, URRShopScreenWidget;
 * last level: the run won end screen). The shop helpers below are legacy (task-316 single screen): the shop now lives on
 * URRShopScreenWidget. No gameplay logic here.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRIntermissionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Stores Recap and calls OnRecapShown (the controller calls it right after AddToViewport). */
	void ShowRecap(const FRRIntermissionRecap& InRecap);

	/** Next level button: URRRunTransitionSubsystem::NextLevel (shop screen, or the run won end screen). False outside the stats phase. */
	UFUNCTION(BlueprintCallable, Category = "Run|Transition")
	bool NextLevel();

	/** True when the recap is the last level's (run won): the button leads to the run won end screen, label it accordingly. */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	bool IsRunWonRecap() const { return GetRecap().bRunWon; }

	/** Legacy button (task-316 single screen): NextLevel on the stats phase, else URRRunTransitionSubsystem::Continue. */
	UFUNCTION(BlueprintCallable, Category = "Run|Transition")
	bool Continue();

	/** Recap given to ShowRecap (also readable at Construct: the subsystem's recap until ShowRecap runs). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	FRRIntermissionRecap GetRecap() const;

	/** Legacy (use URRShopScreenWidget): the intermission shop (offer, prices, balance, buy, reroll); null without a game instance. */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	URRIntermissionShopSubsystem* GetShop() const;

protected:
	/** Binds the shop's OnShopChanged to OnShopRefreshed, then runs the Blueprint Construct. */
	virtual void NativeConstruct() override;

	/** Unbinds the shop. */
	virtual void NativeDestruct() override;

	/** Shop OnShopChanged callback: calls OnShopRefreshed. */
	UFUNCTION()
	void HandleShopChanged();

	/**
	 * Redraw the shop part (5 cards with price and greyed state, balance, reroll cost and state): called whenever the shop
	 * changed (offer, buy, reroll, ore balance). Implement in Blueprint; also call it once from Construct or OnRecapShown.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Run|Shop")
	void OnShopRefreshed();

	/**
	 * Fill the recap texts: finished level, ore secured this level, run total, level count, KillsByRobotType (DisplayName, Count),
	 * DamageByWeapon (DisplayName, Damage), bRunWon. Implement in Blueprint.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Run|Transition")
	void OnRecapShown(const FRRIntermissionRecap& InRecap);

	/** Recap given to ShowRecap. */
	UPROPERTY(Transient)
	FRRIntermissionRecap Recap;

	/** True once ShowRecap ran. */
	bool bHasRecap = false;
};
