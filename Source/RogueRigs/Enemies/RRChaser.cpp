// Rogue Rigs - chaser robot: melee rusher driven by a C++ state machine (Chase, WindUp, Attack, Recover).

#include "Enemies/RRChaser.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Engine/World.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRChaser)

ARRChaser::ARRChaser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StatsRowName = TEXT("Chaser");
	// XP burst on death (task-027-01): the row XPValue split over 3 cubes.
	XPCubeCount = 3;
	ArrivalRangeRatio = 0.9f;
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
	MarkStateStart();
	SetMovementActive(NewState == ERRChaserState::Chase);
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
			UpdateTelegraphBlink(WindUpFlashRate);
		}
		break;

	case ERRChaserState::WindUp:
		if (HasStateLasted(Stats.WindUp))
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
			UpdateTelegraphBlink(WindUpFlashRate);
		}
		break;

	case ERRChaserState::Attack:
		EnterState(ERRChaserState::Recover);
		break;

	case ERRChaserState::Recover:
	{
		const double RecoverTime = Stats.AttackRate > 0.f ? 1.0 / Stats.AttackRate : 0.0;
		if (Now - LastAttackTime + TimeTolerance >= RecoverTime)
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
	// The miner's i-frames or death refuse the spec (target tag requirements of URRGE_DamageFlat).
	ApplyDamageSpecTo(Target->GetAbilitySystemComponent(), DamageEffectClass, Stats.Damage);
}

void ARRChaser::HandleDeath(AActor* DeathInstigator)
{
	if (State != ERRChaserState::Dead)
	{
		EnterState(ERRChaserState::Dead);
	}
	Super::HandleDeath(DeathInstigator);
}

void ARRChaser::ResetForNewLife()
{
	Super::ResetForNewLife();
	LastAttackTime = -1.0;
	AttackCount = 0;
	EnemyMovement->SetArrivalDistance(Stats.AttackRange * ArrivalRangeRatio);
	EnterState(ERRChaserState::Chase);
}

void ARRChaser::ResetStateMachine()
{
	if (State != ERRChaserState::Dead)
	{
		EnterState(ERRChaserState::Chase);
	}
}
