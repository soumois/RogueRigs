// Rogue Rigs - base class of the in-run HUD widget: health, XP, level-up, upgrade choice, alert bar, objective and game-over events fed by C++.

#include "UI/RRHUDWidget.h"

#include "AbilitySystemComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Player/RRGameMode.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
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

	if (URRLevelUpComponent* LevelUp = Miner->GetLevelUp())
	{
		BoundLevelUp = LevelUp;
		LevelUp->OnUpgradeOfferShown.AddUniqueDynamic(this, &URRHUDWidget::HandleUpgradeOfferShown);
		LevelUp->OnUpgradeOfferClosed.AddUniqueDynamic(this, &URRHUDWidget::HandleUpgradeOfferClosed);
		if (LevelUp->IsAwaitingChoice())
		{
			HandleUpgradeOfferShown(LevelUp->GetOfferCards(), LevelUp->CanReroll());
		}
	}
}

bool URRHUDWidget::ChooseUpgrade(int32 Index)
{
	URRLevelUpComponent* LevelUp = BoundLevelUp.Get();
	return LevelUp && LevelUp->ChooseUpgrade(Index);
}

bool URRHUDWidget::RequestReroll()
{
	URRLevelUpComponent* LevelUp = BoundLevelUp.Get();
	return LevelUp && LevelUp->RequestReroll();
}

bool URRHUDWidget::CanRerollUpgrade() const
{
	const URRLevelUpComponent* LevelUp = BoundLevelUp.Get();
	return LevelUp && LevelUp->CanReroll();
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

void URRHUDWidget::BindToRunFlow(URRRunFlowSubsystem* RunFlow)
{
	if (URRRunFlowSubsystem* Previous = BoundRunFlow.Get())
	{
		Previous->OnAlertChanged.RemoveDynamic(this, &URRHUDWidget::HandleAlertChanged);
		Previous->OnObjectiveChanged.RemoveDynamic(this, &URRHUDWidget::HandleObjectiveChanged);
		Previous->OnObjectiveProgressChanged.RemoveDynamic(this, &URRHUDWidget::HandleObjectiveProgressChanged);
		Previous->OnExtractionReady.RemoveDynamic(this, &URRHUDWidget::HandleExtractionReady);
		Previous->OnShuttleCountdown.RemoveDynamic(this, &URRHUDWidget::HandleShuttleCountdown);
		Previous->OnRunEnded.RemoveDynamic(this, &URRHUDWidget::HandleRunEnded);
	}
	BoundRunFlow = RunFlow;
	if (!RunFlow)
	{
		return;
	}
	RunFlow->OnAlertChanged.AddUniqueDynamic(this, &URRHUDWidget::HandleAlertChanged);
	RunFlow->OnObjectiveChanged.AddUniqueDynamic(this, &URRHUDWidget::HandleObjectiveChanged);
	RunFlow->OnObjectiveProgressChanged.AddUniqueDynamic(this, &URRHUDWidget::HandleObjectiveProgressChanged);
	RunFlow->OnExtractionReady.AddUniqueDynamic(this, &URRHUDWidget::HandleExtractionReady);
	RunFlow->OnShuttleCountdown.AddUniqueDynamic(this, &URRHUDWidget::HandleShuttleCountdown);
	RunFlow->OnRunEnded.AddUniqueDynamic(this, &URRHUDWidget::HandleRunEnded);

	// Push the current state once.
	OnAlertChanged(RunFlow->GetAlert(), RunFlow->HasPendingObjective());
	if (ARRConsole* Console = RunFlow->GetConsole())
	{
		HandleObjectiveChanged(Console, Console->GetConsoleState(), Console->GetMarkedRockCount(), Console->GetInitialMarkedRockCount());
		OnObjectiveProgressChanged(Console->GetHackProgress(), Console->GetConsoleState() == ERRConsoleState::Hacking);
	}
	else
	{
		HandleObjectiveChanged(nullptr, ERRConsoleState::Locked, 0, 0);
	}
	if (RunFlow->IsExtractionReady())
	{
		OnExtractionReady();
	}
	if (const ARRShuttle* Shuttle = RunFlow->GetShuttle())
	{
		OnShuttleCountdown(Shuttle->GetRemainingSeconds(), Shuttle->GetActorLocation());
	}
	if (RunFlow->IsRunEnded())
	{
		OnRunEnded(RunFlow->WasRunSuccessful());
	}
}

void URRHUDWidget::RestartLevel()
{
	URRRunFlowSubsystem* RunFlow = BoundRunFlow.Get();
	if (!RunFlow)
	{
		const UWorld* World = GetWorld();
		RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
	}
	if (RunFlow)
	{
		RunFlow->RestartLevel();
	}
}

void URRHUDWidget::HandleShuttleCountdown(float RemainingSeconds, FVector ShuttleLocation)
{
	OnShuttleCountdown(RemainingSeconds, ShuttleLocation);
}

void URRHUDWidget::HandleRunEnded(bool bSuccess)
{
	OnRunEnded(bSuccess);
}

void URRHUDWidget::HandleAlertChanged(float Alert, bool bPaused)
{
	OnAlertChanged(Alert, bPaused);
}

void URRHUDWidget::HandleObjectiveChanged(ARRConsole* Console, ERRConsoleState State, int32 MarkedRocksLeft, int32 MarkedRocksTotal)
{
	OnObjectiveChanged(IsValid(Console), State, MarkedRocksLeft, MarkedRocksTotal);
}

void URRHUDWidget::HandleObjectiveProgressChanged(float Progress, bool bHacking)
{
	OnObjectiveProgressChanged(Progress, bHacking);
}

void URRHUDWidget::HandleExtractionReady()
{
	OnExtractionReady();
}

void URRHUDWidget::NativeDestruct()
{
	UnbindMiner();
	BindToGameMode(nullptr);
	BindToRunFlow(nullptr);
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

void URRHUDWidget::HandleUpgradeOfferShown(const TArray<FRRUpgradeCard>& Cards, bool bCanReroll)
{
	OnUpgradeOfferShown(Cards, bCanReroll);
}

void URRHUDWidget::HandleUpgradeOfferClosed()
{
	OnUpgradeOfferClosed();
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
	if (URRLevelUpComponent* LevelUp = BoundLevelUp.Get())
	{
		LevelUp->OnUpgradeOfferShown.RemoveDynamic(this, &URRHUDWidget::HandleUpgradeOfferShown);
		LevelUp->OnUpgradeOfferClosed.RemoveDynamic(this, &URRHUDWidget::HandleUpgradeOfferClosed);
	}
	BoundLevelUp.Reset();
	BoundProgression.Reset();
	BoundASC.Reset();
	HealthChangedHandle.Reset();
	MaxHealthChangedHandle.Reset();
}
