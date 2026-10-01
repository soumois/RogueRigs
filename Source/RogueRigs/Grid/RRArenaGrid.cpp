// Rogue Rigs - arena grid actor: loads a layout JSON and builds floor and block instances.

#include "Grid/RRArenaGrid.h"

#include "RogueRigs.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRArenaGrid)

ARRArenaGrid::ARRArenaGrid()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GridRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GridRoot"));
	SetRootComponent(GridRoot);

	FloorInstances = CreateGridComponent(TEXT("FloorInstances"));
	RockInstances = CreateGridComponent(TEXT("RockInstances"));
	OreInstances = CreateGridComponent(TEXT("OreInstances"));
	HealOreInstances = CreateGridComponent(TEXT("HealOreInstances"));
	SpecialOreInstances = CreateGridComponent(TEXT("SpecialOreInstances"));
	BarrelInstances = CreateGridComponent(TEXT("BarrelInstances"));
}

UHierarchicalInstancedStaticMeshComponent* ARRArenaGrid::CreateGridComponent(FName Name)
{
	UHierarchicalInstancedStaticMeshComponent* Component = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(Name);
	Component->SetupAttachment(GridRoot);
	Component->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	return Component;
}

void ARRArenaGrid::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Never rebuild during play: it would wipe destroyed blocks, health and the instance maps.
	if (HasActorBegunPlay() || IsActorBeginningPlay())
	{
		return;
	}

	RebuildFromLayout();
}

void ARRArenaGrid::BeginPlay()
{
	// Placed actors keep their serialized instances but not the transient state (PIE duplication, cooked load):
	// rebuild once so cells, health and maps match the instances.
	RebuildFromLayout();

	Super::BeginPlay();
}

void ARRArenaGrid::RebuildFromLayout()
{
	if (!LoadLayout())
	{
		Cells.Reset();
		CellHealth.Reset();
		CellInstanceIndex.Reset();
		ShakingCells.Reset();
		SetActorTickEnabled(false);
		GridWidth = 0;
		GridHeight = 0;
		SpawnCell = FIntPoint(-1, -1);
		ClearAllInstances();
		return;
	}

	BuildInstances();
}

FString ARRArenaGrid::GetLayoutFilePath() const
{
	return FPaths::Combine(FPaths::ProjectContentDir(), TEXT("RogueRigs/Data/Levels"), LayoutName + TEXT(".json"));
}

bool ARRArenaGrid::LoadLayout()
{
	if (LayoutName.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("ARRArenaGrid %s: LayoutName is empty, nothing to build."), *GetName());
		return false;
	}

	const FString FilePath = GetLayoutFilePath();
	FString FileText;
	if (!FFileHelper::LoadFileToString(FileText, *FilePath))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: layout file not found or unreadable: %s"), *FilePath);
		return false;
	}

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileText);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: JSON parse error in %s: %s"), *FilePath, *Reader->GetErrorMessage());
		return false;
	}

	int32 Version = 0;
	int32 Width = 0;
	int32 Height = 0;
	const TArray<TSharedPtr<FJsonValue>>* CellValues = nullptr;
	if (!Root->TryGetNumberField(TEXT("version"), Version) || Version != 1)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: missing or unsupported 'version' (expected 1) in %s"), *FilePath);
		return false;
	}
	if (!Root->TryGetNumberField(TEXT("width"), Width) || !Root->TryGetNumberField(TEXT("height"), Height) || Width <= 0 || Height <= 0)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: missing or invalid 'width'/'height' in %s"), *FilePath);
		return false;
	}
	if (!Root->TryGetArrayField(TEXT("cells"), CellValues) || CellValues == nullptr)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: missing 'cells' array in %s"), *FilePath);
		return false;
	}

	TArray<ERRCellType> NewCells;
	NewCells.Init(ERRCellType::Floor, Width * Height);
	FIntPoint NewSpawn(-1, -1);

	for (int32 Index = 0; Index < CellValues->Num(); ++Index)
	{
		const TSharedPtr<FJsonObject>* CellObject = nullptr;
		if (!(*CellValues)[Index].IsValid() || !(*CellValues)[Index]->TryGetObject(CellObject) || CellObject == nullptr)
		{
			UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: cells[%d] is not an object in %s"), Index, *FilePath);
			return false;
		}

		int32 X = 0;
		int32 Y = 0;
		FString TypeName;
		ERRCellType Type = ERRCellType::Floor;
		if (!(*CellObject)->TryGetNumberField(TEXT("x"), X) || !(*CellObject)->TryGetNumberField(TEXT("y"), Y)
			|| !(*CellObject)->TryGetStringField(TEXT("type"), TypeName))
		{
			UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: cells[%d] lacks x, y or type in %s"), Index, *FilePath);
			return false;
		}
		if (X < 0 || X >= Width || Y < 0 || Y >= Height)
		{
			UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: cells[%d] (%d, %d) is out of bounds in %s"), Index, X, Y, *FilePath);
			return false;
		}
		if (!ParseCellType(TypeName, Type))
		{
			UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: cells[%d] has unknown type '%s' in %s"), Index, *TypeName, *FilePath);
			return false;
		}

		NewCells[Y * Width + X] = Type;
		if (Type == ERRCellType::Spawn)
		{
			NewSpawn = FIntPoint(X, Y);
		}
	}

	if (NewSpawn.X < 0)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("ARRArenaGrid: no spawn cell in %s"), *FilePath);
	}

	Cells = MoveTemp(NewCells);
	GridWidth = Width;
	GridHeight = Height;
	SpawnCell = NewSpawn;
	return true;
}

