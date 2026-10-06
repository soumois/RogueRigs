// Rogue Rigs - level-up flow of the miner: heal, effect event, delayed pause on the upgrade offer, chained choice screens.

#include "Progression/RRLevelUpComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRGE_Heal.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Player/RRMiner.h"
#include "Progression/RRProgressionComponent.h"
#include "RogueRigs.h"
#include "TimerManager.h"
#include "Upgrades/RRUpgradeComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRLevelUpComponent)

URRLevelUpComponent::URRLevelUpComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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
}

void URRLevelUpComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (URRProgressionComponent* Progression = BoundProgression.Get())
	{
		Progression->OnLevelUp.RemoveDynamic(this, &URRLevelUpComponent::HandleLevelUp);
	}
	BoundProgression.Reset();
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
	return bAwaitingChoice && Upgrades && Upgrades->CanReroll();
}

void URRLevelUpComponent::HandleLevelUp(int32 NewLevel)
{
	ApplyLevelUpHeal();
	OnLevelUpEffect.Broadcast(NewLevel);

	if (bChoicesBlocked)
	{
		return;
	}
	++PendingChoices;
	PendingLevels.Add(NewLevel);
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
	const TArray<FRRUpgradeCard> Cards = Upgrades ? Upgrades->BuildOfferWithOwnerLuck(CardCount, 0.f, OfferLevel) : TArray<FRRUpgradeCard>();
	if (Cards.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRLevelUpComponent: no upgrade card could be offered on %s; choice skipped."), *GetNameSafe(GetOwner()));
		CloseChoice();
		return;
	}
	OnUpgradeOfferShown.Broadcast(Cards, Upgrades->CanReroll());
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
	TArray<FRRUpgradeCard> Cards;
	if (!bAwaitingChoice || !Upgrades || !Upgrades->Reroll(Cards))
	{
		return false;
	}
	OnUpgradeOfferShown.Broadcast(Cards, Upgrades->CanReroll());
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
