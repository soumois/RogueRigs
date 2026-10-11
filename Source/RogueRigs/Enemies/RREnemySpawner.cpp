// Rogue Rigs - enemy spawner: wave director (calm, lone robots, packs of one type from several sides, announced hordes) or the older trickle then packs, capped alive count, on reachable floor cells just outside the camera view; robots left far off screen are relocated into the spawn band.

#include "Enemies/RREnemySpawner.h"

#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemy.h"
#include "Enemies/RRShooter.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Grid/RRArenaGrid.h"
#include "HAL/IConsoleManager.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "RogueRigs.h"

namespace RRSpawner
{
	TAutoConsoleVariable<int32> CVarEnabled(TEXT("rr.Spawner.Enabled"), 1,
		TEXT("0: enemy spawners stop spawning on their own. 1: they spawn (default)."), ECVF_Cheat);

	/** Shallowest downward slope (Z per horizontal unit) of a frustum corner ray, so rays above the horizon still hit the floor. */
	constexpr float MinRaySlope = 0.05f;
}

ARREnemySpawner::ARREnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	DirectorConfig = FRRWaveDirectorConfig::MakeDefault();
}

void ARREnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	RandomStream.GenerateNewSeed();
	Director.Reset();
}

void ARREnemySpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CancelPendingSpawns();
	Super::EndPlay(EndPlayReason);
}

void ARREnemySpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RecordHeadingSample();
	RecycleFarEnemies(DeltaSeconds);
	if (!IsSpawningEnabled())
	{
		return;
	}
	RunTime += DeltaSeconds;
	if (DirectorConfig.bEnabled)
	{
		TickDirector(DeltaSeconds);
		return;
	}

	if (!bPackPhase && RunTime >= TrickleDuration)
	{
		// The first pack comes as the phase starts.
		bPackPhase = true;
		PackTimer = PackInterval;
		LoneTimer = 0.f;
	}
	else if (bPackPhase)
	{
		PackTimer += DeltaSeconds;
	}

	if (!bPackPhase)
	{
		SpawnTimer += DeltaSeconds;
		if (SpawnTimer >= SpawnInterval)
		{
			SpawnTimer -= SpawnInterval;
			TrySpawnEnemy();
		}
		return;
	}

	if (PackTimer >= PackInterval)
	{
		PackTimer = FMath::Fmod(PackTimer, PackInterval);
		LoneTimer = 0.f;
		TrySpawnPack();
		return;
	}
	LoneTimer += DeltaSeconds;
	if (LoneTimer >= SpawnInterval)
	{
		LoneTimer -= SpawnInterval;
		if (RandomStream.FRand() < LoneSpawnChance)
		{
			TrySpawnEnemy();
		}
	}
}

bool ARREnemySpawner::IsSpawningEnabled() const
{
	return bEnabled && RRSpawner::CVarEnabled.GetValueOnGameThread() != 0;
}

void ARREnemySpawner::SetViewRectangleOverride(FBox2D Rectangle)
{
	ViewOverride = Rectangle;
	bHasViewOverride = true;
}

URRFlowFieldSubsystem* ARREnemySpawner::GetFlowField() const
{
	UWorld* World = GetWorld();
	return World ? World->GetSubsystem<URRFlowFieldSubsystem>() : nullptr;
}

ARRArenaGrid* ARREnemySpawner::GetGrid() const
{
	URRFlowFieldSubsystem* FlowField = GetFlowField();
	if (!FlowField)
	{
		return nullptr;
	}
	FlowField->UpdateField();
	return FlowField->GetGrid();
}

FBox2D ARREnemySpawner::ProjectViewOnFloor(const FMinimalViewInfo& View, float AspectRatio, float FloorZ)
{
	const float TanX = FMath::Tan(FMath::DegreesToRadians(View.FOV * 0.5f));
	const float TanY = TanX / FMath::Max(AspectRatio, KINDA_SMALL_NUMBER);
	const FRotationMatrix Axes(View.Rotation);
	const FVector Forward = Axes.GetUnitAxis(EAxis::X);
	const FVector Right = Axes.GetUnitAxis(EAxis::Y);
	const FVector Up = Axes.GetUnitAxis(EAxis::Z);
	const float Height = View.Location.Z - FloorZ;

	FBox2D Result(ForceInit);
	for (const float SignX : { -1.f, 1.f })
	{
		for (const float SignY : { -1.f, 1.f })
		{
			FVector Ray = Forward + Right * (SignX * TanX) + Up * (SignY * TanY);
			Ray.Z = FMath::Min(Ray.Z, -RRSpawner::MinRaySlope * Ray.Size2D());
			const float Distance = Height / -Ray.Z;
			const FVector Hit = View.Location + Ray * Distance;
			Result += FVector2D(Hit.X, Hit.Y);
		}
	}
	return Result;
}

float ARREnemySpawner::GetDistanceToRectangle(const FBox2D& Rectangle, const FVector2D& Point)
{
	const double GapX = FMath::Max3(0.0, Rectangle.Min.X - Point.X, Point.X - Rectangle.Max.X);
	const double GapY = FMath::Max3(0.0, Rectangle.Min.Y - Point.Y, Point.Y - Rectangle.Max.Y);
	return static_cast<float>(FMath::Max(GapX, GapY));
}

ERRSpawnSide ARREnemySpawner::GetSideOfRectangle(const FBox2D& Rectangle, const FVector2D& Point)
{
	const double GapNegX = Rectangle.Min.X - Point.X;
	const double GapPosX = Point.X - Rectangle.Max.X;
	const double GapNegY = Rectangle.Min.Y - Point.Y;
	const double GapPosY = Point.Y - Rectangle.Max.Y;
	const double GapX = FMath::Max3(0.0, GapNegX, GapPosX);
	const double GapY = FMath::Max3(0.0, GapNegY, GapPosY);
	if (GapX <= 0.0 && GapY <= 0.0)
	{
		return ERRSpawnSide::None;
	}
	if (GapX >= GapY)
	{
		return GapNegX > 0.0 ? ERRSpawnSide::NegativeX : ERRSpawnSide::PositiveX;
	}
	return GapNegY > 0.0 ? ERRSpawnSide::NegativeY : ERRSpawnSide::PositiveY;
}

bool ARREnemySpawner::GetViewRectangle(FBox2D& OutRectangle) const
{
	if (bHasViewOverride)
	{
		OutRectangle = ViewOverride;
		return true;
	}
	UWorld* World = GetWorld();
	const ARRArenaGrid* Grid = GetGrid();
	if (!World || !Grid)
	{
		return false;
	}
	// The goal's (miner's) camera is the fallback: headless worlds have no player controller.
	const URRFlowFieldSubsystem* FlowField = GetFlowField();
	return ComputeCameraViewRectangle(World, Grid->GetActorLocation().Z, FlowField ? FlowField->GetGoalActor() : nullptr, OutRectangle);
}

bool ARREnemySpawner::ComputeCameraViewRectangle(UWorld* World, float FloorZ, const AActor* FallbackCameraOwner, FBox2D& OutRectangle)
{
	if (!World)
	{
		return false;
	}

	// 1. The local player's camera manager (what the screen shows).
	const APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController && PlayerController->PlayerCameraManager)
	{
		const FMinimalViewInfo& View = PlayerController->PlayerCameraManager->GetCameraCacheView();
		float AspectRatio = View.AspectRatio;
		if (UGameViewportClient* Viewport = World->GetGameViewport())
		{
			FVector2D ViewportSize;
			Viewport->GetViewportSize(ViewportSize);
			if (ViewportSize.X > 0.f && ViewportSize.Y > 0.f)
			{
				AspectRatio = ViewportSize.X / ViewportSize.Y;
			}
		}
		if (View.FOV > 0.f)
		{
			OutRectangle = ProjectViewOnFloor(View, AspectRatio, FloorZ);
			return true;
		}
	}

	// 2. The fallback actor's (miner's) camera component: headless worlds have no player controller.
	if (UCameraComponent* Camera = FallbackCameraOwner ? FallbackCameraOwner->FindComponentByClass<UCameraComponent>() : nullptr)
	{
		FMinimalViewInfo View;
		Camera->GetCameraView(0.f, View);
		OutRectangle = ProjectViewOnFloor(View, View.AspectRatio, FloorZ);
		return true;
	}
	return false;
}

