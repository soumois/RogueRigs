// Rogue Rigs - enemy spawner: a trickle of single robots, then packs from one side, capped alive count, on reachable floor cells just outside the camera view.

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
}

void ARREnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	RandomStream.GenerateNewSeed();
}

void ARREnemySpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!IsSpawningEnabled())
	{
		return;
	}
	RunTime += DeltaSeconds;

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
	const TSubclassOf<ARREnemy> SpawnClass = PickEnemyClass();
	if (!GetWorld() || !SpawnClass || GetAliveCount() >= MaxAlive)
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
