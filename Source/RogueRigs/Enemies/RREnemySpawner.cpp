// Rogue Rigs - enemy spawner: wave director (calm, lone robots, packs of one type from several sides, announced hordes) or the older trickle then packs, capped alive count, on reachable floor cells just outside the camera view.

#include "Enemies/RREnemySpawner.h"

#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Enemies/RREnemy.h"
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

void ARREnemySpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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

	// Reachable free cells in the ring between ViewMargin and MaxSpawnDistanceCells outside the view rectangle.
	const float CellSize = Grid->GetCellSize();
	const float MinDistance = (ViewMargin + 0.5f) * CellSize;
	const float MaxDistance = (MaxSpawnDistanceCells + 0.5f) * CellSize;
	FlowField->GatherReachableCells(OutCells, [Grid, &View, MinDistance, MaxDistance](FIntPoint Cell, int32 Steps)
	{
		const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
		const float Distance = GetDistanceToRectangle(View, FVector2D(Center.X, Center.Y));
		return Distance >= MinDistance && Distance <= MaxDistance;
	}, /*bFloorOnly*/ false, &Occupied);
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
		Count += Robot && Class && Robot->IsA(Class) && !Robot->IsActorBeingDestroyed() && !Robot->IsDead() ? 1 : 0;
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
	if (!GetWorld() || !SpawnClass || GetAliveCount() >= MaxAlive || (WantedClass && GetClassRoom(SpawnClass) <= 0))
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
	return SpawnEnemyOnCell(SpawnClass, Candidates[RandomStream.RandRange(0, Candidates.Num() - 1)]);
}

TArray<ARREnemy*> ARREnemySpawner::TrySpawnPack()
{
	TArray<ARREnemy*> Spawned;
	const int32 FreeSlots = MaxAlive - GetAliveCount();
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
		if (ARREnemy* Enemy = SpawnClass ? SpawnEnemyOnCell(SpawnClass, Cells[Index]) : nullptr)
		{
			Spawned.Add(Enemy);
		}
	}
	UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: pack of %d (drawn %d) from side %d, %d alive."), *GetName(), Spawned.Num(),
		PackSize, SideIndex + 1, AliveEnemies.Num());
	return Spawned;
}

ARREnemy* ARREnemySpawner::SpawnEnemyOnCell(TSubclassOf<ARREnemy> SpawnClass, const FIntPoint& Cell)
{
	UWorld* World = GetWorld();
	const ARRArenaGrid* Grid = GetGrid();
	if (!World || !Grid || !SpawnClass)
	{
		return nullptr;
	}
	const float HalfHeight = SpawnClass->GetDefaultObject<ARREnemy>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector Location = Grid->GetCellCenter(Cell.X, Cell.Y) + FVector(0.f, 0.f, HalfHeight + SpawnFloorClearance);
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ARREnemy* Enemy = World->SpawnActor<ARREnemy>(SpawnClass, Location, FRotator::ZeroRotator, Params);
	if (Enemy)
	{
		AliveEnemies.Add(Enemy);
		UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: %s spawned on (%d, %d), %d alive."), *GetName(), *Enemy->GetName(),
			Cell.X, Cell.Y, AliveEnemies.Num());
	}
	return Enemy;
}

void ARREnemySpawner::PruneAlive()
{
	AliveEnemies.RemoveAll([](const TWeakObjectPtr<ARREnemy>& Enemy)
	{
		return !Enemy.IsValid() || Enemy->IsActorBeingDestroyed() || Enemy->IsDead();
	});
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
	}
}

