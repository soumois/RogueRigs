// Rogue Rigs - shooter robot: closes in to its hold distance and never backs away from the miner, fires dodgeable shots with grid line of sight while on screen, telegraphs each shot with a targeting laser (C++ state machine Approach, Hold, Reposition, Aim, Dead).

#include "Enemies/RRShooter.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Enemies/RREnemySpawner.h"
#include "Engine/World.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "Grid/RRArenaGrid.h"
#include "Player/RRMiner.h"
#include "Projectiles/RREnemyProjectile.h"
#include "RogueRigs.h"
#include "World/RRWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRShooter)

ARRShooter::ARRShooter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StatsRowName = TEXT("Shooter");
	// XP burst on death (task-027-01): the row XPValue split over 6 cubes.
	XPCubeCount = 6;
	// The movement arrives at the preferred distance itself (the ring is the engage range).
	ArrivalRangeRatio = 1.f;
	DamageEffectClass = URRGE_DamageFlat::StaticClass();
	ProjectileClass = ARREnemyProjectile::StaticClass();

	// 1.0 x the miner's capsule (enemy-shooter size, provisional). Editable in the Blueprint.
	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);

	// Muzzle 50 uu above the floor (the weapons' shot height, below the block height) and 40 uu in front.
	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(GetCapsuleComponent());
	MuzzlePoint->SetRelativeLocation(FVector(40.f, 0.f, -88.f + 50.f));

	LaserBeam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserBeam"));
	LaserBeam->SetupAttachment(MuzzlePoint);
	LaserBeam->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	LaserBeam->SetGenerateOverlapEvents(false);
	LaserBeam->SetCanEverAffectNavigation(false);
	LaserBeam->CastShadow = false;
	LaserBeam->SetVisibleFlag(false);
}

void ARRShooter::BeginPlay()
{
	Super::BeginPlay();

	EnemyMovement->SetArrivalDistance(Stats.PreferredDistance * ArrivalRangeRatio);
	// Within the hold distance every flow-field step is stripped of its away-from-the-miner part (it never backs away).
	EnemyMovement->SetDirectionFilter([this](const FVector& Direction, const FVector& Goal)
	{
		return FVector::Dist2D(GetActorLocation(), Goal) <= GetRingOuterDistance() ? MakeNonRetreatingDirection(Direction, Goal) : Direction;
	});
	EnterState(ERRShooterState::Approach);
}

AActor* ARRShooter::GetAimTarget() const
{
	return State == ERRShooterState::Aim ? AimTarget.Get() : nullptr;
}

FVector ARRShooter::GetAimOrigin() const
{
	return MuzzlePoint->GetComponentLocation();
}

FVector ARRShooter::GetAimTargetPoint() const
{
	const FVector Origin = GetAimOrigin();
	const AActor* Target = AimTarget.IsValid() ? static_cast<const AActor*>(AimTarget.Get()) : CachedTarget.Get();
	if (!Target)
	{
		return Origin;
	}
	const FVector TargetLocation = Target->GetActorLocation();
	return FVector(TargetLocation.X, TargetLocation.Y, Origin.Z);
}

float ARRShooter::GetLaserLength() const
{
	const FVector Origin = GetAimOrigin();
	const FVector Point = GetAimTargetPoint();
	const float Distance = FVector::Dist2D(Origin, Point);
	const ARRArenaGrid* Grid = CachedGrid.Get();
	return Grid ? Grid->GetClearDistance(Origin, Point - Origin, Distance) : Distance;
}

float ARRShooter::GetAimProgress() const
{
	if (State != ERRShooterState::Aim)
	{
		return 0.f;
	}
	return Stats.WindUp > 0.f ? static_cast<float>(FMath::Clamp(GetTimeInState() / Stats.WindUp, 0.0, 1.0)) : 1.f;
}

bool ARRShooter::IsOnScreen() const
{
	FBox2D Rectangle(ForceInit);
	if (bHasViewOverride)
	{
		Rectangle = ViewOverride;
	}
	else
	{
		const ARRArenaGrid* Grid = CachedGrid.IsValid() ? CachedGrid.Get() : RRWorld::FindArenaGrid(GetWorld());
		const AActor* CameraOwner = CachedTarget.IsValid() ? CachedTarget.Get() : RRWorld::FindLivingMiner(GetWorld());
		if (!Grid || !ARREnemySpawner::ComputeCameraViewRectangle(GetWorld(), Grid->GetActorLocation().Z, CameraOwner, Rectangle))
		{
			return false;
		}
	}
	return Rectangle.bIsValid && Rectangle.IsInsideOrOn(FVector2D(GetActorLocation()));
}

