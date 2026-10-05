// Rogue Rigs - arena grid actor: loads a layout JSON and builds floor and block instances.

#include "Grid/RRArenaGrid.h"

#include "RogueRigs.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "Grid/RRBarrelSource.h"
#include "Materials/MaterialInterface.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Pickups/RRPickup.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRArenaGrid)

ARRArenaGrid::ARRArenaGrid()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GridRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GridRoot"));
	SetRootComponent(GridRoot);

	FloorInstances = CreateGridComponent(TEXT("FloorInstances"), /*bWithCollision*/ true);
	RockInstances = CreateGridComponent(TEXT("RockInstances"), /*bWithCollision*/ false);
	OreInstances = CreateGridComponent(TEXT("OreInstances"), /*bWithCollision*/ false);
	HealOreInstances = CreateGridComponent(TEXT("HealOreInstances"), /*bWithCollision*/ false);
	SpecialOreInstances = CreateGridComponent(TEXT("SpecialOreInstances"), /*bWithCollision*/ false);
	BarrelInstances = CreateGridComponent(TEXT("BarrelInstances"), /*bWithCollision*/ false);

	RockCollision = CreateCollisionComponent(TEXT("RockCollision"));
	OreCollision = CreateCollisionComponent(TEXT("OreCollision"));
	HealOreCollision = CreateCollisionComponent(TEXT("HealOreCollision"));
	SpecialOreCollision = CreateCollisionComponent(TEXT("SpecialOreCollision"));
	BarrelCollision = CreateCollisionComponent(TEXT("BarrelCollision"));
}

UHierarchicalInstancedStaticMeshComponent* ARRArenaGrid::CreateGridComponent(FName Name, bool bWithCollision)
{
	UHierarchicalInstancedStaticMeshComponent* Component = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(Name);
	Component->SetupAttachment(GridRoot);
	Component->SetCollisionProfileName(bWithCollision ? UCollisionProfile::BlockAll_ProfileName : UCollisionProfile::NoCollision_ProfileName);
	// No navmesh in the arena (robots use the flow field): a navigation-relevant ISM recomputes its bounds over every instance on each add or remove.
	Component->SetCanEverAffectNavigation(false);
	return Component;
}