void ARREnemySpawner::GetSpawnCandidates(TArray<FIntPoint>& OutCells)
{
	OutCells.Reset();
	URRFlowFieldSubsystem* FlowField = GetFlowField();
	const ARRArenaGrid* Grid = GetGrid();
	FBox2D View;
	if (!FlowField || !Grid || !GetViewRectangle(View))
	{
		return;
	}

	TSet<FIntPoint> Occupied;
	FlowField->GatherPawnCells(Occupied);

	// Reachable free cells between SpawnBandMinDistance and the fallback limit outside the view rectangle (one scan).
	const float MinDistance = SpawnBandMinDistance;
	const float BandWidth = FMath::Max(SpawnBandMaxDistance - SpawnBandMinDistance, 0.f);
	const float SearchLimit = FMath::Max(SpawnBandMaxDistance, SpawnBandFallbackMaxDistance + BandWidth);
	float Nearest = TNumericLimits<float>::Max();
	auto CellDistance = [Grid, &View](const FIntPoint& Cell)
	{
		const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
		return GetDistanceToRectangle(View, FVector2D(Center.X, Center.Y));
	};
	// Cells whose walk to the miner is a long detour (path steps x cell size over the straight distance above MaxPathDetourRatio).
	// While the miner runs, front sector cells are exempt: robots must also wait beyond the rock it digs toward.
	const AActor* Goal = FlowField->GetGoalActor();
	FVector2D Heading;
	FVector2D HeadingOrigin;
	const bool bRunning = GetRunHeading(Heading) && GetHeadingOrigin(HeadingOrigin);
	const FVector2D GoalLocation = Goal ? FVector2D(Goal->GetActorLocation()) : FVector2D::ZeroVector;
	const float CellSize = Grid->GetCellSize();
	TSet<FIntPoint> Detours;
	FlowField->GatherReachableCells(OutCells, [this, Grid, Goal, &GoalLocation, CellSize, &CellDistance, MinDistance, SearchLimit, &Nearest, &Detours, bRunning, &Heading, &HeadingOrigin](FIntPoint Cell, int32 Steps)
	{
		const float Distance = CellDistance(Cell);
		if (Distance < MinDistance || Distance > SearchLimit)
		{
			return false;
		}
		Nearest = FMath::Min(Nearest, Distance);
		if (Goal && MaxPathDetourRatio > 0.f)
		{
			const FVector2D Center(Grid->GetCellCenter(Cell.X, Cell.Y));
			const double Straight = FVector2D::Distance(Center, GoalLocation);
			if (Steps * CellSize > Straight * MaxPathDetourRatio
				&& (!bRunning || GetSectorOfPoint(HeadingOrigin, Heading, Center, FrontHalfAngle, BackHalfAngle) != ERRSpawnSector::Front))
			{
				Detours.Add(Cell);
			}
		}
		return true;
	}, /*bFloorOnly*/ false, &Occupied);
	if (OutCells.IsEmpty())
	{
		return;
	}

	// The band; when it is empty, the nearest band of the same width beyond it (near edge within SpawnBandFallbackMaxDistance).
	float Limit = SpawnBandMaxDistance;
	if (Nearest > SpawnBandMaxDistance)
	{
		if (Nearest > SpawnBandFallbackMaxDistance)
		{
			OutCells.Reset();
			return;
		}
		Limit = Nearest + BandWidth;
		UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: spawn band empty, fallback band %.0f-%.0f uu."), *GetName(), Nearest, Limit);
	}
	OutCells.RemoveAll([&CellDistance, Limit](const FIntPoint& Cell) { return CellDistance(Cell) > Limit; });

	// Direct cells first: detour cells only when the band has nothing else.
	if (!Detours.IsEmpty() && OutCells.ContainsByPredicate([&Detours](const FIntPoint& Cell) { return !Detours.Contains(Cell); }))
	{
		OutCells.RemoveAll([&Detours](const FIntPoint& Cell) { return Detours.Contains(Cell); });
	}
}

TSubclassOf<ARREnemy> ARREnemySpawner::PickEnemyClass(const FRandomStream& Stream) const
{
	// Entries with a class and a positive weight; those at their alive cap are left out of this draw.
	TArray<const FRRWeightedEnemyClass*, TInlineAllocator<8>> Open;
	bool bAnyValid = false;
	float TotalWeight = 0.f;
	for (const FRRWeightedEnemyClass& Entry : WeightedEnemyClasses)
	{
		if (!Entry.EnemyClass || Entry.Weight <= 0.f)
		{
			continue;
		}
		bAnyValid = true;
		if (Entry.MaxAlive > 0 && GetAliveCountOfClass(Entry.EnemyClass) >= Entry.MaxAlive)
		{
			continue;
		}
		Open.Add(&Entry);
		TotalWeight += Entry.Weight;
	}
	if (!bAnyValid)
	{
		return EnemyClass;
	}
	if (Open.IsEmpty())
	{
		return nullptr;
	}
	float Draw = Stream.FRand() * TotalWeight;
	for (const FRRWeightedEnemyClass* Entry : Open)
	{
		if (Draw < Entry->Weight)
		{
			return Entry->EnemyClass;
		}
		Draw -= Entry->Weight;
	}
	// Float rounding at the top of the range.
	return Open.Last()->EnemyClass;
}

int32 ARREnemySpawner::GetAliveCountOfClass(TSubclassOf<ARREnemy> Class) const
{
	int32 Count = 0;
	for (const TWeakObjectPtr<ARREnemy>& Enemy : AliveEnemies)
	{
		const ARREnemy* Robot = Enemy.Get();
		Count += Robot && Class && Robot->IsA(Class) && !Robot->IsActorBeingDestroyed() && Robot->HoldsSpawnSlot() ? 1 : 0;
	}
	// Pending robots hold their class slot too (a wave never overshoots a class cap while the pool creates it).
	for (const FPendingSpawn& Pending : PendingSpawns)
	{
		Count += Class && Pending.Class && Pending.Class->IsChildOf(Class) ? 1 : 0;
	}
	return Count;
}

TSubclassOf<ARREnemy> ARREnemySpawner::PickEnemyClass() const
{
	return PickEnemyClass(RandomStream);
}

void ARREnemySpawner::GetSpawnableClasses(TArray<TSubclassOf<ARREnemy>>& OutClasses) const
{
	if (EnemyClass)
	{
		OutClasses.AddUnique(EnemyClass);
	}
	for (const FRRWeightedEnemyClass& Entry : WeightedEnemyClasses)
	{
		if (Entry.EnemyClass)
		{
			OutClasses.AddUnique(Entry.EnemyClass);
		}
	}
}

ARREnemy* ARREnemySpawner::TrySpawnEnemy()
{
	return TrySpawnEnemyOfClass(nullptr);
}

ARREnemy* ARREnemySpawner::TrySpawnEnemyOfClass(TSubclassOf<ARREnemy> WantedClass)
{
	const TSubclassOf<ARREnemy> SpawnClass = WantedClass ? ResolveRobotClass(WantedClass) : PickEnemyClass();
	if (!GetWorld() || !SpawnClass || GetOccupiedCount() >= MaxAlive || (WantedClass && GetClassRoom(SpawnClass) <= 0))
	{
		return nullptr;
	}
	TArray<FIntPoint> Candidates;
	GetSpawnCandidates(Candidates);
	if (Candidates.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: no spawn cell available."), *GetName());
		return nullptr;
	}
	ARREnemy* Enemy = nullptr;
	SpawnEnemyOnCell(SpawnClass, Candidates[PickSpawnCellIndex(Candidates)], Enemy);
	return Enemy;
}

