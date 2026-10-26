// Rogue Rigs - arena grid actor: loads a layout JSON and builds floor and block instances.

#include "Grid/RRArenaGrid.h"

#include "RogueRigs.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Algo/StableSort.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "GAS/RRBaseStatsLoader.h"
#include "HAL/PlatformTime.h"
#include "Grid/RRBarrelSource.h"
#include "Grid/RRLayoutData.h"
#include "Grid/RROreTypeRow.h"
#include "Materials/MaterialInterface.h"
#include "Misc/App.h"
#include "Misc/Crc.h"
#include "Misc/Paths.h"
#include "Pickups/RRPickup.h"
#include "Pickups/RRPickupSubsystem.h"
#include "Pickups/RRPulseDesync.h"
#include "Run/RRRunSubsystem.h"
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

	// Ore crystal clusters (task-230-08, rock-render-damage): Duskite and heal ore use SM_OreChunk_01 by default.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultOreCrystalMesh(TEXT("/Game/RogueRigs/Art/OreChunk/SM_OreChunk_01.SM_OreChunk_01"));
	OreCrystalMesh = DefaultOreCrystalMesh.Object;
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

	// One barrel telegraph created at level start, hidden (task-201-01): its BeginPlay loads the telegraph material and draws
	// it once out of sight (ARRDecalWarmUp), so the first fuse is not drawn with the engine default decal. Rendering runs only.
	if (BarrelTelegraphs.IsEmpty() && FApp::CanEverRender() && GetWorld() && GetWorld()->IsGameWorld())
	{
		FreeBarrelTelegraphs.Add(CreateBarrelTelegraph());
	}
}

bool ARRArenaGrid::RebuildForRunLevel()
{
	RebuildFromLayout();
	return GridWidth > 0 && GridHeight > 0;
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
	return RRLayout::GetLayoutFilePath(LayoutName);
}

bool ARRArenaGrid::LoadLayout()
{
	if (const TSharedPtr<const FRRLayoutData> Prepared = FindRunPreparedLayout())
	{
		LayoutName = Prepared->Name;
		ApplyLayoutData(*Prepared);
		UE_LOG(LogRogueRigs, Log, TEXT("ARRArenaGrid %s: built from the prepared layout %s (%d x %d)."), *GetName(), *LayoutName, GridWidth, GridHeight);
		return true;
	}

	if (LayoutName.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("ARRArenaGrid %s: LayoutName is empty, nothing to build."), *GetName());
		return false;
	}

	FRRLayoutData Layout;
	if (!RRLayout::LoadLayoutFile(LayoutName, GetLayoutFilePath(), Layout))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: %s"), *Layout.Error);
		return false;
	}
	ApplyLayoutData(Layout);
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		UE_LOG(LogRogueRigs, Log, TEXT("ARRArenaGrid %s: built from its own layout %s (%d x %d), read synchronously."), *GetName(), *LayoutName, GridWidth, GridHeight);
	}
	return true;
}

TSharedPtr<const FRRLayoutData> ARRArenaGrid::FindRunPreparedLayout() const
{
	const UWorld* World = GetWorld();
	URRRunSubsystem* Run = World && World->IsGameWorld() ? URRRunSubsystem::Get(this) : nullptr;
	if (!Run || !Run->IsRunInProgress() || Run->GetLevelIndex() <= 1)
	{
		return nullptr;
	}
	const int32 Level = Run->GetLevelIndex();
	if (TSharedPtr<const FRRLayoutData> Prepared = Run->GetPreparedLayout(Level))
	{
		return Prepared;
	}
	if (Run->IsLayoutPending(Level))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("ARRArenaGrid %s: layout of level %d still in preparation at build, waiting for it."), *GetName(), Level);
		if (TSharedPtr<const FRRLayoutData> Prepared = Run->WaitForPreparedLayout(Level))
		{
			return Prepared;
		}
	}
	// Provisional decision (task-206-03): a layout that failed to load falls back to the grid's own LayoutName, logged as an error.
	UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid %s: no prepared layout for run level %d, falling back to %s."), *GetName(), Level, *LayoutName);
	return nullptr;
}

