// Rogue Rigs - arena grid actor: loads a layout JSON and builds floor and block instances.

#include "Grid/RRArenaGrid.h"

#include "RogueRigs.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Algo/StableSort.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "GAS/RRBaseStatsLoader.h"
#include "HAL/PlatformTime.h"
#include "Grid/RRBarrelSource.h"
#include "Grid/RROreTypeRow.h"
#include "Materials/MaterialInterface.h"
#include "Misc/Crc.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Pickups/RRPickup.h"
#include "Pickups/RRPickupSubsystem.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Telegraph/RRGroundTelegraphComponent.h"
#include "UObject/ConstructorHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRArenaGrid)

const TCHAR* ARRArenaGrid::OreTypesPath = TEXT("RogueRigs/Data/Tables/OreTypes.csv");

namespace RRArenaGridPrivate
{
	/** 32-bit integer hash with good avalanche (lowbias32): the same input always gives the same value, on every platform and run. */
	uint32 MixHash(uint32 Value)
	{
		Value ^= Value >> 16;
		Value *= 0x7feb352du;
		Value ^= Value >> 15;
		Value *= 0x846ca68bu;
		Value ^= Value >> 16;
		return Value;
	}
}

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

	// The barrel collision keeps the shape it had before the decorative barrel mesh (task-107): the engine cylinder.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultBarrelCollisionMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	BarrelCollisionMesh = DefaultBarrelCollisionMesh.Object;
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
		CellFloorInstanceIndex.Reset();
		CellDepositId.Reset();
		Deposits.Reset();
		PlacedDepositCounts.Reset();
		ShakingCells.Reset();
		ClearBarrelFuses();
		ClearPendingCellDestructions();
		SetActorTickEnabled(false);
		GridWidth = 0;
		GridHeight = 0;
		SpawnCell = FIntPoint(-1, -1);
		ClearAllInstances();
		return;
	}

	BuildDeposits();
	BuildInstances();
}

bool ARRArenaGrid::GetOreType(FName OreId, FRROreTypeRow& OutRow)
{
	return FRRBaseStatsLoader::FindRow(OreTypesPath, OreId, OutRow, TEXT("ARRArenaGrid"));
}

void ARRArenaGrid::BuildDeposits()
{
	CellDepositId.Init(INDEX_NONE, Cells.Num());
	Deposits.Reset();
	PlacedDepositCounts.Reset();

	// Ore type rows sorted by id, so placement order (and result) never depends on the table's map order.
	const UDataTable* Table = FRRBaseStatsLoader::GetCachedTable(OreTypesPath, FRROreTypeRow::StaticStruct());
	TArray<TPair<FName, const FRROreTypeRow*>> Rows;
	if (Table)
	{
		for (const TPair<FName, uint8*>& Pair : Table->GetRowMap())
		{
			Rows.Emplace(Pair.Key, reinterpret_cast<const FRROreTypeRow*>(Pair.Value));
		}
		Rows.Sort([](const TPair<FName, const FRROreTypeRow*>& A, const TPair<FName, const FRROreTypeRow*>& B)
		{
			return A.Key.LexicalLess(B.Key);
		});
	}

	// 1. Layout veins: each 4-connected group of layout Ore or HealOre cells is one deposit (the former whole-vein rule).
	static const FIntPoint Steps[] = { FIntPoint(1, 0), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(0, -1) };
	for (int32 Index = 0; Index < Cells.Num(); ++Index)
	{
		const ERRCellType Type = Cells[Index];
		if ((Type != ERRCellType::Ore && Type != ERRCellType::HealOre) || CellDepositId[Index] != INDEX_NONE)
		{
			continue;
		}
		const int32 DepositId = Deposits.AddDefaulted();
		FRRGridDeposit& Deposit = Deposits[DepositId];
		Deposit.CellType = Type;
		for (const TPair<FName, const FRROreTypeRow*>& Row : Rows)
		{
			if (Row.Value->CellType == Type)
			{
				Deposit.OreId = Row.Key;
				Deposit.ChunkValue = Row.Value->ChunkValue;
				Deposit.HealValue = Row.Value->HealValue;
				Deposit.Color = Row.Value->Color;
				Deposit.bHasColor = Row.Value->Color.A > 0.f;
				break;
			}
		}
		Deposit.Cells.Add(FIntPoint(Index % GridWidth, Index / GridWidth));
		CellDepositId[Index] = DepositId;
		for (int32 Visit = 0; Visit < Deposit.Cells.Num(); ++Visit)
		{
			for (const FIntPoint& Step : Steps)
			{
				const FIntPoint Next = Deposit.Cells[Visit] + Step;
				if (IsValidCell(Next) && Cells[GetCellIndex(Next)] == Type && CellDepositId[GetCellIndex(Next)] == INDEX_NONE)
				{
					CellDepositId[GetCellIndex(Next)] = DepositId;
					Deposit.Cells.Add(Next);
				}
			}
		}
	}

	// 2. Ore table deposits on the layout rock; every row's share is taken from the rock count before any placement.
	if (!bPlaceOreDeposits)
	{
		return;
	}
	int32 RockCount = 0;
	for (const ERRCellType Type : Cells)
	{
		RockCount += Type == ERRCellType::Rock ? 1 : 0;
	}
	for (const TPair<FName, const FRROreTypeRow*>& Row : Rows)
	{
		if (Row.Value->RockShare <= 0.f)
		{
			continue;
		}
		const int32 Placed = PlaceOreDeposits(Row.Key, *Row.Value, RockCount);
		PlacedDepositCounts.Add(Row.Key, Placed);
		UE_LOG(LogRogueRigs, Log, TEXT("ARRArenaGrid %s: %d %s deposits of %d x %d placed on layout %s (share %.3f of %d rock blocks, seed %d)."),
			*GetName(), Placed, *Row.Key.ToString(), Row.Value->GroupSize, Row.Value->GroupSize, *LayoutName, Row.Value->RockShare, RockCount, OreSeed);
	}
}