int32 ARREnemySpawner::PickSpawnCellIndex(const TArray<FIntPoint>& Candidates)
{
	if (Candidates.IsEmpty())
	{
		return INDEX_NONE;
	}
	// Running: keep the cells of the drawn sector (front, sides or back of the run heading).
	const ERRSpawnSector Sector = DrawSpawnSector(Candidates);
	if (Sector == ERRSpawnSector::None)
	{
		return RandomStream.RandRange(0, Candidates.Num() - 1);
	}
	TArray<int32> InSector;
	for (int32 Index = 0; Index < Candidates.Num(); ++Index)
	{
		if (GetCellSector(Candidates[Index]) == Sector)
		{
			InSector.Add(Index);
		}
	}
	// DrawSpawnSector only returns a sector holding a cell; the guard keeps a draw if the heading changed in between.
	return InSector.IsEmpty() ? RandomStream.RandRange(0, Candidates.Num() - 1) : InSector[RandomStream.RandRange(0, InSector.Num() - 1)];
}

FVector ARREnemySpawner::GetStandingLocation(const ARREnemy* Enemy, const FIntPoint& Cell) const
{
	const ARRArenaGrid* Grid = GetGrid();
	const float HalfHeight = Enemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	return Grid->GetCellCenter(Cell.X, Cell.Y) + FVector(0.f, 0.f, HalfHeight + SpawnFloorClearance);
}

TArray<ARREnemy*> ARREnemySpawner::TrySpawnPack()
{
	TArray<ARREnemy*> Spawned;
	const int32 FreeSlots = MaxAlive - GetOccupiedCount();
	FBox2D View;
	if (!GetWorld() || FreeSlots <= 0 || !GetViewRectangle(View))
	{
		return Spawned;
	}
	TArray<FIntPoint> Candidates;
	GetSpawnCandidates(Candidates);
	const ARRArenaGrid* Grid = GetGrid();
	if (Candidates.IsEmpty() || !Grid)
	{
		UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: no spawn cell available for a pack."), *GetName());
		return Spawned;
	}

	// Split the candidates by side (index = ERRSpawnSide - 1).
	constexpr int32 SideCount = 4;
	TArray<FIntPoint> BySide[SideCount];
	for (const FIntPoint& Cell : Candidates)
	{
		const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
		const ERRSpawnSide Side = GetSideOfRectangle(View, FVector2D(Center.X, Center.Y));
		if (Side != ERRSpawnSide::None)
		{
			BySide[static_cast<int32>(Side) - 1].Add(Cell);
		}
	}

	const int32 PackSize = FMath::Min(RandomStream.RandRange(PackSizeMin, FMath::Max(PackSizeMin, PackSizeMax)), FreeSlots);
	TArray<int32, TInlineAllocator<SideCount>> FullSides;
	int32 LargestSide = 0;
	for (int32 SideIndex = 0; SideIndex < SideCount; ++SideIndex)
	{
		if (BySide[SideIndex].Num() >= PackSize)
		{
			FullSides.Add(SideIndex);
		}
		if (BySide[SideIndex].Num() > BySide[LargestSide].Num())
		{
			LargestSide = SideIndex;
		}
	}
	const int32 SideIndex = FullSides.IsEmpty() ? LargestSide : FullSides[RandomStream.RandRange(0, FullSides.Num() - 1)];
	TArray<FIntPoint>& Cells = BySide[SideIndex];
	const int32 Count = FMath::Min(PackSize, Cells.Num());

	// Partial Fisher-Yates: Count distinct random cells of the side.
	for (int32 Index = 0; Index < Count; ++Index)
	{
		Cells.Swap(Index, RandomStream.RandRange(Index, Cells.Num() - 1));
		const TSubclassOf<ARREnemy> SpawnClass = PickEnemyClass();
		ARREnemy* Enemy = nullptr;
		if (SpawnClass && SpawnEnemyOnCell(SpawnClass, Cells[Index], Enemy) && Enemy)
		{
			Spawned.Add(Enemy);
		}
	}
	UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: pack of %d (drawn %d) from side %d, %d alive."), *GetName(), Spawned.Num(),
		PackSize, SideIndex + 1, AliveEnemies.Num());
	return Spawned;
}

bool ARREnemySpawner::SpawnEnemyOnCell(TSubclassOf<ARREnemy> SpawnClass, const FIntPoint& Cell, ARREnemy*& OutEnemy)
{
	OutEnemy = nullptr;
	UWorld* World = GetWorld();
	const ARRArenaGrid* Grid = GetGrid();
	if (!World || !Grid || !SpawnClass)
	{
		return false;
	}
	const float HalfHeight = SpawnClass->GetDefaultObject<ARREnemy>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector Location = Grid->GetCellCenter(Cell.X, Cell.Y) + FVector(0.f, 0.f, HalfHeight + SpawnFloorClearance);

	URRActorPoolSubsystem* Pool = World->GetSubsystem<URRActorPoolSubsystem>();
	if (!Pool)
	{
		// No pool in this world type: plain spawn, as before task-091-01.
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		OutEnemy = World->SpawnActor<ARREnemy>(SpawnClass, Location, FRotator::ZeroRotator, SpawnParams);
		TrackSpawnedEnemy(OutEnemy);
		return OutEnemy != nullptr;
	}

	const FTransform Transform(FRotator::ZeroRotator, Location);
	FRRPoolAcquireParams Params;
	Params.CollisionHandling = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Params.Requester = this;
	if (SpawnMaxFrames <= 0)
	{
		OutEnemy = Pool->AcquireImmediate<ARREnemy>(TSubclassOf<ARREnemy>(SpawnClass), Transform, Params);
		TrackSpawnedEnemy(OutEnemy);
		return OutEnemy != nullptr;
	}
	Params.MaxFrames = SpawnMaxFrames;
	// The robot joins the alive list only on delivery (at once for a reused robot, inside Acquire; later for a creation).
	TSharedRef<TWeakObjectPtr<ARREnemy>> Delivered = MakeShared<TWeakObjectPtr<ARREnemy>>();
	TWeakObjectPtr<ARREnemySpawner> WeakThis(this);
	Params.OnReady = [WeakThis, Delivered](AActor* Actor)
	{
		ARREnemy* Enemy = Cast<ARREnemy>(Actor);
		*Delivered = Enemy;
		if (ARREnemySpawner* Spawner = WeakThis.Get())
		{
			Spawner->TrackSpawnedEnemy(Enemy);
		}
	};
	const FRRPoolRequestHandle Handle = Pool->Acquire(SpawnClass, Transform, MoveTemp(Params));
	if (Handle.IsValid())
	{
		PendingSpawns.Add({ Handle, SpawnClass });
		return true;
	}
	OutEnemy = Delivered->Get();
	return OutEnemy != nullptr;
}

void ARREnemySpawner::TrackSpawnedEnemy(ARREnemy* Enemy)
{
	PrunePending();
	if (!Enemy)
	{
		return;
	}
	AliveEnemies.AddUnique(Enemy);
	RecycleStates.Add(Enemy, FRecycleState());
	UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: %s spawned, %d alive, %d pending."), *GetName(), *Enemy->GetName(),
		AliveEnemies.Num(), PendingSpawns.Num());
}

void ARREnemySpawner::PrunePending()
{
	const URRActorPoolSubsystem* Pool = GetWorld() ? GetWorld()->GetSubsystem<URRActorPoolSubsystem>() : nullptr;
	PendingSpawns.RemoveAll([Pool](const FPendingSpawn& Pending) { return !Pool || !Pool->IsRequestPending(Pending.Handle); });
}

void ARREnemySpawner::CancelPendingSpawns()
{
	URRActorPoolSubsystem* Pool = GetWorld() ? GetWorld()->GetSubsystem<URRActorPoolSubsystem>() : nullptr;
	for (FPendingSpawn& Pending : PendingSpawns)
	{
		if (Pool)
		{
			Pool->CancelRequest(Pending.Handle);
		}
	}
	PendingSpawns.Reset();
}

int32 ARREnemySpawner::GetPendingCount()
{
	PrunePending();
	return PendingSpawns.Num();
}

int32 ARREnemySpawner::GetOccupiedCount()
{
	return GetAliveCount() + GetPendingCount();
}

void ARREnemySpawner::PruneAlive()
{
	AliveEnemies.RemoveAll([](const TWeakObjectPtr<ARREnemy>& Enemy)
	{
		return !Enemy.IsValid() || Enemy->IsActorBeingDestroyed() || !Enemy->HoldsSpawnSlot() || URRActorPoolSubsystem::IsPooledIdle(Enemy.Get());
	});
}

