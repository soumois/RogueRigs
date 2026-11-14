// Rogue Rigs - C++ base of the run-lost screen (WBP_RunLost, death-screen gdd, task-484).

#include "UI/RRRunLostWidget.h"

#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "RogueRigs.h"
#include "UI/RRButtonHover.h"

URRRunLostWidget::URRRunLostWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EntranceParts.TitleGroup = {TEXT("Title"), TEXT("TitleBarL"), TEXT("TitleBarR"), TEXT("Subtitle"), TEXT("SubLineL"), TEXT("SubLineR")};
	EntranceParts.FrameGroup = {TEXT("RecapBlock")};
	EntranceParts.Buttons = {TEXT("RetryButton"), TEXT("MainMenuButton")};
}

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
	// Mouse hover of both plates (ui-button-hover, task-485-03); the invisible SkipButton gets none.
	URRButtonHoverSubsystem::Attach(RetryButton);
	URRButtonHoverSubsystem::Attach(MainMenuButton);
	// Shared entrance (task-547): the end of the appearance, played or skipped, gives the focus to Retry.
	Entrance = NewObject<URRScreenEntrance>(this);
	Entrance->Setup(this, EntranceParts);
	Entrance->OnFinished.BindUObject(this, &URRRunLostWidget::FinishAppearance);
}

void URRRunLostWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (Entrance)
	{
		Entrance->Tick(InDeltaTime);
	}
}

void URRRunLostWidget::NativeDestruct()
{
	if (Entrance)
	{
		Entrance->Restore();
	}
	Super::NativeDestruct();
}

void URRRunLostWidget::ShowRunLost(const FRRRunLostRecap& InRecap)
{
	Recap = InRecap;
	bAppearanceFinished = false;
	// The widget itself holds the focus during the appearance, so the first key reaches its OnPreviewKeyDown (skip).
	SetFocus();
	OnRunLostShown(Recap);
	if (Entrance)
	{
		Entrance->Start();
	}
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
	if (Entrance && Entrance->IsPlaying())
	{
		Entrance->Skip();
	}
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
	return Entrance && Entrance->SetLevelDesaturation(Amount);
}

bool URRRunLostWidget::RequestSkipAppearance()
{
	if (bAppearanceFinished)
	{
		return false;
	}
	if (Entrance)
	{
		Entrance->Skip();
	}
	FinishAppearance();
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