int32 ARRArenaGrid::PlaceOreDeposits(FName OreId, const FRROreTypeRow& Row, int32 OriginalRockCount)
{
	const int32 Size = FMath::Max(1, Row.GroupSize);
	if (!IsBlockType(Row.CellType) || Row.CellType == ERRCellType::Rock || Row.CellType == ERRCellType::Barrel)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: ore type %s has CellType %s, not an ore block type; no deposit placed."),
			*OreId.ToString(), *UEnum::GetValueAsString(Row.CellType));
		return 0;
	}
	const int32 Target = FMath::RoundToInt32(Row.RockShare * OriginalRockCount / static_cast<float>(Size * Size));
	if (Target <= 0 || GridWidth < Size || GridHeight < Size)
	{
		return 0;
	}

	// Every anchor (top-left cell) whose Size x Size square is all rock, scored by a hash of the anchor, the seed and the ore id.
	const uint32 OreHash = FCrc::StrCrc32(*OreId.ToString().ToLower());
	const uint32 SeedHash = RRArenaGridPrivate::MixHash(static_cast<uint32>(OreSeed) ^ OreHash);
	auto IsFreeRockSquare = [this, Size](int32 AnchorX, int32 AnchorY)
	{
		for (int32 DY = 0; DY < Size; ++DY)
		{
			for (int32 DX = 0; DX < Size; ++DX)
			{
				if (Cells[GetCellIndex(FIntPoint(AnchorX + DX, AnchorY + DY))] != ERRCellType::Rock)
				{
					return false;
				}
			}
		}
		return true;
	};
	TArray<TPair<uint32, int32>> Candidates;
	for (int32 Y = 0; Y + Size <= GridHeight; ++Y)
	{
		for (int32 X = 0; X + Size <= GridWidth; ++X)
		{
			if (IsFreeRockSquare(X, Y))
			{
				const uint32 Score = RRArenaGridPrivate::MixHash(RRArenaGridPrivate::MixHash(SeedHash + static_cast<uint32>(X)) + static_cast<uint32>(Y));
				Candidates.Emplace(Score, GetCellIndex(FIntPoint(X, Y)));
			}
		}
	}
	Candidates.Sort([](const TPair<uint32, int32>& A, const TPair<uint32, int32>& B)
	{
		return A.Key != B.Key ? A.Key < B.Key : A.Value < B.Value;
	});

	// Greedy in score order: an anchor is kept while its whole square is still rock (earlier deposits claim cells).
	int32 Placed = 0;
	for (const TPair<uint32, int32>& Candidate : Candidates)
	{
		if (Placed >= Target)
		{
			break;
		}
		const int32 AnchorX = Candidate.Value % GridWidth;
		const int32 AnchorY = Candidate.Value / GridWidth;
		if (!IsFreeRockSquare(AnchorX, AnchorY))
		{
			continue;
		}
		const int32 DepositId = Deposits.AddDefaulted();
		FRRGridDeposit& Deposit = Deposits[DepositId];
		Deposit.OreId = OreId;
		Deposit.CellType = Row.CellType;
		Deposit.ChunkValue = Row.ChunkValue;
		Deposit.HealValue = Row.HealValue;
		Deposit.Color = Row.Color;
		Deposit.bHasColor = Row.Color.A > 0.f;
		Deposit.bPlacedFromTable = true;
		for (int32 DY = 0; DY < Size; ++DY)
		{
			for (int32 DX = 0; DX < Size; ++DX)
			{
				const FIntPoint Cell(AnchorX + DX, AnchorY + DY);
				Cells[GetCellIndex(Cell)] = Row.CellType;
				CellDepositId[GetCellIndex(Cell)] = DepositId;
				Deposit.Cells.Add(Cell);
			}
		}
		++Placed;
	}
	if (Placed < Target)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("ARRArenaGrid: only %d of %d %s deposits fit on layout %s."), Placed, Target, *OreId.ToString(), *LayoutName);
	}
	return Placed;
}

