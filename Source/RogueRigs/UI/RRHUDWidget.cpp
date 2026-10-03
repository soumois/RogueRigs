// Rogue Rigs - base class of the in-run HUD widget: health, XP, level-up and game-over events fed by C++.

#include "UI/RRHUDWidget.h"

#include "AbilitySystemComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Player/RRGameMode.h"
#include "Player/RRMiner.h"
#include "Progression/RRProgressionComponent.h"

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

	if (URRProgressionComponent* Progression = Miner->GetProgression())
	{
		BoundProgression = Progression;
		Progression->OnXPChanged.AddUniqueDynamic(this, &URRHUDWidget::HandleXPChanged);
		Progression->OnLevelUp.AddUniqueDynamic(this, &URRHUDWidget::HandleLevelUp);
		HandleXPChanged(Progression->GetXP(), Progression->GetXPToNextLevel(), Progression->GetLevel());
	}
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

void URRHUDWidget::HandleXPChanged(float Current, float Needed, int32 Level)
{
	// The Blueprint divides by Needed: skip until the curve gives a need.
	if (Needed > 0.f)
	{
		OnXPChanged(Current, Needed, Level);
	}
}

void URRHUDWidget::HandleLevelUp(int32 NewLevel)
{
	OnLevelUp(NewLevel);
}

void URRHUDWidget::BroadcastHealth()
{
	const UAbilitySystemComponent* ASC = BoundASC.Get();
	if (!ASC)
	{
		return;
	}
	// The Blueprint divides by Max: before the miner applied its base stats MaxHealth is 0, skip until it is set
	// (the MaxHealth change then calls this again with the real values).
	const float Max = ASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	if (Max > 0.f)
	{
		OnHealthChanged(ASC->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute()), Max);
	}
}

void URRHUDWidget::UnbindMiner()
{
	if (UAbilitySystemComponent* ASC = BoundASC.Get())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetHealthAttribute()).Remove(HealthChangedHandle);
		ASC->GetGameplayAttributeValueChangeDelegate(URRHealthAttributeSet::GetMaxHealthAttribute()).Remove(MaxHealthChangedHandle);
	}
	if (URRProgressionComponent* Progression = BoundProgression.Get())
	{
		Progression->OnXPChanged.RemoveDynamic(this, &URRHUDWidget::HandleXPChanged);
		Progression->OnLevelUp.RemoveDynamic(this, &URRHUDWidget::HandleLevelUp);
	}
	BoundProgression.Reset();
	BoundASC.Reset();
	HealthChangedHandle.Reset();
	MaxHealthChangedHandle.Reset();
}