void ARREnemySpawner::RecycleFarEnemies(float DeltaSeconds)
{
	PruneAlive();
	if (!bRecycleEnabled || AliveEnemies.IsEmpty())
	{
		RecycleStates.Reset();
		return;
	}
	FVector2D MinerLocation;
	FBox2D View;
	if (!GetHeadingOrigin(MinerLocation) || !GetViewRectangle(View))
	{
		return;
	}

	// Off-screen timers; robots far away whose delay ran out are due.
	const double DistanceSquared = FMath::Square(static_cast<double>(RecycleDistance));
	TArray<ARREnemy*, TInlineAllocator<16>> Due;
	for (const TWeakObjectPtr<ARREnemy>& Entry : AliveEnemies)
	{
		ARREnemy* Enemy = Entry.Get();
		FRecycleState& State = RecycleStates.FindOrAdd(Enemy);
		const FVector2D Location(Enemy->GetActorLocation());
		if (GetDistanceToRectangle(View, Location) <= RecycleOffScreenMargin)
		{
			State.bSeenOnScreen = true;
			State.OffScreenSeconds = 0.f;
			continue;
		}
		State.OffScreenSeconds += DeltaSeconds;
		const float Delay = State.bSeenOnScreen ? RecycleDelay : RecycleUnseenDelay;
		if (State.OffScreenSeconds >= Delay && FVector2D::DistSquared(Location, MinerLocation) > DistanceSquared)
		{
			Due.Add(Enemy);
		}
	}

	// One candidate scan for every robot due; each takes a distinct cell. No cell: it stays due and retries next tick.
	TArray<FIntPoint> Candidates;
	if (!Due.IsEmpty())
	{
		GetSpawnCandidates(Candidates);
	}
	const ARRArenaGrid* Grid = Candidates.IsEmpty() ? nullptr : GetGrid();
	for (ARREnemy* Enemy : Due)
	{
		const int32 CellIndex = Grid ? PickSpawnCellIndex(Candidates) : INDEX_NONE;
		if (CellIndex == INDEX_NONE)
		{
			UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: no spawn cell to relocate %s, retry next tick."), *GetName(), *Enemy->GetName());
			continue;
		}
		const FIntPoint Cell = Candidates[CellIndex];
		const FVector2D From(Enemy->GetActorLocation());
		// Not a kill: no HandleDeath, so no XP drop, no kill count, no explosion; the robot keeps its slot, class and health.
		if (!Enemy->RelocateTo(GetStandingLocation(Enemy, Cell)))
		{
			continue;
		}
		Candidates.RemoveAt(CellIndex);
		RecycleStates.Add(Enemy, FRecycleState());
		++RecycledCount;
		UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: %s relocated from %.0f uu to cell (%d, %d), %.0f uu from the miner."), *GetName(),
			*Enemy->GetName(), FVector2D::Distance(From, MinerLocation), Cell.X, Cell.Y, FVector2D::Distance(FVector2D(Enemy->GetActorLocation()), MinerLocation));
		OnEnemyRecycled.Broadcast(Enemy);
	}

	for (auto It = RecycleStates.CreateIterator(); It; ++It)
	{
		// Destroyed, dead or parked in the pool (a reused robot gets a fresh state on delivery).
		const ARREnemy* Tracked = It.Key().ResolveObjectPtr();
		if (!Tracked || Tracked->IsDead())
		{
			It.RemoveCurrent();
		}
	}
}

float ARREnemySpawner::GetRecycleSeconds(const ARREnemy* Enemy) const
{
	const FRecycleState* State = Enemy ? RecycleStates.Find(Enemy) : nullptr;
	return State ? State->OffScreenSeconds : 0.f;
}

bool ARREnemySpawner::HasBeenOnScreen(const ARREnemy* Enemy) const
{
	const FRecycleState* State = Enemy ? RecycleStates.Find(Enemy) : nullptr;
	return State && State->bSeenOnScreen;
}

int32 ARREnemySpawner::GetAliveCount()
{
	PruneAlive();
	return AliveEnemies.Num();
}

TArray<ARREnemy*> ARREnemySpawner::GetAliveEnemies()
{
	PruneAlive();
	TArray<ARREnemy*> Result;
	for (const TWeakObjectPtr<ARREnemy>& Enemy : AliveEnemies)
	{
		Result.Add(Enemy.Get());
	}
	return Result;
}

// --- Wave director ---

TSubclassOf<ARREnemy> ARREnemySpawner::ResolveRobotClass(TSubclassOf<ARREnemy> Wanted) const
{
	if (!Wanted)
	{
		return nullptr;
	}
	auto NativeOf = [](UClass* Class)
	{
		while (Class && !Class->HasAnyClassFlags(CLASS_Native))
		{
			Class = Class->GetSuperClass();
		}
		return Class;
	};
	for (const FRRWeightedEnemyClass& Entry : WeightedEnemyClasses)
	{
		if (Entry.EnemyClass && NativeOf(Entry.EnemyClass) == Wanted.Get())
		{
			return Entry.EnemyClass;
		}
	}
	if (EnemyClass && NativeOf(EnemyClass) == Wanted.Get())
	{
		return EnemyClass;
	}
	return Wanted;
}

int32 ARREnemySpawner::GetClassRoom(TSubclassOf<ARREnemy> SpawnClass) const
{
	for (const FRRWeightedEnemyClass& Entry : WeightedEnemyClasses)
	{
		if (Entry.EnemyClass == SpawnClass && Entry.MaxAlive > 0)
		{
			return FMath::Max(Entry.MaxAlive - GetAliveCountOfClass(Entry.EnemyClass), 0);
		}
	}
	return MAX_int32;
}

float ARREnemySpawner::GetDirectorAlert() const
{
	const UWorld* World = GetWorld();
	const URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
	return RunFlow ? RunFlow->GetAlert() : 0.f;
}

void ARREnemySpawner::TickDirector(float DeltaSeconds)
{
	DirectorActions.Reset();
	Director.Advance(DirectorConfig, DeltaSeconds, GetDirectorAlert(), RandomStream, DirectorActions);
	for (const FRRDirectorAction& Action : DirectorActions)
	{
		ExecuteDirectorAction(Action);
	}
	if (Director.GetActiveHorde() != INDEX_NONE && Director.GetHordeState(Director.GetActiveHorde()) == ERRHordeState::Spawning)
	{
		SpawnHordeUpTo(Director.GetHordeSpawnFraction(DirectorConfig));
	}
}

void ARREnemySpawner::ExecuteDirectorAction(const FRRDirectorAction& Action)
{
	switch (Action.Type)
	{
	case FRRDirectorAction::EType::PhaseChanged:
	{
		const FName PhaseName = DirectorConfig.Phases.IsValidIndex(Action.Index) ? DirectorConfig.Phases[Action.Index].Name : NAME_None;
		UE_LOG(LogRogueRigs, Log, TEXT("ARREnemySpawner %s: director phase %d (%s) at %.1f s."), *GetName(), Action.Index, *PhaseName.ToString(), RunTime);
		OnDirectorPhaseChanged.Broadcast(Action.Index, PhaseName);
		break;
	}
	case FRRDirectorAction::EType::SpawnSingle:
		TrySpawnEnemyOfClass(Action.SingleClass);
		break;
	case FRRDirectorAction::EType::SpawnWave:
		TrySpawnDirectorWave(TArray<int32>(Action.PackTypes));
		break;
	case FRRDirectorAction::EType::HordeLull:
		UE_LOG(LogRogueRigs, Log, TEXT("ARREnemySpawner %s: horde %d lull at %.1f s."), *GetName(), Action.Index, RunTime);
		OnHordeLull.Broadcast(Action.Index);
		break;
	case FRRDirectorAction::EType::HordeAnnounced:
		OnHordeIncoming.Broadcast(Action.Index, Action.Seconds);
		break;
	case FRRDirectorAction::EType::HordeStarted:
	{
		const ERRSpawnSide Side = PrepareHorde(Action.Index);
		UE_LOG(LogRogueRigs, Log, TEXT("ARREnemySpawner %s: horde %d front of %d from side %d at %.1f s."), *GetName(), Action.Index,
			HordePlan.Num(), static_cast<int32>(Side), RunTime);
		OnHordeStarted.Broadcast(Action.Index, Side);
		break;
	}
	case FRRDirectorAction::EType::HordeFinished:
	{
		const bool bPlanned = HordePlanIndex == Action.Index;
		if (bPlanned)
		{
			SpawnHordeUpTo(1.f);
		}
		OnHordeFinished.Broadcast(Action.Index, bPlanned ? HordeSpawned : 0);
		HordePlanIndex = INDEX_NONE;
		break;
	}
	case FRRDirectorAction::EType::OvertimeStarted:
		UE_LOG(LogRogueRigs, Log, TEXT("ARREnemySpawner %s: overtime at %.1f s."), *GetName(), RunTime);
		OnOvertimeStarted.Broadcast();
		break;
	}
}