void ARRArenaGrid::ApplyLayoutData(const FRRLayoutData& Layout)
{
	Cells = Layout.Cells;
	GridWidth = Layout.Width;
	GridHeight = Layout.Height;
	SpawnCell = Layout.SpawnCell;
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

UInstancedStaticMeshComponent* ARRArenaGrid::CreateChunkComponent(TSubclassOf<UInstancedStaticMeshComponent> Class, const UInstancedStaticMeshComponent* Template, FIntPoint Chunk, bool bRegister)
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
	Component->bCastDynamicShadow = Template->bCastDynamicShadow;
	Component->bCastContactShadow = Template->bCastContactShadow;
	// Shadow casters (task-263-01): the player's light is the only shadow-casting light; blocks cast per bBlocksCastShadows, the
	// floor never casts (it receives). Collision templates (hidden, no shadow) keep their own flags.
	if (Template == FloorInstances)
	{
		Component->SetCastShadow(false);
		Component->bCastDynamicShadow = false;
	}
	else if (Template == RockInstances || Template == OreInstances || Template == HealOreInstances
		|| Template == SpecialOreInstances || Template == BarrelInstances)
	{
		Component->SetCastShadow(bBlocksCastShadows);
		Component->bCastDynamicShadow = bBlocksCastShadows;
	}
	// Ore and heal ore crystals (per-cell cluster chunks and deposit formation ISMs, both built from these templates) cast no
	// shadow at all unless bDepositCrystalsCastShadow (task-237-01). Rock pieces, barrels and collision ISMs are untouched.
	if (!bDepositCrystalsCastShadow && (Template == GetTemplateComponent(ERRCellType::Ore) || Template == GetTemplateComponent(ERRCellType::HealOre)))
	{
		Component->SetCastShadow(false);
		Component->bCastDynamicShadow = false;
		Component->bCastStaticShadow = false;
		Component->bCastContactShadow = false;
	}
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
	if (bRegister && GetWorld())
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
	if (!Visual.Render && !Visual.Collision)
	{
		Visual.MapKey = MapKey;
		Visual.Chunk = FIntPoint(Cell.X / ChunkCells, Cell.Y / ChunkCells);
		// Rock and special ore (task-230-07) have no per-cell rendering instance: the dual-grid pieces draw them; collision only.
		// Ore and heal ore get one crystal cluster per cell (task-230-08), instance for instance with their collision.
		if (HasCellRenderInstance(MapKey))
		{
			Visual.Render = CastChecked<UHierarchicalInstancedStaticMeshComponent>(
				CreateChunkComponent(UHierarchicalInstancedStaticMeshComponent::StaticClass(), Template, Visual.Chunk));
			VisualByComponent.Add(Visual.Render, VisualIndex);
		}
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
	for (FRRGridPieceVisual& Visual : PieceVisuals)
	{
		if (IsValid(Visual.Render))
		{
			Visual.Render->DestroyComponent();
		}
	}
	PieceVisuals.Reset();
	for (FRRGridDepositVisual& Visual : DepositVisuals)
	{
		if (IsValid(Visual.Render))
		{
			Visual.Render->DestroyComponent();
		}
	}
	DepositVisuals.Reset();
	DepositCrystalVisual.Reset();
	DepositCrystalInstance.Reset();
	CornerPieceVisual.Reset();
	CornerPieceInstance.Reset();
	CornerBroken.Reset();
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

FTransform ARRArenaGrid::GetCrystalTransform(FIntPoint Cell) const
{
	// Bottom of the mesh bounds on the block top at the cell centre; the yaw is a hash of the cell (random look, same on every build).
	const float Scale = FMath::Max(0.01f, OreCrystalScale);
	float Z = BlockHeight;
	if (OreCrystalMesh)
	{
		Z -= Scale * OreCrystalMesh->GetBoundingBox().Min.Z;
	}
	const uint32 Hash = RRArenaGridPrivate::MixHash(RRArenaGridPrivate::MixHash(static_cast<uint32>(Cell.X) * 0x85ebca6bu + static_cast<uint32>(RockPieceSeed)) ^ (static_cast<uint32>(Cell.Y) * 0xc2b2ae35u));
	const float Yaw = static_cast<float>(Hash % 360u);
	return FTransform(FRotator(0.f, Yaw, 0.f), FVector((Cell.X + 0.5f) * CellSize, (Cell.Y + 0.5f) * CellSize, Z), FVector(Scale));
}

FTransform ARRArenaGrid::GetBlockRenderTransform(FIntPoint Cell) const
{
	// Ore and heal ore draw their crystal cluster (task-230-08).
	if (IsValidCell(Cell) && (Cells[GetCellIndex(Cell)] == ERRCellType::Ore || Cells[GetCellIndex(Cell)] == ERRCellType::HealOre))
	{
		return GetCrystalTransform(Cell);
	}
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
		/** Mesh of the template HISM (rendering). */
		UStaticMesh* Mesh;
		UMaterialInterface* Material;
		/** Collision shape (ore and heal ore: the crystal draws, the block mesh collides). */
		UStaticMesh* CollisionMesh;
		/** True when Material applies to every slot of Mesh (crystal clusters have two); else slot 0 only. */
		bool bAllSlots;
	};
	// Ore and heal ore render their crystal cluster (task-230-08); their block meshes only shape the collision ISMs.
	const FVisualAsset Assets[] = {
		{ ERRCellType::Floor, FloorMesh, FloorMaterial, FloorMesh, false },
		{ ERRCellType::Rock, RockMesh, RockMaterial, RockMesh, false },
		{ ERRCellType::Ore, OreCrystalMesh, OreCrystalMaterial, OreMesh, true },
		{ ERRCellType::HealOre, OreCrystalMesh, HealOreCrystalMaterial, HealOreMesh, true },
		{ ERRCellType::SpecialOre, SpecialOreMesh, SpecialOreMaterial, SpecialOreMesh, false },
		{ ERRCellType::Barrel, BarrelMesh, BarrelMaterial, BarrelMesh, false },
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
			if (Asset.bAllSlots)
			{
				// Null keeps the mesh materials; otherwise every slot gets the material.
				Template->EmptyOverrideMaterials();
				for (int32 Slot = 0; Asset.Material && Asset.Mesh && Slot < Asset.Mesh->GetStaticMaterials().Num(); ++Slot)
				{
					Template->SetMaterial(Slot, Asset.Material);
				}
			}
			else
			{
				Template->SetMaterial(0, Asset.Material);
			}
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
				// Room for the fuse progress on barrels (task-197-01, material blink).
				if (Asset.MapKey == ERRCellType::Barrel && (BarrelFuseCustomDataIndex == MarkedCustomDataIndex
					|| (ColoredTypes.Contains(Asset.MapKey) && BarrelFuseCustomDataIndex >= OreColorCustomDataIndex && BarrelFuseCustomDataIndex < OreColorCustomDataIndex + 3)))
				{
					UE_LOG(LogRogueRigs, Warning, TEXT("ARRArenaGrid: BarrelFuseCustomDataIndex %d overlaps the marked or ore colour floats."), BarrelFuseCustomDataIndex);
				}
				if (Asset.MapKey == ERRCellType::Barrel && Template->NumCustomDataFloats <= BarrelFuseCustomDataIndex)
				{
					Template->SetNumCustomDataFloats(BarrelFuseCustomDataIndex + 1);
				}
				// Room for the crystal pulse phase and rate (task-257-02) on ore and heal ore: per-cell clusters and deposit formations copy it.
				if ((Asset.MapKey == ERRCellType::Ore || Asset.MapKey == ERRCellType::HealOre) && Template->NumCustomDataFloats < GetCrystalPulseFloatCount())
				{
					Template->SetNumCustomDataFloats(GetCrystalPulseFloatCount());
				}
			}
		}
		if (UInstancedStaticMeshComponent* CollisionTemplate = GetTemplateCollisionComponent(Asset.MapKey))
		{
			// Same mesh, so the collision shape matches the block at rest. Barrels: the decorative BarrelMesh (no simple collision)
			// never drives collision; BarrelCollisionMesh keeps the block-sized shape.
			UStaticMesh* CollisionMesh = Asset.CollisionMesh;
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

			// Floor slab under every non-void cell (task-230-07): rock pieces and barrels stand on the floor top, so a rounded
			// rock outline never shows the void and a destruction adds no floor.
			const bool bIsBlock = IsBlockType(Type) && GetTemplateComponent(Type) != nullptr;
			if (HasFloorSlab(Type))
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
		if (!Visual || (!Visual->Render && !Visual->Collision))
		{
			continue;
		}

		// Rock piece types have the collision ISM only; Render and Collision, when both exist, hold the same instances.
		const int32 FirstIndex = Visual->InstanceCells.Num();
		if (Visual->Render)
		{
			Visual->Render->AddInstances(Batch.Transforms, /*bShouldReturnIndices*/ false, /*bWorldSpace*/ false);
		}
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

	BuildCornerPieces();
	WriteDepositColors();
	WriteCellCrystalPulseData();
	BuildDepositCrystals();
}

void ARRArenaGrid::BuildCornerPieces()
{
	BuildPieceTable();

	PieceVariantSlots = 1;
	bool bAnyMeshMissing = false;
	for (int32 SetIndex = 0; SetIndex < RockPieceSetCount; ++SetIndex)
	{
		for (int32 ShapeIndex = 0; ShapeIndex < RockPieceShapeCount; ++ShapeIndex)
		{
			const ERRRockPieceSet Set = static_cast<ERRRockPieceSet>(SetIndex);
			const ERRRockPieceShape Shape = static_cast<ERRRockPieceShape>(ShapeIndex);
			const int32 Count = GetPieceVariantCount(Set, Shape);
			PieceVariantSlots = FMath::Max(PieceVariantSlots, Count);
			for (int32 Variant = 0; Variant < Count; ++Variant)
			{
				bAnyMeshMissing |= GetPieceMesh(Set, Shape, Variant) == nullptr;
			}
		}
	}
	// Log, not warning: until the meshes are assigned (task-230-11) every build would warn, in each test too.
	if (bAnyMeshMissing && GridWidth > 0 && GetWorld() && GetWorld()->IsGameWorld())
	{
		UE_LOG(LogRogueRigs, Log, TEXT("ARRArenaGrid %s: some rock piece meshes are not set (RockPieceMeshes, RockPieceBrokenMeshes): those pieces are placed but not drawn."), *GetName());
	}

	const FIntPoint Corners(GridWidth + 1, GridHeight + 1);
	const int32 CornerTotal = GridWidth > 0 && GridHeight > 0 ? Corners.X * Corners.Y : 0;
	CornerPieceVisual.Init(INDEX_NONE, CornerTotal);
	CornerPieceInstance.Init(INDEX_NONE, CornerTotal);
	CornerBroken.Init(false, CornerTotal);
	PieceVisuals.SetNum(RockPieceSetCount * RockPieceShapeCount * PieceVariantSlots * ChunkCount.X * ChunkCount.Y);

	// Gather per piece visual, then add in one batch per ISM (AddInstances appends: instance i of a batch gets count + i).
	TArray<TArray<FTransform>> Transforms;
	TArray<TArray<int32>> BatchCorners;
	Transforms.SetNum(PieceVisuals.Num());
	BatchCorners.SetNum(PieceVisuals.Num());
	for (int32 CornerIndex = 0; CornerIndex < CornerTotal; ++CornerIndex)
	{
		const FIntPoint Corner = GetCornerFromIndex(CornerIndex);
		const uint8 Mask = GetCornerMask(Corner);
		if (Mask == 0)
		{
			continue;
		}
		const ERRRockPieceShape Shape = PieceShapeByMask[Mask];
		const int32 VisualIndex = GetPieceVisualIndex(ERRRockPieceSet::Intact, Shape, GetPieceVariant(Corner, Shape), Corner);
		if (VisualIndex == INDEX_NONE)
		{
			continue;
		}
		Transforms[VisualIndex].Add(GetCornerPieceTransform(Corner, GetPieceTurns(Corner, Mask)));
		BatchCorners[VisualIndex].Add(CornerIndex);
	}
	for (int32 VisualIndex = 0; VisualIndex < PieceVisuals.Num(); ++VisualIndex)
	{
		if (Transforms[VisualIndex].Num() == 0)
		{
			continue;
		}
		FRRGridPieceVisual* Visual = GetOrCreatePieceVisual(VisualIndex);
		if (!Visual || !Visual->Render)
		{
			continue;
		}
		const int32 FirstIndex = Visual->InstanceCorners.Num();
		Visual->Render->AddInstances(Transforms[VisualIndex], /*bShouldReturnIndices*/ false, /*bWorldSpace*/ false);
		for (int32 Offset = 0; Offset < BatchCorners[VisualIndex].Num(); ++Offset)
		{
			const int32 CornerIndex = BatchCorners[VisualIndex][Offset];
			Visual->InstanceCorners.Add(CornerIndex);
			CornerPieceVisual[CornerIndex] = VisualIndex;
			CornerPieceInstance[CornerIndex] = FirstIndex + Offset;
			// Custom data at build (task-230-08): no damage, shake or mark yet, only the solid mask is not 0.
			float Data[PieceCustomDataCount];
			ComputeCornerPieceData(GetCornerFromIndex(CornerIndex), Data);
			Visual->Render->SetCustomData(FirstIndex + Offset, MakeArrayView(Data), /*bMarkRenderStateDirty*/ false);
		}
		Visual->Render->MarkRenderStateDirty();
	}

	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		int32 PieceCount = 0;
		int32 PieceComponents = 0;
		for (const FRRGridPieceVisual& Visual : PieceVisuals)
		{
			PieceCount += Visual.InstanceCorners.Num();
			PieceComponents += Visual.Render ? 1 : 0;
		}
		UE_LOG(LogRogueRigs, Log, TEXT("ARRArenaGrid %s: %d rock pieces in %d piece ISMs (%d variant slot(s) per shape)."), *GetName(), PieceCount, PieceComponents, PieceVariantSlots);
	}
}

