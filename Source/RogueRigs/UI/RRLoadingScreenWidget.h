// Rogue Rigs - C++ base of the loading screen between two run levels (WBP_Loading, task-206-05).

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "RRLoadingScreenWidget.generated.h"

/**
 * Loading screen of a level transition (intermission gdd: covers the layout, the map load and the pool prewarm). A widget
 * does not survive OpenLevel, so ARRPlayerController (LoadingScreenWidgetClass) creates one instance on
 * URRRunTransitionSubsystem::OnLoadingStarted in the old world and another at its BeginPlay in the new world while
 * IsLevelLoading; each is removed on OnLevelReady. Main menu New Run (task-379-01): ARRMainMenuPlayerController creates the
 * old-world instance (ARRMainMenuGameMode::LoadingScreenWidgetClass). Layout lives in the Blueprint subclass: implement
 * OnLoadingShown and OnLoadingStageChanged; read IsLoadingProgressMeasured to choose the bar or the animation. No gameplay logic here.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRLoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Calls OnLoadingShown with the level and layout being loaded (the controller calls it right after AddToViewport). */
	void ShowLoading(int32 LevelIndex, const FString& LayoutName);

	/** Current loading step (None when not loading). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	ERRRunLoadingStage GetLoadingStage() const;

	/**
	 * Progress 0-1 for the progress bar (task-318-02): URRRunTransitionSubsystem::GetLoadingProgress (next level preparation in
	 * this world; 0 on the map-travel fallback). Readable from a binding or Tick, also pushed by OnLoadingProgressChanged.
	 */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	float GetLoadingProgress() const;

	/**
	 * True when GetLoadingProgress is measured (task-379-01): only on the in-world next level preparation path between levels.
	 * False on the main menu New Run and on the between-levels map-travel fallback (a map load: hide the bar, play the animation).
	 * Valid when OnLoadingShown fires (URRRunTransitionSubsystem::IsLoadingProgressMeasured).
	 */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	bool IsLoadingProgressMeasured() const;

protected:
	/** Binds URRRunTransitionSubsystem::OnLoadingStageChanged. */
	virtual void NativeConstruct() override;

	/** Unbinds it. */
	virtual void NativeDestruct() override;

	/**
	 * Reports the first paint of this widget to URRRunTransitionSubsystem::NotifyLoadingScreenPainted (task-208-01: the
	 * travel waits until the old-world loading screen has been drawn). Later paints are not reported.
	 */
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	/** Subsystem OnLoadingStageChanged callback: forwards to OnLoadingStageChanged. */
	UFUNCTION()
	void HandleLoadingStageChanged(ERRRunLoadingStage Stage);

	/** Subsystem OnNextLevelProgressChanged callback: forwards GetLoadingProgress to OnLoadingProgressChanged. */
	UFUNCTION()
	void HandleNextLevelProgressChanged(float Progress);

	/** Loading progress changed (0-1, task-318-02); also once at ShowLoading. Implement in Blueprint (progress bar percent). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Run|Transition")
	void OnLoadingProgressChanged(float Progress);

	/** Show the loading screen for level LevelIndex (layout LayoutName). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Run|Transition")
	void OnLoadingShown(int32 LevelIndex, const FString& LayoutName);

	/** Loading step changed (waiting for the layout, opening the map, preparing the level); also once at ShowLoading. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Run|Transition")
	void OnLoadingStageChanged(ERRRunLoadingStage Stage);

	/** Subsystem bound at NativeConstruct. */
	TWeakObjectPtr<URRRunTransitionSubsystem> BoundTransition;

	/** True once the first paint was reported (mutable: NativePaint is const). */
	mutable bool bFirstPaintReported = false;
};
