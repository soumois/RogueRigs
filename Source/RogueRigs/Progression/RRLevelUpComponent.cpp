// Rogue Rigs - level-up flow of the miner: heal, effect event, delayed pause on the upgrade offer, chained choice screens.

#include "Progression/RRLevelUpComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/RREnemy.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraComponent.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRGE_Heal.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Player/RRMiner.h"
#include "Pool/RRNiagaraPoolSubsystem.h"
#include "Progression/RRProgressionComponent.h"
#include "RogueRigs.h"
#include "TimerManager.h"
#include "Upgrades/RRRerollCost.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRWeapon.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRLevelUpComponent)

const FName URRLevelUpComponent::WaveRadiusParameter(TEXT("User.WaveRadius"));

URRLevelUpComponent::URRLevelUpComponent()
{
	// Ticks only while a level-up wave plays, after the miner moved, to keep the wave under him (task-449).
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
	HealEffectClass = URRGE_Heal::StaticClass();
}

void URRLevelUpComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (URRProgressionComponent* Progression = Owner ? Owner->FindComponentByClass<URRProgressionComponent>() : nullptr)
	{
		BoundProgression = Progression;
		Progression->OnLevelUp.AddUniqueDynamic(this, &URRLevelUpComponent::HandleLevelUp);
	}
	else
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRLevelUpComponent: %s has no progression component; no level-up flow."), *GetNameSafe(Owner));
	}
	UpgradeComponent = Owner ? Owner->FindComponentByClass<URRUpgradeComponent>() : nullptr;

	if (URRNiagaraPoolSubsystem* Pool = GetWorld() ? GetWorld()->GetSubsystem<URRNiagaraPoolSubsystem>() : nullptr)
	{
		Pool->Prewarm(LevelUpTextSystem, BurstPoolSize, BurstPoolSize);
		Pool->Prewarm(LevelUpWaveSystem, BurstPoolSize, BurstPoolSize);
	}
}

void URRLevelUpComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (URRProgressionComponent* Progression = BoundProgression.Get())
	{
		Progression->OnLevelUp.RemoveDynamic(this, &URRLevelUpComponent::HandleLevelUp);
	}
	BoundProgression.Reset();
	FollowedWave.Reset();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ChoiceDelayTimer);
	}
	if (bAwaitingChoice)
	{
		ExitChoicePause();
	}
	bAwaitingChoice = false;
	PendingChoices = 0;
	PendingLevels.Reset();
	PendingBonusLuck.Reset();
	Super::EndPlay(EndPlayReason);
}

bool URRLevelUpComponent::IsChoiceDelayActive() const
{
	const UWorld* World = GetWorld();
	return World && World->GetTimerManager().IsTimerActive(ChoiceDelayTimer);
}

TArray<FRRUpgradeCard> URRLevelUpComponent::GetOfferCards() const
{
	const URRUpgradeComponent* Upgrades = UpgradeComponent.Get();
	return bAwaitingChoice && Upgrades ? Upgrades->GetCurrentOffer() : TArray<FRRUpgradeCard>();
}

bool URRLevelUpComponent::CanReroll() const
{
	const URRUpgradeComponent* Upgrades = UpgradeComponent.Get();
	const URRRunFlowSubsystem* RunFlow = GetRunFlow();
	return bAwaitingChoice && Upgrades && Upgrades->CanReroll() && RunFlow && RunFlow->CanSpendOre(GetRerollCost());
}

int32 URRLevelUpComponent::GetRerollCost() const
{
	return URRRerollCostLibrary::GetRerollCostForCount(RerollCount);
}

URRRunFlowSubsystem* URRLevelUpComponent::GetRunFlow() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
}

void URRLevelUpComponent::HandleLevelUp(int32 NewLevel)
{
	ApplyLevelUpHeal();
	OnLevelUpEffect.Broadcast(NewLevel);
	PlayLevelUpBurst();

	if (bChoicesBlocked)
	{
		return;
	}
	QueueChoice(NewLevel, 0.f);
}

