// Rogue Rigs - full-screen black fade of a level transition (task-318-02), drawn above every transition widget.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RRScreenFadeWidget.generated.h"

class UImage;

/**
 * Native full-screen black image, no Blueprint (the fade is engine plumbing, cpp-bp-split): the widget tree (one UImage filling
 * the viewport) is built in NativeOnInitialized. ARRPlayerController creates it on URRRunTransitionSubsystem::OnScreenFadeStarted,
 * adds it at ScreenFadeZOrder and sets SetFadeAlpha from GetScreenFadeAlpha every frame on the core ticker (runs while paused).
 * Not hit-testable.
 */
UCLASS()
class ROGUERIGS_API URRScreenFadeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Black opacity 0-1 (0 = nothing drawn). */
	void SetFadeAlpha(float Alpha);

protected:
	/** Builds the tree: a black UImage as root; not hit-testable. */
	virtual void NativeOnInitialized() override;

	/** The full-screen image. */
	UPROPERTY(Transient)
	TObjectPtr<UImage> FadeImage;
};
