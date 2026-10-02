// Rogue Rigs - base class of the in-run HUD widget: health and game-over events fed by C++.

#include "UI/RRHUDWidget.h"

#include "AbilitySystemComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Player/RRGameMode.h"
#include "Player/RRMiner.h"

void URRHUDWidget::BindToMiner(ARRMiner* Miner)
{
	UnbindMiner();

	UAbilitySystemComponent* ASC = Miner ? Miner->GetAbilitySystemComponent() : nullptr;
	if (!ASC)
	{
		return;
	}
	BoundASC = ASC;
	HealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetHealthAttribute())
		.AddUObject(this, &URRHUDWidget::HandleHealthAttributeChanged);
	MaxHealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetMaxHealthAttribute())
		.AddUObject(this, &URRHUDWidget::HandleHealthAttributeChanged);
	BroadcastHealth();
}

void URRHUDWidget::BindToGameMode(ARRGameMode* GameMode)
{
	if (ARRGameMode* Previous = BoundGameMode.Get())
	{
		Previous->OnGameOver.RemoveDynamic(this, &URRHUDWidget::HandleGameOver);
	}
	BoundGameMode = GameMode;
	if (GameMode)
	{
		GameMode->OnGameOver.AddUniqueDynamic(this, &URRHUDWidget::HandleGameOver);
	}
}

void URRHUDWidget::NativeDestruct()
{
	UnbindMiner();
	BindToGameMode(nullptr);
	Super::NativeDestruct();
}

void URRHUDWidget::HandleHealthAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
	BroadcastHealth();
}

void URRHUDWidget::HandleGameOver()
{
	OnGameOver();
}

void URRHUDWidget::BroadcastHealth()
{
	if (const UAbilitySystemComponent* ASC = BoundASC.Get())
	{
		OnHealthChanged(ASC->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute()),
			ASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute()));
	}
}

void URRHUDWidget::UnbindMiner()
{
	if (UAbilitySystemComponent* ASC = BoundASC.Get())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetHealthAttribute()).Remove(HealthChangedHandle);
		ASC->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetMaxHealthAttribute()).Remove(MaxHealthChangedHandle);
	}
	BoundASC.Reset();
	HealthChangedHandle.Reset();
	MaxHealthChangedHandle.Reset();
}