bool ARRArenaGrid::IsRockPieceType(ERRCellType Type)
{
	return Type == ERRCellType::Rock || Type == ERRCellType::Ore || Type == ERRCellType::HealOre || Type == ERRCellType::SpecialOre;
}

bool ARRArenaGrid::IsRockPieceCell(FIntPoint Cell) const
{
	return IsValidCell(Cell) && IsRockPieceType(Cells[GetCellIndex(Cell)]);
}

int32 ARRArenaGrid::GetCornerIndex(FIntPoint Corner) const
{
	if (Corner.X < 0 || Corner.Y < 0 || Corner.X > GridWidth || Corner.Y > GridHeight)
	{
		return INDEX_NONE;
	}
	const int32 Index = Corner.Y * (GridWidth + 1) + Corner.X;
	return CornerPieceVisual.IsValidIndex(Index) ? Index : INDEX_NONE;
}

uint8 ARRArenaGrid::GetCornerMask(FIntPoint Corner) const
{
	// Bit = (+X side) | (+Y side) << 1: the cell on the +X side of the corner is (x, .), on the -X side (x - 1, .).
	uint8 Mask = 0;
	for (int32 Bit = 0; Bit < 4; ++Bit)
	{
		const FIntPoint Cell(Corner.X - ((Bit & 1) ? 0 : 1), Corner.Y - ((Bit & 2) ? 0 : 1));
		Mask |= IsRockPieceCell(Cell) ? static_cast<uint8>(1u << Bit) : 0;
	}
	return Mask;
}

void ARRArenaGrid::BuildPieceTable()
{
	// Canonical rock corners per shape in Blender space (SM_RockSlab.report.json): A (-,-), B (+,-), C (+,+), D (-,+).
	struct FCanonical
	{
		ERRRockPieceShape Shape;
		TArray<FIntPoint> Quadrants;
	};
	const FIntPoint A(-1, -1), B(1, -1), C(1, 1), D(-1, 1);
	const FCanonical Canonicals[] = {
		{ ERRRockPieceShape::Full, { A, B, C, D } },
		{ ERRRockPieceShape::Edge, { B, C } },
		{ ERRRockPieceShape::Outer, { C } },
		{ ERRRockPieceShape::Inner, { B, C, D } },
		{ ERRRockPieceShape::Diagonal, { A, C } },
	};
	bool bFilled[16] = {};
	for (const FCanonical& Canonical : Canonicals)
	{
		for (int32 Turns = 0; Turns < 4; ++Turns)
		{
			// Unreal yaw rotates +X toward +Y; the FBX import mirrors Blender Y (bRockPieceImportFlipsY).
			const FRotator Rotation(0.f, 90.f * Turns, 0.f);
			uint8 Mask = 0;
			for (const FIntPoint& Quadrant : Canonical.Quadrants)
			{
				const FVector Local(Quadrant.X, bRockPieceImportFlipsY ? -Quadrant.Y : Quadrant.Y, 0.f);
				const FVector Rotated = Rotation.RotateVector(Local);
				const int32 Bit = (Rotated.X > 0.0 ? 1 : 0) | (Rotated.Y > 0.0 ? 2 : 0);
				Mask |= static_cast<uint8>(1u << Bit);
			}
			if (!bFilled[Mask])
			{
				bFilled[Mask] = true;
				PieceShapeByMask[Mask] = Canonical.Shape;
				PieceTurnsByMask[Mask] = static_cast<uint8>(Turns);
			}
		}
	}
	for (int32 Mask = 1; Mask < 16; ++Mask)
	{
		ensureMsgf(bFilled[Mask], TEXT("ARRArenaGrid: rock piece mask %d has no shape."), Mask);
	}
}

int32 ARRArenaGrid::GetPieceVariantCount(ERRRockPieceSet Set, ERRRockPieceShape Shape) const
{
	return FMath::Max(1, GetPieceMeshSet(Set).GetMeshes(Shape).Num());
}

UStaticMesh* ARRArenaGrid::GetPieceMesh(ERRRockPieceSet Set, ERRRockPieceShape Shape, int32 Variant) const
{
	const TArray<TObjectPtr<UStaticMesh>>& Meshes = GetPieceMeshSet(Set).GetMeshes(Shape);
	return Meshes.IsValidIndex(Variant) ? Meshes[Variant].Get() : nullptr;
}

int32 ARRArenaGrid::GetPieceVariant(FIntPoint Corner, ERRRockPieceShape Shape, ERRRockPieceSet Set) const
{
	const int32 Count = GetPieceVariantCount(Set, Shape);
	if (Count <= 1)
	{
		return 0;
	}
	const uint32 Hash = RRArenaGridPrivate::MixHash(RRArenaGridPrivate::MixHash(static_cast<uint32>(RockPieceSeed) + static_cast<uint32>(Corner.X)) + static_cast<uint32>(Corner.Y));
	return static_cast<int32>(Hash % static_cast<uint32>(Count));
}

int32 ARRArenaGrid::GetPieceTurns(FIntPoint Corner, uint8 Mask) const
{
	if (PieceShapeByMask[Mask & 15] != ERRRockPieceShape::Full)
	{
		return PieceTurnsByMask[Mask & 15];
	}
	// Random rotation only on the full piece (its top has no outline): a second hash of the corner, independent of the variant.
	const uint32 Hash = RRArenaGridPrivate::MixHash(RRArenaGridPrivate::MixHash(static_cast<uint32>(Corner.Y) * 0x9e3779b9u + static_cast<uint32>(RockPieceSeed)) ^ static_cast<uint32>(Corner.X));
	return static_cast<int32>(Hash & 3u);
}

FTransform ARRArenaGrid::GetCornerPieceTransform(FIntPoint Corner, int32 Turns) const
{
	// Piece pivot = cell corner on the floor top; footprint one cell, top at BlockHeight.
	const float HorizontalScale = CellSize / RockPieceMeshSize;
	const FVector Location(Corner.X * CellSize, Corner.Y * CellSize, 0.f);
	return FTransform(FRotator(0.f, 90.f * (Turns & 3), 0.f), Location, FVector(HorizontalScale, HorizontalScale, BlockHeight / RockPieceMeshHeight));
}

int32 ARRArenaGrid::GetPieceVisualIndex(ERRRockPieceSet Set, ERRRockPieceShape Shape, int32 Variant, FIntPoint Corner) const
{
	const int32 SetIndex = static_cast<int32>(Set);
	const int32 ShapeIndex = static_cast<int32>(Shape);
	if (ChunkCells <= 0 || ChunkCount.X <= 0 || ChunkCount.Y <= 0 || Variant < 0 || Variant >= PieceVariantSlots
		|| SetIndex >= RockPieceSetCount || ShapeIndex >= RockPieceShapeCount || GetCornerIndex(Corner) == INDEX_NONE)
	{
		return INDEX_NONE;
	}
	// A corner on a chunk line belongs to the higher chunk; the last corner line belongs to the last chunk.
	const int32 ChunkX = FMath::Min(Corner.X / ChunkCells, ChunkCount.X - 1);
	const int32 ChunkY = FMath::Min(Corner.Y / ChunkCells, ChunkCount.Y - 1);
	const int32 Slot = (SetIndex * RockPieceShapeCount + ShapeIndex) * PieceVariantSlots + Variant;
	const int32 Index = Slot * ChunkCount.X * ChunkCount.Y + ChunkY * ChunkCount.X + ChunkX;
	return PieceVisuals.IsValidIndex(Index) ? Index : INDEX_NONE;
}