void ARRArenaGrid::WriteDepositColors()
{
	TSet<UHierarchicalInstancedStaticMeshComponent*> Dirty;
	for (const FRRGridDeposit& Deposit : Deposits)
	{
		if (!Deposit.bHasColor)
		{
			continue;
		}
		for (const FIntPoint& Cell : Deposit.Cells)
		{
			const int32 Index = GetCellIndex(Cell);
			UHierarchicalInstancedStaticMeshComponent* Component = GetCellRenderComponent(Cell);
			if (!Component || CellInstanceIndex[Index] == INDEX_NONE || Component->NumCustomDataFloats < OreColorCustomDataIndex + 3)
			{
				continue;
			}
			const int32 Instance = CellInstanceIndex[Index];
			Component->SetCustomDataValue(Instance, OreColorCustomDataIndex, Deposit.Color.R, /*bMarkRenderStateDirty*/ false);
			Component->SetCustomDataValue(Instance, OreColorCustomDataIndex + 1, Deposit.Color.G, /*bMarkRenderStateDirty*/ false);
			Component->SetCustomDataValue(Instance, OreColorCustomDataIndex + 2, Deposit.Color.B, /*bMarkRenderStateDirty*/ false);
			Dirty.Add(Component);
		}
	}
	for (UHierarchicalInstancedStaticMeshComponent* Component : Dirty)
	{
		Component->MarkRenderStateDirty();
	}
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

FTransform ARRArenaGrid::GetBlockRenderTransform(FIntPoint Cell) const
{
	if (!BarrelMesh || !IsValidCell(Cell) || Cells[GetCellIndex(Cell)] != ERRCellType::Barrel)
	{
		return GetBlockInstanceTransform(Cell);
	}
	const FBox Box = BarrelMesh->GetBoundingBox();
	const FVector Size = Box.GetSize();
	const float Width = FMath::Max(Size.X, Size.Y);
	if (Width <= UE_KINDA_SMALL_NUMBER || Size.Z <= UE_KINDA_SMALL_NUMBER)
	{
		return GetBlockInstanceTransform(Cell);
	}
	// Uniform scale: the mesh fits one cell in width and BlockHeight in height. Its bounds bottom centre (the pivot of a
	// bottom-pivot mesh) sits on the cell centre at the floor top (z = 0): a barrel cell keeps its floor slab, the barrel stands on it.
	const float Scale = FMath::Min(CellSize / Width, BlockHeight / Size.Z);
	const FVector Anchor(Box.GetCenter().X, Box.GetCenter().Y, Box.Min.Z);
	const FVector Location = FVector((Cell.X + 0.5f) * CellSize, (Cell.Y + 0.5f) * CellSize, 0.f) - Scale * Anchor;
	return FTransform(FRotator::ZeroRotator, Location, FVector(Scale));
}

void ARRArenaGrid::BuildInstances()
{
	ClearAllInstances();
	ShakingCells.Reset();
	MarkedCells.Reset();
	ClearBarrelFuses();
	ClearPendingCellDestructions();
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
	// Deposits come from BuildDeposits on the same cells; a caller that replaced Cells (stress tool) gets none.
	if (CellDepositId.Num() != Cells.Num())
	{
		CellDepositId.Init(INDEX_NONE, Cells.Num());
		Deposits.Reset();
		PlacedDepositCounts.Reset();
	}
	TSet<ERRCellType> ColoredTypes;
	for (const FRRGridDeposit& Deposit : Deposits)
	{
		if (Deposit.bHasColor)
		{
			ColoredTypes.Add(Deposit.CellType);
		}
	}

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
				// Room for the ore colour (R, G, B) on the block types of deposits with an ore type row.
				if (ColoredTypes.Contains(Asset.MapKey) && Template->NumCustomDataFloats < OreColorCustomDataIndex + 3)
				{
					Template->SetNumCustomDataFloats(OreColorCustomDataIndex + 3);
				}
			}
		}
		if (UInstancedStaticMeshComponent* CollisionTemplate = GetTemplateCollisionComponent(Asset.MapKey))
		{
			// Same mesh, so the collision shape matches the block at rest. Barrels: the decorative BarrelMesh (no simple collision)
			// never drives collision; BarrelCollisionMesh keeps the block-sized shape.
			UStaticMesh* CollisionMesh = Asset.Mesh;
			if (Asset.MapKey == ERRCellType::Barrel && BarrelCollisionMesh)
			{
				CollisionMesh = BarrelCollisionMesh;
			}
			CollisionTemplate->SetStaticMesh(CollisionMesh);
		}
	}

	CellHealth.Init(0.f, Cells.Num());
	CellShotHealth.Init(0.f, Cells.Num());
	BarrelSources.Reset();
	CellDigDrops.Init(0, Cells.Num());
	CellInstanceIndex.Init(INDEX_NONE, Cells.Num());
	CellFloorInstanceIndex.Init(INDEX_NONE, Cells.Num());

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
		TArray<FTransform> CollisionTransforms;
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

			// A block cell has no floor instance (a floor top coplanar with the block bottom is what we avoid), except a barrel:
			// it stands on its floor slab, so its cell gets both a floor instance and a barrel instance.
			const bool bIsBlock = IsBlockType(Type) && GetTemplateComponent(Type) != nullptr;
			if (!bIsBlock || HasFloorSlab(Type))
			{
				const int32 FloorVisualIndex = GetVisualIndex(ERRCellType::Floor, Cell);
				if (FloorVisualIndex != INDEX_NONE)
				{
					FBatch& FloorBatch = Batches[FloorVisualIndex];
					FloorBatch.MapKey = ERRCellType::Floor;
					FloorBatch.Transforms.Add(GetFloorInstanceTransform(Cell));
					FloorBatch.CollisionTransforms.Add(GetFloorInstanceTransform(Cell));
					FloorBatch.BatchCells.Add(Cell);
				}
			}
			if (!bIsBlock)
			{
				continue;
			}
			const int32 VisualIndex = GetVisualIndex(Type, Cell);
			if (VisualIndex == INDEX_NONE)
			{
				continue;
			}
			FBatch& Batch = Batches[VisualIndex];
			Batch.MapKey = Type;
			Batch.Transforms.Add(GetBlockRenderTransform(Cell));
			Batch.CollisionTransforms.Add(GetBlockInstanceTransform(Cell));
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
			Visual->Collision->AddInstances(Batch.CollisionTransforms, /*bShouldReturnIndices*/ false, /*bWorldSpace*/ false);
		}

		Visual->InstanceCells.SetNum(FirstIndex);
		TArray<int32>& IndexMap = GetInstanceIndexMap(Batch.MapKey);
		for (int32 Offset = 0; Offset < Batch.BatchCells.Num(); ++Offset)
		{
			const FIntPoint Cell = Batch.BatchCells[Offset];
			Visual->InstanceCells.Add(Cell);
			IndexMap[GetCellIndex(Cell)] = FirstIndex + Offset;
		}
	}

	WriteDepositColors();
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
	GetInstanceIndexMap(MapKey)[GetCellIndex(Cell)] = InstanceIndex;
	return InstanceIndex;
}

