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
	PrimaryActorTick.bCanEverTick = false;

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

	if (!LoadLayout())
	{
		Cells.Reset();
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
}

void ARRArenaGrid::BuildInstances()
{
	ClearAllInstances();

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

	// Source meshes are MeshBaseSize cubes with a centered pivot: scale to the cell footprint,
	// sink the floor below the actor's Z, stand blocks on top of it.
	const float HorizontalScale = CellSize / MeshBaseSize;
	const FVector FloorScale(HorizontalScale, HorizontalScale, FloorThickness / MeshBaseSize);
	const FVector BlockScale(HorizontalScale, HorizontalScale, BlockHeight / MeshBaseSize);

	TArray<FTransform> FloorTransforms;
	TMap<UHierarchicalInstancedStaticMeshComponent*, TArray<FTransform>> BlockTransforms;
	FloorTransforms.Reserve(Cells.Num());

	for (int32 Y = 0; Y < GridHeight; ++Y)
	{
		for (int32 X = 0; X < GridWidth; ++X)
		{
			const ERRCellType Type = Cells[Y * GridWidth + X];
			if (Type == ERRCellType::Void)
			{
				continue;
			}

			const float CenterX = (X + 0.5f) * CellSize;
			const float CenterY = (Y + 0.5f) * CellSize;
			FloorTransforms.Emplace(FRotator::ZeroRotator, FVector(CenterX, CenterY, -0.5f * FloorThickness), FloorScale);

			if (UHierarchicalInstancedStaticMeshComponent* BlockComponent = GetBlockComponent(Type))
			{
				BlockTransforms.FindOrAdd(BlockComponent).Emplace(FRotator::ZeroRotator, FVector(CenterX, CenterY, 0.5f * BlockHeight), BlockScale);
			}
		}
	}

	if (FloorInstances && FloorTransforms.Num() > 0)
	{
		FloorInstances->AddInstances(FloorTransforms, /*bShouldReturnIndices*/ false, /*bWorldSpace*/ false);
	}
	for (const TPair<UHierarchicalInstancedStaticMeshComponent*, TArray<FTransform>>& Pair : BlockTransforms)
	{
		Pair.Key->AddInstances(Pair.Value, /*bShouldReturnIndices*/ false, /*bWorldSpace*/ false);
	}
}

FVector ARRArenaGrid::GetCellCenter(int32 X, int32 Y) const
{
	const FVector LocalCenter((X + 0.5f) * CellSize, (Y + 0.5f) * CellSize, 0.f);
	return GetActorTransform().TransformPosition(LocalCenter);
}

FIntPoint ARRArenaGrid::GetSpawnCell() const
{
	return SpawnCell;
}

ERRCellType ARRArenaGrid::GetCellType(int32 X, int32 Y) const
{
	if (X < 0 || X >= GridWidth || Y < 0 || Y >= GridHeight || !Cells.IsValidIndex(Y * GridWidth + X))
	{
		return ERRCellType::Void;
	}
	return Cells[Y * GridWidth + X];
}