void URRLevelUpComponent::PlayLevelUpBurst()
{
	const AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World || bChoicesBlocked || IsOwnerDead())
	{
		return;
	}
	const double Now = World->GetTimeSeconds();
	if (LastBurstTime >= 0.0 && Now - LastBurstTime < BurstMinInterval)
	{
		return;
	}
	LastBurstTime = Now;
	++LevelUpBurstCount;
	PushNearbyRobots();

	URRNiagaraPoolSubsystem* Pool = World->GetSubsystem<URRNiagaraPoolSubsystem>();
	if (!Pool)
	{
		return;
	}
	const ACharacter* Character = Cast<ACharacter>(Owner);
	const UCapsuleComponent* Capsule = Character ? Character->GetCapsuleComponent() : nullptr;
	const float HalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 0.f;
	// The text stays where it appeared (task-449, Q21); the wave follows the miner while it plays.
	Pool->Spawn(LevelUpTextSystem, Owner->GetActorLocation() + FVector(0.f, 0.f, HalfHeight + LevelUpTextHeight), FRotator::ZeroRotator, BurstPoolSize);
	UNiagaraComponent* Wave = Pool->Spawn(LevelUpWaveSystem, GetWaveLocation(), FRotator::ZeroRotator, BurstPoolSize,
		[this](UNiagaraComponent& Spawned) { Spawned.SetVariableFloat(WaveRadiusParameter, PushRadius); });
	FollowedWave = Wave;
	SetComponentTickEnabled(Wave != nullptr);
}

FVector URRLevelUpComponent::GetWaveLocation() const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return FVector::ZeroVector;
	}
	const ACharacter* Character = Cast<ACharacter>(Owner);
	const UCapsuleComponent* Capsule = Character ? Character->GetCapsuleComponent() : nullptr;
	const float HalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 0.f;
	return Owner->GetActorLocation() - FVector(0.f, 0.f, HalfHeight - LevelUpWaveFloorOffset);
}

UNiagaraComponent* URRLevelUpComponent::GetFollowedWave() const
{
	return FollowedWave.Get();
}

void URRLevelUpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Playing, or activated and waiting for its system to be ready (inactive with its tick on, as URRNiagaraPoolSubsystem reads it).
	UNiagaraComponent* Wave = FollowedWave.Get();
	if (!Wave || (!Wave->IsActive() && !Wave->IsComponentTickEnabled()))
	{
		FollowedWave.Reset();
		SetComponentTickEnabled(false);
		return;
	}
	// Local-space emitters: moving the component carries the ring and the shards with the miner.
	Wave->SetWorldLocation(GetWaveLocation());
}

void URRLevelUpComponent::PushNearbyRobots()
{
	const AActor* Owner = GetOwner();
	const FVector Center = Owner->GetActorLocation();
	const double RadiusSquared = FMath::Square(static_cast<double>(PushRadius));
	for (TActorIterator<ARREnemy> It(GetWorld()); It; ++It)
	{
		ARREnemy* Robot = *It;
		// A pooled idle robot keeps State.Dead, so IsDead filters it; ApplyKnockback scales by the robot's resistance.
		if (!Robot->IsDead() && FVector::DistSquared2D(Robot->GetActorLocation(), Center) <= RadiusSquared)
		{
			Robot->ApplyKnockback((Robot->GetActorLocation() - Center).GetSafeNormal2D() * PushSpeed);
		}
	}
}

bool URRLevelUpComponent::QueueBonusChoice(float BonusLuck)
{
	if (bChoicesBlocked || IsOwnerDead())
	{
		return false;
	}
	QueueChoice(0, BonusLuck);
	return true;
}

void URRLevelUpComponent::QueueChoice(int32 OfferLevel, float BonusLuck)
{
	++PendingChoices;
	PendingLevels.Add(OfferLevel);
	PendingBonusLuck.Add(BonusLuck);
	// A running delay or an open screen picks the new choice up through the queue.
	if (!bAwaitingChoice && !IsChoiceDelayActive())
	{
		UWorld* World = GetWorld();
		if (World && ChoiceDelay > 0.f)
		{
			World->GetTimerManager().SetTimer(ChoiceDelayTimer, this, &URRLevelUpComponent::HandleChoiceDelayElapsed, ChoiceDelay, false);
		}
		else
		{
			HandleChoiceDelayElapsed();
		}
	}
}

void URRLevelUpComponent::ApplyLevelUpHeal()
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (!ASC || !HealEffectClass || HealFraction <= 0.f || IsOwnerDead()
		|| !ASC->HasAttributeSetForAttribute(URRHealthAttributeSet::GetMaxHealthAttribute()))
	{
		return;
	}
	const float Amount = HealFraction * ASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	if (Amount <= 0.f)
	{
		return;
	}
	FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(HealEffectClass, 1.f, ASC->MakeEffectContext());
	if (Spec.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Heal, Amount);
		ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}

void URRLevelUpComponent::HandleChoiceDelayElapsed()
{
	if (PendingChoices <= 0 || bAwaitingChoice || bChoicesBlocked)
	{
		return;
	}
	if (IsOwnerDead())
	{
		// The run is over: no choice screen.
		PendingChoices = 0;
		PendingLevels.Reset();
		PendingBonusLuck.Reset();
		return;
	}
	EnterChoicePause();
	bAwaitingChoice = true;
	ShowNextOffer();
}