TArray<int32>& ARRArenaGrid::GetInstanceIndexMap(ERRCellType MapKey)
{
	return MapKey == ERRCellType::Floor ? CellFloorInstanceIndex : CellInstanceIndex;
}

bool ARRArenaGrid::HasFloorSlab(ERRCellType Type)
{
	return Type == ERRCellType::Floor || Type == ERRCellType::Spawn || Type == ERRCellType::Barrel;
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
	// The floor map and the block map are separate (a barrel cell is in both visuals), so patch the map of MapKey only.
	TArray<int32>& IndexMap = GetInstanceIndexMap(MapKey);
	Visual->InstanceCells.RemoveAtSwap(InstanceIndex, EAllowShrinking::No);
	if (InstanceIndex != LastIndex)
	{
		const FIntPoint MovedCell = Visual->InstanceCells[InstanceIndex];
		IndexMap[GetCellIndex(MovedCell)] = InstanceIndex;
	}
	IndexMap[GetCellIndex(RemovedCell)] = INDEX_NONE;
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

int32 ARRArenaGrid::GetDepositCells(FIntPoint Cell, TArray<FIntPoint>& OutCells) const
{
	OutCells.Reset();
	if (!IsBlockCell(Cell))
	{
		return 0;
	}
	const int32 DepositId = GetDepositId(Cell);
	if (DepositId == INDEX_NONE)
	{
		OutCells.Add(Cell);
		return 1;
	}

	// By id only: a neighbouring deposit of the same type is never pulled in. Cell comes first.
	OutCells.Add(Cell);
	for (const FIntPoint& Member : Deposits[DepositId].Cells)
	{
		if (Member != Cell && GetDepositId(Member) == DepositId)
		{
			OutCells.Add(Member);
		}
	}
	return OutCells.Num();
}

int32 ARRArenaGrid::GetDepositId(FIntPoint Cell) const
{
	if (!IsValidCell(Cell) || !CellDepositId.IsValidIndex(GetCellIndex(Cell)))
	{
		return INDEX_NONE;
	}
	const int32 DepositId = CellDepositId[GetCellIndex(Cell)];
	// The cell must still hold the deposit's block (destroyed cells are cleared; a replaced grid keeps nothing stale).
	return Deposits.IsValidIndex(DepositId) && Deposits[DepositId].CellType == Cells[GetCellIndex(Cell)] ? DepositId : INDEX_NONE;
}

FName ARRArenaGrid::GetDepositOreId(FIntPoint Cell) const
{
	const int32 DepositId = GetDepositId(Cell);
	return DepositId != INDEX_NONE ? Deposits[DepositId].OreId : NAME_None;
}

bool ARRArenaGrid::IsTableDepositCell(FIntPoint Cell) const
{
	const int32 DepositId = GetDepositId(Cell);
	return DepositId != INDEX_NONE && Deposits[DepositId].bPlacedFromTable;
}

int32 ARRArenaGrid::GetPlacedDepositCount(FName OreId) const
{
	const int32* Count = PlacedDepositCounts.Find(OreId);
	return Count ? *Count : 0;
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
	// A fusing barrel ignores every damage until it explodes.
	if (Amount <= 0.f || !IsBlockCell(Cell) || !CellHealth.IsValidIndex(GetCellIndex(Cell)) || BarrelFuses.Contains(Cell))
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

bool ARRArenaGrid::IsExplosionSource(ERRTerrainDamageSource Source)
{
	return Source == ERRTerrainDamageSource::Barrel || Source == ERRTerrainDamageSource::Kamikaze;
}

bool ARRArenaGrid::DestroyCell(FIntPoint Cell, ERRTerrainDamageSource Source)
{
	// Whole deposit (task-187-01, ore-blocks): an explosion never leaves a deposit partly destroyed. The other standing
	// blocks are read by id before Cell leaves the deposit; each goes through DestroySingleCell (no re-expansion), and a
	// block already broken meanwhile (a listener, the explosion queue) is skipped there.
	TArray<FIntPoint> DepositCells;
	if (IsExplosionSource(Source) && GetDepositId(Cell) != INDEX_NONE)
	{
		GetDepositCells(Cell, DepositCells);
	}

	if (!DestroySingleCell(Cell, Source))
	{
		return false;
	}
	if (DepositCells.Num() > 1)
	{
		int32 Broken = 1;
		for (int32 Member = 1; Member < DepositCells.Num(); ++Member)
		{
			Broken += DestroySingleCell(DepositCells[Member], Source) ? 1 : 0;
		}
		UE_LOG(LogRogueRigs, Verbose, TEXT("ARRArenaGrid: deposit broken whole from (%d, %d), %d block(s), source %s."), Cell.X, Cell.Y,
			Broken, *UEnum::GetValueAsString(Source));
	}
	return true;
}

bool ARRArenaGrid::DestroySingleCell(FIntPoint Cell, ERRTerrainDamageSource Source)
{
	if (!IsBlockCell(Cell))
	{
		return false;
	}

	const int32 Index = GetCellIndex(Cell);
	const ERRCellType Type = Cells[Index];

	// Barrel fuse (task-114-01): a barrel brought to 0 during play lights its fuse instead of breaking; while it fuses every
	// destruction request is ignored, except once its explosion is queued (that explosion destroys its own cell).
	bool bExplosionAlreadyQueued = false;
	if (Type == ERRCellType::Barrel)
	{
		if (const FBarrelFuse* Fuse = BarrelFuses.Find(Cell))
		{
			if (!Fuse->bExplosionQueued)
			{
				return false;
			}
			bExplosionAlreadyQueued = true;
			EndBarrelFuse(Cell);
		}
		else if (BarrelFuseSeconds > 0.f && GetWorld() && GetWorld()->IsGameWorld())
		{
			StartBarrelFuse(Cell);
			UE_LOG(LogRogueRigs, Verbose, TEXT("ARRArenaGrid: barrel (%d, %d) fuse lit (%.2f s), source %s."), Cell.X, Cell.Y,
				BarrelFuseSeconds, *UEnum::GetValueAsString(Source));
			return true;
		}
	}

	ShakingCells.Remove(Cell);
	UpdateTickEnabled();
	const bool bWasMarked = MarkedCells.Remove(Cell) > 0;

	// Chunk and heal values of the block's deposit (ore type row), read before the cell leaves it.
	const int32 DepositId = GetDepositId(Cell);
	const int32 ChunkValue = DepositId != INDEX_NONE ? Deposits[DepositId].ChunkValue : 1;
	const float HealValue = DepositId != INDEX_NONE ? Deposits[DepositId].HealValue : 0.f;
	if (CellDepositId.IsValidIndex(Index))
	{
		CellDepositId[Index] = INDEX_NONE;
	}

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
	// A barrel cell already has its floor slab (BuildInstances): keep it, never add a second one (no duplicate, no z-fighting).
	if (!CellFloorInstanceIndex.IsValidIndex(Index) || CellFloorInstanceIndex[Index] == INDEX_NONE)
	{
		AddCellInstance(ERRCellType::Floor, Cell, GetFloorInstanceTransform(Cell));
	}

	UE_LOG(LogRogueRigs, Verbose, TEXT("ARRArenaGrid: block destroyed at (%d, %d), type %s, source %s."), Cell.X, Cell.Y,
		*UEnum::GetValueAsString(Type), *UEnum::GetValueAsString(Source));

	// One chunk per broken block whatever the source (drone or explosion, start zone included: no protection, ore-blocks).
	SpawnPickups(Cell, Type, DestroyDropCount, ChunkValue, HealValue);

	// Every destroyed barrel explodes, whatever destroyed it (shots, dig, another explosion); queued, never synchronous.
	if (Type == ERRCellType::Barrel && !bExplosionAlreadyQueued)
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
	if (Damage <= 0.f || GetCellType(Cell) != ERRCellType::Barrel || !CellShotHealth.IsValidIndex(GetCellIndex(Cell)) || BarrelFuses.Contains(Cell))
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

bool ARRArenaGrid::QueueBarrelExplosion(FIntPoint Cell)
{
	UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return false;
	}
	URRExplosionSubsystem* Explosions = World->GetSubsystem<URRExplosionSubsystem>();
	if (!Explosions)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("ARRArenaGrid: no explosion subsystem, barrel (%d, %d) does not explode."), Cell.X, Cell.Y);
		return false;
	}
	return Explosions->QueueExplosion(GetCellCenter(Cell.X, Cell.Y), BarrelExplosionRow, this, GetBarrelSource(Cell));
}

