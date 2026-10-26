// Rogue Rigs - C++ base of the loading screen between two run levels (WBP_Loading, task-206-05; progress task-318-02).

#include "UI/RRLoadingScreenWidget.h"

void URRLoadingScreenWidget::ShowLoading(int32 LevelIndex, const FString& LayoutName)
{
	OnLoadingShown(LevelIndex, LayoutName);
	OnLoadingStageChanged(GetLoadingStage());
	OnLoadingProgressChanged(GetLoadingProgress());
}

ERRRunLoadingStage URRLoadingScreenWidget::GetLoadingStage() const
{
	const URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	return Transition ? Transition->GetLoadingStage() : ERRRunLoadingStage::None;
}

float URRLoadingScreenWidget::GetLoadingProgress() const
{
	const URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	return Transition ? Transition->GetLoadingProgress() : 0.f;
}

void URRLoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this))
	{
		Transition->OnLoadingStageChanged.AddUniqueDynamic(this, &URRLoadingScreenWidget::HandleLoadingStageChanged);
		Transition->OnNextLevelProgressChanged.AddUniqueDynamic(this, &URRLoadingScreenWidget::HandleNextLevelProgressChanged);
		BoundTransition = Transition;
	}
}

void URRLoadingScreenWidget::NativeDestruct()
{
	if (URRRunTransitionSubsystem* Transition = BoundTransition.Get())
	{
		Transition->OnLoadingStageChanged.RemoveDynamic(this, &URRLoadingScreenWidget::HandleLoadingStageChanged);
		Transition->OnNextLevelProgressChanged.RemoveDynamic(this, &URRLoadingScreenWidget::HandleNextLevelProgressChanged);
	}
	BoundTransition.Reset();
	Super::NativeDestruct();
}

void URRLoadingScreenWidget::HandleLoadingStageChanged(ERRRunLoadingStage Stage)
{
	OnLoadingStageChanged(Stage);
}

void URRLoadingScreenWidget::HandleNextLevelProgressChanged(float Progress)
{
	OnLoadingProgressChanged(GetLoadingProgress());
}

int32 URRLoadingScreenWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const int32 MaxLayerId = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	if (!bFirstPaintReported)
	{
		bFirstPaintReported = true;
		if (URRRunTransitionSubsystem* Transition = BoundTransition.Get())
		{
			Transition->NotifyLoadingScreenPainted(this);
		}
	}
	return MaxLayerId;
}