void URRLevelUpComponent::ShowNextOffer()
{
	URRUpgradeComponent* Upgrades = UpgradeComponent.Get();
	const int32 OfferLevel = PendingLevels.IsEmpty() ? 0 : PendingLevels[0];
	const float BonusLuck = PendingBonusLuck.IsEmpty() ? 0.f : PendingBonusLuck[0];
	const TArray<FRRUpgradeCard> Cards = Upgrades ? Upgrades->BuildOfferWithOwnerLuck(CardCount, BonusLuck, OfferLevel) : TArray<FRRUpgradeCard>();
	if (Cards.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRLevelUpComponent: no upgrade card could be offered on %s; choice skipped."), *GetNameSafe(GetOwner()));
		CloseChoice();
		return;
	}
	if (OfferLevel > 0)
	{
		// A level-up screen (chained ones included) restarts the reroll cost; a bonus chest screen (level 0) keeps it.
		RerollCount = 0;
	}
	OnUpgradeOfferShown.Broadcast(Cards, CanReroll());
}

bool URRLevelUpComponent::ChooseUpgrade(int32 Index)
{
	URRUpgradeComponent* Upgrades = UpgradeComponent.Get();
	if (!bAwaitingChoice || !Upgrades || !Upgrades->GetCurrentOffer().IsValidIndex(Index))
	{
		return false;
	}
	const FRRUpgradeCard Card = Upgrades->GetCurrentOffer()[Index];
	if (!Upgrades->ApplyUpgrade(Card))
	{
		return false;
	}

	--PendingChoices;
	if (!PendingLevels.IsEmpty())
	{
		PendingLevels.RemoveAt(0);
	}
	if (!PendingBonusLuck.IsEmpty())
	{
		PendingBonusLuck.RemoveAt(0);
	}
	if (PendingChoices > 0)
	{
		ShowNextOffer();
	}
	else
	{
		CloseChoice();
	}
	return true;
}

bool URRLevelUpComponent::RequestReroll()
{
	URRUpgradeComponent* Upgrades = UpgradeComponent.Get();
	URRRunFlowSubsystem* RunFlow = GetRunFlow();
	const int32 Cost = GetRerollCost();
	if (!CanReroll() || !RunFlow->SpendOre(Cost))
	{
		return false;
	}
	TArray<FRRUpgradeCard> Cards;
	Upgrades->Reroll(Cards);
	++RerollCount;
	UE_LOG(LogRogueRigs, Log, TEXT("URRLevelUpComponent: reroll paid %d ore (reroll %d since the last level-up screen), next cost %d."), Cost, RerollCount, GetRerollCost());
	OnUpgradeOfferShown.Broadcast(Cards, CanReroll());
	return true;
}

void URRLevelUpComponent::CancelChoices()
{
	bChoicesBlocked = true;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ChoiceDelayTimer);
	}
	CloseChoice();
}

void URRLevelUpComponent::CloseChoice()
{
	PendingChoices = 0;
	PendingLevels.Reset();
	PendingBonusLuck.Reset();
	const bool bWasAwaiting = bAwaitingChoice;
	bAwaitingChoice = false;
	if (bWasAwaiting)
	{
		OnUpgradeOfferClosed.Broadcast();
		ExitChoicePause();
	}
}

void URRLevelUpComponent::EnterChoicePause()
{
	bPausedByFlow = false;
	APlayerController* Controller = GetOwnerPlayerController();
	PausingController = Controller;
	if (!Controller)
	{
		return;
	}
	// Standard pause: actors and timers stop (auto-fire, enemies); the controller and UMG keep running.
	bPausedByFlow = !Controller->IsPaused() && Controller->SetPause(true);
	bCursorShownBeforePause = Controller->ShouldShowMouseCursor();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Controller->SetInputMode(InputMode);
	Controller->SetShowMouseCursor(true);
}

void URRLevelUpComponent::ExitChoicePause()
{
	APlayerController* Controller = PausingController.Get();
	PausingController.Reset();
	if (!Controller)
	{
		bPausedByFlow = false;
		return;
	}
	if (bPausedByFlow)
	{
		Controller->SetPause(false);
	}
	bPausedByFlow = false;

	// Back to the project's capture mode (DefaultInput.ini: CapturePermanently_IncludingInitialMouseDown).
	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(false);
	Controller->SetInputMode(InputMode);
	Controller->SetShowMouseCursor(bCursorShownBeforePause);
}

bool URRLevelUpComponent::IsOwnerDead() const
{
	const ARRMiner* Miner = Cast<ARRMiner>(GetOwner());
	return Miner && Miner->IsDead();
}

APlayerController* URRLevelUpComponent::GetOwnerPlayerController() const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	return Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
}