bool ARRArenaGrid::ParseCellType(const FString& TypeName, ERRCellType& OutType)
{
	static const TMap<FString, ERRCellType> TypeByName = {
		{ TEXT("void"), ERRCellType::Void },
		{ TEXT("rock"), ERRCellType::Rock },
		{ TEXT("ore"), ERRCellType::Ore },
		{ TEXT("heal_ore"), ERRCellType::HealOre },
		{ TEXT("special_ore"), ERRCellType::SpecialOre },
		{ TEXT("barrel"), ERRCellType::Barrel },
		{ TEXT("spawn"), ERRCellType::Spawn },
	};

	if (const ERRCellType* Found = TypeByName.Find(TypeName))
	{
		OutType = *Found;
		return true;
	}
	return false;
}

bool ARRArenaGrid::IsBlockType(ERRCellType Type)
{
	switch (Type)
	{
	case ERRCellType::Rock:
	case ERRCellType::Ore:
	case ERRCellType::HealOre:
	case ERRCellType::SpecialOre:
	case ERRCellType::Barrel:
		return true;
	default:
		return false;
	}
}

UHierarchicalInstancedStaticMeshComponent* ARRArenaGrid::GetBlockComponent(ERRCellType Type) const
{
	switch (Type)
	{
	case ERRCellType::Rock:       return RockInstances;
	case ERRCellType::Ore:        return OreInstances;
	case ERRCellType::HealOre:    return HealOreInstances;
	case ERRCellType::SpecialOre: return SpecialOreInstances;
	case ERRCellType::Barrel:     return BarrelInstances;
	default:                      return nullptr;
	}
}

float ARRArenaGrid::GetBlockMaxHealth(ERRCellType Type) const
{
	switch (Type)
	{
	case ERRCellType::Rock:       return RockHealth;
	case ERRCellType::Ore:        return OreHealth;
	case ERRCellType::HealOre:    return HealOreHealth;
	case ERRCellType::SpecialOre: return SpecialOreHealth;
	case ERRCellType::Barrel:     return BarrelHealth;
	default:                      return 0.f;
	}
}

void ARRArenaGrid::ClearAllInstances()
{
	for (UHierarchicalInstancedStaticMeshComponent* Component : { FloorInstances.Get(), RockInstances.Get(), OreInstances.Get(),
		HealOreInstances.Get(), SpecialOreInstances.Get(), BarrelInstances.Get() })
	{
		if (Component)
		{
			Component->ClearInstances();
		}
	}
	InstanceCellMaps.Reset();
}

