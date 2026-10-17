// Rogue Rigs - C++ base of the intermission screen (WBP_Intermission, task-206-04): level recap and the Continue button.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "RRIntermissionWidget.generated.h"

/**
 * Intermission screen between two run levels (intermission gdd). Created by ARRPlayerController (IntermissionWidgetClass) on
 * URRRunTransitionSubsystem::OnIntermissionStarted, game paused, and removed on OnLoadingStarted. Layout and text live in the
 * Blueprint subclass: implement OnRecapShown; the Continue button calls Continue. No gameplay logic here.
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

protected:
	/** Fill the recap texts: finished level, ore secured this level, run total, level count. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Run|Transition")
	void OnRecapShown(const FRRIntermissionRecap& InRecap);

	/** Recap given to ShowRecap. */
	UPROPERTY(Transient)
	FRRIntermissionRecap Recap;

	/** True once ShowRecap ran. */
	bool bHasRecap = false;
};
