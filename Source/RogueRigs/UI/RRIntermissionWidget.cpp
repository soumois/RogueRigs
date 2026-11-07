// Rogue Rigs - C++ base of the end-of-level stats screen (WBP_Intermission, task-206-04, task-317-01): level recap and the Next
// level button.

#include "UI/RRIntermissionWidget.h"

#include "Run/RRIntermissionShopSubsystem.h"
#include "UI/RRButtonHover.h"

URRIntermissionShopSubsystem* URRIntermissionWidget::GetShop() const
{
	return URRIntermissionShopSubsystem::Get(this);
}

void URRIntermissionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	URRButtonHoverSubsystem::AttachNamed(this, {TEXT("ContinueButton")});
}

void URRIntermissionWidget::NativeConstruct()
{
	if (URRIntermissionShopSubsystem* Shop = GetShop())
	{
		Shop->OnShopChanged.AddUniqueDynamic(this, &URRIntermissionWidget::HandleShopChanged);
	}
	Super::NativeConstruct();
}

void URRIntermissionWidget::NativeDestruct()
{
	if (URRIntermissionShopSubsystem* Shop = GetShop())
	{
		Shop->OnShopChanged.RemoveDynamic(this, &URRIntermissionWidget::HandleShopChanged);
	}
	Super::NativeDestruct();
}

void URRIntermissionWidget::HandleShopChanged()
{
	OnShopRefreshed();
}

void URRIntermissionWidget::ShowRecap(const FRRIntermissionRecap& InRecap)
{
	Recap = InRecap;
	bHasRecap = true;
	OnRecapShown(Recap);
}

bool URRIntermissionWidget::NextLevel()
{
	URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	return Transition && Transition->NextLevel();
}

bool URRIntermissionWidget::Continue()
{
	URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	if (!Transition)
	{
		return false;
	}
	return Transition->IsInStatsScreen() ? Transition->NextLevel() : Transition->Continue();
}

FRRIntermissionRecap URRIntermissionWidget::GetRecap() const
{
	if (bHasRecap)
	{
		return Recap;
	}
	const URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	return Transition ? Transition->GetIntermissionRecap() : FRRIntermissionRecap();
}