void ARREnemySpawner::DrawMembers(const TArray<FRRDirectorPackMember>& Members, TMap<UClass*, int32>& Room, int32& ShooterRoom,
	TArray<TSubclassOf<ARREnemy>>& OutFront, TArray<TSubclassOf<ARREnemy>>& OutBack)
{
	// Room left for a resolved class (its WeightedEnemyClasses cap, shared by the whole wave).
	auto TakeRoom = [this, &Room](const TSubclassOf<ARREnemy>& SpawnClass, int32 Wanted)
	{
		int32* ClassRoom = Room.Find(SpawnClass.Get());
		if (!ClassRoom)
		{
			ClassRoom = &Room.Add(SpawnClass.Get(), GetClassRoom(SpawnClass));
		}
		const int32 Taken = FMath::Clamp(Wanted, 0, *ClassRoom);
		*ClassRoom -= Taken;
		return Taken;
	};
	TArray<TSubclassOf<ARREnemy>> Behind;
	for (const FRRDirectorPackMember& Member : Members)
	{
		const TSubclassOf<ARREnemy> SpawnClass = ResolveRobotClass(Member.RobotClass);
		if (!SpawnClass)
		{
			continue;
		}
		const int32 CountMin = FMath::Max(Member.CountMin, 0);
		const int32 Drawn = RandomStream.RandRange(CountMin, FMath::Max(CountMin, Member.CountMax));
		int32 Count = Drawn;
		int32 Replaced = 0;
		if (SpawnClass->IsChildOf(ARRShooter::StaticClass()))
		{
			// Shooter counter: the shooters beyond the room become replacement robots in the same places.
			Count = FMath::Clamp(Drawn, 0, FMath::Max(ShooterRoom, 0));
			ShooterRoom -= Count;
			Replaced = Drawn - Count;
		}
		Count = TakeRoom(SpawnClass, Count);
		TArray<TSubclassOf<ARREnemy>>& Target = Member.bFront ? OutFront : (Member.bBehind ? Behind : OutBack);
		for (int32 Index = 0; Index < Count; ++Index)
		{
			Target.Add(SpawnClass);
		}
		if (Replaced > 0)
		{
			const TSubclassOf<ARREnemy> Replacement = ResolveRobotClass(DirectorConfig.ShooterCapReplacement
				? DirectorConfig.ShooterCapReplacement : TSubclassOf<ARREnemy>(ARRChaser::StaticClass()));
			if (Replacement && !Replacement->IsChildOf(ARRShooter::StaticClass()))
			{
				Replaced = TakeRoom(Replacement, Replaced);
				for (int32 Index = 0; Index < Replaced; ++Index)
				{
					Target.Add(Replacement);
				}
			}
		}
	}
	for (int32 Index = OutBack.Num() - 1; Index > 0; --Index)
	{
		OutBack.Swap(Index, RandomStream.RandRange(0, Index));
	}
	// Behind members: at random in the back half of the mixed members, never leading the pack.
	const int32 FirstBehindSlot = (OutBack.Num() + Behind.Num()) / 2;
	for (const TSubclassOf<ARREnemy>& SpawnClass : Behind)
	{
		OutBack.Insert(SpawnClass, RandomStream.RandRange(FMath::Min(FirstBehindSlot, OutBack.Num()), OutBack.Num()));
	}
}

int32 ARREnemySpawner::GetDirectorShooterRoom() const
{
	return FMath::Max(DirectorConfig.ShooterMaxAlive - GetAliveCountOfClass(ARRShooter::StaticClass()), 0);
}

void ARREnemySpawner::SplitCandidatesBySide(const FBox2D& View, const TArray<FIntPoint>& Candidates, TArray<FIntPoint> (&OutBySide)[4]) const
{
	const ARRArenaGrid* Grid = GetGrid();
	if (!Grid)
	{
		return;
	}
	for (const FIntPoint& Cell : Candidates)
	{
		const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
		const ERRSpawnSide Side = GetSideOfRectangle(View, FVector2D(Center.X, Center.Y));
		if (Side != ERRSpawnSide::None)
		{
			OutBySide[static_cast<int32>(Side) - 1].Add(Cell);
		}
	}
}

namespace RRSpawner
{
	/** Picks a side index among BySide: a random one (not in Used) with at least Needed cells, else the unused one with the most cells, else any with the most; INDEX_NONE when every side is empty. */
	int32 PickSide(const TArray<FIntPoint> (&BySide)[4], const bool (&Used)[4], int32 Needed, FRandomStream& Stream)
	{
		TArray<int32, TInlineAllocator<4>> FullSides;
		int32 Largest = INDEX_NONE;
		for (int32 SideIndex = 0; SideIndex < 4; ++SideIndex)
		{
			if (Used[SideIndex] || BySide[SideIndex].IsEmpty())
			{
				continue;
			}
			if (BySide[SideIndex].Num() >= Needed)
			{
				FullSides.Add(SideIndex);
			}
			if (Largest == INDEX_NONE || BySide[SideIndex].Num() > BySide[Largest].Num())
			{
				Largest = SideIndex;
			}
		}
		if (!FullSides.IsEmpty())
		{
			return FullSides[Stream.RandRange(0, FullSides.Num() - 1)];
		}
		if (Largest == INDEX_NONE)
		{
			for (int32 SideIndex = 0; SideIndex < 4; ++SideIndex)
			{
				if (!BySide[SideIndex].IsEmpty() && (Largest == INDEX_NONE || BySide[SideIndex].Num() > BySide[Largest].Num()))
				{
					Largest = SideIndex;
				}
			}
		}
		return Largest;
	}
}

