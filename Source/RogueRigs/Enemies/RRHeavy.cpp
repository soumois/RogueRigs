// Rogue Rigs - heavy robot: slow, tough biped with a telegraphed ground slam (Chase, Telegraph, Slam, Recover), high knockback resistance.

#include "Enemies/RRHeavy.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Engine/World.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRHeavy)

ARRHeavy::ARRHeavy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StatsRowName = TEXT("Heavy");
	// XP burst on death (task-027-01): the row XPValue split over 12 cubes.
	XPCubeCount = 12;
	ArrivalRangeRatio = 0.5f;
	DamageEffectClass = URRGE_DamageFlat::StaticClass();

	// Twice the chaser (24 x 62), radius well under the 80 uu limit so it crosses one-cell (200 uu) corridors (enemy-heavy).
	GetCapsuleComponent()->InitCapsuleSize(48.f, 124.f);
}

void ARRHeavy::BeginPlay()
{
	Super::BeginPlay();

	EnemyMovement->SetArrivalDistance(Stats.AttackRange * ArrivalRangeRatio);
	EnterState(ERRHeavyState::Chase);
}

float ARRHeavy::GetTelegraphProgress() const
{
	if (State != ERRHeavyState::Telegraph)
	{
		return 0.f;
	}
	return Stats.WindUp > 0.f ? FMath::Clamp(static_cast<float>(GetTimeInState() / Stats.WindUp), 0.f, 1.f) : 1.f;
}

void ARRHeavy::EnterState(ERRHeavyState NewState)
{
	if (State == ERRHeavyState::Telegraph && NewState != ERRHeavyState::Telegraph)
	{
		SetMeshesVisible(true);
	}

	State = NewState;
	MarkStateStart();
	SetMovementActive(NewState == ERRHeavyState::Chase);
}

bool ARRHeavy::IsSlamReady(double Now) const
{
	if (LastSlamTime < 0.0)
	{
		return true;
	}
	const double Interval = Stats.AttackRate > 0.f ? 1.0 / Stats.AttackRate : 0.0;
	return Now + TimeTolerance >= LastSlamTime + Interval - Stats.WindUp;
}

void ARRHeavy::UpdateBehavior(float DeltaSeconds)
{
	const double Now = GetWorld()->GetTimeSeconds();

	switch (State)
	{
	case ERRHeavyState::Chase:
		if (IsSlamReady(Now) && IsTargetWithin(FindTarget(), Stats.AttackRange * SlamTriggerRatio))
		{
			EnterState(ERRHeavyState::Telegraph);
			OnTelegraphStart();
			UpdateTelegraphBlink(TelegraphFlashRate);
		}
		break;

	case ERRHeavyState::Telegraph:
		if (HasStateLasted(Stats.WindUp))
		{
			EnterState(ERRHeavyState::Slam);
			PerformSlam(FindTarget(), Now);
		}
		else
		{
			UpdateTelegraphBlink(TelegraphFlashRate);
		}
		break;

	case ERRHeavyState::Slam:
		EnterState(ERRHeavyState::Recover);
		break;

	case ERRHeavyState::Recover:
		if (HasStateLasted(RecoverDuration))
		{
			EnterState(ERRHeavyState::Chase);
			OnRecover();
		}
		break;

	case ERRHeavyState::Dead:
		break;
	}
}

void ARRHeavy::PerformSlam(ARRMiner* Target, double Now)
{
	LastSlamTime = Now;
	++SlamCount;

	// The shockwave only looks for the miner: no terrain damage (destruction-rules), no robot damage.
	if (IsTargetWithin(Target, Stats.AttackRange)
		&& ApplyDamageSpecTo(Target->GetAbilitySystemComponent(), DamageEffectClass, Stats.Damage))
	{
		++SlamHitCount;
	}

	const FVector Feet = GetActorLocation() - FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	UE_LOG(LogRogueRigs, Verbose, TEXT("ARRHeavy %s: slam %d at t=%.3f, hits %d."), *GetName(), SlamCount, Now, SlamHitCount);
	OnSlam(Feet, Stats.AttackRange);
}

void ARRHeavy::HandleDeath(AActor* DeathInstigator)
{
	const bool bResetPose = State == ERRHeavyState::Telegraph || State == ERRHeavyState::Slam || State == ERRHeavyState::Recover;
	if (State != ERRHeavyState::Dead)
	{
		EnterState(ERRHeavyState::Dead);
	}
	if (bResetPose)
	{
		OnRecover();
	}
	Super::HandleDeath(DeathInstigator);
}
