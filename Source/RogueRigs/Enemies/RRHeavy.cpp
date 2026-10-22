// Rogue Rigs - heavy robot: slow, tough biped with a telegraphed ground slam (Chase, Telegraph, Slam, Recover), high knockback resistance.

#include "Enemies/RRHeavy.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Engine/World.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "Telegraph/RRGroundTelegraphComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRHeavy)

ARRHeavy::ARRHeavy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StatsRowName = TEXT("Heavy");
	// XP burst on death (task-027-01): the row XPValue split over 12 cubes.
	XPCubeCount = 12;
	ArrivalRangeRatio = 0.5f;
	DamageEffectClass = URRGE_DamageFlat::StaticClass();
	// Wider damage number disc for the larger body (damage-numbers: 45 uu, task-059-01).
	DamageNumberSpawnRadius = 45.f;
	// Baked clips (task-284-05): walk = clip 0 while moving; rest = clip 3 held at its last frame (row 61) while stopped. The
	// slam clips 1..3 (task-285) play on the state events; clip 3 (recover) ends at the rest pose, so it stays the rest clip.
	RestClipId = 3.f;

	// Twice the chaser (24 x 62), radius well under the 80 uu limit so it crosses one-cell (200 uu) corridors (enemy-heavy).
	GetCapsuleComponent()->InitCapsuleSize(48.f, 124.f);

	// Slam ground signal (task-115-01): centered on the feet, hidden until a telegraph starts.
	GroundTelegraph = CreateDefaultSubobject<URRGroundTelegraphComponent>(TEXT("GroundTelegraph"));
	GroundTelegraph->SetupAttachment(GetCapsuleComponent());
	GroundTelegraph->SetRelativeLocation(FVector(0.f, 0.f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
	GroundTelegraph->SetMode(ERRGroundTelegraphMode::Danger);
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
		if (GroundTelegraph)
		{
			GroundTelegraph->Hide();
		}
	}

	State = NewState;
	MarkStateStart();
	SetMovementActive(NewState == ERRHeavyState::Chase);

	if (NewState == ERRHeavyState::Telegraph && GroundTelegraph)
	{
		// The danger zone is the slam radius itself (Stats.AttackRange, the value PerformSlam tests against).
		GroundTelegraph->SetRadius(GetSlamRadius());
		GroundTelegraph->SetMode(ERRGroundTelegraphMode::Danger);
		GroundTelegraph->SetProgress(0.f);
		GroundTelegraph->Show();
	}
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
			// After EnterState (its movement-off rest hold): fists up now, slam-down clip SlamContactLead before the slam frame.
			PlayClip(TelegraphClipId, FMath::Max(static_cast<double>(Stats.WindUp - SlamContactLead), UE_KINDA_SMALL_NUMBER));
			OnTelegraphStart();
			UpdateTelegraphBlink(TelegraphFlashRate);
		}
		break;

	case ERRHeavyState::Telegraph:
		if (GroundTelegraph)
		{
			// Reaches 1 on the slam frame, just before EnterState(Slam) hides the signal.
			GroundTelegraph->SetProgress(GetTelegraphProgress());
		}
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

	// Fists on the ground (contact held 3 frames), then back to the rest pose before the recovery ends.
	PlayClip(RecoverClipId, 0.0);

	const FVector Feet = GetActorLocation() - FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	UE_LOG(LogRogueRigs, Verbose, TEXT("ARRHeavy %s: slam %d at t=%.3f, hits %d."), *GetName(), SlamCount, Now, SlamHitCount);
	OnSlam(Feet, Stats.AttackRange);
}

void ARRHeavy::OnAnimClipEnded()
{
	if (State == ERRHeavyState::Telegraph)
	{
		// Untimed: the slam frame replaces it with the recover clip (PerformSlam) well before its loop restarts (0.533 s).
		PlayClip(SlamClipId, 0.0);
		return;
	}
	if (State == ERRHeavyState::Dead)
	{
		// Death leaves the clip (base rule).
		return;
	}
	Super::OnAnimClipEnded();
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

void ARRHeavy::ResetForNewLife()
{
	Super::ResetForNewLife();
	LastSlamTime = -1.0;
	SlamCount = 0;
	SlamHitCount = 0;
	EnemyMovement->SetArrivalDistance(Stats.AttackRange * ArrivalRangeRatio);
	EnterState(ERRHeavyState::Chase);
	if (GroundTelegraph)
	{
		// No stale signal from the last life: hidden, progress back to 0, radius from this life's stats.
		GroundTelegraph->SetProgress(0.f);
		GroundTelegraph->SetRadius(GetSlamRadius());
		GroundTelegraph->Hide();
	}
}

void ARRHeavy::ResetStateMachine()
{
	if (State == ERRHeavyState::Dead)
	{
		return;
	}
	// Relocated mid-slam cycle: the Blueprint telegraph visuals (glow, tilt) are reset like at the end of a recovery,
	// so the heavy never chases with its slam zone shown (task-119-01).
	const bool bResetPose = State == ERRHeavyState::Telegraph || State == ERRHeavyState::Slam || State == ERRHeavyState::Recover;
	EnterState(ERRHeavyState::Chase);
	if (bResetPose)
	{
		OnRecover();
	}
}