TArray<FRRSpawnedPack> ARREnemySpawner::TrySpawnDirectorWave(const TArray<int32>& PackTypeIndices)
{
	TArray<FRRSpawnedPack> Packs;
	int32 FreeSlots = MaxAlive - GetOccupiedCount();
	FBox2D View;
	const ARRArenaGrid* Grid = GetGrid();
	if (!GetWorld() || !Grid || FreeSlots <= 0 || PackTypeIndices.IsEmpty() || !GetViewRectangle(View))
	{
		return Packs;
	}
	TArray<FIntPoint> Candidates;
	GetSpawnCandidates(Candidates);
	if (Candidates.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: no spawn cell available for a wave."), *GetName());
		return Packs;
	}
	TArray<FIntPoint> BySide[4];
	SplitCandidatesBySide(View, Candidates, BySide);
	auto ViewDistance = [Grid, &View](const FIntPoint& Cell)
	{
		const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
		return GetDistanceToRectangle(View, FVector2D(Center.X, Center.Y));
	};

	TMap<UClass*, int32> Room;
	int32 ShooterRoom = GetDirectorShooterRoom();
	bool bSideUsed[4] = { false, false, false, false };
	// Angles around the miner (the view center without one) of the anchors already placed in this wave.
	const bool bSeparated = DirectorConfig.PackMinSeparationDegrees > 0.f;
	// When the packs cannot all keep the angle at random (3 packs at 120 degrees), the later ones aim at even steps from the first.
	const bool bEvenSpread = bSeparated && PackTypeIndices.Num() > 1
		&& PackTypeIndices.Num() * DirectorConfig.PackMinSeparationDegrees >= 360.f - UE_KINDA_SMALL_NUMBER;
	FVector2D Origin;
	if (!GetHeadingOrigin(Origin))
	{
		Origin = View.GetCenter();
	}
	TArray<double> UsedAngles;
	for (const int32 TypeIndex : PackTypeIndices)
	{
		if (!DirectorConfig.PackTypes.IsValidIndex(TypeIndex) || FreeSlots <= 0)
		{
			continue;
		}
		// Front members first, then the others mixed; clamped to the free slots.
		TArray<TSubclassOf<ARREnemy>> Classes;
		TArray<TSubclassOf<ARREnemy>> Back;
		DrawMembers(DirectorConfig.PackTypes[TypeIndex].Members, Room, ShooterRoom, Classes, Back);
		Classes.Append(Back);
		Classes.SetNum(FMath::Min(Classes.Num(), FreeSlots));
		if (Classes.IsEmpty())
		{
			continue;
		}
		// Running: the pack's sector is drawn first (front, sides, back), its side follows a random cell of that sector.
		// Level 1 (PackMinSeparationDegrees > 0): the anchor keeps that angle from the anchors already placed, else a distinct side per pack.
		const ERRSpawnSector Sector = DrawSpawnSector(Candidates);
		FIntPoint Anchor;
		const int32 SideIndex = bEvenSpread && !UsedAngles.IsEmpty()
			? PickAnchorNearAngle(BySide, Classes.Num(), Origin, UsedAngles[0] + UsedAngles.Num() * 360.0 / PackTypeIndices.Num(), Anchor)
			: bEvenSpread
			? PickEvenSpreadStart(BySide, Classes.Num(), Sector, Origin, PackTypeIndices.Num(), Anchor)
			: bSeparated
			? PickSeparatedAnchor(BySide, Classes.Num(), Sector, Origin, UsedAngles, DirectorConfig.PackMinSeparationDegrees, Anchor)
			: PickSideForSector(BySide, bSideUsed, Classes.Num(), Sector, &Anchor);
		if (SideIndex == INDEX_NONE)
		{
			break;
		}
		bSideUsed[SideIndex] = true;
		const FVector AnchorCenter = Grid->GetCellCenter(Anchor.X, Anchor.Y);
		UsedAngles.Add(FMath::RadiansToDegrees(FMath::Atan2(AnchorCenter.Y - Origin.Y, AnchorCenter.X - Origin.X)));
		TArray<FIntPoint>& SideCells = BySide[SideIndex];

		// Adjacent cells: the ones nearest to the anchor of the side, then the nearest to the view first (the front).
		SideCells.StableSort([Anchor](const FIntPoint& A, const FIntPoint& B) { return (A - Anchor).SizeSquared() < (B - Anchor).SizeSquared(); });
		const int32 Count = FMath::Min(Classes.Num(), SideCells.Num());
		TArray<FIntPoint> PackCells(SideCells.GetData(), Count);
		SideCells.RemoveAt(0, Count);
		PackCells.StableSort([&ViewDistance](const FIntPoint& A, const FIntPoint& B) { return ViewDistance(A) < ViewDistance(B); });

		FRRSpawnedPack& Pack = Packs.AddDefaulted_GetRef();
		Pack.PackType = TypeIndex;
		Pack.Side = static_cast<ERRSpawnSide>(SideIndex + 1);
		Pack.AnchorAngle = UsedAngles.Last();
		for (int32 Index = 0; Index < Count; ++Index)
		{
			ARREnemy* Enemy = nullptr;
			if (!SpawnEnemyOnCell(Classes[Index], PackCells[Index], Enemy))
			{
				continue;
			}
			--FreeSlots;
			if (Enemy)
			{
				Pack.Robots.Add(Enemy);
				Pack.Cells.Add(PackCells[Index]);
			}
			else
			{
				++Pack.Queued;
			}
		}
		UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: %s pack of %d from side %d, %d alive."), *GetName(),
			*DirectorConfig.PackTypes[TypeIndex].Name.ToString(), Pack.GetCount(), SideIndex + 1, AliveEnemies.Num());
	}
	return Packs;
}

int32 ARREnemySpawner::PickSeparatedAnchor(const TArray<FIntPoint> (&BySide)[4], int32 Needed, ERRSpawnSector Sector, const FVector2D& Origin,
	const TArray<double>& UsedAngles, float MinSeparationDegrees, FIntPoint& OutAnchor)
{
	const ARRArenaGrid* Grid = GetGrid();
	if (!Grid)
	{
		return INDEX_NONE;
	}
	// Smallest angle between the direction of Cell from Origin and the anchors already placed (360 when none).
	auto Separation = [Grid, &Origin, &UsedAngles](const FIntPoint& Cell)
	{
		const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
		const double Angle = FMath::RadiansToDegrees(FMath::Atan2(Center.Y - Origin.Y, Center.X - Origin.X));
		double Smallest = 360.0;
		for (const double Used : UsedAngles)
		{
			Smallest = FMath::Min(Smallest, FMath::Abs(FMath::FindDeltaAngleDegrees(Used, Angle)));
		}
		return Smallest;
	};
	// Four passes from the strictest rule to the loosest: sector and room, sector, room, anything; all far enough.
	const double MinSeparation = MinSeparationDegrees - UE_KINDA_SMALL_NUMBER;
	TArray<TPair<int32, FIntPoint>> Entries;
	for (int32 Pass = 0; Pass < 4 && Entries.IsEmpty(); ++Pass)
	{
		const bool bSectorRule = Pass < 2 && Sector != ERRSpawnSector::None;
		const bool bRoomRule = Pass % 2 == 0;
		for (int32 SideIndex = 0; SideIndex < 4; ++SideIndex)
		{
			if (bRoomRule && BySide[SideIndex].Num() < Needed)
			{
				continue;
			}
			for (const FIntPoint& Cell : BySide[SideIndex])
			{
				if ((!bSectorRule || GetCellSector(Cell) == Sector) && Separation(Cell) >= MinSeparation)
				{
					Entries.Emplace(SideIndex, Cell);
				}
			}
		}
	}
	if (!Entries.IsEmpty())
	{
		const TPair<int32, FIntPoint>& Entry = Entries[RandomStream.RandRange(0, Entries.Num() - 1)];
		OutAnchor = Entry.Value;
		return Entry.Key;
	}
	// No cell far enough: the farthest one (the first on ties, no random draw).
	int32 BestSide = INDEX_NONE;
	double BestSeparation = -1.0;
	for (int32 SideIndex = 0; SideIndex < 4; ++SideIndex)
	{
		for (const FIntPoint& Cell : BySide[SideIndex])
		{
			const double CellSeparation = Separation(Cell);
			if (CellSeparation > BestSeparation)
			{
				BestSeparation = CellSeparation;
				BestSide = SideIndex;
				OutAnchor = Cell;
			}
		}
	}
	return BestSide;
}