void ARRShooter::SetViewRectangleOverride(FBox2D Rectangle)
{
	ViewOverride = Rectangle;
	bHasViewOverride = true;
}

void ARRShooter::EnterState(ERRShooterState NewState)
{
	if (State == ERRShooterState::Aim && NewState != ERRShooterState::Aim)
	{
		LaserBeam->SetVisibility(false);
		AimTarget.Reset();
		OnAimEnd();
	}
	State = NewState;
	MarkStateStart();

	if (NewState == ERRShooterState::Dead)
	{
		// The base death stops the movement.
		return;
	}
	if (NewState == ERRShooterState::Reposition)
	{
		// Detour without sight: walk the flow field like a chaser, no arrival stop, until sight returns.
		EnemyMovement->SetArrivalDistance(0.f);
	}
	// The flow field drives Approach and Reposition; Hold and Aim stand still.
	SetMovementActive(NewState == ERRShooterState::Approach || NewState == ERRShooterState::Reposition);
}

void ARRShooter::UpdateBehavior(float DeltaSeconds)
{
	if (State == ERRShooterState::Dead)
	{
		return;
	}

	ARRMiner* Target = FindTarget();
	if (!Target)
	{
		bHasSight = false;
		if (State != ERRShooterState::Hold)
		{
			EnterState(ERRShooterState::Hold);
		}
		return;
	}

	const FVector Location = GetActorLocation();
	const FVector TargetLocation = Target->GetActorLocation();
	const float Distance = FVector::Dist2D(Location, TargetLocation);
	const ARRArenaGrid* Grid = GetGrid();
	bHasSight = Grid && Distance <= Stats.AttackRange && Grid->HasLineOfSight(Location, TargetLocation);
	const bool bCanFire = bHasSight && IsOnScreen();
	const double Now = GetWorld()->GetTimeSeconds();

	if (State == ERRShooterState::Aim)
	{
		if (!bCanFire || AimTarget.Get() != Target)
		{
			// Sight, range or screen lost: the aim is cancelled without a shot (the cadence is not consumed).
			EnterState(ERRShooterState::Hold);
		}
		else
		{
			FaceTarget(Target);
			if (HasStateLasted(Stats.WindUp))
			{
				EnterState(ERRShooterState::Hold);
				Fire(Target);
			}
			else
			{
				UpdateLaserBeam();
			}
			return;
		}
	}
	else if (bCanFire && Now + TimeTolerance >= GetNextAimTime())
	{
		StartAim(Target);
		return;
	}

	UpdateMovement(Target, Distance);
}

void ARRShooter::UpdateMovement(const ARRMiner* Target, float Distance)
{
	const float PreferredDistance = Stats.PreferredDistance;
	const float OuterDistance = GetRingOuterDistance();

	switch (State)
	{
	case ERRShooterState::Approach:
		// With sight it stops at the preferred distance; without sight it keeps closing in until it sees the target.
		EnemyMovement->SetArrivalDistance(bHasSight ? PreferredDistance * ArrivalRangeRatio : 0.f);
		// Arrived (the movement stops at the preferred distance) or already inside it.
		if (bHasSight && (Distance <= PreferredDistance || EnemyMovement->HasArrived()))
		{
			EnterState(ERRShooterState::Hold);
		}
		else if (!bHasSight && Distance <= OuterDistance)
		{
			// In range without sight: detour around the rock along the flow field.
			EnterState(ERRShooterState::Reposition);
		}
		break;

	case ERRShooterState::Hold:
		if (Distance > OuterDistance)
		{
			EnterState(ERRShooterState::Approach);
		}
		else if (!bHasSight)
		{
			EnterState(ERRShooterState::Reposition);
		}
		// With sight it stays put, however close the miner comes (it never backs away).
		break;

	case ERRShooterState::Reposition:
		if (Distance > OuterDistance)
		{
			EnterState(ERRShooterState::Approach);
		}
		else if (bHasSight)
		{
			// Sight regained: stop here and fire.
			EnterState(ERRShooterState::Hold);
		}
		break;

	case ERRShooterState::Aim:
	case ERRShooterState::Dead:
		break;
	}
}

float ARRShooter::GetRingOuterDistance() const
{
	return FMath::Min(Stats.PreferredDistance + Stats.PreferredDistanceTolerance, Stats.AttackRange);
}