UInstancedStaticMeshComponent* ARRArenaGrid::CreateCollisionComponent(FName Name)
{
	UInstancedStaticMeshComponent* Component = CreateDefaultSubobject<UInstancedStaticMeshComponent>(Name);
	Component->SetupAttachment(GridRoot);
	Component->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	// Collision only: never drawn, no shadow. Remove-at-swap like the HISMs, so both keep the same instance order.
	Component->SetVisibility(false);
	Component->SetHiddenInGame(true);
	Component->SetCastShadow(false);
	Component->SetRemoveSwap();
	Component->SetCanEverAffectNavigation(false);
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
		CellShotHealth.Reset();
		CellDigDrops.Reset();
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

UHierarchicalInstancedStaticMeshComponent* ARRArenaGrid::GetTemplateComponent(ERRCellType MapKey) const
{
	switch (MapKey)
	{
	case ERRCellType::Floor:      return FloorInstances;
	case ERRCellType::Rock:       return RockInstances;
	case ERRCellType::Ore:        return OreInstances;
	case ERRCellType::HealOre:    return HealOreInstances;
	case ERRCellType::SpecialOre: return SpecialOreInstances;
	case ERRCellType::Barrel:     return BarrelInstances;
	default:                      return nullptr;
	}
}

UInstancedStaticMeshComponent* ARRArenaGrid::GetTemplateCollisionComponent(ERRCellType MapKey) const
{
	switch (MapKey)
	{
	case ERRCellType::Rock:       return RockCollision;
	case ERRCellType::Ore:        return OreCollision;
	case ERRCellType::HealOre:    return HealOreCollision;
	case ERRCellType::SpecialOre: return SpecialOreCollision;
	case ERRCellType::Barrel:     return BarrelCollision;
	default:                      return nullptr;
	}
}

int32 ARRArenaGrid::GetVisualSlot(ERRCellType MapKey)
{
	switch (MapKey)
	{
	case ERRCellType::Floor:      return 0;
	case ERRCellType::Rock:       return 1;
	case ERRCellType::Ore:        return 2;
	case ERRCellType::HealOre:    return 3;
	case ERRCellType::SpecialOre: return 4;
	case ERRCellType::Barrel:     return 5;
	default:                      return INDEX_NONE;
	}
}

int32 ARRArenaGrid::GetVisualIndex(ERRCellType MapKey, FIntPoint Cell) const
{
	const int32 Slot = GetVisualSlot(MapKey);
	if (Slot == INDEX_NONE || !IsValidCell(Cell) || ChunkCells <= 0)
	{
		return INDEX_NONE;
	}
	const int32 ChunksPerSlot = ChunkCount.X * ChunkCount.Y;
	const int32 Index = Slot * ChunksPerSlot + (Cell.Y / ChunkCells) * ChunkCount.X + Cell.X / ChunkCells;
	return ChunkVisuals.IsValidIndex(Index) ? Index : INDEX_NONE;
}

ERRCellType ARRArenaGrid::GetCellMapKey(FIntPoint Cell) const
{
	const ERRCellType Type = GetCellType(Cell);
	if (Type == ERRCellType::Void)
	{
		return ERRCellType::Void;
	}
	return IsBlockType(Type) ? Type : ERRCellType::Floor;
}

UHierarchicalInstancedStaticMeshComponent* ARRArenaGrid::GetCellRenderComponent(FIntPoint Cell) const
{
	const int32 VisualIndex = GetVisualIndex(GetCellMapKey(Cell), Cell);
	return VisualIndex != INDEX_NONE ? ChunkVisuals[VisualIndex].Render.Get() : nullptr;
}

UInstancedStaticMeshComponent* ARRArenaGrid::CreateChunkComponent(TSubclassOf<UInstancedStaticMeshComponent> Class, const UInstancedStaticMeshComponent* Template, FIntPoint Chunk)
{
	const FName BaseName(*FString::Printf(TEXT("%s_%d_%d"), *Template->GetName(), Chunk.X, Chunk.Y));
	UInstancedStaticMeshComponent* Component = NewObject<UInstancedStaticMeshComponent>(this, Class, MakeUniqueObjectName(this, Class, BaseName), RF_Transient);
	// Same look and collision as the template (set in C++ or by a Blueprint subclass).
	Component->SetMobility(Template->Mobility);
	Component->SetCollisionProfileName(Template->GetCollisionProfileName());
	Component->SetCollisionEnabled(Template->GetCollisionEnabled());
	Component->SetVisibility(Template->GetVisibleFlag());
	Component->SetHiddenInGame(Template->bHiddenInGame);
	Component->SetCastShadow(Template->CastShadow);
	Component->SetStaticMesh(Template->GetStaticMesh());
	for (int32 MaterialIndex = 0; MaterialIndex < Template->GetNumOverrideMaterials(); ++MaterialIndex)
	{
		Component->SetMaterial(MaterialIndex, Template->OverrideMaterials[MaterialIndex]);
	}
	Component->SetNumCustomDataFloats(Template->NumCustomDataFloats);
	// No navmesh in the arena; distance fields as the template, unless bGridAffectsDistanceFields is off.
	Component->SetCanEverAffectNavigation(false);
	Component->bAffectDistanceFieldLighting = bGridAffectsDistanceFields && Template->bAffectDistanceFieldLighting;
	if (Template->SupportsRemoveSwap())
	{
		Component->SetRemoveSwap();
	}
	Component->SetupAttachment(GridRoot);
	if (GetWorld())
	{
		Component->RegisterComponent();
	}
	return Component;
}

FRRGridChunkVisual* ARRArenaGrid::GetOrCreateVisual(ERRCellType MapKey, FIntPoint Cell)
{
	const int32 VisualIndex = GetVisualIndex(MapKey, Cell);
	UHierarchicalInstancedStaticMeshComponent* Template = GetTemplateComponent(MapKey);
	if (VisualIndex == INDEX_NONE || !Template)
	{
		return nullptr;
	}

	FRRGridChunkVisual& Visual = ChunkVisuals[VisualIndex];
	if (!Visual.Render)
	{
		Visual.MapKey = MapKey;
		Visual.Chunk = FIntPoint(Cell.X / ChunkCells, Cell.Y / ChunkCells);
		Visual.Render = CastChecked<UHierarchicalInstancedStaticMeshComponent>(
			CreateChunkComponent(UHierarchicalInstancedStaticMeshComponent::StaticClass(), Template, Visual.Chunk));
		VisualByComponent.Add(Visual.Render, VisualIndex);
		if (const UInstancedStaticMeshComponent* CollisionTemplate = GetTemplateCollisionComponent(MapKey))
		{
			Visual.Collision = CreateChunkComponent(UInstancedStaticMeshComponent::StaticClass(), CollisionTemplate, Visual.Chunk);
			VisualByComponent.Add(Visual.Collision, VisualIndex);
		}
	}
	return &Visual;
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
	for (FRRGridChunkVisual& Visual : ChunkVisuals)
	{
		if (IsValid(Visual.Render))
		{
			Visual.Render->DestroyComponent();
		}
		if (IsValid(Visual.Collision))
		{
			Visual.Collision->DestroyComponent();
		}
	}
	ChunkVisuals.Reset();
	VisualByComponent.Reset();
	ChunkCount = FIntPoint::ZeroValue;

	// Templates hold no instances (a level saved before chunking may still carry some).
	const TArray<UInstancedStaticMeshComponent*> Templates = { FloorInstances.Get(), RockInstances.Get(), OreInstances.Get(),
		HealOreInstances.Get(), SpecialOreInstances.Get(), BarrelInstances.Get(),
		RockCollision.Get(), OreCollision.Get(), HealOreCollision.Get(), SpecialOreCollision.Get(), BarrelCollision.Get() };
	for (UInstancedStaticMeshComponent* Component : Templates)
	{
		if (Component && Component->GetInstanceCount() > 0)
		{
			Component->ClearInstances();
		}
	}
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
	MarkedCells.Reset();
	SetActorTickEnabled(false);

	struct FVisualAsset
	{
		ERRCellType MapKey;
		UStaticMesh* Mesh;
		UMaterialInterface* Material;
	};
	const FVisualAsset Assets[] = {
		{ ERRCellType::Floor, FloorMesh, FloorMaterial },
		{ ERRCellType::Rock, RockMesh, RockMaterial },
		{ ERRCellType::Ore, OreMesh, OreMaterial },
		{ ERRCellType::HealOre, HealOreMesh, HealOreMaterial },
		{ ERRCellType::SpecialOre, SpecialOreMesh, SpecialOreMaterial },
		{ ERRCellType::Barrel, BarrelMesh, BarrelMaterial },
	};
	// The templates carry the settings every chunk component copies (CreateChunkComponent).
	for (const FVisualAsset& Asset : Assets)
	{
		if (UHierarchicalInstancedStaticMeshComponent* Template = GetTemplateComponent(Asset.MapKey))
		{
			Template->SetStaticMesh(Asset.Mesh);
			Template->SetMaterial(0, Asset.Material);
			// Block HISMs only render: their collision lives in the matching collision ISM (enforced in case a subclass changed it).
			if (Asset.MapKey != ERRCellType::Floor)
			{
				Template->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				// Room for the marked flag (objective rocks); set while the component is empty.
				if (Template->NumCustomDataFloats <= MarkedCustomDataIndex)
				{
					Template->SetNumCustomDataFloats(MarkedCustomDataIndex + 1);
				}
			}
		}
		if (UInstancedStaticMeshComponent* CollisionTemplate = GetTemplateCollisionComponent(Asset.MapKey))
		{
			// Same mesh, so the collision shape matches the block at rest.
			CollisionTemplate->SetStaticMesh(Asset.Mesh);
		}
	}

	CellHealth.Init(0.f, Cells.Num());
	CellShotHealth.Init(0.f, Cells.Num());
	BarrelSources.Reset();
	CellDigDrops.Init(0, Cells.Num());
	CellInstanceIndex.Init(INDEX_NONE, Cells.Num());

	ChunkCells = FMath::Max(1, ChunkCells);
	ChunkCount = FIntPoint(FMath::DivideAndRoundUp(GridWidth, ChunkCells), FMath::DivideAndRoundUp(GridHeight, ChunkCells));
	constexpr int32 VisualSlotCount = 6;
	ChunkVisuals.SetNum(VisualSlotCount * ChunkCount.X * ChunkCount.Y);

	// Gather per chunk visual first and add in one batch; AddInstances appends, so instance i of a batch
	// gets index (count before batch) + i, which is recorded in both maps.
	struct FBatch
	{
		ERRCellType MapKey = ERRCellType::Floor;
		TArray<FTransform> Transforms;
		TArray<FIntPoint> BatchCells;
	};
	TArray<FBatch> Batches;
	Batches.SetNum(ChunkVisuals.Num());

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
			const bool bIsBlock = IsBlockType(Type) && GetTemplateComponent(Type) != nullptr;
			const ERRCellType MapKey = bIsBlock ? Type : ERRCellType::Floor;
			const int32 VisualIndex = GetVisualIndex(MapKey, Cell);
			if (VisualIndex == INDEX_NONE)
			{
				continue;
			}
			FBatch& Batch = Batches[VisualIndex];
			Batch.MapKey = MapKey;
			Batch.Transforms.Add(bIsBlock ? GetBlockInstanceTransform(Cell) : GetFloorInstanceTransform(Cell));
			Batch.BatchCells.Add(Cell);

			if (bIsBlock)
			{
				CellHealth[GetCellIndex(Cell)] = GetBlockMaxHealth(Type);
				CellShotHealth[GetCellIndex(Cell)] = Type == ERRCellType::Barrel ? BarrelShotHealth : 0.f;
			}
		}
	}

	for (FBatch& Batch : Batches)
	{
		if (Batch.Transforms.Num() == 0)
		{
			continue;
		}
		FRRGridChunkVisual* Visual = GetOrCreateVisual(Batch.MapKey, Batch.BatchCells[0]);
		if (!Visual || !Visual->Render)
		{
			continue;
		}

		const int32 FirstIndex = Visual->Render->GetInstanceCount();
		Visual->Render->AddInstances(Batch.Transforms, /*bShouldReturnIndices*/ false, /*bWorldSpace*/ false);
		if (Visual->Collision)
		{
			Visual->Collision->AddInstances(Batch.Transforms, /*bShouldReturnIndices*/ false, /*bWorldSpace*/ false);
		}

		Visual->InstanceCells.SetNum(FirstIndex);
		for (int32 Offset = 0; Offset < Batch.BatchCells.Num(); ++Offset)
		{
			const FIntPoint Cell = Batch.BatchCells[Offset];
			Visual->InstanceCells.Add(Cell);
			CellInstanceIndex[GetCellIndex(Cell)] = FirstIndex + Offset;
		}
	}
}