int32 ARREnemySpawner::PickEvenSpreadStart(const TArray<FIntPoint> (&BySide)[4], int32 Needed, ERRSpawnSector Sector, const FVector2D& Origin, int32 PackCount, FIntPoint& OutAnchor)
{
	const ARRArenaGrid* Grid = GetGrid();
	if (!Grid)
	{
		return INDEX_NONE;
	}
	auto AngleOf = [Grid, &Origin](const FIntPoint& Cell)
	{
		const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
		return FMath::RadiansToDegrees(FMath::Atan2(Center.Y - Origin.Y, Center.X - Origin.X));
	};
	auto BinOf = [](double Angle) { return static_cast<int32>(FMath::Fmod(FMath::Fmod(Angle + 180.0, 360.0) + 360.0, 360.0)) % 360; };
	// One-degree bins around the miner that hold a candidate cell.
	bool bOccupied[360] = {};
	for (int32 SideIndex = 0; SideIndex < 4; ++SideIndex)
	{
		for (const FIntPoint& Cell : BySide[SideIndex])
		{
			bOccupied[BinOf(AngleOf(Cell))] = true;
		}
	}
	// Degrees from Angle to the nearest occupied bin (360 when none).
	auto Miss = [&bOccupied, &BinOf](double Angle)
	{
		const int32 Bin = BinOf(Angle);
		for (int32 Offset = 0; Offset <= 180; ++Offset)
		{
			if (bOccupied[(Bin + Offset) % 360] || bOccupied[(Bin - Offset + 360) % 360])
			{
				return static_cast<double>(Offset);
			}
		}
		return 360.0;
	};
	// Candidate first anchors: sector and room, sector, room, anything (as PickSeparatedAnchor); each scored by the worst miss of
	// the later packs at even steps. A random one within 3 degrees, else the best.
	constexpr double GoodMiss = 3.0;
	TArray<TPair<int32, FIntPoint>> Good;
	int32 BestSide = INDEX_NONE;
	double BestMiss = TNumericLimits<double>::Max();
	for (int32 Pass = 0; Pass < 4 && Good.IsEmpty(); ++Pass)
	{
		const bool bSectorRule = Pass < 2 && Sector != ERRSpawnSector::None;
		const bool bRoomRule = Pass % 2 == 0;
		for (int32 SideIndex = 0; SideIndex < 4; ++SideIndex)
		{
			if (bRoomRule && BySide[SideIndex].Num() < Needed)
			{
				continue;
			}
			for (const FIntPoint& Cell : BySide[SideIndex])
			{
				if (bSectorRule && GetCellSector(Cell) != Sector)
				{
					continue;
				}
				const double Angle = AngleOf(Cell);
				double WorstMiss = 0.0;
				for (int32 Step = 1; Step < PackCount; ++Step)
				{
					WorstMiss = FMath::Max(WorstMiss, Miss(Angle + Step * 360.0 / PackCount));
				}
				if (WorstMiss <= GoodMiss)
				{
					Good.Emplace(SideIndex, Cell);
				}
				if (WorstMiss < BestMiss)
				{
					BestMiss = WorstMiss;
					BestSide = SideIndex;
					OutAnchor = Cell;
				}
			}
		}
	}
	if (!Good.IsEmpty())
	{
		const TPair<int32, FIntPoint>& Entry = Good[RandomStream.RandRange(0, Good.Num() - 1)];
		OutAnchor = Entry.Value;
		return Entry.Key;
	}
	return BestSide;
}

int32 ARREnemySpawner::PickAnchorNearAngle(const TArray<FIntPoint> (&BySide)[4], int32 Needed, const FVector2D& Origin, double TargetAngle, FIntPoint& OutAnchor)
{
	const ARRArenaGrid* Grid = GetGrid();
	if (!Grid)
	{
		return INDEX_NONE;
	}
	// Two passes: sides with at least Needed cells, then any side; the first cell on ties (no random draw).
	int32 BestSide = INDEX_NONE;
	double BestDelta = TNumericLimits<double>::Max();
	for (int32 Pass = 0; Pass < 2 && BestSide == INDEX_NONE; ++Pass)
	{
		for (int32 SideIndex = 0; SideIndex < 4; ++SideIndex)
		{
			if (Pass == 0 && BySide[SideIndex].Num() < Needed)
			{
				continue;
			}
			for (const FIntPoint& Cell : BySide[SideIndex])
			{
				const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
				const double Angle = FMath::RadiansToDegrees(FMath::Atan2(Center.Y - Origin.Y, Center.X - Origin.X));
				const double Delta = FMath::Abs(FMath::FindDeltaAngleDegrees(TargetAngle, Angle));
				if (Delta < BestDelta)
				{
					BestDelta = Delta;
					BestSide = SideIndex;
					OutAnchor = Cell;
				}
			}
		}
	}
	return BestSide;
}

ERRSpawnSide ARREnemySpawner::PrepareHorde(int32 HordeIndex)
{
	HordePlan.Reset();
	HordePlanCursor = 0;
	HordeSpawned = 0;
	HordePlanIndex = HordeIndex;
	HordeSide = ERRSpawnSide::None;
	const ARRArenaGrid* Grid = GetGrid();
	FBox2D View;
	const int32 FreeSlots = MaxAlive - GetOccupiedCount();
	if (!DirectorConfig.FindHorde(HordeIndex) || !Grid || FreeSlots <= 0 || !GetViewRectangle(View))
	{
		return HordeSide;
	}
	const FRRDirectorHorde& Horde = *DirectorConfig.FindHorde(HordeIndex);

	// Robots: listed groups (front ones first), the rest of the size filled with FillClass, clamped to the free slots.
	const int32 SizeMin = FMath::Max(Horde.SizeMin, 1);
	const int32 Size = FMath::Min(RandomStream.RandRange(SizeMin, FMath::Max(SizeMin, Horde.SizeMax)), FreeSlots);
	TMap<UClass*, int32> Room;
	int32 ShooterRoom = GetDirectorShooterRoom();
	TArray<TSubclassOf<ARREnemy>> Classes;
	TArray<TSubclassOf<ARREnemy>> Back;
	DrawMembers(Horde.Members, Room, ShooterRoom, Classes, Back);
	if (const TSubclassOf<ARREnemy> Fill = ResolveRobotClass(Horde.FillClass))
	{
		for (int32 Index = Classes.Num() + Back.Num(); Index < Size; ++Index)
		{
			Back.Add(Fill);
		}
		for (int32 Index = Back.Num() - 1; Index > 0; --Index)
		{
			Back.Swap(Index, RandomStream.RandRange(0, Index));
		}
	}
	Classes.Append(Back);
	Classes.SetNum(FMath::Min(Classes.Num(), Size));

	TArray<FIntPoint> Candidates;
	GetSpawnCandidates(Candidates);
	TArray<FIntPoint> BySide[4];
	SplitCandidatesBySide(View, Candidates, BySide);
	const bool bNoneUsed[4] = { false, false, false, false };
	// Running: the side follows a random cell of the drawn sector, like a pack.
	const int32 SideIndex = Classes.IsEmpty() ? INDEX_NONE : PickSideForSector(BySide, bNoneUsed, Classes.Num(), DrawSpawnSector(Candidates), nullptr);
	if (SideIndex == INDEX_NONE)
	{
		UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: no spawn cell available for horde %d."), *GetName(), HordeIndex);
		return HordeSide;
	}
	HordeSide = static_cast<ERRSpawnSide>(SideIndex + 1);

	// Front: the side cells nearest to the view (shuffled first so equal rows spread along the whole side).
	TArray<FIntPoint>& SideCells = BySide[SideIndex];
	for (int32 Index = SideCells.Num() - 1; Index > 0; --Index)
	{
		SideCells.Swap(Index, RandomStream.RandRange(0, Index));
	}
	auto ViewDistance = [Grid, &View](const FIntPoint& Cell)
	{
		const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
		return GetDistanceToRectangle(View, FVector2D(Center.X, Center.Y));
	};
	SideCells.StableSort([&ViewDistance](const FIntPoint& A, const FIntPoint& B) { return ViewDistance(A) < ViewDistance(B); });
	const int32 Count = FMath::Min(Classes.Num(), SideCells.Num());
	for (int32 Index = 0; Index < Count; ++Index)
	{
		FRRHordeSlot& Slot = HordePlan.AddDefaulted_GetRef();
		Slot.Class = Classes[Index];
		Slot.Cell = SideCells[Index];
	}
	return HordeSide;
}

void ARREnemySpawner::SpawnHordeUpTo(float Fraction)
{
	const int32 Target = FMath::Min(FMath::CeilToInt(HordePlan.Num() * FMath::Clamp(Fraction, 0.f, 1.f)), HordePlan.Num());
	if (HordePlanCursor >= Target)
	{
		return;
	}
	const ARRArenaGrid* Grid = GetGrid();
	FBox2D View;
	const bool bHasView = Grid && GetViewRectangle(View);
	const float MinDistance = SpawnBandMinDistance;
	for (; HordePlanCursor < Target; ++HordePlanCursor)
	{
		if (GetOccupiedCount() >= MaxAlive)
		{
			continue;
		}
		const FRRHordeSlot& Slot = HordePlan[HordePlanCursor];
		// The view moved since the plan: a cell now inside the spawn margin is skipped.
		if (bHasView)
		{
			const FVector Center = Grid->GetCellCenter(Slot.Cell.X, Slot.Cell.Y);
			if (GetDistanceToRectangle(View, FVector2D(Center.X, Center.Y)) < MinDistance)
			{
				continue;
			}
		}
		ARREnemy* Enemy = nullptr;
		HordeSpawned += SpawnEnemyOnCell(Slot.Class, Slot.Cell, Enemy) ? 1 : 0;
	}
}