float ARRArenaGrid::GetBarrelExplosionRadius() const
{
	const UWorld* World = GetWorld();
	URRExplosionSubsystem* Explosions = World ? World->GetSubsystem<URRExplosionSubsystem>() : nullptr;
	FRRExplosionStatsRow Stats;
	return Explosions && Explosions->GetExplosionStats(BarrelExplosionRow, Stats) ? Stats.Radius : 0.f;
}

float ARRArenaGrid::GetBarrelFuseProgress(FIntPoint Cell) const
{
	const FBarrelFuse* Fuse = BarrelFuses.Find(Cell);
	if (!Fuse)
	{
		return 0.f;
	}
	if (Fuse->bExplosionQueued || BarrelFuseSeconds <= 0.f)
	{
		return 1.f;
	}
	const UWorld* World = GetWorld();
	const double Elapsed = World ? World->GetTimeSeconds() - Fuse->StartTime : 0.0;
	return FMath::Clamp(static_cast<float>(Elapsed / BarrelFuseSeconds), 0.f, 1.f);
}

URRGroundTelegraphComponent* ARRArenaGrid::GetBarrelTelegraph(FIntPoint Cell) const
{
	const FBarrelFuse* Fuse = BarrelFuses.Find(Cell);
	return Fuse ? Fuse->Telegraph : nullptr;
}