FRRGridPieceVisual* ARRArenaGrid::GetOrCreatePieceVisual(int32 VisualIndex)
{
	if (!PieceVisuals.IsValidIndex(VisualIndex) || !RockInstances)
	{
		return nullptr;
	}
	FRRGridPieceVisual& Visual = PieceVisuals[VisualIndex];
	if (Visual.Render)
	{
		return &Visual;
	}
	const int32 ChunksPerSlot = ChunkCount.X * ChunkCount.Y;
	const int32 Slot = VisualIndex / ChunksPerSlot;
	const int32 ChunkIndex = VisualIndex % ChunksPerSlot;
	Visual.Variant = Slot % PieceVariantSlots;
	Visual.Shape = static_cast<ERRRockPieceShape>((Slot / PieceVariantSlots) % RockPieceShapeCount);
	Visual.Set = static_cast<ERRRockPieceSet>(Slot / (PieceVariantSlots * RockPieceShapeCount));
	Visual.Chunk = FIntPoint(ChunkIndex % ChunkCount.X, ChunkIndex / ChunkCount.X);

	// RockInstances is the template (shadow, distance field, visibility); mesh and material are the piece's own.
	// Plain ISM, not a HISM (task-230-14): pieces are removed and added every dig, and a HISM removal leaves an unmapped render slot
	// with uninitialized instance data until its async tree rebuild; the distance field and Lumen card scenes read every slot
	// (precision ensure in UpdateDistanceFieldObjectBuffers, LumenMeshCards.cpp assert). The ISM tracks instances by id, no slot gap.
	UInstancedStaticMeshComponent* Component = CreateChunkComponent(UInstancedStaticMeshComponent::StaticClass(), RockInstances, Visual.Chunk, /*bRegister*/ false);
	// The corner maps assume remove at swap (the last instance takes the removed index).
	Component->SetRemoveSwap();
	Component->EmptyOverrideMaterials();
	Component->SetStaticMesh(GetPieceMesh(Visual.Set, Visual.Shape, Visual.Variant));
	UMaterialInterface* Material = Visual.Set == ERRRockPieceSet::Broken && RockPieceBrokenMaterial ? RockPieceBrokenMaterial.Get() : RockPieceMaterial.Get();
	if (Material)
	{
		Component->SetMaterial(0, Material);
	}
	// Damage, shake, marked and solid mask per quadrant for the piece material (task-230-08); set while the component is empty.
	Component->SetNumCustomDataFloats(PieceCustomDataCount);
	// Rendering only: collision stays one cube per cell in the collision ISMs.
	Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Component->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	// Nanite (owner 2026-10-17): drawn through Nanite when the mesh has Nanite data; never forced to the fallback mesh here.
	// Broken pieces off Nanite by default (task-230-17): their sag is world position offset, not drawn under Nanite in game.
	Component->bDisallowNanite = !bRockPiecesUseNanite || (Visual.Set == ERRRockPieceSet::Broken && !bBrokenRockPiecesUseNanite);
	Component->SetForceDisableNanite(false);
	if (GetWorld())
	{
		Component->RegisterComponent();
	}
	Visual.Render = Component;
	return &Visual;
}

void ARRArenaGrid::RemoveCornerPiece(int32 CornerIndex)
{
	if (!CornerPieceVisual.IsValidIndex(CornerIndex) || CornerPieceVisual[CornerIndex] == INDEX_NONE)
	{
		return;
	}
	FRRGridPieceVisual& Visual = PieceVisuals[CornerPieceVisual[CornerIndex]];
	const int32 InstanceIndex = CornerPieceInstance[CornerIndex];
	CornerPieceVisual[CornerIndex] = INDEX_NONE;
	CornerPieceInstance[CornerIndex] = INDEX_NONE;
	if (!Visual.Render || !Visual.InstanceCorners.IsValidIndex(InstanceIndex))
	{
		return;
	}
	const int32 LastIndex = Visual.InstanceCorners.Num() - 1;
	if (!Visual.Render->RemoveInstance(InstanceIndex))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: failed to remove rock piece instance %d of corner %d."), InstanceIndex, CornerIndex);
		return;
	}
	// HISM removes at swap: the last instance now occupies InstanceIndex.
	Visual.InstanceCorners.RemoveAtSwap(InstanceIndex, EAllowShrinking::No);
	if (InstanceIndex != LastIndex)
	{
		CornerPieceInstance[Visual.InstanceCorners[InstanceIndex]] = InstanceIndex;
	}
}

void ARRArenaGrid::RefreshCornerPiece(FIntPoint Corner)
{
	const int32 CornerIndex = GetCornerIndex(Corner);
	if (CornerIndex == INDEX_NONE)
	{
		return;
	}
	const uint8 Mask = GetCornerMask(Corner);
	int32 NewVisualIndex = INDEX_NONE;
	if (Mask != 0)
	{
		// Broken set once a cell of the corner was damaged (task-230-08), for good.
		const ERRRockPieceSet Set = CornerBroken.IsValidIndex(CornerIndex) && CornerBroken[CornerIndex] ? ERRRockPieceSet::Broken : ERRRockPieceSet::Intact;
		const ERRRockPieceShape Shape = PieceShapeByMask[Mask];
		NewVisualIndex = GetPieceVisualIndex(Set, Shape, GetPieceVariant(Corner, Shape, Set), Corner);
	}
	const int32 OldVisualIndex = CornerPieceVisual[CornerIndex];

	// Same HISM (same set, shape and variant, another rotation): a transform update is enough.
	if (OldVisualIndex != INDEX_NONE && OldVisualIndex == NewVisualIndex)
	{
		FRRGridPieceVisual& Visual = PieceVisuals[OldVisualIndex];
		if (Visual.Render)
		{
			Visual.Render->UpdateInstanceTransform(CornerPieceInstance[CornerIndex], GetCornerPieceTransform(Corner, GetPieceTurns(Corner, Mask)),
				/*bWorldSpace*/ false, /*bMarkRenderStateDirty*/ true, /*bTeleport*/ true);
		}
		WriteCornerPieceData(CornerIndex);
		return;
	}
	RemoveCornerPiece(CornerIndex);
	if (NewVisualIndex == INDEX_NONE)
	{
		return;
	}
	FRRGridPieceVisual* Visual = GetOrCreatePieceVisual(NewVisualIndex);
	if (!Visual || !Visual->Render)
	{
		return;
	}
	const int32 InstanceIndex = Visual->Render->AddInstance(GetCornerPieceTransform(Corner, GetPieceTurns(Corner, Mask)), /*bWorldSpace*/ false);
	if (InstanceIndex == INDEX_NONE)
	{
		return;
	}
	// Recreate the proxy at the end of the frame (one recreation per ISM per frame; the custom data write below dirties it anyway).
	Visual->Render->MarkRenderStateDirty();
	if (Visual->InstanceCorners.Num() <= InstanceIndex)
	{
		Visual->InstanceCorners.SetNum(InstanceIndex + 1);
	}
	Visual->InstanceCorners[InstanceIndex] = CornerIndex;
	CornerPieceVisual[CornerIndex] = NewVisualIndex;
	CornerPieceInstance[CornerIndex] = InstanceIndex;
	// A new instance starts with zeroed custom data.
	WriteCornerPieceData(CornerIndex);
}

void ARRArenaGrid::UpdateCellCornerPieces(FIntPoint Cell)
{
	// The 4 corners of a cell: (x, y), (x + 1, y), (x, y + 1), (x + 1, y + 1).
	for (int32 DY = 0; DY <= 1; ++DY)
	{
		for (int32 DX = 0; DX <= 1; ++DX)
		{
			RefreshCornerPiece(FIntPoint(Cell.X + DX, Cell.Y + DY));
		}
	}
}

float ARRArenaGrid::GetSolidCellDamage(FIntPoint Cell) const
{
	if (!IsRockPieceCell(Cell))
	{
		return 0.f;
	}
	const int32 Index = GetCellIndex(Cell);
	const float MaxHealth = GetBlockMaxHealth(Cells[Index]);
	if (MaxHealth <= 0.f || !CellHealth.IsValidIndex(Index))
	{
		return 0.f;
	}
	return FMath::Clamp(1.f - CellHealth[Index] / MaxHealth, 0.f, 1.f);
}

float ARRArenaGrid::GetCellDisplayDamage(FIntPoint Cell) const
{
	if (IsRockPieceCell(Cell))
	{
		return GetSolidCellDamage(Cell);
	}
	// Empty cell (provisional, rock-render-damage): the largest damage of its solid 4-neighbours, as in the task-230-06 previews.
	const FIntPoint Steps[] = { FIntPoint(1, 0), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(0, -1) };
	float Damage = 0.f;
	for (const FIntPoint& Step : Steps)
	{
		Damage = FMath::Max(Damage, GetSolidCellDamage(Cell + Step));
	}
	return Damage;
}

void ARRArenaGrid::ComputeCornerPieceData(FIntPoint Corner, float (&OutData)[PieceCustomDataCount]) const
{
	FMemory::Memzero(OutData);
	int32 Mask = 0;
	for (int32 Quadrant = 0; Quadrant < 4; ++Quadrant)
	{
		// Quadrant = (+X side) + 2 * (+Y side), the bit order of GetCornerMask.
		const FIntPoint Cell(Corner.X - ((Quadrant & 1) ? 0 : 1), Corner.Y - ((Quadrant & 2) ? 0 : 1));
		OutData[PieceDamageCustomDataIndex + Quadrant] = GetCellDisplayDamage(Cell);
		if (!IsRockPieceCell(Cell))
		{
			continue;
		}
		Mask |= 1 << Quadrant;
		if (const float* Phase = ShakingCells.Find(Cell))
		{
			// 1 + phase fraction: the material shakes the quadrant with the same per-cell phase as the crystal jitter.
			OutData[PieceShakeCustomDataIndex + Quadrant] = 1.f + FMath::Clamp(*Phase / UE_TWO_PI, 0.f, 0.9999f);
		}
		if (MarkedCells.Contains(Cell))
		{
			OutData[PieceMarkedCustomDataIndex + Quadrant] = MarkedCustomDataValue;
		}
	}
	OutData[PieceSolidMaskCustomDataIndex] = static_cast<float>(Mask);
}

