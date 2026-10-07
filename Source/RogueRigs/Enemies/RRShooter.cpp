// Rogue Rigs - shooter robot: closes in to its hold distance and never backs away from the miner, fires dodgeable shots with grid line of sight while on screen, telegraphs each shot with a targeting laser (C++ state machine Approach, Hold, Reposition, Aim, Dead).

#include "Enemies/RRShooter.h"

#include "Algo/Reverse.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Enemies/RREnemySpawner.h"
#include "Engine/World.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "FlowField/RRFlowFieldSubsystem.h"
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
	if (NewState != ERRShooterState::Reposition)
	{
		RepositionWaypoints.Reset();
		RepositionCell = FIntPoint(INDEX_NONE, INDEX_NONE);
	}

	State = NewState;
	MarkStateStart();

	if (NewState == ERRShooterState::Dead)
	{
		// The base death stops the movement.
		return;
	}
	// Reposition steers with its own input; the flow field drives Approach only.
	SetMovementActive(NewState == ERRShooterState::Approach);
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
			TryReposition(Target);
		}
		break;

	case ERRShooterState::Hold:
		if (Distance > OuterDistance)
		{
			EnterState(ERRShooterState::Approach);
			break;
		}
		// With sight it stays put, however close the miner comes (it never backs away).
		if (!bHasSight)
		{
			TryReposition(Target);
		}
		break;

	case ERRShooterState::Reposition:
	{
		if (bHasSight && Distance <= OuterDistance)
		{
			EnterState(ERRShooterState::Hold);
			break;
		}
		const ARRArenaGrid* Grid = CachedGrid.Get();
		if (Grid && Grid->GetCellAtWorld(Target->GetActorLocation()) != RepositionTargetCell)
		{
			// The target changed cell: plan again (throttled).
			TryReposition(Target);
			if (State != ERRShooterState::Reposition)
			{
				break;
			}
		}
		if (FollowRepositionPath(Target) || HasStateLasted(RepositionMaxDuration))
		{
			EnterState(ERRShooterState::Hold);
		}
		break;
	}

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

void ARRShooter::TryReposition(const ARRMiner* Target)
{
	const double Now = GetWorld()->GetTimeSeconds();
	if (Now - LastSearchTime + TimeTolerance < RepositionSearchInterval)
	{
		return;
	}
	LastSearchTime = Now;

	TArray<FVector> Waypoints;
	FIntPoint Cell;
	if (FindRepositionPath(Target, Waypoints, Cell))
	{
		if (State != ERRShooterState::Reposition)
		{
			EnterState(ERRShooterState::Reposition);
		}
		else
		{
			MarkStateStart();
		}
		RepositionWaypoints = MoveTemp(Waypoints);
		RepositionCell = Cell;
		RepositionTargetCell = CachedGrid.IsValid() ? CachedGrid->GetCellAtWorld(Target->GetActorLocation()) : FIntPoint(INDEX_NONE, INDEX_NONE);
		return;
	}

	// Nowhere better: close in without sight (sight comes nearer), stand with sight.
	const ERRShooterState Fallback = bHasSight ? ERRShooterState::Hold : ERRShooterState::Approach;
	if (State != Fallback)
	{
		EnterState(Fallback);
	}
}