void ARRArenaGrid::StartBarrelFuse(FIntPoint Cell)
{
	StopShaking(Cell);
	const int32 Index = GetCellIndex(Cell);
	CellHealth[Index] = 0.f;
	if (CellShotHealth.IsValidIndex(Index))
	{
		CellShotHealth[Index] = 0.f;
	}

	FBarrelFuse& Fuse = BarrelFuses.Add(Cell);
	Fuse.StartTime = GetWorld()->GetTimeSeconds();

	// One decal per fusing barrel, hosted by the grid (barrels are cells, not actors): reused from the free list, created only
	// when every created one is in use, so a long chain never allocates more than the peak of simultaneous fuses.
	URRGroundTelegraphComponent* Telegraph = nullptr;
	while (!Telegraph && FreeBarrelTelegraphs.Num() > 0)
	{
		Telegraph = FreeBarrelTelegraphs.Pop(EAllowShrinking::No);
	}
	if (!Telegraph)
	{
		Telegraph = NewObject<URRGroundTelegraphComponent>(this, NAME_None, RF_Transient);
		Telegraph->SetupAttachment(GetRootComponent());
		Telegraph->RegisterComponent();
		BarrelTelegraphs.Add(Telegraph);
	}
	Fuse.Telegraph = Telegraph;
	// Cell centre on the floor top (the decal projects ProjectionHalfDepth above and below).
	Telegraph->SetWorldLocation(GetCellCenter(Cell.X, Cell.Y));
	Telegraph->SetRadius(GetBarrelExplosionRadius());
	Telegraph->SetMode(ERRGroundTelegraphMode::Danger);
	Telegraph->SetProgress(0.f);
	Telegraph->Show();

	UpdateTickEnabled();
}

void ARRArenaGrid::EndBarrelFuse(FIntPoint Cell)
{
	FBarrelFuse Fuse;
	if (!BarrelFuses.RemoveAndCopyValue(Cell, Fuse))
	{
		return;
	}
	if (Fuse.Telegraph)
	{
		Fuse.Telegraph->Hide();
		FreeBarrelTelegraphs.Add(Fuse.Telegraph);
	}
	UpdateTickEnabled();
}

void ARRArenaGrid::ClearBarrelFuses()
{
	for (const TPair<FIntPoint, FBarrelFuse>& Pair : BarrelFuses)
	{
		if (Pair.Value.Telegraph)
		{
			Pair.Value.Telegraph->Hide();
			FreeBarrelTelegraphs.Add(Pair.Value.Telegraph);
		}
	}
	BarrelFuses.Reset();
}