FTransform ARRArenaGrid::GetFloorInstanceTransform(FIntPoint Cell) const
{
	// Source meshes are MeshBaseSize cubes with a centered pivot: the slab spans [-FloorThickness, 0].
	const float HorizontalScale = CellSize / MeshBaseSize;
	const FVector Location((Cell.X + 0.5f) * CellSize, (Cell.Y + 0.5f) * CellSize, -0.5f * FloorThickness);
	return FTransform(FRotator::ZeroRotator, Location, FVector(HorizontalScale, HorizontalScale, FloorThickness / MeshBaseSize));
}

FTransform ARRArenaGrid::GetBlockInstanceTransform(FIntPoint Cell) const
{
	// The block spans [0, BlockHeight], or [-FloorThickness, BlockHeight] when it fills the slab of its floorless cell.
	const float Bottom = bBlocksFillFloorSlab ? -FloorThickness : 0.f;
	const float Height = BlockHeight - Bottom;
	const float HorizontalScale = CellSize / MeshBaseSize;
	const FVector Location((Cell.X + 0.5f) * CellSize, (Cell.Y + 0.5f) * CellSize, Bottom + 0.5f * Height);
	return FTransform(FRotator::ZeroRotator, Location, FVector(HorizontalScale, HorizontalScale, Height / MeshBaseSize));
}

void ARRArenaGrid::BuildInstances()
{
	ClearAllInstances();
	ShakingCells.Reset();
	SetActorTickEnabled(false);

	struct FVisual
	{
		UHierarchicalInstancedStaticMeshComponent* Component;
		UStaticMesh* Mesh;
		UMaterialInterface* Material;
	};
	const FVisual Visuals[] = {
		{ FloorInstances, FloorMesh, FloorMaterial },
		{ RockInstances, RockMesh, RockMaterial },
		{ OreInstances, OreMesh, OreMaterial },
		{ HealOreInstances, HealOreMesh, HealOreMaterial },
		{ SpecialOreInstances, SpecialOreMesh, SpecialOreMaterial },
		{ BarrelInstances, BarrelMesh, BarrelMaterial },
	};
	for (const FVisual& Visual : Visuals)
	{
		if (Visual.Component)
		{
			Visual.Component->SetStaticMesh(Visual.Mesh);
			Visual.Component->SetMaterial(0, Visual.Material);
		}
	}

	CellHealth.Init(0.f, Cells.Num());
	CellInstanceIndex.Init(INDEX_NONE, Cells.Num());

	// Gather per component first and add in one batch; AddInstances appends, so instance i of a batch
	// gets index (count before batch) + i, which is recorded in both maps.
	struct FBatch
	{
		TArray<FTransform> Transforms;
		TArray<FIntPoint> BatchCells;
	};
	TMap<ERRCellType, FBatch> Batches;

	for (int32 Y = 0; Y < GridHeight; ++Y)
	{
		for (int32 X = 0; X < GridWidth; ++X)
		{
			const FIntPoint Cell(X, Y);
			const ERRCellType Type = Cells[GetCellIndex(Cell)];
			if (Type == ERRCellType::Void)
			{
				continue;
			}

			// A block cell has no floor instance: a floor top coplanar with the block bottom is what we avoid.
			const bool bIsBlock = IsBlockType(Type) && GetBlockComponent(Type) != nullptr;
			const ERRCellType MapKey = bIsBlock ? Type : ERRCellType::Floor;
			FBatch& Batch = Batches.FindOrAdd(MapKey);
			Batch.Transforms.Add(bIsBlock ? GetBlockInstanceTransform(Cell) : GetFloorInstanceTransform(Cell));
			Batch.BatchCells.Add(Cell);

			if (bIsBlock)
			{
				CellHealth[GetCellIndex(Cell)] = GetBlockMaxHealth(Type);
			}
		}
	}

	for (TPair<ERRCellType, FBatch>& Pair : Batches)
	{
		UHierarchicalInstancedStaticMeshComponent* Component = Pair.Key == ERRCellType::Floor ? FloorInstances.Get() : GetBlockComponent(Pair.Key);
		if (!Component || Pair.Value.Transforms.Num() == 0)
		{
			continue;
		}

		const int32 FirstIndex = Component->GetInstanceCount();
		Component->AddInstances(Pair.Value.Transforms, /*bShouldReturnIndices*/ false, /*bWorldSpace*/ false);

		FRRInstanceCellMap& Map = InstanceCellMaps.FindOrAdd(Pair.Key);
		Map.InstanceCells.SetNum(FirstIndex);
		for (int32 Offset = 0; Offset < Pair.Value.BatchCells.Num(); ++Offset)
		{
			const FIntPoint Cell = Pair.Value.BatchCells[Offset];
			Map.InstanceCells.Add(Cell);
			CellInstanceIndex[GetCellIndex(Cell)] = FirstIndex + Offset;
		}
	}
}

