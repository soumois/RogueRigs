// Rogue Rigs - base class of the in-run HUD widget: health, XP, level-up, upgrade choice, alert bar, objective and game-over events fed by C++.

#include "UI/RRHUDWidget.h"

#include "AbilitySystemComponent.h"
#include "Enemies/RRWaveDirector.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Player/RRGameMode.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "Weapons/RRTurretLauncher.h"

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

	BoundMiner = Miner;
	Miner->OnWeaponAdded.AddUniqueDynamic(this, &URRHUDWidget::HandleWeaponAdded);
	BindToTurretLauncher(Miner->FindTurretLauncher());
}

void URRHUDWidget::BindToTurretLauncher(ARRTurretLauncher* Launcher)
{
	if (ARRTurretLauncher* Previous = BoundTurretLauncher.Get())
	{
		Previous->OnTurretChargesChanged.RemoveDynamic(this, &URRHUDWidget::HandleTurretChargesChanged);
	}
	BoundTurretLauncher.Reset();
	if (!Launcher)
	{
		return;
	}
	BoundTurretLauncher = Launcher;
	Launcher->OnTurretChargesChanged.AddUniqueDynamic(this, &URRHUDWidget::HandleTurretChargesChanged);
	HandleTurretChargesChanged(Launcher->GetCharges(), Launcher->GetMaxCharges(), Launcher->IsRecharging() ? Launcher->GetRechargeDuration() : 0.f);
}

void URRHUDWidget::HandleWeaponAdded(ARRWeapon* Weapon)
{
	if (!BoundTurretLauncher.IsValid())
	{
		if (ARRTurretLauncher* Launcher = Cast<ARRTurretLauncher>(Weapon))
		{
			BindToTurretLauncher(Launcher);
		}
	}
}

void URRHUDWidget::HandleTurretChargesChanged(int32 Charges, int32 MaxCharges, float RechargeDuration)
{
	OnTurretChargesChanged(Charges, MaxCharges, RechargeDuration);
}

float URRHUDWidget::GetTurretRechargeProgress() const
{
	const ARRTurretLauncher* Launcher = BoundTurretLauncher.Get();
	return Launcher ? Launcher->GetRechargeProgress() : 0.f;
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
	RefreshAlertBarEvents();
}

void URRHUDWidget::BindToSpawner(ARREnemySpawner* Spawner)
{
	if (ARREnemySpawner* Previous = BoundSpawner.Get())
	{
		Previous->OnHordeLull.RemoveDynamic(this, &URRHUDWidget::HandleHordeLull);
		Previous->OnHordeIncoming.RemoveDynamic(this, &URRHUDWidget::HandleHordeIncoming);
		Previous->OnHordeStarted.RemoveDynamic(this, &URRHUDWidget::HandleHordeStarted);
		Previous->OnHordeFinished.RemoveDynamic(this, &URRHUDWidget::HandleHordeFinished);
	}
	BoundSpawner = Spawner;
	DoneHordePositions.Reset();
	if (!Spawner)
	{
		return;
	}
	Spawner->OnHordeLull.AddUniqueDynamic(this, &URRHUDWidget::HandleHordeLull);
	Spawner->OnHordeIncoming.AddUniqueDynamic(this, &URRHUDWidget::HandleHordeIncoming);
	Spawner->OnHordeStarted.AddUniqueDynamic(this, &URRHUDWidget::HandleHordeStarted);
	Spawner->OnHordeFinished.AddUniqueDynamic(this, &URRHUDWidget::HandleHordeFinished);

	// Bound during a banner: show it for the time left.
	const int32 Active = Spawner->GetActiveHorde();
	if (Active != INDEX_NONE && Spawner->GetHordeState(Active) == ERRHordeState::Announced)
	{
		OnHordeIncoming(Spawner->GetSecondsUntilHordeFront());
	}
	RefreshAlertBarEvents();
}

float URRHUDWidget::EstimateBarPosition(float Alert, float SecondsUntil, float AlertFillSeconds)
{
	const float Current = FMath::Clamp(Alert, 0.f, 1.f);
	return FMath::Clamp(Current + FMath::Max(SecondsUntil, 0.f) / FMath::Max(AlertFillSeconds, 1.f), Current, 1.f);
}

