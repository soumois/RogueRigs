// Rogue Rigs - chaser robot: melee rusher driven by a C++ state machine (Chase, WindUp, Attack, Recover).

#include "Enemies/RRChaser.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Engine/World.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "TimerManager.h"

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
	ResolveAnimatedMesh();
	PlayRunClip();
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

	// Animation events only: attack clip from each wind-up start; back to the run loop when a chase starts before the attack
	// clip ended (wind-up cancelled by a relocation). Death leaves the clip as it is.
	if (NewState == ERRChaserState::WindUp)
	{
		PlayAttackClip();
	}
	else if (NewState == ERRChaserState::Chase && GetWorldTimerManager().IsTimerActive(AttackClipTimer))
	{
		PlayRunClip();
	}
}

void ARRChaser::ResolveAnimatedMesh()
{
	AnimatedMesh = nullptr;
	if (AnimatedMeshName.IsNone())
	{
		return;
	}
	TInlineComponentArray<UStaticMeshComponent*> Meshes(this);
	for (UStaticMeshComponent* MeshComponent : Meshes)
	{
		if (MeshComponent->GetFName() == AnimatedMeshName)
		{
			AnimatedMesh = MeshComponent;
			return;
		}
	}
	UE_LOG(LogRogueRigs, Verbose, TEXT("%s: no static mesh component named %s; the chaser clips are not played."), *GetName(), *AnimatedMeshName.ToString());
}

void ARRChaser::SetAnimClip(float ClipId, double StartTime)
{
	static_assert(AnimStartDataIndex == AnimClipDataIndex + 1, "SetAnimClip writes both floats in one call.");
	if (AnimatedMesh)
	{
		// One render update for both floats. The material reads the same world time (Time node), so pause and time dilation agree.
		AnimatedMesh->SetCustomPrimitiveDataVector2f(AnimClipDataIndex, FVector2f(ClipId, static_cast<float>(StartTime)));
	}
}

void ARRChaser::PlayRunClip()
{
	GetWorldTimerManager().ClearTimer(AttackClipTimer);
	// Own phase per chaser, drawn at each run start (BeginPlay, pool reuse, end of attack, cancelled wind-up).
	SetAnimClip(RunClipId, GetWorld()->GetTimeSeconds() - FMath::FRand() * RunPhaseSpread);
}

void ARRChaser::PlayAttackClip()
{
	SetAnimClip(AttackClipId, GetWorld()->GetTimeSeconds());
	GetWorldTimerManager().SetTimer(AttackClipTimer, this, &ARRChaser::PlayRunClip, static_cast<float>(AttackClipDuration), false);
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
	PlayRunClip();
	EnterState(ERRChaserState::Chase);
}

void ARRChaser::ResetStateMachine()
{
	if (State != ERRChaserState::Dead)
	{
		EnterState(ERRChaserState::Chase);
	}
}