// --- Run heading ---

ERRSpawnSector ARREnemySpawner::GetSectorOfPoint(const FVector2D& Origin, const FVector2D& Heading, const FVector2D& Point, float InFrontHalfAngle, float InBackHalfAngle)
{
	const FVector2D Direction = Heading.GetSafeNormal();
	const FVector2D ToPoint = (Point - Origin).GetSafeNormal();
	if (Direction.IsNearlyZero() || ToPoint.IsNearlyZero())
	{
		return ERRSpawnSector::None;
	}
	const double Angle = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(FVector2D::DotProduct(Direction, ToPoint), -1.0, 1.0)));
	if (Angle <= InFrontHalfAngle)
	{
		return ERRSpawnSector::Front;
	}
	return Angle >= 180.0 - InBackHalfAngle ? ERRSpawnSector::Back : ERRSpawnSector::Side;
}

void ARREnemySpawner::SetHeadingOverride(const FVector2D& Direction)
{
	HeadingOverride = Direction.GetSafeNormal();
	bHasHeadingOverride = true;
}

bool ARREnemySpawner::GetHeadingOrigin(FVector2D& OutOrigin) const
{
	const URRFlowFieldSubsystem* FlowField = GetFlowField();
	const AActor* Miner = FlowField ? FlowField->GetGoalActor() : nullptr;
	if (!Miner)
	{
		return false;
	}
	OutOrigin = FVector2D(Miner->GetActorLocation());
	return true;
}

void ARREnemySpawner::RecordHeadingSample()
{
	const UWorld* World = GetWorld();
	FVector2D Location;
	if (!World || !GetHeadingOrigin(Location))
	{
		HeadingSamples.Reset();
		return;
	}
	const double Now = World->GetTimeSeconds();
	HeadingSamples.Add({ Now, Location });
	// Keep one sample at or before the window start, so the measure spans the whole window once it is full.
	int32 Drop = 0;
	while (Drop + 1 < HeadingSamples.Num() && HeadingSamples[Drop + 1].Time <= Now - HeadingWindowSeconds)
	{
		++Drop;
	}
	HeadingSamples.RemoveAt(0, Drop, EAllowShrinking::No);
}

float ARREnemySpawner::GetRunSpeed() const
{
	if (bHasHeadingOverride)
	{
		return HeadingOverride.IsNearlyZero() ? 0.f : HeadingMinSpeed;
	}
	if (HeadingSamples.Num() < 2)
	{
		return 0.f;
	}
	const FHeadingSample& Oldest = HeadingSamples[0];
	const FHeadingSample& Latest = HeadingSamples.Last();
	const double Elapsed = Latest.Time - Oldest.Time;
	return Elapsed > KINDA_SMALL_NUMBER ? static_cast<float>(FVector2D::Distance(Latest.Location, Oldest.Location) / Elapsed) : 0.f;
}

bool ARREnemySpawner::GetRunHeading(FVector2D& OutDirection) const
{
	OutDirection = FVector2D::ZeroVector;
	if (bHasHeadingOverride)
	{
		OutDirection = HeadingOverride;
		return !OutDirection.IsNearlyZero();
	}
	if (HeadingSamples.Num() < 2 || GetRunSpeed() < FMath::Max(HeadingMinSpeed, KINDA_SMALL_NUMBER))
	{
		return false;
	}
	OutDirection = (HeadingSamples.Last().Location - HeadingSamples[0].Location).GetSafeNormal();
	return !OutDirection.IsNearlyZero();
}

ERRSpawnSector ARREnemySpawner::GetSpawnSector(const FVector2D& Point) const
{
	FVector2D Heading;
	FVector2D Origin;
	if (!GetRunHeading(Heading) || !GetHeadingOrigin(Origin))
	{
		return ERRSpawnSector::None;
	}
	return GetSectorOfPoint(Origin, Heading, Point, FrontHalfAngle, BackHalfAngle);
}

ERRSpawnSector ARREnemySpawner::GetCellSector(const FIntPoint& Cell) const
{
	const ARRArenaGrid* Grid = GetGrid();
	return Grid ? GetSpawnSector(FVector2D(Grid->GetCellCenter(Cell.X, Cell.Y))) : ERRSpawnSector::None;
}

ERRSpawnSector ARREnemySpawner::DrawSpawnSector(const TArray<FIntPoint>& Cells)
{
	FVector2D Heading;
	FVector2D Origin;
	const ARRArenaGrid* Grid = GetGrid();
	if (Cells.IsEmpty() || !Grid || !GetRunHeading(Heading) || !GetHeadingOrigin(Origin))
	{
		return ERRSpawnSector::None;
	}
	// Sectors holding at least one cell (index = ERRSpawnSector - 1).
	bool bHasCells[3] = { false, false, false };
	for (const FIntPoint& Cell : Cells)
	{
		const ERRSpawnSector Sector = GetSectorOfPoint(Origin, Heading, FVector2D(Grid->GetCellCenter(Cell.X, Cell.Y)), FrontHalfAngle, BackHalfAngle);
		if (Sector != ERRSpawnSector::None)
		{
			bHasCells[static_cast<int32>(Sector) - 1] = true;
		}
	}
	const float Weights[3] = {
		bHasCells[0] ? FMath::Max(FrontSectorWeight, 0.f) : 0.f,
		bHasCells[1] ? FMath::Max(SideSectorWeight, 0.f) : 0.f,
		bHasCells[2] ? FMath::Max(BackSectorWeight, 0.f) : 0.f };
	const float Total = Weights[0] + Weights[1] + Weights[2];
	if (Total <= 0.f)
	{
		return ERRSpawnSector::None;
	}
	float Draw = RandomStream.FRand() * Total;
	int32 LastWeighted = INDEX_NONE;
	for (int32 Index = 0; Index < 3; ++Index)
	{
		if (Weights[Index] <= 0.f)
		{
			continue;
		}
		LastWeighted = Index;
		if (Draw < Weights[Index])
		{
			return static_cast<ERRSpawnSector>(Index + 1);
		}
		Draw -= Weights[Index];
	}
	// Float rounding at the top of the range: the last sector with a weight.
	return static_cast<ERRSpawnSector>(LastWeighted + 1);
}

int32 ARREnemySpawner::PickSideForSector(const TArray<FIntPoint> (&BySide)[4], const bool (&Used)[4], int32 Needed, ERRSpawnSector Sector, FIntPoint* OutAnchor)
{
	if (Sector != ERRSpawnSector::None)
	{
		// (side, cell) of every cell of the sector; three passes from the strictest side rule to the loosest.
		TArray<TPair<int32, FIntPoint>> Entries;
		for (int32 Pass = 0; Pass < 3 && Entries.IsEmpty(); ++Pass)
		{
			for (int32 SideIndex = 0; SideIndex < 4; ++SideIndex)
			{
				if ((Pass < 2 && Used[SideIndex]) || (Pass == 0 && BySide[SideIndex].Num() < Needed))
				{
					continue;
				}
				for (const FIntPoint& Cell : BySide[SideIndex])
				{
					if (GetCellSector(Cell) == Sector)
					{
						Entries.Emplace(SideIndex, Cell);
					}
				}
			}
		}
		if (!Entries.IsEmpty())
		{
			const TPair<int32, FIntPoint>& Entry = Entries[RandomStream.RandRange(0, Entries.Num() - 1)];
			if (OutAnchor)
			{
				*OutAnchor = Entry.Value;
			}
			return Entry.Key;
		}
	}
	const int32 SideIndex = RRSpawner::PickSide(BySide, Used, Needed, RandomStream);
	if (SideIndex != INDEX_NONE && OutAnchor)
	{
		*OutAnchor = BySide[SideIndex][RandomStream.RandRange(0, BySide[SideIndex].Num() - 1)];
	}
	return SideIndex;
}
