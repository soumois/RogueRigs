// Rogue Rigs - C++ base of the run-lost screen (WBP_RunLost, death-screen gdd, task-484).

#include "UI/RRRunLostWidget.h"

#include "Camera/CameraComponent.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "RogueRigs.h"

void URRRunLostWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetIsFocusable(true);
	if (RetryButton)
	{
		RetryButton->OnClicked.AddUniqueDynamic(this, &URRRunLostWidget::HandleRetryClicked);
	}
	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddUniqueDynamic(this, &URRRunLostWidget::HandleMainMenuClicked);
	}
}

void URRRunLostWidget::ShowRunLost(const FRRRunLostRecap& InRecap)
{
	Recap = InRecap;
	bAppearanceFinished = false;
	// The widget itself holds the focus during the appearance, so the first key reaches its OnPreviewKeyDown (skip).
	SetFocus();
	OnRunLostShown(Recap);
}

bool URRRunLostWidget::Retry()
{
	URRRunLostSubsystem* RunLost = URRRunLostSubsystem::Get(this);
	return RunLost && RunLost->Retry();
}

bool URRRunLostWidget::ReturnToMainMenu()
{
	URRRunLostSubsystem* RunLost = URRRunLostSubsystem::Get(this);
	return RunLost && RunLost->ReturnToMainMenu();
}

void URRRunLostWidget::FinishAppearance()
{
	if (bAppearanceFinished)
	{
		return;
	}
	bAppearanceFinished = true;
	FocusRetry();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunLostWidget: appearance finished, focus on %s."), RetryButton ? TEXT("RetryButton") : TEXT("the screen"));
}

UWidget* URRRunLostWidget::GetRetryFocusWidget()
{
	return RetryButton ? static_cast<UWidget*>(RetryButton) : static_cast<UWidget*>(this);
}

void URRRunLostWidget::FocusRetry()
{
	GetRetryFocusWidget()->SetFocus();
}

bool URRRunLostWidget::SetLevelDesaturation(float Amount)
{
	const APlayerController* Controller = GetOwningPlayer();
	const AActor* ViewTarget = Controller ? Controller->GetViewTarget() : nullptr;
	UCameraComponent* Camera = ViewTarget ? ViewTarget->FindComponentByClass<UCameraComponent>() : nullptr;
	if (!Camera)
	{
		return false;
	}
	// Global saturation (W) scaled from the camera's own value: 1 - Amount of it, 0 = grey.
	FPostProcessSettings& Settings = Camera->PostProcessSettings;
	const FVector4 Base = Settings.bOverride_ColorSaturation ? Settings.ColorSaturation : FVector4(1.f, 1.f, 1.f, 1.f);
	if (!bSaturationSaved || SavedCamera.Get() != Camera)
	{
		SavedCamera = Camera;
		SavedSaturation = Base;
		bSavedOverride = Settings.bOverride_ColorSaturation;
		bSaturationSaved = true;
	}
	const float Keep = 1.f - FMath::Clamp(Amount, 0.f, 1.f);
	Settings.bOverride_ColorSaturation = bSavedOverride || Keep < 1.f;
	Settings.ColorSaturation = FVector4(SavedSaturation.X, SavedSaturation.Y, SavedSaturation.Z, SavedSaturation.W * Keep);
	return true;
}

bool URRRunLostWidget::RequestSkipAppearance()
{
	if (bAppearanceFinished)
	{
		return false;
	}
	OnSkipAppearanceRequested();
	return true;
}

void URRRunLostWidget::HandleRetryClicked()
{
	Retry();
}

void URRRunLostWidget::HandleMainMenuClicked()
{
	ReturnToMainMenu();
}