int32 ARRArenaGrid::AddCellInstance(UHierarchicalInstancedStaticMeshComponent* Component, ERRCellType MapKey, FIntPoint Cell, const FTransform& LocalTransform)
{
	if (!Component)
	{
		return INDEX_NONE;
	}

	// HISM appends: the returned index is the previous instance count.
	const int32 InstanceIndex = Component->AddInstance(LocalTransform, /*bWorldSpace*/ false);
	if (InstanceIndex == INDEX_NONE)
	{
		return INDEX_NONE;
	}

	TArray<FIntPoint>& InstanceCells = InstanceCellMaps.FindOrAdd(MapKey).InstanceCells;
	if (InstanceCells.Num() <= InstanceIndex)
	{
		InstanceCells.SetNum(InstanceIndex + 1);
	}
	InstanceCells[InstanceIndex] = Cell;
	CellInstanceIndex[GetCellIndex(Cell)] = InstanceIndex;
	return InstanceIndex;
}

void ARRArenaGrid::RemoveCellInstance(UHierarchicalInstancedStaticMeshComponent* Component, ERRCellType MapKey, int32 InstanceIndex)
{
	FRRInstanceCellMap* Map = InstanceCellMaps.Find(MapKey);
	if (!Component || !Map || !Map->InstanceCells.IsValidIndex(InstanceIndex))
	{
		return;
	}

	const FIntPoint RemovedCell = Map->InstanceCells[InstanceIndex];
	const int32 LastIndex = Map->InstanceCells.Num() - 1;
	if (!Component->RemoveInstance(InstanceIndex))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: failed to remove instance %d of cell (%d, %d)."), InstanceIndex, RemovedCell.X, RemovedCell.Y);
		return;
	}

	// HISM always removes at swap: the last instance now occupies InstanceIndex. Mirror it in both maps.
	Map->InstanceCells.RemoveAtSwap(InstanceIndex, EAllowShrinking::No);
	if (InstanceIndex != LastIndex)
	{
		const FIntPoint MovedCell = Map->InstanceCells[InstanceIndex];
		CellInstanceIndex[GetCellIndex(MovedCell)] = InstanceIndex;
	}
	CellInstanceIndex[GetCellIndex(RemovedCell)] = INDEX_NONE;
}

FVector ARRArenaGrid::GetCellCenter(int32 X, int32 Y) const
{
	const FVector LocalCenter((X + 0.5f) * CellSize, (Y + 0.5f) * CellSize, 0.f);
	return GetActorTransform().TransformPosition(LocalCenter);
}

FIntPoint ARRArenaGrid::GetCellAtWorld(FVector WorldLocation) const
{
	const FVector Local = GetActorTransform().InverseTransformPosition(WorldLocation);
	return FIntPoint(FMath::FloorToInt32(Local.X / CellSize), FMath::FloorToInt32(Local.Y / CellSize));
}