void ARRArenaGrid::TickBarrelFuses()
{
	const UWorld* World = GetWorld();
	if (BarrelFuses.IsEmpty() || !World)
	{
		return;
	}
	const double Now = World->GetTimeSeconds();
	TArray<FIntPoint, TInlineAllocator<8>> Ended;
	for (TPair<FIntPoint, FBarrelFuse>& Pair : BarrelFuses)
	{
		FBarrelFuse& Fuse = Pair.Value;
		if (Fuse.bExplosionQueued)
		{
			continue;
		}
		const double Elapsed = Now - Fuse.StartTime;
		const float Progress = BarrelFuseSeconds > 0.f ? FMath::Clamp(static_cast<float>(Elapsed / BarrelFuseSeconds), 0.f, 1.f) : 1.f;
		if (Fuse.Telegraph)
		{
			Fuse.Telegraph->SetProgress(Progress);
		}
		if (Elapsed >= BarrelFuseSeconds - UE_KINDA_SMALL_NUMBER)
		{
			Ended.Add(Pair.Key);
		}
	}
	// Queue after the loop: queuing never resolves synchronously, but DestroyCell (fallback) edits BarrelFuses.
	for (const FIntPoint& Cell : Ended)
	{
		if (FBarrelFuse* Fuse = BarrelFuses.Find(Cell))
		{
			Fuse->bExplosionQueued = true;
			if (!QueueBarrelExplosion(Cell))
			{
				// Explosion refused (no subsystem, missing row, source spent): never leave a barrel fusing forever.
				DestroyCell(Cell, ERRTerrainDamageSource::Barrel);
			}
		}
	}
}

void ARRArenaGrid::UpdateTickEnabled()
{
	SetActorTickEnabled(!ShakingCells.IsEmpty() || !BarrelFuses.IsEmpty() || GetPendingCellDestructionCount() > 0);
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
		const int32 DepositId = GetDepositId(Cell);
		SpawnPickups(Cell, Type, NewDrops, DepositId != INDEX_NONE ? Deposits[DepositId].ChunkValue : 1,
			DepositId != INDEX_NONE ? Deposits[DepositId].HealValue : 0.f);
	}
}

void ARRArenaGrid::SpawnPickups(FIntPoint Cell, ERRCellType Type, int32 Count, int32 OreValue, float HealValue)
{
	UWorld* World = GetWorld();
	const TSubclassOf<ARRPickup> PickupClass = GetPickupClass(Type);
	if (Count <= 0 || !PickupClass || !World || !World->IsGameWorld())
	{
		return;
	}

	// Through the pickup subsystem: from the actor pool, missing pickups created within its SpawnMaxFrames (explosions break many blocks at once).
	URRPickupSubsystem* Pickups = World->GetSubsystem<URRPickupSubsystem>();
	const FVector Location = GetCellCenter(Cell.X, Cell.Y);
	for (int32 Drop = 0; Drop < Count; ++Drop)
	{
		if (Pickups)
		{
			Pickups->RequestSpawn(PickupClass, Location, this, OreValue, HealValue);
			continue;
		}
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.bDeferConstruction = true;
		if (ARRPickup* Pickup = World->SpawnActor<ARRPickup>(PickupClass, Location, FRotator::ZeroRotator, SpawnParams))
		{
			Pickup->SetOreValue(OreValue);
			if (HealValue > 0.f)
			{
				Pickup->SetHealAmount(HealValue);
			}
			Pickup->FinishSpawning(FTransform(Location));
		}
	}

	UE_LOG(LogRogueRigs, Verbose, TEXT("ARRArenaGrid: %d pickup(s) dropped at (%d, %d), type %s, value %d, heal %.2f."), Count, Cell.X, Cell.Y,
		*UEnum::GetValueAsString(Type), OreValue, HealValue);
}

void ARRArenaGrid::CollectBlocksInRadius(const FVector& Center, float Radius, TArray<FIntPoint>& OutCells) const
{
	if (Radius < 0.f || GridWidth <= 0 || GridHeight <= 0)
	{
		return;
	}

	// Candidate cells: the local bounding square of the circle, widened by one cell for rounding.
	const FIntPoint CenterCell = GetCellAtWorld(Center);
	const int32 Reach = FMath::CeilToInt32(Radius / CellSize) + 1;
	const int32 MinX = FMath::Max(0, CenterCell.X - Reach);
	const int32 MaxX = FMath::Min(GridWidth - 1, CenterCell.X + Reach);
	const int32 MinY = FMath::Max(0, CenterCell.Y - Reach);
	const int32 MaxY = FMath::Min(GridHeight - 1, CenterCell.Y + Reach);

	const float RadiusSquared = FMath::Square(Radius);
	for (int32 Y = MinY; Y <= MaxY; ++Y)
	{
		for (int32 X = MinX; X <= MaxX; ++X)
		{
			const FIntPoint Cell(X, Y);
			if (IsBlockCell(Cell) && FVector::DistSquared2D(GetCellCenter(X, Y), Center) <= RadiusSquared)
			{
				OutCells.Add(Cell);
			}
		}
	}
}

