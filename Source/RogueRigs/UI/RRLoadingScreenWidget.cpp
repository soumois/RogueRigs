// Rogue Rigs - C++ base of the loading screen between two run levels (WBP_Loading, task-206-05).

#include "UI/RRLoadingScreenWidget.h"

void URRLoadingScreenWidget::ShowLoading(int32 LevelIndex, const FString& LayoutName)
{
	OnLoadingShown(LevelIndex, LayoutName);
	OnLoadingStageChanged(GetLoadingStage());
}

ERRRunLoadingStage URRLoadingScreenWidget::GetLoadingStage() const
{
	const URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	return Transition ? Transition->GetLoadingStage() : ERRRunLoadingStage::None;
}

void URRLoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this))
	{
		Transition->OnLoadingStageChanged.AddUniqueDynamic(this, &URRLoadingScreenWidget::HandleLoadingStageChanged);
		BoundTransition = Transition;
	}
}

void URRLoadingScreenWidget::NativeDestruct()
{
	if (URRRunTransitionSubsystem* Transition = BoundTransition.Get())
	{
		Transition->OnLoadingStageChanged.RemoveDynamic(this, &URRLoadingScreenWidget::HandleLoadingStageChanged);
	}
	BoundTransition.Reset();
	Super::NativeDestruct();
}

void URRLoadingScreenWidget::HandleLoadingStageChanged(ERRRunLoadingStage Stage)
{
	OnLoadingStageChanged(Stage);
}