FVector ARRArenaGrid::GetCellFaceCenter(FIntPoint Cell, FIntPoint Side) const
{
	// Reduce Side to one unit step along its dominant axis.
	FIntPoint Step(0, 0);
	if (FMath::Abs(Side.X) >= FMath::Abs(Side.Y))
	{
		Step.X = FMath::Sign(Side.X);
	}
	else
	{
		Step.Y = FMath::Sign(Side.Y);
	}

	const FVector LocalFace((Cell.X + 0.5f + 0.5f * Step.X) * CellSize, (Cell.Y + 0.5f + 0.5f * Step.Y) * CellSize, 0.5f * BlockHeight);
	return GetActorTransform().TransformPosition(LocalFace);
}

FIntPoint ARRArenaGrid::GetSpawnCell() const
{
	return SpawnCell;
}

bool ARRArenaGrid::IsValidCell(FIntPoint Cell) const
{
	return Cell.X >= 0 && Cell.X < GridWidth && Cell.Y >= 0 && Cell.Y < GridHeight && Cells.IsValidIndex(Cell.Y * GridWidth + Cell.X);
}

ERRCellType ARRArenaGrid::GetCellType(FIntPoint Cell) const
{
	return IsValidCell(Cell) ? Cells[GetCellIndex(Cell)] : ERRCellType::Void;
}

bool ARRArenaGrid::IsBlockCell(FIntPoint Cell) const
{
	return IsBlockType(GetCellType(Cell));
}

float ARRArenaGrid::GetCellHealth(FIntPoint Cell) const
{
	return IsBlockCell(Cell) && CellHealth.IsValidIndex(GetCellIndex(Cell)) ? CellHealth[GetCellIndex(Cell)] : 0.f;
}

bool ARRArenaGrid::ApplyTerrainDamage(FIntPoint Cell, float Amount, ERRTerrainDamageSource Source)
{
	if (Amount <= 0.f || !IsBlockCell(Cell) || !CellHealth.IsValidIndex(GetCellIndex(Cell)))
	{
		return false;
	}

	const int32 Index = GetCellIndex(Cell);
	const ERRCellType Type = Cells[Index];
	const float Applied = FMath::Min(Amount, CellHealth[Index]);
	CellHealth[Index] -= Applied;

	OnCellDamaged.Broadcast(Cell, Type, Applied, Source);

	// A listener may already have destroyed the block (e.g. a chain reaction).
	if (Cells[Index] == Type && CellHealth[Index] <= UE_KINDA_SMALL_NUMBER)
	{
		return DestroyCell(Cell, Source);
	}
	return false;
}

bool ARRArenaGrid::DestroyCell(FIntPoint Cell, ERRTerrainDamageSource Source)
{
	if (!IsBlockCell(Cell))
	{
		return false;
	}

	const int32 Index = GetCellIndex(Cell);
	const ERRCellType Type = Cells[Index];

	ShakingCells.Remove(Cell);
	if (ShakingCells.IsEmpty())
	{
		SetActorTickEnabled(false);
	}

	if (CellInstanceIndex.IsValidIndex(Index) && CellInstanceIndex[Index] != INDEX_NONE)
	{
		RemoveCellInstance(GetBlockComponent(Type), Type, CellInstanceIndex[Index]);
	}

	Cells[Index] = ERRCellType::Floor;
	CellHealth[Index] = 0.f;
	AddCellInstance(FloorInstances, ERRCellType::Floor, Cell, GetFloorInstanceTransform(Cell));

	UE_LOG(LogRogueRigs, Verbose, TEXT("ARRArenaGrid: block destroyed at (%d, %d), type %s, source %s."), Cell.X, Cell.Y,
		*UEnum::GetValueAsString(Type), *UEnum::GetValueAsString(Source));

	OnCellDestroyed.Broadcast(Cell, Type, Source);
	return true;
}