void ARRArenaGrid::WriteCornerPieceData(int32 CornerIndex)
{
	if (!CornerPieceVisual.IsValidIndex(CornerIndex) || CornerPieceVisual[CornerIndex] == INDEX_NONE)
	{
		return;
	}
	UInstancedStaticMeshComponent* Component = PieceVisuals[CornerPieceVisual[CornerIndex]].Render;
	const int32 Instance = CornerPieceInstance[CornerIndex];
	if (!Component || Component->NumCustomDataFloats < PieceCustomDataCount || Instance == INDEX_NONE)
	{
		return;
	}
	float Data[PieceCustomDataCount];
	ComputeCornerPieceData(GetCornerFromIndex(CornerIndex), Data);
	const int32 First = Instance * Component->NumCustomDataFloats;
	if (!Component->PerInstanceSMCustomData.IsValidIndex(First + PieceCustomDataCount - 1))
	{
		return;
	}
	if (FMemory::Memcmp(&Component->PerInstanceSMCustomData[First], Data, sizeof(Data)) == 0)
	{
		return;
	}
	// One proxy recreation per touched HISM per frame (render state dirty is per component and per frame).
	Component->SetCustomData(Instance, MakeArrayView(Data), /*bMarkRenderStateDirty*/ true);
}

void ARRArenaGrid::RefreshCellCornerPieceData(FIntPoint Cell)
{
	for (int32 DY = 0; DY <= 1; ++DY)
	{
		for (int32 DX = 0; DX <= 1; ++DX)
		{
			WriteCornerPieceData(GetCornerIndex(FIntPoint(Cell.X + DX, Cell.Y + DY)));
		}
	}
}

void ARRArenaGrid::RefreshPieceDataAround(FIntPoint Cell)
{
	// Pieces holding Cell (corners x .. x + 1) or a 4-neighbour of it (one more corner on each side): a neighbour that is empty
	// derives its display damage from Cell.
	for (int32 Y = Cell.Y - 1; Y <= Cell.Y + 2; ++Y)
	{
		for (int32 X = Cell.X - 1; X <= Cell.X + 2; ++X)
		{
			WriteCornerPieceData(GetCornerIndex(FIntPoint(X, Y)));
		}
	}
}

void ARRArenaGrid::BreakCellCornerPieces(FIntPoint Cell)
{
	if (GetSolidCellDamage(Cell) <= 0.f)
	{
		return;
	}
	for (int32 DY = 0; DY <= 1; ++DY)
	{
		for (int32 DX = 0; DX <= 1; ++DX)
		{
			const FIntPoint Corner(Cell.X + DX, Cell.Y + DY);
			const int32 CornerIndex = GetCornerIndex(Corner);
			if (CornerIndex == INDEX_NONE || !CornerBroken.IsValidIndex(CornerIndex) || CornerBroken[CornerIndex])
			{
				continue;
			}
			CornerBroken[CornerIndex] = true;
			RefreshCornerPiece(Corner);
		}
	}
}

bool ARRArenaGrid::HasCellRenderInstance(ERRCellType Type) const
{
	if (Type == ERRCellType::Ore || Type == ERRCellType::HealOre)
	{
		// Per-cell crystal clusters only while the type has no deposit meshes (task-234-03).
		return !UsesDepositCrystals(Type);
	}
	return !IsRockPieceType(Type);
}

void ARRArenaGrid::GetDepositMeshes(ERRCellType Type, TArray<UStaticMesh*>& OutMeshes) const
{
	OutMeshes.Reset();
	const TArray<TObjectPtr<UStaticMesh>>* List = Type == ERRCellType::Ore ? &OreDepositMeshes
		: (Type == ERRCellType::HealOre ? &HealDepositMeshes : nullptr);
	if (!List)
	{
		return;
	}
	for (UStaticMesh* Mesh : *List)
	{
		if (Mesh)
		{
			OutMeshes.Add(Mesh);
		}
	}
}

bool ARRArenaGrid::UsesDepositCrystals(ERRCellType Type) const
{
	const TArray<TObjectPtr<UStaticMesh>>* List = Type == ERRCellType::Ore ? &OreDepositMeshes
		: (Type == ERRCellType::HealOre ? &HealDepositMeshes : nullptr);
	return List && List->ContainsByPredicate([](const TObjectPtr<UStaticMesh>& Mesh) { return Mesh != nullptr; });
}

UMaterialInterface* ARRArenaGrid::GetDepositMaterial(ERRCellType Type) const
{
	return Type == ERRCellType::HealOre ? HealDepositMaterial.Get() : OreDepositMaterial.Get();
}

uint32 ARRArenaGrid::GetDepositCrystalHash(int32 DepositId) const
{
	return RRArenaGridPrivate::MixHash(static_cast<uint32>(DepositId) * 0x9e3779b9u ^ (static_cast<uint32>(DepositCrystalSeed) * 0x85ebca6bu));
}

FTransform ARRArenaGrid::GetDepositCrystalTransform(int32 DepositId) const
{
	if (!Deposits.IsValidIndex(DepositId) || Deposits[DepositId].Cells.IsEmpty())
	{
		return FTransform::Identity;
	}
	// Centre of the cells' bounding box (a 2 x 2 deposit: its shared corner), on the block top: the mesh pivot is its footprint centre, base at z 0.
	FIntPoint Min = Deposits[DepositId].Cells[0];
	FIntPoint Max = Min;
	for (const FIntPoint& Cell : Deposits[DepositId].Cells)
	{
		Min = Min.ComponentMin(Cell);
		Max = Max.ComponentMax(Cell);
	}
	const FVector Location(0.5f * (Min.X + Max.X + 1) * CellSize, 0.5f * (Min.Y + Max.Y + 1) * CellSize, BlockHeight);
	// Yaw: a quarter turn hashed from the deposit (owner 2026-10-19: play with rotation), independent of the variant bits.
	const int32 Turns = static_cast<int32>(RRArenaGridPrivate::MixHash(GetDepositCrystalHash(DepositId) ^ 0x68e31da4u) % 4u);
	return FTransform(FRotator(0.f, 90.f * Turns, 0.f), Location, FVector(FMath::Max(0.01f, DepositCrystalScale)));
}

void ARRArenaGrid::BuildDepositCrystals()
{
	DepositCrystalVisual.Init(INDEX_NONE, Deposits.Num());
	DepositCrystalInstance.Init(INDEX_NONE, Deposits.Num());

	for (const ERRCellType Type : { ERRCellType::Ore, ERRCellType::HealOre })
	{
		TArray<UStaticMesh*> Meshes;
		GetDepositMeshes(Type, Meshes);
		UHierarchicalInstancedStaticMeshComponent* Template = GetTemplateComponent(Type);
		if (Meshes.IsEmpty() || !Template)
		{
			continue;
		}

		// Gather per variant, then add each batch at once: instance i of a batch gets index i (fresh ISM).
		TArray<TArray<int32>> BatchDeposits;
		BatchDeposits.SetNum(Meshes.Num());
		for (int32 DepositId = 0; DepositId < Deposits.Num(); ++DepositId)
		{
			const FRRGridDeposit& Deposit = Deposits[DepositId];
			if (Deposit.CellType != Type || Deposit.Cells.IsEmpty())
			{
				continue;
			}
			BatchDeposits[GetDepositCrystalHash(DepositId) % static_cast<uint32>(Meshes.Num())].Add(DepositId);
		}

		UMaterialInterface* Material = GetDepositMaterial(Type);
		for (int32 Variant = 0; Variant < Meshes.Num(); ++Variant)
		{
			if (BatchDeposits[Variant].IsEmpty())
			{
				continue;
			}
			// Template settings (shadow, visibility, distance fields, custom data floats sized for marked and ore colour), own mesh
			// and material. Plain ISM, remove at swap (task-230-14 lesson: no HISM render slot gap after a removal).
			UInstancedStaticMeshComponent* Component = CreateChunkComponent(UInstancedStaticMeshComponent::StaticClass(), Template, FIntPoint(Variant, 0), /*bRegister*/ false);
			Component->SetRemoveSwap();
			Component->SetStaticMesh(Meshes[Variant]);
			// The mesh keeps its own slot materials (set at import: one `crystal` slot for Duskite, `rock` and `crystal` for heal);
			// a set per-type material overrides the slot named DepositCrystalSlotName only.
			Component->EmptyOverrideMaterials();
			const TArray<FStaticMaterial>& Slots = Meshes[Variant]->GetStaticMaterials();
			for (int32 Slot = 0; Material && Slot < Slots.Num(); ++Slot)
			{
				if (Slots[Slot].MaterialSlotName == DepositCrystalSlotName)
				{
					Component->SetMaterial(Slot, Material);
				}
			}
			// Marked 0, ore colour 1-3 and the pulse phase and rate at DepositPulsePhase/RateCustomDataIndex (13, 14; task-267-01):
			// 15 floats. task-258-01: a mesh with a rock slot (heal deposit, MI_RockSlab) shares the data with M_RockSlab, which reads
			// 0-12 with the rock piece layout (4-7 = quadrant shake, 1 or more = shaking). 4-12 are never written here, so they stay 0
			// (rock still); the pulse sits beyond them. With indices below 13 such an ISM keeps marked and colour only (4 floats).
			const bool bPulseData = CanWriteDepositPulseData(Meshes[Variant]);
			const int32 BaseFloats = FMath::Max(MarkedCustomDataIndex + 1, OreColorCustomDataIndex + 3);
			Component->SetNumCustomDataFloats(bPulseData ? FMath::Max3(Template->NumCustomDataFloats, BaseFloats, GetDepositPulseFloatCount()) : BaseFloats);
			// Rendering only: collision stays one cube per cell in the collision ISMs.
			Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Component->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			if (GetWorld())
			{
				Component->RegisterComponent();
			}

			TArray<FTransform> Transforms;
			Transforms.Reserve(BatchDeposits[Variant].Num());
			for (const int32 DepositId : BatchDeposits[Variant])
			{
				Transforms.Add(GetDepositCrystalTransform(DepositId));
			}
			Component->AddInstances(Transforms, /*bShouldReturnIndices*/ false, /*bWorldSpace*/ false);

			const int32 VisualIndex = DepositVisuals.AddDefaulted();
			FRRGridDepositVisual& Visual = DepositVisuals[VisualIndex];
			Visual.CellType = Type;
			Visual.Variant = Variant;
			Visual.Render = Component;
			Visual.InstanceDeposits = BatchDeposits[Variant];
			Visual.bPulseData = bPulseData;
			for (int32 Instance = 0; Instance < Visual.InstanceDeposits.Num(); ++Instance)
			{
				DepositCrystalVisual[Visual.InstanceDeposits[Instance]] = VisualIndex;
				DepositCrystalInstance[Visual.InstanceDeposits[Instance]] = Instance;
				WriteDepositCrystalData(Visual.InstanceDeposits[Instance], /*bMarkDirty*/ false);
				// Pulse data written once: it follows the instance through remove-at-swap (the ISM swaps whole custom data blocks).
				if (bPulseData)
				{
					WriteCrystalPulseData(Component, Instance, FRRPulseDesync::DepositDomain, static_cast<uint32>(Visual.InstanceDeposits[Instance]),
						DepositPulsePhaseCustomDataIndex, DepositPulseRateCustomDataIndex);
				}
			}
			Component->MarkRenderStateDirty();
		}
	}
}