bool ARRShooter::FindRepositionPath(const ARRMiner* Target, TArray<FVector>& OutWaypoints, FIntPoint& OutCell)
{
	OutWaypoints.Reset();
	const ARRArenaGrid* Grid = GetGrid();
	URRFlowFieldSubsystem* FlowField = GetFlowField();
	if (!Grid || !FlowField || !Target)
	{
		return false;
	}
	FlowField->UpdateField();

	// Local BFS from the robot's cell over walkable cells of the square (8-neighbour, a diagonal only when both orthogonal
	// cells are walkable, like the flow field).
	const int32 Radius = FMath::Max(1, RepositionSearchRadiusCells);
	const int32 Side = 2 * Radius + 1;
	const FIntPoint StartCell = Grid->GetCellAtWorld(GetActorLocation());
	const FIntPoint Corner = StartCell - FIntPoint(Radius, Radius);
	auto LocalIndex = [Corner, Side](FIntPoint Cell) -> int32
	{
		const FIntPoint Local = Cell - Corner;
		return (Local.X < 0 || Local.Y < 0 || Local.X >= Side || Local.Y >= Side) ? INDEX_NONE : Local.Y * Side + Local.X;
	};

	TArray<int32> Steps;
	Steps.Init(INDEX_NONE, Side * Side);
	TArray<int32> Parents;
	Parents.Init(INDEX_NONE, Side * Side);
	TArray<FIntPoint> Queue;
	Queue.Reserve(Side * Side);
	Steps[LocalIndex(StartCell)] = 0;
	Queue.Add(StartCell);

	static const FIntPoint Offsets[8] = {
		FIntPoint(1, 0), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(0, -1),
		FIntPoint(1, 1), FIntPoint(1, -1), FIntPoint(-1, 1), FIntPoint(-1, -1) };
	for (int32 Head = 0; Head < Queue.Num(); ++Head)
	{
		const FIntPoint Cell = Queue[Head];
		const int32 CellIndex = LocalIndex(Cell);
		for (const FIntPoint& Offset : Offsets)
		{
			const FIntPoint Next = Cell + Offset;
			const int32 NextIndex = LocalIndex(Next);
			if (NextIndex == INDEX_NONE || Steps[NextIndex] != INDEX_NONE || !FlowField->IsWalkableCell(Next))
			{
				continue;
			}
			if (Offset.X != 0 && Offset.Y != 0
				&& (!FlowField->IsWalkableCell(FIntPoint(Next.X, Cell.Y)) || !FlowField->IsWalkableCell(FIntPoint(Cell.X, Next.Y))))
			{
				continue;
			}
			Steps[NextIndex] = Steps[CellIndex] + 1;
			Parents[NextIndex] = CellIndex;
			Queue.Add(Next);
		}
	}

	// Candidates: reachable cells of the square, locally reachable, no farther from the target than the robot now (it never
	// backs away) and within the hold distance, with grid sight to the target.
	const FVector TargetLocation = Target->GetActorLocation();
	const float MaxDistance = FMath::Min(GetRingOuterDistance(), static_cast<float>(FVector::Dist2D(GetActorLocation(), TargetLocation)));
	TArray<FIntPoint> Candidates;
	FlowField->GatherReachableCells(Candidates, [&](FIntPoint Cell, int32 /*GoalSteps*/)
	{
		const int32 Index = LocalIndex(Cell);
		if (Index == INDEX_NONE || Steps[Index] == INDEX_NONE)
		{
			return false;
		}
		const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
		const float Distance = FVector::Dist2D(Center, TargetLocation);
		return Distance <= MaxDistance && Grid->HasLineOfSight(Center, TargetLocation);
	});

	int32 BestIndex = INDEX_NONE;
	float BestRingError = 0.f;
	for (const FIntPoint& Cell : Candidates)
	{
		const int32 Index = LocalIndex(Cell);
		const float RingError = FMath::Abs(FVector::Dist2D(Grid->GetCellCenter(Cell.X, Cell.Y), TargetLocation) - Stats.PreferredDistance);
		if (BestIndex == INDEX_NONE || Steps[Index] < Steps[BestIndex] || (Steps[Index] == Steps[BestIndex] && RingError < BestRingError))
		{
			BestIndex = Index;
			BestRingError = RingError;
		}
	}
	if (BestIndex == INDEX_NONE)
	{
		return false;
	}

	OutCell = Corner + FIntPoint(BestIndex % Side, BestIndex / Side);
	const float Z = GetActorLocation().Z;
	for (int32 Index = BestIndex; Index != INDEX_NONE; Index = Parents[Index])
	{
		const FIntPoint Cell = Corner + FIntPoint(Index % Side, Index / Side);
		// The start cell is skipped unless it is the destination itself.
		if (Index != LocalIndex(StartCell) || OutWaypoints.IsEmpty())
		{
			const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
			OutWaypoints.Add(FVector(Center.X, Center.Y, Z));
		}
	}
	Algo::Reverse(OutWaypoints);
	return true;
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

bool ARRShooter::FollowRepositionPath(const ARRMiner* Target)
{
	if (RepositionWaypoints.IsEmpty())
	{
		return true;
	}
	const FVector Location = GetActorLocation();
	URRFlowFieldSubsystem* FlowField = GetFlowField();
	if (RepositionWaypoints.Num() > 1 && FlowField && FlowField->HasDirectPath(Location, RepositionWaypoints[1]))
	{
		RepositionWaypoints.RemoveAt(0);
	}
	if (FVector::Dist2D(Location, RepositionWaypoints[0]) <= WaypointAcceptanceRadius)
	{
		RepositionWaypoints.RemoveAt(0);
		if (RepositionWaypoints.IsEmpty())
		{
			return true;
		}
	}
	FVector Direction = FVector(RepositionWaypoints[0].X - Location.X, RepositionWaypoints[0].Y - Location.Y, 0.f).GetSafeNormal();
	if (Target)
	{
		Direction = MakeNonRetreatingDirection(Direction, Target->GetActorLocation());
	}
	if (!Direction.IsNearlyZero())
	{
		AddMovementInput(Direction, 1.f);
	}
	return false;
}

ARRArenaGrid* ARRShooter::GetGrid()
{
	if (!CachedGrid.IsValid())
	{
		CachedGrid = RRWorld::FindArenaGrid(GetWorld());
	}
	return CachedGrid.Get();
}

URRFlowFieldSubsystem* ARRShooter::GetFlowField() const
{
	UWorld* World = GetWorld();
	return World ? World->GetSubsystem<URRFlowFieldSubsystem>() : nullptr;
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