double ARRShooter::GetNextAimTime() const
{
	if (ShotCount == 0)
	{
		return 0.0;
	}
	// The aim is part of the interval: continuous shots are 1 / AttackRate apart.
	const double Interval = Stats.AttackRate > 0.f ? 1.0 / Stats.AttackRate : 0.0;
	return LastShotTime + FMath::Max(0.0, Interval - Stats.WindUp);
}

void ARRShooter::StartAim(ARRMiner* Target)
{
	EnterState(ERRShooterState::Aim);
	AimTarget = Target;
	LastAimStartTime = GetWorld()->GetTimeSeconds();
	FaceTarget(Target);
	UpdateLaserBeam();
	OnAimStart(Target);
}

void ARRShooter::Fire(const ARRMiner* Target)
{
	++ShotCount;
	LastShotTime = GetWorld()->GetTimeSeconds();

	if (!ProjectileClass || Stats.ProjectileSpeed <= 0.f)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: no shot fired (projectile class %s, speed %.0f)."), *GetName(), *GetNameSafe(ProjectileClass), Stats.ProjectileSpeed);
		OnFire();
		return;
	}

	const FVector Origin = GetAimOrigin();
	const FVector TargetLocation = Target->GetActorLocation();
	FVector Direction = FVector(TargetLocation.X - Origin.X, TargetLocation.Y - Origin.Y, 0.f).GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		Direction = GetActorForwardVector();
	}
	const float Lifetime = Stats.AttackRange / Stats.ProjectileSpeed;
	ARRProjectile::SpawnProjectile(GetWorld(), ProjectileClass, FTransform(Direction.Rotation(), Origin), this, this,
		MakeDamageSpec(DamageEffectClass, Stats.Damage), Stats.ProjectileSpeed, Lifetime);
	OnFire();
}

void ARRShooter::FaceTarget(const AActor* Target)
{
	if (!Target)
	{
		return;
	}
	const FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
	if (!ToTarget.IsNearlyZero())
	{
		SetActorRotation(FRotator(0.f, ToTarget.Rotation().Yaw, 0.f));
	}
}

void ARRShooter::UpdateLaserBeam()
{
	const FVector Origin = GetAimOrigin();
	FVector Direction = GetAimTargetPoint() - Origin;
	Direction.Z = 0.f;
	Direction = Direction.GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		Direction = GetActorForwardVector();
	}
	const float Length = GetLaserLength();
	const float Thickness = LaserThickness / LaserMeshSize;
	LaserBeam->SetWorldLocationAndRotation(Origin + Direction * (Length * 0.5f), Direction.Rotation());
	LaserBeam->SetWorldScale3D(FVector(FMath::Max(Length, KINDA_SMALL_NUMBER) / LaserMeshSize, Thickness, Thickness));
	LaserBeam->SetVisibility(Length > 0.f);
}

FVector ARRShooter::MakeNonRetreatingDirection(const FVector& Direction, const FVector& TargetLocation) const
{
	const FVector Location = GetActorLocation();
	const FVector Away = FVector(Location.X - TargetLocation.X, Location.Y - TargetLocation.Y, 0.f).GetSafeNormal();
	const double AwayPart = FVector::DotProduct(Direction, Away);
	if (Away.IsNearlyZero() || AwayPart <= 0.0)
	{
		return Direction;
	}
	// Slide around the target instead: the tangential part, nudged inward so the distance never grows.
	const FVector Tangent = Direction - Away * AwayPart;
	if (Tangent.IsNearlyZero(1.e-3))
	{
		return FVector::ZeroVector;
	}
	return (Tangent.GetSafeNormal() - Away * RepositionInwardBias).GetSafeNormal();
}

ARRArenaGrid* ARRShooter::GetGrid()
{
	if (!CachedGrid.IsValid())
	{
		CachedGrid = RRWorld::FindArenaGrid(GetWorld());
	}
	return CachedGrid.Get();
}

void ARRShooter::HandleDeath(AActor* DeathInstigator)
{
	if (State != ERRShooterState::Dead)
	{
		EnterState(ERRShooterState::Dead);
	}
	Super::HandleDeath(DeathInstigator);
	// The base death shows every mesh again; the laser stays off.
	LaserBeam->SetVisibility(false);
}

void ARRShooter::ResetStateMachine()
{
	if (State != ERRShooterState::Dead)
	{
		EnterState(ERRShooterState::Approach);
	}
}