UInstancedStaticMeshComponent* ARRArenaGrid::GetDepositCrystalComponent(int32 DepositId, int32& OutInstance) const
{
	OutInstance = INDEX_NONE;
	if (!DepositCrystalVisual.IsValidIndex(DepositId) || DepositCrystalVisual[DepositId] == INDEX_NONE)
	{
		return nullptr;
	}
	UInstancedStaticMeshComponent* Component = DepositVisuals[DepositCrystalVisual[DepositId]].Render;
	OutInstance = Component ? DepositCrystalInstance[DepositId] : INDEX_NONE;
	return Component;
}

void ARRArenaGrid::RemoveDepositCrystal(int32 DepositId)
{
	if (!DepositCrystalVisual.IsValidIndex(DepositId) || DepositCrystalVisual[DepositId] == INDEX_NONE)
	{
		return;
	}
	FRRGridDepositVisual& Visual = DepositVisuals[DepositCrystalVisual[DepositId]];
	const int32 InstanceIndex = DepositCrystalInstance[DepositId];
	DepositCrystalVisual[DepositId] = INDEX_NONE;
	DepositCrystalInstance[DepositId] = INDEX_NONE;
	if (!Visual.Render || !Visual.InstanceDeposits.IsValidIndex(InstanceIndex))
	{
		return;
	}
	const int32 LastIndex = Visual.InstanceDeposits.Num() - 1;
	if (!Visual.Render->RemoveInstance(InstanceIndex))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARRArenaGrid: failed to remove the crystal instance %d of deposit %d."), InstanceIndex, DepositId);
		return;
	}
	// Remove at swap: the last instance now occupies InstanceIndex.
	Visual.InstanceDeposits.RemoveAtSwap(InstanceIndex, EAllowShrinking::No);
	if (InstanceIndex != LastIndex)
	{
		DepositCrystalInstance[Visual.InstanceDeposits[InstanceIndex]] = InstanceIndex;
	}
}

void ARRArenaGrid::WriteDepositCrystalData(int32 DepositId, bool bMarkDirty)
{
	int32 Instance = INDEX_NONE;
	UInstancedStaticMeshComponent* Component = GetDepositCrystalComponent(DepositId, Instance);
	if (!Component || Instance == INDEX_NONE)
	{
		return;
	}
	const FRRGridDeposit& Deposit = Deposits[DepositId];
	bool bMarked = false;
	for (const FIntPoint& Cell : Deposit.Cells)
	{
		bMarked |= GetDepositId(Cell) == DepositId && MarkedCells.Contains(Cell);
	}
	if (MarkedCustomDataIndex >= 0 && Component->NumCustomDataFloats > MarkedCustomDataIndex)
	{
		Component->SetCustomDataValue(Instance, MarkedCustomDataIndex, bMarked ? MarkedCustomDataValue : 0.f, /*bMarkRenderStateDirty*/ false);
	}
	if (Deposit.bHasColor && Component->NumCustomDataFloats >= OreColorCustomDataIndex + 3)
	{
		Component->SetCustomDataValue(Instance, OreColorCustomDataIndex, Deposit.Color.R, /*bMarkRenderStateDirty*/ false);
		Component->SetCustomDataValue(Instance, OreColorCustomDataIndex + 1, Deposit.Color.G, /*bMarkRenderStateDirty*/ false);
		Component->SetCustomDataValue(Instance, OreColorCustomDataIndex + 2, Deposit.Color.B, /*bMarkRenderStateDirty*/ false);
	}
	if (bMarkDirty)
	{
		Component->MarkRenderStateDirty();
	}
}

bool ARRArenaGrid::DepositMeshHasRockSlot(const UStaticMesh* Mesh) const
{
	if (!Mesh)
	{
		return false;
	}
	for (const FStaticMaterial& Slot : Mesh->GetStaticMaterials())
	{
		if (Slot.MaterialSlotName != DepositCrystalSlotName)
		{
			return true;
		}
	}
	return false;
}

bool ARRArenaGrid::CanWriteDepositPulseData(const UStaticMesh* Mesh) const
{
	return !DepositMeshHasRockSlot(Mesh) || FMath::Min(DepositPulsePhaseCustomDataIndex, DepositPulseRateCustomDataIndex) >= PieceCustomDataCount;
}

void ARRArenaGrid::WriteCrystalPulseData(UInstancedStaticMeshComponent* Component, int32 Instance, uint32 Domain, uint32 Id, int32 PhaseIndex, int32 RateIndex)
{
	if (!Component || Instance == INDEX_NONE || PhaseIndex < 0 || RateIndex < 0 || Component->NumCustomDataFloats <= FMath::Max(PhaseIndex, RateIndex))
	{
		return;
	}
	Component->SetCustomDataValue(Instance, PhaseIndex, FRRPulseDesync::Phase(PulseSeed, Domain, Id), /*bMarkRenderStateDirty*/ false);
	Component->SetCustomDataValue(Instance, RateIndex, FRRPulseDesync::Rate(PulseSeed, Domain, Id, PulseRateMin, PulseRateMax), /*bMarkRenderStateDirty*/ false);
}

void ARRArenaGrid::WriteCellCrystalPulseData()
{
	TSet<UHierarchicalInstancedStaticMeshComponent*> Dirty;
	for (int32 Index = 0; Index < Cells.Num(); ++Index)
	{
		const ERRCellType Type = Cells[Index];
		if ((Type != ERRCellType::Ore && Type != ERRCellType::HealOre) || CellInstanceIndex[Index] == INDEX_NONE)
		{
			continue;
		}
		const FIntPoint Cell(Index % GridWidth, Index / GridWidth);
		// Only per-cell clusters have a render HISM (types with deposit meshes have collision only: GetCellRenderComponent is null).
		UHierarchicalInstancedStaticMeshComponent* Component = GetCellRenderComponent(Cell);
		if (!Component)
		{
			continue;
		}
		// The cluster material (OreCrystalMaterial / HealOreCrystalMaterial, M_XPNutGlow children) covers every slot of the cluster
		// mesh: no rock slot reads this data, so the M_XPNutGlow indices 4 / 5 stay (task-267-01).
		WriteCrystalPulseData(Component, CellInstanceIndex[Index], FRRPulseDesync::CellDomain, static_cast<uint32>(Index), PulsePhaseCustomDataIndex, PulseRateCustomDataIndex);
		Dirty.Add(Component);
	}
	for (UHierarchicalInstancedStaticMeshComponent* Component : Dirty)
	{
		Component->MarkRenderStateDirty();
	}
}

