// Rogue Rigs - chaser robot: melee rusher driven by a C++ state machine (Chase, WindUp, Attack, Recover).

#include "Enemies/RRChaser.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Engine/World.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRGameplayTags.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRChaser)

namespace RRChaser
{
	/** Tolerance on state timers (s): absorbs float drift of the accumulated world time. */
	constexpr double TimeTolerance = 1e-4;
}

ARRChaser::ARRChaser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StatsRowName = TEXT("Chaser");
	DamageEffectClass = URRGE_DamageFlat::StaticClass();

	// Small robot: about 0.7 x the default character capsule (enemy-chaser size, provisional). Editable in the Blueprint.
	GetCapsuleComponent()->InitCapsuleSize(24.f, 62.f);
}

void ARRChaser::BeginPlay()
{
	Super::BeginPlay();

	EnemyMovement->SetArrivalDistance(Stats.AttackRange * ArrivalRangeRatio);
	EnterState(ERRChaserState::Chase);
}

void ARRChaser::EnterState(ERRChaserState NewState)
{
	if (State == ERRChaserState::WindUp && NewState != ERRChaserState::WindUp)
	{
		SetMeshesVisible(true);
	}

	State = NewState;
	StateStartTime = GetWorld()->GetTimeSeconds();

	const bool bMove = NewState == ERRChaserState::Chase;
	EnemyMovement->SetMovementEnabled(bMove);
	if (!bMove)
	{
		ConsumeMovementInputVector();
		GetCharacterMovement()->StopMovementImmediately();
	}
}

bool ARRChaser::IsTargetInRange(const ARRMiner* Target) const
{
	return Target && !Target->IsDead()
		&& FVector::DistSquared2D(GetActorLocation(), Target->GetActorLocation()) <= FMath::Square(Stats.AttackRange);
}

void ARRChaser::UpdateBehavior(float DeltaSeconds)
{
	const double Now = GetWorld()->GetTimeSeconds();
	ARRMiner* Target = FindTarget();

	switch (State)
	{
	case ERRChaserState::Chase:
		if (IsTargetInRange(Target))
		{
			EnterState(ERRChaserState::WindUp);
			UpdateFlash();
		}
		break;

	case ERRChaserState::WindUp:
		if (Now - StateStartTime + RRChaser::TimeTolerance >= Stats.WindUp)
		{
			EnterState(ERRChaserState::Attack);
			LastAttackTime = Now;
			++AttackCount;
			if (IsTargetInRange(Target))
			{
				ApplyAttack(Target);
			}
		}
		else
		{
			UpdateFlash();
		}
		break;

	case ERRChaserState::Attack:
		EnterState(ERRChaserState::Recover);
		break;

	case ERRChaserState::Recover:
	{
		const double RecoverTime = Stats.AttackRate > 0.f ? 1.0 / Stats.AttackRate : 0.0;
		if (Now - LastAttackTime + RRChaser::TimeTolerance >= RecoverTime)
		{
			EnterState(ERRChaserState::Chase);
		}
		break;
	}

	case ERRChaserState::Dead:
		break;
	}
}

void ARRChaser::ApplyAttack(ARRMiner* Target)
{
	UAbilitySystemComponent* TargetASC = Target->GetAbilitySystemComponent();
	if (!DamageEffectClass || !TargetASC)
	{
		return;
	}
	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(DamageEffectClass, 1.f, AbilitySystemComponent->MakeEffectContext());
	if (!Spec.IsValid())
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: damage spec of class %s could not be made."), *GetName(), *DamageEffectClass->GetName());
		return;
	}
	Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, Stats.Damage);
	// The miner's i-frames or death refuse the spec (target tag requirements of URRGE_DamageFlat).
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spec.Data, TargetASC);
}

void ARRChaser::UpdateFlash()
{
	if (WindUpFlashRate <= 0.f)
	{
		return;
	}
	const double Phase = FMath::Frac((GetWorld()->GetTimeSeconds() - StateStartTime) * WindUpFlashRate);
	SetMeshesVisible(Phase >= 0.5);
}

void ARRChaser::HandleDeath(AActor* DeathInstigator)
{
	if (State != ERRChaserState::Dead)
	{
		EnterState(ERRChaserState::Dead);
	}
	Super::HandleDeath(DeathInstigator);
}