int32 ARRArenaGrid::AddCellInstance(ERRCellType MapKey, FIntPoint Cell, const FTransform& LocalTransform)
{
	FRRGridChunkVisual* Visual = GetOrCreateVisual(MapKey, Cell);
	if (!Visual || !Visual->Render)
	{
		return INDEX_NONE;
	}

	// HISM appends: the returned index is the previous instance count. The collision ISM appends the same instance.
	const int32 InstanceIndex = Visual->Render->AddInstance(LocalTransform, /*bWorldSpace*/ false);
	if (InstanceIndex == INDEX_NONE)
	{
		return INDEX_NONE;
	}
	if (Visual->Collision)
	{
		Visual->Collision->AddInstance(LocalTransform, /*bWorldSpace*/ false);
	}

	if (Visual->InstanceCells.Num() <= InstanceIndex)
	{
		Visual->InstanceCells.SetNum(InstanceIndex + 1);
	}
	Visual->InstanceCells[InstanceIndex] = Cell;
	CellInstanceIndex[GetCellIndex(Cell)] = InstanceIndex;
	return InstanceIndex;
}

void ARRArenaGrid::RemoveCellInstance(ERRCellType MapKey, FIntPoint Cell, int32 InstanceIndex)
{
	const int32 VisualIndex = GetVisualIndex(MapKey, Cell);
	FRRGridChunkVisual* Visual = VisualIndex != INDEX_NONE ? &ChunkVisuals[VisualIndex] : nullptr;
	if (!Visual || !Visual->Render || !Visual->InstanceCells.IsValidIndex(InstanceIndex))
	{
		return;
	}

	const FIntPoint RemovedCell = Visual->InstanceCells[InstanceIndex];
	const int32 LastIndex = Visual->InstanceCells.Num() - 1;
	if (!Visual->Render->RemoveInstance(InstanceIndex))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: failed to remove instance %d of cell (%d, %d)."), InstanceIndex, RemovedCell.X, RemovedCell.Y);
		return;
	}
	if (Visual->Collision && !Visual->Collision->RemoveInstance(InstanceIndex))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: failed to remove collision instance %d of cell (%d, %d)."), InstanceIndex, RemovedCell.X, RemovedCell.Y);
	}

	// HISM and the collision ISM (SetRemoveSwap) remove at swap: the last instance now occupies InstanceIndex. Mirror it in both maps.
	Visual->InstanceCells.RemoveAtSwap(InstanceIndex, EAllowShrinking::No);
	if (InstanceIndex != LastIndex)
	{
		const FIntPoint MovedCell = Visual->InstanceCells[InstanceIndex];
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

float ARRArenaGrid::GetClearDistance(FVector From, FVector Direction, float MaxDistance) const
{
	if (MaxDistance <= 0.f || CellSize <= 0.f)
	{
		return FMath::Max(MaxDistance, 0.f);
	}
	// Grid-local 2D ray (unscaled grid: local distances equal world distances).
	const FTransform& GridTransform = GetActorTransform();
	const FVector LocalStart = GridTransform.InverseTransformPosition(From);
	const FVector LocalDirection = GridTransform.InverseTransformVectorNoScale(Direction).GetSafeNormal2D();
	FIntPoint Cell(FMath::FloorToInt32(LocalStart.X / CellSize), FMath::FloorToInt32(LocalStart.Y / CellSize));
	if (IsBlockCell(Cell))
	{
		return 0.f;
	}
	if (LocalDirection.IsNearlyZero())
	{
		return MaxDistance;
	}

	// Amanatides-Woo traversal: TMax* = ray distance to the next vertical / horizontal cell boundary.
	const int32 StepX = LocalDirection.X > 0.0 ? 1 : (LocalDirection.X < 0.0 ? -1 : 0);
	const int32 StepY = LocalDirection.Y > 0.0 ? 1 : (LocalDirection.Y < 0.0 ? -1 : 0);
	const double DeltaX = StepX != 0 ? CellSize / FMath::Abs(LocalDirection.X) : TNumericLimits<double>::Max();
	const double DeltaY = StepY != 0 ? CellSize / FMath::Abs(LocalDirection.Y) : TNumericLimits<double>::Max();
	double TMaxX = StepX > 0 ? ((Cell.X + 1) * CellSize - LocalStart.X) / LocalDirection.X
		: (StepX < 0 ? (Cell.X * CellSize - LocalStart.X) / LocalDirection.X : TNumericLimits<double>::Max());
	double TMaxY = StepY > 0 ? ((Cell.Y + 1) * CellSize - LocalStart.Y) / LocalDirection.Y
		: (StepY < 0 ? (Cell.Y * CellSize - LocalStart.Y) / LocalDirection.Y : TNumericLimits<double>::Max());
	const double CornerTolerance = CellSize * 1.0e-4;

	while (true)
	{
		const double T = FMath::Min(TMaxX, TMaxY);
		if (T >= MaxDistance)
		{
			return MaxDistance;
		}
		if (FMath::Abs(TMaxX - TMaxY) <= CornerTolerance)
		{
			// Exactly through a corner: blocked when either side cell holds a block.
			if (IsBlockCell(FIntPoint(Cell.X + StepX, Cell.Y)) || IsBlockCell(FIntPoint(Cell.X, Cell.Y + StepY)))
			{
				return static_cast<float>(T);
			}
			Cell += FIntPoint(StepX, StepY);
			TMaxX += DeltaX;
			TMaxY += DeltaY;
		}
		else if (TMaxX < TMaxY)
		{
			Cell.X += StepX;
			TMaxX += DeltaX;
		}
		else
		{
			Cell.Y += StepY;
			TMaxY += DeltaY;
		}
		if (IsBlockCell(Cell))
		{
			return static_cast<float>(T);
		}
	}
}

bool ARRArenaGrid::HasLineOfSight(FVector From, FVector To) const
{
	const float Distance = FVector::Dist2D(From, To);
	if (Distance <= UE_KINDA_SMALL_NUMBER)
	{
		return !IsBlockCell(GetCellAtWorld(From));
	}
	return GetClearDistance(From, To - From, Distance) >= Distance;
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

	// The destroying hit never drops a dig pickup: DestroyCell drops the destruction bundle instead.
	if (CellHealth[Index] > UE_KINDA_SMALL_NUMBER)
	{
		SpawnDigDrops(Cell, Type);
	}

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
	const bool bWasMarked = MarkedCells.Remove(Cell) > 0;

	if (CellInstanceIndex.IsValidIndex(Index) && CellInstanceIndex[Index] != INDEX_NONE)
	{
		RemoveCellInstance(Type, Cell, CellInstanceIndex[Index]);
	}

	Cells[Index] = ERRCellType::Floor;
	CellHealth[Index] = 0.f;
	if (CellShotHealth.IsValidIndex(Index))
	{
		CellShotHealth[Index] = 0.f;
	}
	if (CellDigDrops.IsValidIndex(Index))
	{
		CellDigDrops[Index] = 0;
	}
	AddCellInstance(ERRCellType::Floor, Cell, GetFloorInstanceTransform(Cell));

	UE_LOG(LogRogueRigs, Verbose, TEXT("ARRArenaGrid: block destroyed at (%d, %d), type %s, source %s."), Cell.X, Cell.Y,
		*UEnum::GetValueAsString(Type), *UEnum::GetValueAsString(Source));

	SpawnPickups(Cell, Type, DestroyDropCount);

	// Every destroyed barrel explodes, whatever destroyed it (shots, dig, another explosion); queued, never synchronous.
	if (Type == ERRCellType::Barrel)
	{
		QueueBarrelExplosion(Cell);
	}

	if (bWasMarked)
	{
		OnCellMarkedChanged(Cell, false, GetCellCenter(Cell.X, Cell.Y));
	}
	OnCellDestroyed.Broadcast(Cell, Type, Source);
	return true;
}

bool ARRArenaGrid::SetCellMarked(FIntPoint Cell, bool bMarked)
{
	if (bMarked)
	{
		if (!IsBlockCell(Cell) || MarkedCells.Contains(Cell))
		{
			return false;
		}
		MarkedCells.Add(Cell);
		WriteMarkedCustomData(Cell, MarkedCustomDataValue);
	}
	else
	{
		if (MarkedCells.Remove(Cell) == 0)
		{
			return false;
		}
		WriteMarkedCustomData(Cell, 0.f);
	}
	OnCellMarkedChanged(Cell, bMarked, GetCellCenter(Cell.X, Cell.Y));
	return true;
}

void ARRArenaGrid::WriteMarkedCustomData(FIntPoint Cell, float Value)
{
	if (!IsBlockCell(Cell))
	{
		return;
	}
	const int32 Index = GetCellIndex(Cell);
	UHierarchicalInstancedStaticMeshComponent* Component = GetCellRenderComponent(Cell);
	if (!Component || !CellInstanceIndex.IsValidIndex(Index) || CellInstanceIndex[Index] == INDEX_NONE || Component->NumCustomDataFloats <= MarkedCustomDataIndex)
	{
		return;
	}
	Component->SetCustomDataValue(CellInstanceIndex[Index], MarkedCustomDataIndex, Value, /*bMarkRenderStateDirty*/ true);
}

float ARRArenaGrid::GetCellShotHealth(FIntPoint Cell) const
{
	return GetCellType(Cell) == ERRCellType::Barrel && CellShotHealth.IsValidIndex(GetCellIndex(Cell)) ? CellShotHealth[GetCellIndex(Cell)] : 0.f;
}

FIntPoint ARRArenaGrid::GetCellFromHit(const FHitResult& Hit) const
{
	if (const UPrimitiveComponent* HitComponent = Hit.GetComponent())
	{
		// Block chunk components only (render HISM or collision ISM); a floor hit falls through to the point below.
		const int32* VisualIndex = VisualByComponent.Find(HitComponent);
		if (VisualIndex && ChunkVisuals.IsValidIndex(*VisualIndex))
		{
			const FRRGridChunkVisual& Visual = ChunkVisuals[*VisualIndex];
			if (Visual.MapKey != ERRCellType::Floor && Visual.InstanceCells.IsValidIndex(Hit.Item))
			{
				return Visual.InstanceCells[Hit.Item];
			}
		}
	}
	const FVector Normal = Hit.ImpactNormal.IsNearlyZero() ? FVector::ZeroVector : Hit.ImpactNormal.GetSafeNormal();
	return GetCellAtWorld(Hit.ImpactPoint - Normal);
}

bool ARRArenaGrid::ApplyShotDamage(FIntPoint Cell, float Damage)
{
	if (Damage <= 0.f || GetCellType(Cell) != ERRCellType::Barrel || !CellShotHealth.IsValidIndex(GetCellIndex(Cell)))
	{
		return false;
	}

	const int32 Index = GetCellIndex(Cell);
	const float Applied = FMath::Min(Damage, CellShotHealth[Index]);
	CellShotHealth[Index] -= Applied;
	UE_LOG(LogRogueRigs, Verbose, TEXT("ARRArenaGrid: barrel (%d, %d) shot for %.1f, shot health %.1f."), Cell.X, Cell.Y, Applied, CellShotHealth[Index]);

	if (CellShotHealth[Index] <= UE_KINDA_SMALL_NUMBER)
	{
		return DestroyCell(Cell, ERRTerrainDamageSource::Shot);
	}
	return false;
}

URRBarrelSource* ARRArenaGrid::GetBarrelSource(FIntPoint Cell)
{
	if (!IsValidCell(Cell))
	{
		return nullptr;
	}
	TObjectPtr<URRBarrelSource>& Source = BarrelSources.FindOrAdd(Cell);
	if (!Source)
	{
		Source = NewObject<URRBarrelSource>(this);
		Source->Cell = Cell;
	}
	return Source;
}

void ARRArenaGrid::QueueBarrelExplosion(FIntPoint Cell)
{
	UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return;
	}
	URRExplosionSubsystem* Explosions = World->GetSubsystem<URRExplosionSubsystem>();
	if (!Explosions)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("ARRArenaGrid: no explosion subsystem, barrel (%d, %d) does not explode."), Cell.X, Cell.Y);
		return;
	}
	Explosions->QueueExplosion(GetCellCenter(Cell.X, Cell.Y), BarrelExplosionRow, this, GetBarrelSource(Cell));
}