void URRHUDWidget::ComputeAlertBarEvents(TArray<FRRAlertBarEvent>& OutEvents)
{
	OutEvents.Reset();
	const URRRunFlowSubsystem* RunFlow = BoundRunFlow.Get();
	static const FRRRunFlowConfig DefaultConfig;
	const FRRRunFlowConfig& Config = RunFlow ? RunFlow->GetConfig() : DefaultConfig;
	const float Alert = RunFlow ? RunFlow->GetAlert() : 0.f;

	FRRAlertBarEvent& ConsoleIcon = OutEvents.AddDefaulted_GetRef();
	ConsoleIcon.Type = ERRAlertBarEventType::Console;
	ConsoleIcon.Position = FMath::Clamp(Config.ConsoleAlertThreshold, 0.f, 1.f);
	const ARRConsole* Console = RunFlow ? RunFlow->GetConsole() : nullptr;
	ConsoleIcon.bDone = (Console && Console->GetConsoleState() == ERRConsoleState::Complete) || (RunFlow && RunFlow->IsExtractionReady());

	if (const ARREnemySpawner* Spawner = BoundSpawner.Get(); Spawner && Spawner->IsDirectorEnabled())
	{
		const bool bAlertTrigger = Spawner->GetHordeTrigger() == ERRHordeTrigger::Alert;
		const int32 ActiveHorde = Spawner->GetActiveHorde();
		for (int32 Index = 0; Index < Spawner->GetHordeCount(); ++Index)
		{
			FRRAlertBarEvent& HordeIcon = OutEvents.AddDefaulted_GetRef();
			HordeIcon.Type = ERRAlertBarEventType::Horde;
			HordeIcon.Index = Index;
			const ERRHordeState State = Spawner->GetHordeState(Index);
			HordeIcon.bDone = State == ERRHordeState::Spawning || State == ERRHordeState::Done;
			if (bAlertTrigger)
			{
				// Alert trigger: the lull starts at a fixed alert.
				HordeIcon.Position = FMath::Clamp(Spawner->GetHordeTriggerAlert(Index), 0.f, 1.f);
			}
			else if (HordeIcon.bDone)
			{
				// Frozen where the fill was when the front started (first seen now when bound after it).
				HordeIcon.Position = DoneHordePositions.FindOrAdd(Index, Alert);
			}
			else
			{
				const float SecondsUntil = Index == ActiveHorde
					? Spawner->GetSecondsUntilHordeFront()
					: Spawner->GetHordeTriggerTime(Index) - Spawner->GetDirector().GetRunTime();
				HordeIcon.Position = EstimateBarPosition(Alert, SecondsUntil, Config.AlertFillSeconds);
			}
		}
	}

	FRRAlertBarEvent& ShuttleIcon = OutEvents.AddDefaulted_GetRef();
	ShuttleIcon.Type = ERRAlertBarEventType::Extraction;
	ShuttleIcon.Position = 1.f;
	ShuttleIcon.bDone = RunFlow && RunFlow->IsExtractionReady();
}

void URRHUDWidget::RefreshAlertBarEvents()
{
	TArray<FRRAlertBarEvent> Events;
	ComputeAlertBarEvents(Events);
	bool bChanged = !bAlertBarEventsSent || Events.Num() != AlertBarEvents.Num();
	for (int32 Index = 0; !bChanged && Index < Events.Num(); ++Index)
	{
		const FRRAlertBarEvent& New = Events[Index];
		const FRRAlertBarEvent& Old = AlertBarEvents[Index];
		bChanged = New.Type != Old.Type || New.Index != Old.Index || New.bDone != Old.bDone
			|| FMath::Abs(New.Position - Old.Position) > AlertEventPositionTolerance;
	}
	if (!bChanged)
	{
		return;
	}
	AlertBarEvents = MoveTemp(Events);
	bAlertBarEventsSent = true;
	OnAlertEventsChanged(AlertBarEvents);
}

void URRHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshAlertBarEvents();
}

void URRHUDWidget::HandleHordeIncoming(int32 HordeIndex, float SecondsUntil)
{
	OnHordeIncoming(SecondsUntil);
	RefreshAlertBarEvents();
}

void URRHUDWidget::HandleHordeLull(int32 HordeIndex)
{
	RefreshAlertBarEvents();
}

void URRHUDWidget::HandleHordeStarted(int32 HordeIndex, ERRSpawnSide Side)
{
	const URRRunFlowSubsystem* RunFlow = BoundRunFlow.Get();
	DoneHordePositions.FindOrAdd(HordeIndex, RunFlow ? RunFlow->GetAlert() : 0.f);
	RefreshAlertBarEvents();
}

void URRHUDWidget::HandleHordeFinished(int32 HordeIndex, int32 Spawned)
{
	RefreshAlertBarEvents();
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
	RefreshAlertBarEvents();
}

void URRHUDWidget::HandleObjectiveChanged(ARRConsole* Console, ERRConsoleState State, int32 MarkedRocksLeft, int32 MarkedRocksTotal)
{
	OnObjectiveChanged(IsValid(Console), State, MarkedRocksLeft, MarkedRocksTotal);
	RefreshAlertBarEvents();
}

void URRHUDWidget::HandleObjectiveProgressChanged(float Progress, bool bHacking)
{
	OnObjectiveProgressChanged(Progress, bHacking);
	RefreshAlertBarEvents();
}

void URRHUDWidget::HandleExtractionReady()
{
	OnExtractionReady();
	RefreshAlertBarEvents();
}

void URRHUDWidget::NativeDestruct()
{
	UnbindMiner();
	BindToGameMode(nullptr);
	BindToRunFlow(nullptr);
	BindToSpawner(nullptr);
	Super::NativeDestruct();
}

void URRHUDWidget::HandleHealthAttributeChanged(const FOnAttributeChangeData& ChangeData)
{
	BroadcastHealth();

	// Damage: Health went down. A MaxHealth decrease clamps Health to the new maximum (OldValue above it): not a hit.
	const UAbilitySystemComponent* ASC = BoundASC.Get();
	if (!ASC || ChangeData.Attribute != URRHealthAttributeSet::GetHealthAttribute() || ChangeData.NewValue >= ChangeData.OldValue)
	{
		return;
	}
	const float Max = ASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	if (Max > 0.f && ChangeData.OldValue <= Max)
	{
		OnPlayerDamaged(ChangeData.OldValue - ChangeData.NewValue, FMath::Clamp(ChangeData.NewValue / Max, 0.f, 1.f));
	}
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
	if (ARRMiner* Miner = BoundMiner.Get())
	{
		Miner->OnWeaponAdded.RemoveDynamic(this, &URRHUDWidget::HandleWeaponAdded);
	}
	BindToTurretLauncher(nullptr);
	BoundMiner.Reset();
	BoundLevelUp.Reset();
	BoundProgression.Reset();
	BoundASC.Reset();
	HealthChangedHandle.Reset();
	MaxHealthChangedHandle.Reset();
}
