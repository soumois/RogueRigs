// Rogue Rigs - C++ base of the intermission screen (WBP_Intermission, task-206-04): level recap and the Continue button.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "RRIntermissionWidget.generated.h"

class URRIntermissionShopSubsystem;

/**
 * Intermission screen between two run levels (intermission gdd). Created by ARRPlayerController (IntermissionWidgetClass) on
 * URRRunTransitionSubsystem::OnIntermissionStarted, game paused, and removed on OnLoadingStarted. Layout and text live in the
 * Blueprint subclass: implement OnRecapShown; the Continue button calls Continue. Shop (task-316-01): GetShop gives
 * URRIntermissionShopSubsystem (offer, prices, buy, reroll); implement OnShopRefreshed, called on every shop change. No gameplay
 * logic here.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRIntermissionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Stores Recap and calls OnRecapShown (the controller calls it right after AddToViewport). */
	void ShowRecap(const FRRIntermissionRecap& InRecap);

	/** Continue button: URRRunTransitionSubsystem::Continue (loading screen, next level). False outside the intermission. */
	UFUNCTION(BlueprintCallable, Category = "Run|Transition")
	bool Continue();

	/** Recap given to ShowRecap (also readable at Construct: the subsystem's recap until ShowRecap runs). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	FRRIntermissionRecap GetRecap() const;

	/** The intermission shop (offer, prices, balance, buy, reroll); null without a game instance. */
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

	/** Fill the recap texts: finished level, ore secured this level, run total, level count. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Run|Transition")
	void OnRecapShown(const FRRIntermissionRecap& InRecap);

	/** Recap given to ShowRecap. */
	UPROPERTY(Transient)
	FRRIntermissionRecap Recap;

	/** True once ShowRecap ran. */
	bool bHasRecap = false;
};