TSubclassOf<ARRPickup> ARRArenaGrid::GetPickupClass(ERRCellType Type) const
{
	switch (Type)
	{
	case ERRCellType::Ore:     return OrePickupClass;
	case ERRCellType::HealOre: return HealPickupClass;
	default:                   return nullptr;
	}
}

void ARRArenaGrid::SpawnDigDrops(FIntPoint Cell, ERRCellType Type)
{
	const int32 Index = GetCellIndex(Cell);
	if (DropInterval <= 0.f || !GetPickupClass(Type) || !CellDigDrops.IsValidIndex(Index))
	{
		return;
	}

	// Multiples of DropInterval strictly below max health: 4.5 / 1.5 gives 2 (at 1.5 and 3.0); 4.5 is the destroying hit.
	const float MaxHealth = GetBlockMaxHealth(Type);
	const int32 MaxDigDrops = FMath::Max(0, FMath::CeilToInt32(MaxHealth / DropInterval - UE_KINDA_SMALL_NUMBER) - 1);
	const float Accumulated = MaxHealth - CellHealth[Index];
	const int32 Crossed = FMath::Min(FMath::FloorToInt32(Accumulated / DropInterval), MaxDigDrops);

	const int32 NewDrops = Crossed - CellDigDrops[Index];
	if (NewDrops > 0)
	{
		CellDigDrops[Index] = Crossed;
		SpawnPickups(Cell, Type, NewDrops);
	}
}