void ARREnemySpawner::DrawMembers(const TArray<FRRDirectorPackMember>& Members, TMap<UClass*, int32>& Room,
	TArray<TSubclassOf<ARREnemy>>& OutFront, TArray<TSubclassOf<ARREnemy>>& OutBack)
{
	for (const FRRDirectorPackMember& Member : Members)
	{
		const TSubclassOf<ARREnemy> SpawnClass = ResolveRobotClass(Member.RobotClass);
		if (!SpawnClass)
		{
			continue;
		}
		const int32 CountMin = FMath::Max(Member.CountMin, 0);
		int32 Count = RandomStream.RandRange(CountMin, FMath::Max(CountMin, Member.CountMax));
		int32* ClassRoom = Room.Find(SpawnClass.Get());
		if (!ClassRoom)
		{
			ClassRoom = &Room.Add(SpawnClass.Get(), GetClassRoom(SpawnClass));
		}
		Count = FMath::Min(Count, *ClassRoom);
		*ClassRoom -= Count;
		TArray<TSubclassOf<ARREnemy>>& Target = Member.bFront ? OutFront : OutBack;
		for (int32 Index = 0; Index < Count; ++Index)
		{
			Target.Add(SpawnClass);
		}
	}
	for (int32 Index = OutBack.Num() - 1; Index > 0; --Index)
	{
		OutBack.Swap(Index, RandomStream.RandRange(0, Index));
	}
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
	int32 FreeSlots = MaxAlive - GetAliveCount();
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
	bool bSideUsed[4] = { false, false, false, false };
	for (const int32 TypeIndex : PackTypeIndices)
	{
		if (!DirectorConfig.PackTypes.IsValidIndex(TypeIndex) || FreeSlots <= 0)
		{
			continue;
		}
		// Front members first, then the others mixed; clamped to the free slots.
		TArray<TSubclassOf<ARREnemy>> Classes;
		TArray<TSubclassOf<ARREnemy>> Back;
		DrawMembers(DirectorConfig.PackTypes[TypeIndex].Members, Room, Classes, Back);
		Classes.Append(Back);
		Classes.SetNum(FMath::Min(Classes.Num(), FreeSlots));
		if (Classes.IsEmpty())
		{
			continue;
		}
		const int32 SideIndex = RRSpawner::PickSide(BySide, bSideUsed, Classes.Num(), RandomStream);
		if (SideIndex == INDEX_NONE)
		{
			break;
		}
		bSideUsed[SideIndex] = true;
		TArray<FIntPoint>& SideCells = BySide[SideIndex];

		// Adjacent cells: the ones nearest to a random anchor of the side, then the nearest to the view first (the front).
		const FIntPoint Anchor = SideCells[RandomStream.RandRange(0, SideCells.Num() - 1)];
		SideCells.StableSort([Anchor](const FIntPoint& A, const FIntPoint& B) { return (A - Anchor).SizeSquared() < (B - Anchor).SizeSquared(); });
		const int32 Count = FMath::Min(Classes.Num(), SideCells.Num());
		TArray<FIntPoint> PackCells(SideCells.GetData(), Count);
		SideCells.RemoveAt(0, Count);
		PackCells.StableSort([&ViewDistance](const FIntPoint& A, const FIntPoint& B) { return ViewDistance(A) < ViewDistance(B); });

		FRRSpawnedPack& Pack = Packs.AddDefaulted_GetRef();
		Pack.PackType = TypeIndex;
		Pack.Side = static_cast<ERRSpawnSide>(SideIndex + 1);
		for (int32 Index = 0; Index < Count; ++Index)
		{
			if (ARREnemy* Enemy = SpawnEnemyOnCell(Classes[Index], PackCells[Index]))
			{
				Pack.Robots.Add(Enemy);
				Pack.Cells.Add(PackCells[Index]);
				--FreeSlots;
			}
		}
		UE_LOG(LogRogueRigs, Verbose, TEXT("ARREnemySpawner %s: %s pack of %d from side %d, %d alive."), *GetName(),
			*DirectorConfig.PackTypes[TypeIndex].Name.ToString(), Pack.Robots.Num(), SideIndex + 1, AliveEnemies.Num());
	}
	return Packs;
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
	const int32 FreeSlots = MaxAlive - GetAliveCount();
	if (!DirectorConfig.Hordes.IsValidIndex(HordeIndex) || !Grid || FreeSlots <= 0 || !GetViewRectangle(View))
	{
		return HordeSide;
	}
	const FRRDirectorHorde& Horde = DirectorConfig.Hordes[HordeIndex];

	// Robots: listed groups (front ones first), the rest of the size filled with FillClass, clamped to the free slots.
	const int32 SizeMin = FMath::Max(Horde.SizeMin, 1);
	const int32 Size = FMath::Min(RandomStream.RandRange(SizeMin, FMath::Max(SizeMin, Horde.SizeMax)), FreeSlots);
	TMap<UClass*, int32> Room;
	TArray<TSubclassOf<ARREnemy>> Classes;
	TArray<TSubclassOf<ARREnemy>> Back;
	DrawMembers(Horde.Members, Room, Classes, Back);
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
	const int32 SideIndex = Classes.IsEmpty() ? INDEX_NONE : RRSpawner::PickSide(BySide, bNoneUsed, Classes.Num(), RandomStream);
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
	const float MinDistance = Grid ? (ViewMargin + 0.5f) * Grid->GetCellSize() : 0.f;
	for (; HordePlanCursor < Target; ++HordePlanCursor)
	{
		if (GetAliveCount() >= MaxAlive)
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
		HordeSpawned += SpawnEnemyOnCell(Slot.Class, Slot.Cell) ? 1 : 0;
	}
}