bool ARRArenaGrid::IsDepositShaking(int32 DepositId) const
{
	if (!Deposits.IsValidIndex(DepositId))
	{
		return false;
	}
	for (const FIntPoint& Cell : Deposits[DepositId].Cells)
	{
		if (GetDepositId(Cell) == DepositId && ShakingCells.Contains(Cell))
		{
			return true;
		}
	}
	return false;
}

int32 ARRArenaGrid::AddCellInstance(ERRCellType MapKey, FIntPoint Cell, const FTransform& LocalTransform)
{
	FRRGridChunkVisual* Visual = GetOrCreateVisual(MapKey, Cell);
	if (!Visual || !Visual->Render)
	{
		// Only floor slabs are added at runtime (rock piece types never get a per-cell rendering instance).
		return INDEX_NONE;
	}

	// HISM appends: the returned index is the previous instance count. The collision ISM appends the same instance.
	const int32 InstanceIndex = Visual->Render->AddInstance(LocalTransform, /*bWorldSpace*/ false);
	if (InstanceIndex == INDEX_NONE)
	{
		return INDEX_NONE;
	}
	// task-202-01: the HISM scene proxy snapshots its instance count (InstanceCountToRender) when it is created, and AddInstance only
	// sends the new instance data to the existing proxy, so the proxy keeps drawing the old count until the async cluster tree build
	// recreates it, one or more frames later: a floor slab added under a broken block was missing on that frame (the void below drawn
	// black). Recreating the proxy at the end of this frame draws the new instance as an unbuilt one at once, as RemoveInstance already
	// does. The flag is per component and per frame, so a frame breaking many cells of one chunk recreates its proxy once.
	Visual->Render->MarkRenderStateDirty();
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
	return Type != ERRCellType::Void;
}