int32 ARRArenaGrid::DestroyCellsInRadius(FVector Center, float Radius, ERRTerrainDamageSource Source)
{
	if (Radius < 0.f || GridWidth <= 0 || GridHeight <= 0)
	{
		return 0;
	}

	// Candidate cells: the local bounding square of the circle, widened by one cell for rounding.
	const FIntPoint CenterCell = GetCellAtWorld(Center);
	const int32 Reach = FMath::CeilToInt32(Radius / CellSize) + 1;
	const int32 MinX = FMath::Max(0, CenterCell.X - Reach);
	const int32 MaxX = FMath::Min(GridWidth - 1, CenterCell.X + Reach);
	const int32 MinY = FMath::Max(0, CenterCell.Y - Reach);
	const int32 MaxY = FMath::Min(GridHeight - 1, CenterCell.Y + Reach);

	// Collect first: OnCellDestroyed listeners may change cells while we destroy.
	TArray<FIntPoint> Targets;
	const float RadiusSquared = FMath::Square(Radius);
	for (int32 Y = MinY; Y <= MaxY; ++Y)
	{
		for (int32 X = MinX; X <= MaxX; ++X)
		{
			const FIntPoint Cell(X, Y);
			if (IsBlockCell(Cell) && FVector::DistSquared2D(GetCellCenter(X, Y), Center) <= RadiusSquared)
			{
				Targets.Add(Cell);
			}
		}
	}

	int32 Destroyed = 0;
	for (const FIntPoint& Cell : Targets)
	{
		Destroyed += DestroyCell(Cell, Source) ? 1 : 0;
	}
	return Destroyed;
}

void ARRArenaGrid::SetCellShaking(FIntPoint Cell, bool bShaking)
{
	if (!bShaking)
	{
		StopShaking(Cell);
		return;
	}

	if (!IsBlockCell(Cell) || ShakingCells.Contains(Cell))
	{
		return;
	}

	// Deterministic per-cell phase so adjacent shaking blocks do not move in lockstep.
	const float Phase = static_cast<float>(GetTypeHash(Cell) % 1024u) / 1024.f * UE_TWO_PI;
	ShakingCells.Add(Cell, Phase);
	SetActorTickEnabled(true);
}

void ARRArenaGrid::StopShaking(FIntPoint Cell)
{
	if (ShakingCells.Remove(Cell) == 0)
	{
		return;
	}

	if (IsBlockCell(Cell))
	{
		const int32 Index = GetCellIndex(Cell);
		UHierarchicalInstancedStaticMeshComponent* Component = GetBlockComponent(Cells[Index]);
		if (Component && CellInstanceIndex[Index] != INDEX_NONE)
		{
			Component->UpdateInstanceTransform(CellInstanceIndex[Index], GetBlockInstanceTransform(Cell), /*bWorldSpace*/ false,
				/*bMarkRenderStateDirty*/ true, /*bTeleport*/ true);
		}
	}

	if (ShakingCells.IsEmpty())
	{
		SetActorTickEnabled(false);
	}
}

void ARRArenaGrid::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ShakingCells.IsEmpty())
	{
		SetActorTickEnabled(false);
		return;
	}

	const UWorld* World = GetWorld();
	const float Time = World ? World->GetTimeSeconds() : 0.f;
	const float Angle = UE_TWO_PI * ShakeFrequency * Time;

	for (const TPair<FIntPoint, float>& Pair : ShakingCells)
	{
		const FIntPoint Cell = Pair.Key;
		if (!IsBlockCell(Cell))
		{
			continue;
		}

		const int32 Index = GetCellIndex(Cell);
		UHierarchicalInstancedStaticMeshComponent* Component = GetBlockComponent(Cells[Index]);
		if (!Component || CellInstanceIndex[Index] == INDEX_NONE)
		{
			continue;
		}

		// Two incommensurate sines per axis: a horizontal jitter bounded by ShakeAmplitude.
		const FVector Offset(
			ShakeAmplitude * FMath::Sin(Angle + Pair.Value),
			ShakeAmplitude * FMath::Sin(1.37f * Angle + 2.f * Pair.Value),
			0.f);
		FTransform Transform = GetBlockInstanceTransform(Cell);
		Transform.AddToTranslation(Offset);
		Component->UpdateInstanceTransform(CellInstanceIndex[Index], Transform, /*bWorldSpace*/ false,
			/*bMarkRenderStateDirty*/ true, /*bTeleport*/ true);
	}
}