int32 ARRArenaGrid::DestroyCellsInRadius(FVector Center, float Radius, ERRTerrainDamageSource Source)
{
	// Collect first: OnCellDestroyed listeners may change cells while we destroy.
	TArray<FIntPoint> Targets;
	CollectBlocksInRadius(Center, Radius, Targets);

	int32 Destroyed = 0;
	for (const FIntPoint& Cell : Targets)
	{
		Destroyed += DestroyCell(Cell, Source) ? 1 : 0;
	}
	return Destroyed;
}

int32 ARRArenaGrid::QueueDestroyCellsInRadius(FVector Center, float Radius, ERRTerrainDamageSource Source)
{
	const UWorld* World = GetWorld();
	if (CellDestructionBudgetMs <= 0.f || !World || !World->IsGameWorld())
	{
		return DestroyCellsInRadius(Center, Radius, Source);
	}

	TArray<FIntPoint> Targets;
	CollectBlocksInRadius(Center, Radius, Targets);
	// Nearest to the center first; ties in row-major order (stable sort of a row-major list), so the order is deterministic.
	Algo::StableSortBy(Targets, [this, &Center](const FIntPoint& Cell)
	{
		return FVector::DistSquared2D(GetCellCenter(Cell.X, Cell.Y), Center);
	});

	// Barrels at once: lighting a fuse moves no instance, and a chained barrel must start its fuse at the explosion (the
	// exploding barrel's own cell, whose explosion is queued, breaks here too). Every other block waits for its frame.
	int32 Handled = 0;
	TArray<FIntPoint, TInlineAllocator<8>> Barrels;
	PendingCellDestructions.Reserve(PendingCellDestructions.Num() + Targets.Num());
	for (const FIntPoint& Cell : Targets)
	{
		if (GetCellType(Cell) == ERRCellType::Barrel)
		{
			Barrels.Add(Cell);
			continue;
		}
		FPendingCellDestruction& Pending = PendingCellDestructions.AddDefaulted_GetRef();
		Pending.Cell = Cell;
		Pending.Source = Source;
		++Handled;
	}
	for (const FIntPoint& Cell : Barrels)
	{
		Handled += DestroyCell(Cell, Source) ? 1 : 0;
	}

	UpdateTickEnabled();
	// First slice in the explosion frame; the rest in the next grid ticks.
	ProcessPendingCellDestructions();
	return Handled;
}

void ARRArenaGrid::ProcessPendingCellDestructions()
{
	if (GetPendingCellDestructionCount() <= 0)
	{
		ClearPendingCellDestructions();
		return;
	}

	if (DestructionBudgetFrame != GFrameCounter)
	{
		DestructionBudgetFrame = GFrameCounter;
		DestructionSpentSeconds = 0.0;
		DestructionCellsThisFrame = 0;
	}

	const double BudgetSeconds = CellDestructionBudgetMs * 0.001;
	const double Start = FPlatformTime::Seconds();
	double Now = Start;
	while (PendingCellDestructionHead < PendingCellDestructions.Num())
	{
		// Always progress by one block per frame, then stop once the frame's budget is spent.
		if (DestructionCellsThisFrame > 0 && DestructionSpentSeconds + (Now - Start) >= BudgetSeconds)
		{
			break;
		}
		// Copy: DestroyCell's listeners may queue more blocks (array reallocation).
		const FPendingCellDestruction Pending = PendingCellDestructions[PendingCellDestructionHead++];
		// A block destroyed meanwhile (dig, another explosion) is skipped by DestroyCell.
		DestroyCell(Pending.Cell, Pending.Source);
		++DestructionCellsThisFrame;
		Now = FPlatformTime::Seconds();
	}
	DestructionSpentSeconds += Now - Start;

	if (PendingCellDestructionHead >= PendingCellDestructions.Num())
	{
		ClearPendingCellDestructions();
	}
}

void ARRArenaGrid::ClearPendingCellDestructions()
{
	PendingCellDestructions.Reset();
	PendingCellDestructionHead = 0;
	UpdateTickEnabled();
}

void ARRArenaGrid::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearPendingCellDestructions();
	Super::EndPlay(EndPlayReason);
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
			Component->UpdateInstanceTransform(CellInstanceIndex[Index], GetBlockRenderTransform(Cell), /*bWorldSpace*/ false,
				/*bMarkRenderStateDirty*/ true, /*bTeleport*/ true);
		}
	}

	UpdateTickEnabled();
}

void ARRArenaGrid::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickBarrelFuses();
	if (GetPendingCellDestructionCount() > 0)
	{
		ProcessPendingCellDestructions();
	}
	if (ShakingCells.IsEmpty())
	{
		UpdateTickEnabled();
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
		FTransform Transform = GetBlockRenderTransform(Cell);
		Transform.AddToTranslation(Offset);
		Component->UpdateInstanceTransform(CellInstanceIndex[Index], Transform, /*bWorldSpace*/ false,
			/*bMarkRenderStateDirty*/ true, /*bTeleport*/ true);
	}
}