void ARRArenaGrid::RemoveCellInstance(ERRCellType MapKey, FIntPoint Cell, int32 InstanceIndex)
{
	const int32 VisualIndex = GetVisualIndex(MapKey, Cell);
	FRRGridChunkVisual* Visual = VisualIndex != INDEX_NONE ? &ChunkVisuals[VisualIndex] : nullptr;
	if (!Visual || (!Visual->Render && !Visual->Collision) || !Visual->InstanceCells.IsValidIndex(InstanceIndex))
	{
		return;
	}

	const FIntPoint RemovedCell = Visual->InstanceCells[InstanceIndex];
	const int32 LastIndex = Visual->InstanceCells.Num() - 1;
	if (Visual->Render && !Visual->Render->RemoveInstance(InstanceIndex))
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

	// Damage display (task-230-08, rock-render-damage): the corner pieces break at the first damage, and the damage floats of
	// every piece holding the cell or an empty neighbour deriving from it follow the health.
	if (IsRockPieceType(Type))
	{
		BreakCellCornerPieces(Cell);
		RefreshPieceDataAround(Cell);
	}

	// Loot over the dig (task-235-01, ore-blocks / heal-ore, owner 2026-10-19): pickup k of a deposit of N pops when the dig
	// progress reaches k / N, the destroying hit included (progress 1 pops the last ones, so nothing is left for DestroyCell).
	// The drone damages every block of the deposit by the same amount, so any of them gives the deposit progress.
	const int32 DepositId = GetDepositId(Cell);
	if (DepositId != INDEX_NONE)
	{
		const float MaxHealth = GetBlockMaxHealth(Type);
		const float Progress = MaxHealth > 0.f ? FMath::Clamp(1.f - CellHealth[Index] / MaxHealth, 0.f, 1.f) : 1.f;
		// Tolerance: frame sums of dig-seconds land a few 1e-7 short of k / N.
		const int32 Owed = FMath::FloorToInt32(Progress * GetDepositPickupCount(DepositId) + 1.0e-3f);
		PopDepositPickups(DepositId, Owed, Progress, TEXT("dig"));
	}
	// Outside a deposit, the destroying hit never drops a dig pickup: DestroyCell drops the destruction bundle instead.
	else if (CellHealth[Index] > UE_KINDA_SMALL_NUMBER)
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
			++DestroyedBlockCount;
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
	// Deposit crystal (task-234-03): the formation goes with the first broken block of its deposit, whatever the source.
	if (DepositId != INDEX_NONE)
	{
		RemoveDepositCrystal(DepositId);
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
	// Every non-void cell already has its floor slab (BuildInstances, task-230-07): never add a second one (no duplicate, no
	// z-fighting). The add stays as a guard for a grid whose cells were replaced without a slab.
	if (!CellFloorInstanceIndex.IsValidIndex(Index) || CellFloorInstanceIndex[Index] == INDEX_NONE)
	{
		AddCellInstance(ERRCellType::Floor, Cell, GetFloorInstanceTransform(Cell));
	}
	// Dual grid (task-230-07, rock-render-rebuild): the 4 corner pieces of a rock piece cell change their mask.
	if (IsRockPieceType(Type))
	{
		UpdateCellCornerPieces(Cell);
		// The cell is empty now: its own display damage and the one its empty neighbours derived from it change (task-230-08).
		RefreshPieceDataAround(Cell);
	}

	UE_LOG(LogRogueRigs, Verbose, TEXT("ARRArenaGrid: block destroyed at (%d, %d), type %s, source %s."), Cell.X, Cell.Y,
		*UEnum::GetValueAsString(Type), *UEnum::GetValueAsString(Source));

	// Deposit block (task-235-01): the deposit owes at least the share of its broken blocks (a dig already popped everything at
	// progress 1); an explosion pops every remaining pickup at once (it breaks the whole deposit anyway, ore-blocks).
	if (DepositId != INDEX_NONE)
	{
		FRRGridDeposit& Deposit = Deposits[DepositId];
		++Deposit.BrokenBlocks;
		const int32 Owed = IsExplosionSource(Source) ? GetDepositPickupCount(DepositId) : Deposit.BrokenBlocks * FMath::Max(DestroyDropCount, 0);
		PopDepositPickups(DepositId, Owed, 1.f, IsExplosionSource(Source) ? TEXT("explosion") : TEXT("break"));
	}
	else
	{
		// Outside a deposit: one bundle per broken block whatever the source (start zone included: no protection, ore-blocks).
		SpawnPickups(Cell, Type, DestroyDropCount, ChunkValue, HealValue);
	}

	// Every destroyed barrel explodes, whatever destroyed it (shots, dig, another explosion); queued, never synchronous.
	if (Type == ERRCellType::Barrel && !bExplosionAlreadyQueued)
	{
		QueueBarrelExplosion(Cell);
	}

	if (bWasMarked)
	{
		OnCellMarkedChanged(Cell, false, GetCellCenter(Cell.X, Cell.Y));
	}
	++DestroyedBlockCount;
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
	WriteCellCustomData(Cell, MarkedCustomDataIndex, Value);
	// Deposit crystal (task-234-03): marked while one standing block of its deposit is (read from MarkedCells).
	WriteDepositCrystalData(GetDepositId(Cell), /*bMarkDirty*/ true);
	// Rock piece cells: the outline lives in the marked quadrant floats of the 4 corner pieces (task-230-08; read from MarkedCells).
	if (IsRockPieceCell(Cell))
	{
		RefreshCellCornerPieceData(Cell);
	}
}

void ARRArenaGrid::WriteCellCustomData(FIntPoint Cell, int32 DataIndex, float Value)
{
	if (!IsBlockCell(Cell) || DataIndex < 0)
	{
		return;
	}
	const int32 Index = GetCellIndex(Cell);
	UHierarchicalInstancedStaticMeshComponent* Component = GetCellRenderComponent(Cell);
	if (!Component || !CellInstanceIndex.IsValidIndex(Index) || CellInstanceIndex[Index] == INDEX_NONE || Component->NumCustomDataFloats <= DataIndex)
	{
		return;
	}
	Component->SetCustomDataValue(CellInstanceIndex[Index], DataIndex, Value, /*bMarkRenderStateDirty*/ true);
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

URRGroundTelegraphComponent* ARRArenaGrid::CreateBarrelTelegraph()
{
	URRGroundTelegraphComponent* Telegraph = NewObject<URRGroundTelegraphComponent>(this, NAME_None, RF_Transient);
	Telegraph->SetupAttachment(GetRootComponent());
	// The grid owns the floor and rocks: they must keep receiving the fuse zone decal (task-312-01).
	Telegraph->SetKeepDecalsOffOwner(false);
	Telegraph->RegisterComponent();
	BarrelTelegraphs.Add(Telegraph);
	return Telegraph;
}

URRGroundTelegraphComponent* ARRArenaGrid::GetBarrelTelegraph(FIntPoint Cell) const
{
	const FBarrelFuse* Fuse = BarrelFuses.Find(Cell);
	return Fuse ? Fuse->Telegraph : nullptr;
}

void ARRArenaGrid::StartBarrelFuse(FIntPoint Cell)
{
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
		Telegraph = CreateBarrelTelegraph();
	}
	Fuse.Telegraph = Telegraph;
	// Cell centre on the floor top (the decal projects ProjectionHalfDepth above and below).
	Telegraph->SetWorldLocation(GetCellCenter(Cell.X, Cell.Y));
	Telegraph->SetRadius(GetBarrelExplosionRadius());
	Telegraph->SetMode(ERRGroundTelegraphMode::Danger);
	Telegraph->SetProgress(0.f);
	Telegraph->Show();

	// Render-only instability (task-197-01): the cell keeps (or starts) shaking; Tick scales it with the fuse progress.
	if (!ShakingCells.Contains(Cell))
	{
		ShakingCells.Add(Cell, GetShakePhase(Cell));
	}
	WriteCellCustomData(Cell, BarrelFuseCustomDataIndex, 0.f);

	UpdateTickEnabled();
}

void ARRArenaGrid::EndBarrelFuse(FIntPoint Cell)
{
	FBarrelFuse Fuse;
	if (!BarrelFuses.RemoveAndCopyValue(Cell, Fuse))
	{
		return;
	}
	StopShaking(Cell);
	WriteCellCustomData(Cell, BarrelFuseCustomDataIndex, 0.f);
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
		WriteCellCustomData(Pair.Key, BarrelFuseCustomDataIndex, Elapsed >= BarrelFuseSeconds - UE_KINDA_SMALL_NUMBER ? 1.f : Progress);
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

int32 ARRArenaGrid::GetDepositPickupCount(int32 DepositId) const
{
	return Deposits.IsValidIndex(DepositId) ? Deposits[DepositId].Cells.Num() * FMath::Max(DestroyDropCount, 0) : 0;
}

void ARRArenaGrid::PopDepositPickups(int32 DepositId, int32 Target, float Progress, const TCHAR* Cause)
{
	if (!Deposits.IsValidIndex(DepositId))
	{
		return;
	}
	const int32 Total = GetDepositPickupCount(DepositId);
	const int32 PerBlock = FMath::Max(DestroyDropCount, 1);
	const UWorld* World = GetWorld();
	const double Now = World ? World->GetTimeSeconds() : 0.0;
	const bool bDig = FCString::Strcmp(Cause, TEXT("dig")) == 0;
	FRRGridDeposit& Deposit = Deposits[DepositId];
	const int32 Clamped = FMath::Min(Target, Total);
	while (Deposit.PoppedPickups < Clamped)
	{
		// Pickup k belongs to block k / PerBlock and pops from its centre (destroyed or not: the centre stays valid).
		const int32 Pickup = Deposit.PoppedPickups++;
		const FIntPoint FromCell = Deposit.Cells[Pickup / PerBlock];
		SpawnPickups(FromCell, Deposit.CellType, 1, Deposit.ChunkValue, Deposit.HealValue);
		if (bDig)
		{
			UE_LOG(LogRogueRigs, Log, TEXT("ARRArenaGrid: deposit %d (%s) pickup %d/%d popped at %.3f s, progress %.3f (%s) from (%d, %d)."), DepositId,
				*Deposit.OreId.ToString(), Pickup + 1, Total, Now, Progress, Cause, FromCell.X, FromCell.Y);
		}
		else
		{
			UE_LOG(LogRogueRigs, Verbose, TEXT("ARRArenaGrid: deposit %d (%s) pickup %d/%d popped at %.3f s, progress %.3f (%s) from (%d, %d)."), DepositId,
				*Deposit.OreId.ToString(), Pickup + 1, Total, Now, Progress, Cause, FromCell.X, FromCell.Y);
		}
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

	// Counted by DestroySingleCell: a deposit broken whole adds its blocks outside the radius, its later targets add nothing.
	const int32 CountBefore = DestroyedBlockCount;
	for (const FIntPoint& Cell : Targets)
	{
		DestroyCell(Cell, Source);
	}
	return DestroyedBlockCount - CountBefore;
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
		// A fusing barrel keeps its fuse shake (task-197-01) until its fuse ends, whatever the drone does.
		if (!BarrelFuses.Contains(Cell))
		{
			StopShaking(Cell);
		}
		return;
	}

	if (!IsBlockCell(Cell) || ShakingCells.Contains(Cell))
	{
		return;
	}

	ShakingCells.Add(Cell, GetShakePhase(Cell));
	// Rock piece cells shake in the piece material (task-230-08, rock-render-top-texture option C): written once, no moving instance.
	if (IsRockPieceCell(Cell))
	{
		RefreshCellCornerPieceData(Cell);
	}
	SetActorTickEnabled(true);
}

float ARRArenaGrid::GetShakePhase(FIntPoint Cell)
{
	// Deterministic per-cell phase so adjacent shaking blocks do not move in lockstep.
	return static_cast<float>(GetTypeHash(Cell) % 1024u) / 1024.f * UE_TWO_PI;
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
		if (IsRockPieceCell(Cell))
		{
			RefreshCellCornerPieceData(Cell);
		}
		// Deposit crystal (task-234-03): back to rest once no block of its deposit shakes.
		const int32 DepositId = GetDepositId(Cell);
		int32 CrystalInstance = INDEX_NONE;
		UInstancedStaticMeshComponent* Crystals = GetDepositCrystalComponent(DepositId, CrystalInstance);
		if (Crystals && !IsDepositShaking(DepositId))
		{
			Crystals->UpdateInstanceTransform(CrystalInstance, GetDepositCrystalTransform(DepositId), /*bWorldSpace*/ false,
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

	// Deposit crystals (task-234-03) shake once per deposit while any of its blocks shakes; the drone shakes every block it digs.
	TSet<int32> ShakingDeposits;
	for (const TPair<FIntPoint, float>& Pair : ShakingCells)
	{
		const FIntPoint Cell = Pair.Key;
		if (!IsBlockCell(Cell))
		{
			continue;
		}

		const int32 DepositId = GetDepositId(Cell);
		if (DepositCrystalVisual.IsValidIndex(DepositId) && DepositCrystalVisual[DepositId] != INDEX_NONE)
		{
			ShakingDeposits.Add(DepositId);
		}

		const int32 Index = GetCellIndex(Cell);
		UHierarchicalInstancedStaticMeshComponent* Component = GetCellRenderComponent(Cell);
		if (!Component || CellInstanceIndex[Index] == INDEX_NONE)
		{
			continue;
		}

		// A fusing barrel shakes more and more (task-197-01): amplitudes from the fuse start to end values along the progress curve.
		float Horizontal = ShakeAmplitude;
		float Vertical = ShakeVerticalAmplitude;
		if (BarrelFuses.Contains(Cell))
		{
			const float Alpha = FMath::Pow(GetBarrelFuseProgress(Cell), BarrelFuseShakeExponent);
			Horizontal = FMath::Lerp(BarrelFuseShakeStartAmplitude, BarrelFuseShakeEndAmplitude, Alpha);
			Vertical = FMath::Lerp(BarrelFuseShakeVerticalStartAmplitude, BarrelFuseShakeVerticalEndAmplitude, Alpha);
		}

		// Only the rendering instance moves; the collision instance stays at rest. Incommensurate sines per axis: a horizontal jitter bounded by the horizontal amplitude plus
		// a vertical bob bounded by the vertical amplitude.
		const FVector Offset(
			Horizontal * FMath::Sin(Angle + Pair.Value),
			Horizontal * FMath::Sin(1.37f * Angle + 2.f * Pair.Value),
			Vertical * FMath::Sin(1.71f * Angle + 3.f * Pair.Value));
		FTransform Transform = GetBlockRenderTransform(Cell);
		Transform.AddToTranslation(Offset);
		Component->UpdateInstanceTransform(CellInstanceIndex[Index], Transform, /*bWorldSpace*/ false,
			/*bMarkRenderStateDirty*/ true, /*bTeleport*/ true);
	}

	for (const int32 DepositId : ShakingDeposits)
	{
		int32 Instance = INDEX_NONE;
		UInstancedStaticMeshComponent* Component = GetDepositCrystalComponent(DepositId, Instance);
		if (!Component || Instance == INDEX_NONE)
		{
			continue;
		}
		// Same jitter as a block (ShakeAmplitude, ShakeVerticalAmplitude), one phase per deposit (from its first cell).
		const float Phase = GetShakePhase(Deposits[DepositId].Cells[0]);
		const FVector Offset(
			ShakeAmplitude * FMath::Sin(Angle + Phase),
			ShakeAmplitude * FMath::Sin(1.37f * Angle + 2.f * Phase),
			ShakeVerticalAmplitude * FMath::Sin(1.71f * Angle + 3.f * Phase));
		FTransform Transform = GetDepositCrystalTransform(DepositId);
		Transform.AddToTranslation(Offset);
		Component->UpdateInstanceTransform(Instance, Transform, /*bWorldSpace*/ false, /*bMarkRenderStateDirty*/ true, /*bTeleport*/ true);
	}
}
