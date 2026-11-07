// Rogue Rigs - C++ base of the cards shop screen shown after the stats screen (task-317-01): shop access, refresh event and the
// Continue button.

#include "UI/RRShopScreenWidget.h"

#include "Run/RRIntermissionShopSubsystem.h"
#include "UI/RRButtonHover.h"

URRIntermissionShopSubsystem* URRShopScreenWidget::GetShop() const
{
	return URRIntermissionShopSubsystem::Get(this);
}

void URRShopScreenWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// A greyed Reroll (SetIsEnabled false) shows no hover: the subsystem skips disabled buttons.
	URRButtonHoverSubsystem::AttachNamed(this, {TEXT("RerollButton"), TEXT("ContinueButton")});
}

void URRShopScreenWidget::NativeConstruct()
{
	if (URRIntermissionShopSubsystem* Shop = GetShop())
	{
		Shop->OnShopChanged.AddUniqueDynamic(this, &URRShopScreenWidget::HandleShopChanged);
	}
	Super::NativeConstruct();
}

void URRShopScreenWidget::NativeDestruct()
{
	if (URRIntermissionShopSubsystem* Shop = GetShop())
	{
		Shop->OnShopChanged.RemoveDynamic(this, &URRShopScreenWidget::HandleShopChanged);
	}
	Super::NativeDestruct();
}

void URRShopScreenWidget::HandleShopChanged()
{
	OnShopRefreshed();
}

bool URRShopScreenWidget::Continue()
{
	URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	return Transition && Transition->Continue();
}

FRRIntermissionRecap URRShopScreenWidget::GetRecap() const
{
	const URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	return Transition ? Transition->GetIntermissionRecap() : FRRIntermissionRecap();
}