void ARRArenaGrid::SpawnPickups(FIntPoint Cell, ERRCellType Type, int32 Count)
{
	UWorld* World = GetWorld();
	const TSubclassOf<ARRPickup> PickupClass = GetPickupClass(Type);
	if (Count <= 0 || !PickupClass || !World || !World->IsGameWorld())
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector Location = GetCellCenter(Cell.X, Cell.Y);
	for (int32 Drop = 0; Drop < Count; ++Drop)
	{
		World->SpawnActor<ARRPickup>(PickupClass, Location, FRotator::ZeroRotator, SpawnParams);
	}

	UE_LOG(LogRogueRigs, Verbose, TEXT("ARRArenaGrid: %d pickup(s) dropped at (%d, %d), type %s."), Count, Cell.X, Cell.Y,
		*UEnum::GetValueAsString(Type));
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
		UHierarchicalInstancedStaticMeshComponent* Component = GetCellRenderComponent(Cell);
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
		UHierarchicalInstancedStaticMeshComponent* Component = GetCellRenderComponent(Cell);
		if (!Component || CellInstanceIndex[Index] == INDEX_NONE)
		{
			continue;
		}

		// Only the rendering instance moves; the collision instance stays at rest. Incommensurate sines per axis: a horizontal jitter bounded by ShakeAmplitude plus a vertical bob
		// bounded by ShakeVerticalAmplitude.
		const FVector Offset(
			ShakeAmplitude * FMath::Sin(Angle + Pair.Value),
			ShakeAmplitude * FMath::Sin(1.37f * Angle + 2.f * Pair.Value),
			ShakeVerticalAmplitude * FMath::Sin(1.71f * Angle + 3.f * Pair.Value));
		FTransform Transform = GetBlockInstanceTransform(Cell);
		Transform.AddToTranslation(Offset);
		Component->UpdateInstanceTransform(CellInstanceIndex[Index], Transform, /*bWorldSpace*/ false,
			/*bMarkRenderStateDirty*/ true, /*bTeleport*/ true);
	}
}
