// Rogue Rigs - C++ base of the cards shop screen shown after the stats screen (task-317-01): shop access, refresh event and the
// Continue button.

#include "UI/RRShopScreenWidget.h"

#include "Components/Button.h"
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
	for (int32 Index = 0; Index < 5; ++Index)
	{
		UUserWidget* Card = Cast<UUserWidget>(GetWidgetFromName(*FString::Printf(TEXT("ShopCard%d"), Index)));
		if (!Card)
		{
			continue;
		}
		if (UButton* Buy = Cast<UButton>(Card->GetWidgetFromName(TEXT("BuyButton"))))
		{
			URRButtonHoverSubsystem::Attach(Buy, true, Card->GetWidgetFromName(TEXT("BuyPlateBox")));
		}
	}
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
