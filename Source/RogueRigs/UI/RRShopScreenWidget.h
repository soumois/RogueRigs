// Rogue Rigs - C++ base of the cards shop screen shown after the stats screen (task-317-01): shop access, refresh event and the
// Continue button.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "RRShopScreenWidget.generated.h"

class URRIntermissionShopSubsystem;

/**
 * Cards shop screen between two run levels (run-shop, intermission gdd; task-317-01). Created by ARRPlayerController
 * (ShopScreenWidgetClass) on URRRunTransitionSubsystem::OnShopScreenStarted (stats screen Next level, levels 1-3 only), game
 * paused, and removed on OnLoadingStarted. The offer is already drawn when the widget is created. Layout lives in the Blueprint
 * subclass: GetShop gives URRIntermissionShopSubsystem (offer, prices, balance, buy, reroll); implement OnShopRefreshed (called on
 * every shop change; also call it once from Construct); the Continue button calls Continue. No gameplay logic here.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRShopScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Continue button: URRRunTransitionSubsystem::Continue (shop closed, loading screen, next level). False outside the shop phase. */
	UFUNCTION(BlueprintCallable, Category = "Run|Transition")
	bool Continue();

	/** Recap of the level just finished (NextLevelIndex, NextLayoutName, ore figures). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	FRRIntermissionRecap GetRecap() const;

	/** The intermission shop (offer, prices, balance, buy, reroll); null without a game instance. */
	UFUNCTION(BlueprintPure, Category = "Run|Shop")
	URRIntermissionShopSubsystem* GetShop() const;

protected:
	/** Gives RerollButton and ContinueButton the mouse hover of ui-button-hover (task-485-05). */
	virtual void NativeOnInitialized() override;

	/** Binds the shop's OnShopChanged to OnShopRefreshed, then runs the Blueprint Construct. */
	virtual void NativeConstruct() override;

	/** Unbinds the shop. */
	virtual void NativeDestruct() override;

	/** Shop OnShopChanged callback: calls OnShopRefreshed. */
	UFUNCTION()
	void HandleShopChanged();

	/**
	 * Redraw the shop (5 cards with price and greyed state, balance, reroll cost and state): called whenever the shop changed
	 * (offer, buy, reroll, ore balance). Implement in Blueprint; also call it once from Construct (the offer already exists).
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Run|Shop")
	void OnShopRefreshed();
};
