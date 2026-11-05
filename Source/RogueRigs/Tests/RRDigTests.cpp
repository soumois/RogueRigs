// Rogue Rigs - automation tests of the dig rules (RogueRigs.Dig.*): grid damage and destruction, ore drops, radius destruction, drone arc dig (contact, arc limits, speeds, persistence and release), pickup collection, render-only shake, dual-grid rock pieces (task-230-07), rock damage display data and ore crystals (task-230-08).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Drone/RRDrone.h"
#include "Engine/Engine.h"
#include "Engine/TargetPoint.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Explosions/RRExplosionStatsRow.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Pickups/RRPickup.h"
#include "Pickups/RRPickupSubsystem.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Player/RRMiner.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestStatsRows.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRDigTests
{
	// Test world, frame step, grid and miner spawning, RockCell and RockWestFloorCell: Tests/RRTestHarness.h.
	using namespace RRTestHarness;

	/** Blueprint class of the heal pickup. */
	const TCHAR* const HealPickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal.BP_PickupHeal_C");

	/** Heal ore cell of arena_m1 (legacy 200 uu grid) used by the radius test. */
	const FIntPoint HealOreCell(49, 45);

	/** Heal ore cell of arena_m1_fine used by the drop test: center of the 3 x 3 cluster of the legacy heal ore (49, 45). */
	const FIntPoint FineHealOreCell(148, 136);

	// Protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestAccess;

	/** Number of live pickups in the world. */
	int32 CountPickups(UWorld* World)
	{
		int32 Count = 0;
		for (TActorIterator<ARRPickup> It(World); It; ++It)
		{
			// task-092-01: idle pool pickups are not live pickups.
			if (URRActorPoolSubsystem::IsPooledIdle(*It))
			{
				continue;
			}
			Count += IsValid(*It) ? 1 : 0;
		}
		return Count;
	}

	/** True when Cell holds a type drawn by the dual-grid rock pieces (rock, ore, heal ore, special ore; not barrels). */
	bool IsPieceSolid(const ARRArenaGrid* Grid, FIntPoint Cell)
	{
		const ERRCellType Type = Grid->GetCellType(Cell);
		return Type == ERRCellType::Rock || Type == ERRCellType::Ore || Type == ERRCellType::HealOre || Type == ERRCellType::SpecialOre;
	}

	/** Solid mask of the 4 cells around Corner, computed independently of the grid: bit = (+X side) | (+Y side) << 1. */
	uint8 CornerMask(const ARRArenaGrid* Grid, FIntPoint Corner)
	{
		uint8 Mask = 0;
		Mask |= IsPieceSolid(Grid, FIntPoint(Corner.X - 1, Corner.Y - 1)) ? 1 : 0;
		Mask |= IsPieceSolid(Grid, FIntPoint(Corner.X, Corner.Y - 1)) ? 2 : 0;
		Mask |= IsPieceSolid(Grid, FIntPoint(Corner.X - 1, Corner.Y)) ? 4 : 0;
		Mask |= IsPieceSolid(Grid, FIntPoint(Corner.X, Corner.Y)) ? 8 : 0;
		return Mask;
	}

	/** Shape expected for a non-empty mask (rock-render-piece-set): 4 solids full, 1 outer, 3 inner, 2 opposite diagonal, 2 on one side edge. */
	ERRRockPieceShape ExpectedShape(uint8 Mask)
	{
		const int32 Count = FMath::CountBits(Mask);
		if (Count == 4) { return ERRRockPieceShape::Full; }
		if (Count == 1) { return ERRRockPieceShape::Outer; }
		if (Count == 3) { return ERRRockPieceShape::Inner; }
		return Mask == 0b1001 || Mask == 0b0110 ? ERRRockPieceShape::Diagonal : ERRRockPieceShape::Edge;
	}

	/** Mask covered by a piece of Shape placed with InstanceTransform: canonical Blender rock corners (SM_RockSlab.report.json), Y mirrored by the import when bFlipY, turned by the instance rotation. */
	uint8 PlacedPieceMask(ERRRockPieceShape Shape, const FTransform& InstanceTransform, bool bFlipY)
	{
		const FIntPoint A(-1, -1), B(1, -1), C(1, 1), D(-1, 1);
		TArray<FIntPoint> Quadrants;
		switch (Shape)
		{
		case ERRRockPieceShape::Full:     Quadrants = { A, B, C, D }; break;
		case ERRRockPieceShape::Edge:     Quadrants = { B, C }; break;
		case ERRRockPieceShape::Outer:    Quadrants = { C }; break;
		case ERRRockPieceShape::Inner:    Quadrants = { B, C, D }; break;
		case ERRRockPieceShape::Diagonal: Quadrants = { A, C }; break;
		}
		uint8 Mask = 0;
		for (const FIntPoint& Quadrant : Quadrants)
		{
			const FVector Rotated = InstanceTransform.TransformVectorNoScale(FVector(Quadrant.X, bFlipY ? -Quadrant.Y : Quadrant.Y, 0.f));
			Mask |= static_cast<uint8>(1u << ((Rotated.X > 0.0 ? 1 : 0) | (Rotated.Y > 0.0 ? 2 : 0)));
		}
		return Mask;
	}

	/**
	 * Checks the grid's instance maps against its components (task-230-07 dual grid):
	 * - cell visuals: each map has one entry per instance; every instance maps back to a cell whose index points at it, of the
	 *   map's type, on that cell's center; rock and special ore have no rendering HISM, only their collision ISM; barrels, ore and
	 *   heal ore (crystal clusters, task-230-08) have both, instance for instance; every block cell has exactly one block instance and every non-void cell exactly one floor slab;
	 * - rock pieces: one piece per corner with at least one solid cell around it, none elsewhere; each piece maps back to its
	 *   corner, sits on it, lies in the chunk of the corner, has the shape of its mask and an orientation covering exactly the
	 *   solid quadrants; piece ISMs never collide, never affect navigation and allow Nanite; each piece holds the
	 *   PieceCustomDataCount floats with the solid mask of its corner and the display damage of each quadrant (task-230-08).
	 * Returns true when consistent.
	 */
	bool CheckInstanceMaps(FAutomationTestBase& Test, const ARRArenaGrid* Grid)
	{
		const int32 Width = FGridAccess::Width(Grid);
		const int32 Height = FGridAccess::Height(Grid);
		const float CellSize = Grid->GetCellSize();
		const TArray<int32>& BlockIndices = FGridAccess::InstanceIndices(Grid);
		const TArray<int32>& FloorIndices = FGridAccess::FloorInstanceIndices(Grid);
		int32 Errors = 0;
		int32 MappedInstances = 0;
		auto Error = [&Test, &Errors](const FString& Message)
		{
			if (Errors < 10)
			{
				Test.AddError(Message);
			}
			++Errors;
		};

		const int32 ChunkSide = FGridAccess::ChunkSide(Grid);
		for (const FRRGridChunkVisual& Visual : FGridAccess::Visuals(Grid))
		{
			// Rock and special ore are drawn by the pieces only; ore and heal ore also draw one crystal per cell (task-230-08).
			// Task-234-03: ore or heal ore with deposit meshes draw one crystal per deposit, so their chunk visuals hold collision only.
			const bool bPieceType = Visual.MapKey == ERRCellType::Rock || Visual.MapKey == ERRCellType::SpecialOre
				|| ((Visual.MapKey == ERRCellType::Ore || Visual.MapKey == ERRCellType::HealOre) && FGridAccess::UsesDepositCrystalsOf(Grid, Visual.MapKey));
			const bool bNeedsRender = !bPieceType && Visual.MapKey != ERRCellType::Floor && Visual.InstanceCells.Num() > 0;
			const UInstancedStaticMeshComponent* Component = Visual.Render ? static_cast<const UInstancedStaticMeshComponent*>(Visual.Render.Get()) : Visual.Collision.Get();
			const TArray<FIntPoint>& InstanceCells = Visual.InstanceCells;
			if (!Component)
			{
				if (InstanceCells.Num() > 0)
				{
					Error(FString::Printf(TEXT("chunk (%d, %d) %s has %d entries and no component"), Visual.Chunk.X, Visual.Chunk.Y,
						*UEnum::GetValueAsString(Visual.MapKey), InstanceCells.Num()));
				}
				continue;
			}
			if (bPieceType && Visual.Render)
			{
				Error(FString::Printf(TEXT("chunk (%d, %d) %s has a per-cell rendering HISM (rock pieces draw it)"), Visual.Chunk.X, Visual.Chunk.Y,
					*UEnum::GetValueAsString(Visual.MapKey)));
			}
			if (bNeedsRender && !Visual.Render)
			{
				Error(FString::Printf(TEXT("chunk (%d, %d) %s has no rendering HISM (barrel mesh or ore crystal)"), Visual.Chunk.X, Visual.Chunk.Y,
					*UEnum::GetValueAsString(Visual.MapKey)));
			}
			if (Component->GetInstanceCount() != InstanceCells.Num())
			{
				Error(FString::Printf(TEXT("chunk (%d, %d) map %s has %d entries, component has %d instances"), Visual.Chunk.X, Visual.Chunk.Y,
					*UEnum::GetValueAsString(Visual.MapKey), InstanceCells.Num(), Component->GetInstanceCount()));
				continue;
			}
			const UInstancedStaticMeshComponent* Collision = Visual.Collision;
			if (Visual.MapKey != ERRCellType::Floor && (!Collision || Collision->GetInstanceCount() != InstanceCells.Num()))
			{
				Error(FString::Printf(TEXT("chunk (%d, %d) map %s has %d entries, collision ISM has %d instances"), Visual.Chunk.X, Visual.Chunk.Y,
					*UEnum::GetValueAsString(Visual.MapKey), InstanceCells.Num(), Collision ? Collision->GetInstanceCount() : -1));
				continue;
			}
			// No grid component affects navigation (no per-instance navigation bounds update on add or remove).
			if (Component->CanEverAffectNavigation() || (Collision && Collision->CanEverAffectNavigation()))
			{
				Error(FString::Printf(TEXT("chunk (%d, %d) %s is navigation relevant"), Visual.Chunk.X, Visual.Chunk.Y, *UEnum::GetValueAsString(Visual.MapKey)));
			}
			const ERRCellType MapKey = Visual.MapKey;
			const TArray<int32>& Indices = MapKey == ERRCellType::Floor ? FloorIndices : BlockIndices;

			for (int32 Instance = 0; Instance < InstanceCells.Num(); ++Instance)
			{
				const FIntPoint Cell = InstanceCells[Instance];
				const ERRCellType Type = Grid->GetCellType(Cell);
				const bool bTypeMatches = (MapKey == ERRCellType::Floor ? Type != ERRCellType::Void : Type == MapKey)
					&& Cell.X / ChunkSide == Visual.Chunk.X && Cell.Y / ChunkSide == Visual.Chunk.Y;
				const int32 CellIndex = Cell.Y * Width + Cell.X;
				FTransform InstanceTransform;
				Component->GetInstanceTransform(Instance, InstanceTransform, /*bWorldSpace*/ false);
				const FVector2D Expected((Cell.X + 0.5f) * CellSize, (Cell.Y + 0.5f) * CellSize);
				bool bOnCell = FVector2D(InstanceTransform.GetLocation()).Equals(Expected, 1.f);
				if (Collision)
				{
					FTransform CollisionTransform;
					Collision->GetInstanceTransform(Instance, CollisionTransform, /*bWorldSpace*/ false);
					bOnCell &= FVector2D(CollisionTransform.GetLocation()).Equals(Expected, 1.f);
				}
				if (!bTypeMatches || !Indices.IsValidIndex(CellIndex) || Indices[CellIndex] != Instance || !bOnCell)
				{
					Error(FString::Printf(TEXT("%s instance %d -> cell (%d, %d): type %s, cell index %d, on cell %d"),
						*UEnum::GetValueAsString(MapKey), Instance, Cell.X, Cell.Y, *UEnum::GetValueAsString(Type),
						Indices.IsValidIndex(CellIndex) ? Indices[CellIndex] : -2, bOnCell ? 1 : 0));
				}
				++MappedInstances;
			}
		}

		// Expected cell instances: one per block cell (block collision ISM, barrel HISM) plus one floor slab per non-void cell.
		int32 ExpectedInstances = 0;
		for (int32 Y = 0; Y < Height; ++Y)
		{
			for (int32 X = 0; X < Width; ++X)
			{
				const ERRCellType Type = Grid->GetCellType(FIntPoint(X, Y));
				if (Type == ERRCellType::Void)
				{
					continue;
				}
				const bool bBlock = Grid->IsBlockCell(FIntPoint(X, Y));
				ExpectedInstances += (bBlock ? 1 : 0) + 1;
				if ((bBlock && BlockIndices[Y * Width + X] == INDEX_NONE) || FloorIndices[Y * Width + X] == INDEX_NONE)
				{
					Error(FString::Printf(TEXT("cell (%d, %d) misses its block or floor instance"), X, Y));
				}
			}
		}
		Test.TestEqual(TEXT("one block instance per block cell, one floor slab per non-void cell"), MappedInstances, ExpectedInstances);

		// Rock pieces at corners.
		const TArray<FRRGridPieceVisual>& Pieces = FGridAccess::PieceVisualsOf(Grid);
		const TArray<int32>& CornerVisuals = FGridAccess::CornerVisuals(Grid);
		const TArray<int32>& CornerInstances = FGridAccess::CornerInstances(Grid);
		const bool bFlipY = FGridAccess::PieceImportFlipsYOf(Grid);
		const int32 CornerWidth = Width + 1;
		const FIntPoint ChunkCount(FMath::DivideAndRoundUp(Width, ChunkSide), FMath::DivideAndRoundUp(Height, ChunkSide));
		int32 PieceInstances = 0;
		for (int32 VisualIndex = 0; VisualIndex < Pieces.Num(); ++VisualIndex)
		{
			const FRRGridPieceVisual& Visual = Pieces[VisualIndex];
			if (!Visual.Render)
			{
				if (Visual.InstanceCorners.Num() > 0)
				{
					Error(FString::Printf(TEXT("piece visual %d has %d corners and no HISM"), VisualIndex, Visual.InstanceCorners.Num()));
				}
				continue;
			}
			if (Visual.Render->GetInstanceCount() != Visual.InstanceCorners.Num())
			{
				Error(FString::Printf(TEXT("piece visual %d: %d corners, HISM has %d instances"), VisualIndex, Visual.InstanceCorners.Num(), Visual.Render->GetInstanceCount()));
				continue;
			}
			// Grid defaults: intact pieces on Nanite, broken pieces off Nanite (bBrokenRockPiecesUseNanite false, task-230-17: WPO sag).
			const bool bExpectDisallowNanite = Visual.Set == ERRRockPieceSet::Broken;
			if (Visual.Render->GetCollisionEnabled() != ECollisionEnabled::NoCollision || Visual.Render->CanEverAffectNavigation() || Visual.Render->bDisallowNanite != bExpectDisallowNanite)
			{
				Error(FString::Printf(TEXT("piece visual %d collides, affects navigation or has the wrong Nanite setting"), VisualIndex));
			}
			for (int32 Instance = 0; Instance < Visual.InstanceCorners.Num(); ++Instance)
			{
				++PieceInstances;
				const int32 CornerIndex = Visual.InstanceCorners[Instance];
				const FIntPoint Corner(CornerIndex % CornerWidth, CornerIndex / CornerWidth);
				const uint8 Mask = CornerMask(Grid, Corner);
				FTransform InstanceTransform;
				Visual.Render->GetInstanceTransform(Instance, InstanceTransform, /*bWorldSpace*/ false);
				const bool bOnCorner = InstanceTransform.GetLocation().Equals(FVector(Corner.X * CellSize, Corner.Y * CellSize, 0.f), 1.f);
				const bool bChunk = Visual.Chunk == FIntPoint(FMath::Min(Corner.X / ChunkSide, ChunkCount.X - 1), FMath::Min(Corner.Y / ChunkSide, ChunkCount.Y - 1));
				const bool bMapped = CornerVisuals.IsValidIndex(CornerIndex) && CornerVisuals[CornerIndex] == VisualIndex && CornerInstances[CornerIndex] == Instance;
				const bool bShape = Mask != 0 && Visual.Shape == ExpectedShape(Mask);
				const bool bOriented = bShape && PlacedPieceMask(Visual.Shape, InstanceTransform, bFlipY) == Mask;
				// Custom data (task-230-08): the solid mask and the display damage of each quadrant follow the cells, across swaps.
				bool bData = Visual.Render->NumCustomDataFloats == ARRArenaGrid::PieceCustomDataCount;
				const int32 DataStart = Instance * Visual.Render->NumCustomDataFloats;
				bData = bData && Visual.Render->PerInstanceSMCustomData.IsValidIndex(DataStart + ARRArenaGrid::PieceCustomDataCount - 1);
				if (bData)
				{
					const float* Data = &Visual.Render->PerInstanceSMCustomData[DataStart];
					bData = Data[ARRArenaGrid::PieceSolidMaskCustomDataIndex] == static_cast<float>(Mask);
					for (int32 Quadrant = 0; Quadrant < 4 && bData; ++Quadrant)
					{
						const FIntPoint Cell(Corner.X - ((Quadrant & 1) ? 0 : 1), Corner.Y - ((Quadrant & 2) ? 0 : 1));
						bData = FMath::IsNearlyEqual(Data[ARRArenaGrid::PieceDamageCustomDataIndex + Quadrant], Grid->GetCellDisplayDamage(Cell), 1e-5f);
					}
				}
				if (!bOnCorner || !bChunk || !bMapped || !bShape || !bOriented || !bData)
				{
					Error(FString::Printf(TEXT("piece %d of visual %d -> corner (%d, %d) mask %d shape %s: on corner %d, chunk %d, mapped %d, shape %d, oriented %d, data %d"),
						Instance, VisualIndex, Corner.X, Corner.Y, Mask, *UEnum::GetValueAsString(Visual.Shape), bOnCorner ? 1 : 0, bChunk ? 1 : 0,
						bMapped ? 1 : 0, bShape ? 1 : 0, bOriented ? 1 : 0, bData ? 1 : 0));
				}
			}
		}
		int32 ExpectedPieces = 0;
		for (int32 Y = 0; Y <= Height; ++Y)
		{
			for (int32 X = 0; X <= Width; ++X)
			{
				const bool bSolid = CornerMask(Grid, FIntPoint(X, Y)) != 0;
				ExpectedPieces += bSolid ? 1 : 0;
				const int32 CornerIndex = Y * CornerWidth + X;
				if (!CornerVisuals.IsValidIndex(CornerIndex) || (CornerVisuals[CornerIndex] != INDEX_NONE) != bSolid)
				{
					Error(FString::Printf(TEXT("corner (%d, %d): piece %s, expected %s"), X, Y,
						CornerVisuals.IsValidIndex(CornerIndex) && CornerVisuals[CornerIndex] != INDEX_NONE ? TEXT("present") : TEXT("absent"), bSolid ? TEXT("one") : TEXT("none")));
				}
			}
		}
		Test.TestEqual(TEXT("one rock piece per corner touching a rock, ore or heal cell"), PieceInstances, ExpectedPieces);
		return Errors == 0 && MappedInstances == ExpectedInstances && PieceInstances == ExpectedPieces;
	}
}

using namespace RRDigTests;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigRockDestroyTest, "RogueRigs.Dig.RockDestroy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigRockDestroyTest::RunTest(const FString& Parameters)
{
	// Game grid (67 uu): the dig time is the GDD value per 67 uu block.
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnFineGrid(*this, TestWorld);
	if (!Grid)
	{
		return false;
	}

	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Grid->OnCellDestroyed.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleCellDestroyed);

	TestEqual(TEXT("(129, 115) is rock"), Grid->GetCellType(FineRockCell), ERRCellType::Rock);
	TestEqual(TEXT("(128, 115) is floor"), Grid->GetCellType(FineRockWestFloorCell), ERRCellType::Floor);
	TestNearlyEqual(TEXT("rock health 0.65 (drone gdd: 0.65 s per 67 uu block)"), Grid->GetCellHealth(FineRockCell), 0.65f);
	CheckInstanceMaps(*this, Grid);

	const int32 RockBefore = FGridAccess::InstanceCount(Grid, ERRCellType::Rock);
	const int32 FloorBefore = FGridAccess::InstanceCount(Grid, ERRCellType::Floor);

	// One ApplyTerrainDamage(dt) per frame, as the drone does at mining speed 1.
	int32 DestroyFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= 120 && DestroyFrame == INDEX_NONE; ++Frame)
	{
		if (Grid->ApplyTerrainDamage(FineRockCell, FrameSeconds, ERRTerrainDamageSource::Drone))
		{
			DestroyFrame = Frame;
		}
		TestWorld.Tick();
	}

	const float DestroySeconds = DestroyFrame * FrameSeconds;
	AddInfo(FString::Printf(TEXT("rock destroyed at frame %d (%.4f s)"), DestroyFrame, DestroySeconds));
	TestTrue(TEXT("rock destroyed within 0.65 s +- 1 frame"), DestroyFrame >= 38 && DestroyFrame <= 40);
	TestEqual(TEXT("cell is floor after destruction"), Grid->GetCellType(FineRockCell), ERRCellType::Floor);
	TestFalse(TEXT("cell is no longer a block"), Grid->IsBlockCell(FineRockCell));
	TestEqual(TEXT("rock collision lost one instance"), FGridAccess::InstanceCount(Grid, ERRCellType::Rock), RockBefore - 1);
	TestEqual(TEXT("floor unchanged (a slab already lay under the rock, task-230-07)"), FGridAccess::InstanceCount(Grid, ERRCellType::Floor), FloorBefore);
	TestTrue(TEXT("instance maps consistent after destruction"), CheckInstanceMaps(*this, Grid));
	TestEqual(TEXT("OnCellDestroyed fired once"), Recorder->GetDestroyedCells().Num(), 1);
	TestEqual(TEXT("rock drops nothing"), CountPickups(TestWorld.Get()), 0);
	TestFalse(TEXT("damage on a destroyed cell is ignored"), Grid->ApplyTerrainDamage(FineRockCell, 1.f, ERRTerrainDamageSource::Drone));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigHealOreDropsTest, "RogueRigs.Dig.HealOreDrops",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigHealOreDropsTest::RunTest(const FString& Parameters)
{
	// Game grid (67 uu): heal ore takes 2 s per block (task-259-01, was 1.5); its 2 x 2 vein holds 4 pickups, popped one by one over the dig at
	// progress k / 4, i.e. every 0.5 s, the last one on the destroying hit (task-235-01, heal-ore, owner 2026-10-19).
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnFineGrid(*this, TestWorld);
	UClass* HealClass = LoadTestClass<ARRPickup>(*this, HealPickupClassPath);
	if (!Grid || !HealClass)
	{
		return false;
	}

	if (FGridAccess::HealClass(Grid) != HealClass)
	{
		AddInfo(FString::Printf(TEXT("BP_ArenaGrid HealPickupClass was %s; set to BP_PickupHeal for the test"), *GetNameSafe(FGridAccess::HealClass(Grid).Get())));
		FGridAccess::HealClass(Grid) = HealClass;
	}

	TestEqual(TEXT("(148, 136) is heal ore"), Grid->GetCellType(FineHealOreCell), ERRCellType::HealOre);
	TestNearlyEqual(TEXT("heal ore health 2"), Grid->GetCellHealth(FineHealOreCell), 2.f);
	TestEqual(TEXT("one pickup per block"), FGridAccess::DropsOnDestroy(Grid), 1);
	const int32 DepositId = Grid->GetDepositId(FineHealOreCell);
	if (!TestTrue(TEXT("(148, 136) is in a heal vein"), DepositId != INDEX_NONE))
	{
		return false;
	}
	const int32 Expected = Grid->GetDepositPickupCount(DepositId);
	TestEqual(TEXT("a 2 x 2 vein holds 4 pickups"), Expected, 4);
	TestEqual(TEXT("no pickup at start"), CountPickups(TestWorld.Get()), 0);

	TArray<int32> PopFrames;
	int32 DestroyFrame = INDEX_NONE;
	int32 PickupsAtDestroy = 0;
	if (URRPickupSubsystem* PickupSystem = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>())
	{
		PickupSystem->SetSpawnMaxFrames(0); // task-092-01: drops delivered at once (these checks read them in the same frame); spread covered by RogueRigs.PickupPool.
	}
	for (int32 Frame = 1; Frame <= 300 && DestroyFrame == INDEX_NONE; ++Frame)
	{
		const int32 Before = CountPickups(TestWorld.Get());
		const bool bDestroyed = Grid->ApplyTerrainDamage(FineHealOreCell, FrameSeconds, ERRTerrainDamageSource::Drone);
		const int32 After = CountPickups(TestWorld.Get());
		TestTrue(FString::Printf(TEXT("at most one pickup at frame %d"), Frame), After - Before <= 1);
		if (After > Before)
		{
			PopFrames.Add(Frame);
		}
		if (bDestroyed)
		{
			DestroyFrame = Frame;
			PickupsAtDestroy = After - Before;
		}
		TestWorld.Tick();
	}

	FString Frames;
	for (const int32 Frame : PopFrames)
	{
		Frames += FString::Printf(TEXT(" %d (%.4f s)"), Frame, Frame * FrameSeconds);
	}
	AddInfo(FString::Printf(TEXT("pops at frames%s; destroyed at frame %d (%.4f s)"), *Frames, DestroyFrame, DestroyFrame * FrameSeconds));
	TestTrue(TEXT("destroyed at 2 s +- 1 frame"), DestroyFrame >= 119 && DestroyFrame <= 121);
	if (TestEqual(TEXT("4 pops during the dig"), PopFrames.Num(), Expected))
	{
		// Pickup k at progress k / 4: 0.5 s = 30 frames, so frames 30 k (+- 1).
		for (int32 Pop = 0; Pop < PopFrames.Num(); ++Pop)
		{
			const float ExpectedFrame = (Pop + 1) * 2.f / Expected / FrameSeconds;
			TestTrue(FString::Printf(TEXT("pickup %d pops at progress %d / 4 (frame %d, expected %.1f +- 1)"), Pop + 1, Pop + 1, PopFrames[Pop], ExpectedFrame),
				FMath::Abs(PopFrames[Pop] - ExpectedFrame) <= 1.f);
		}
		TestEqual(TEXT("the last pickup pops on the destroying hit"), PopFrames.Last(), DestroyFrame);
	}
	TestEqual(TEXT("the destroying hit pops only the last pickup"), PickupsAtDestroy, 1);
	TestEqual(TEXT("deposit counts 4 popped"), Grid->GetDeposit(DepositId)->PoppedPickups, Expected);

	// The other 3 blocks of the vein owe nothing more: destroying them pops no pickup.
	TArray<FIntPoint> Rest;
	Grid->GetDepositCells(Grid->GetDeposit(DepositId)->Cells[0] == FineHealOreCell ? Grid->GetDeposit(DepositId)->Cells[1] : Grid->GetDeposit(DepositId)->Cells[0], Rest);
	TestEqual(TEXT("3 vein blocks still standing"), Rest.Num(), 3);
	for (const FIntPoint& Cell : Rest)
	{
		Grid->DestroyCell(Cell, ERRTerrainDamageSource::Barrel);
	}
	const int32 Total = CountPickups(TestWorld.Get());
	AddInfo(FString::Printf(TEXT("total pickups: %d"), Total));
	TestEqual(TEXT("4 pickups in total"), Total, Expected);
	for (TActorIterator<ARRPickup> It(TestWorld.Get()); It; ++It)
	{
		// task-092-01: idle pool pickups are not live pickups.
		if (URRActorPoolSubsystem::IsPooledIdle(*It))
		{
			continue;
		}
		TestEqual(TEXT("pickup is Heal"), It->GetPickupType(), ERRPickupType::Heal);
		TestTrue(TEXT("pickup is a BP_PickupHeal"), It->IsA(HealClass));
	}
	TestTrue(TEXT("instance maps consistent after destruction"), CheckInstanceMaps(*this, Grid));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigRadiusTest, "RogueRigs.Dig.DestroyCellsInRadius",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigRadiusTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	if (!Grid)
	{
		return false;
	}

	// Radius 2.5 cells around the heal ore: mixes rock, ore and floor in arena_m1. Independent expectation from the public API.
	const FVector Center = Grid->GetCellCenter(HealOreCell.X, HealOreCell.Y) + FVector(30.f, -20.f, 0.f);
	const float Radius = 2.5f * Grid->GetCellSize();
	TArray<FIntPoint> Inside;
	TArray<FIntPoint> Outside;
	int32 ExpectedPickups = 0;
	for (int32 Y = 0; Y < FGridAccess::Height(Grid); ++Y)
	{
		for (int32 X = 0; X < FGridAccess::Width(Grid); ++X)
		{
			const FIntPoint Cell(X, Y);
			if (!Grid->IsBlockCell(Cell))
			{
				continue;
			}
			if (FVector::Dist2D(Grid->GetCellCenter(X, Y), Center) <= Radius)
			{
				Inside.Add(Cell);
				const ERRCellType Type = Grid->GetCellType(Cell);
				const bool bDrops = (Type == ERRCellType::Ore && FGridAccess::OreClass(Grid)) || (Type == ERRCellType::HealOre && FGridAccess::HealClass(Grid));
				ExpectedPickups += bDrops ? FGridAccess::DropsOnDestroy(Grid) : 0;
			}
			else
			{
				Outside.Add(Cell);
			}
		}
	}
	AddInfo(FString::Printf(TEXT("%d blocks inside the radius, %d outside, %d pickups expected"), Inside.Num(), Outside.Num(), ExpectedPickups));
	TestTrue(TEXT("radius covers several blocks"), Inside.Num() >= 2);

	const int32 Destroyed = Grid->DestroyCellsInRadius(Center, Radius, ERRTerrainDamageSource::Barrel);
	TestEqual(TEXT("returned count = blocks inside"), Destroyed, Inside.Num());
	// Drops go through the pickup spawn queue (4 per frame, task-070-01): let it drain before counting.
	for (int32 Frame = 0; Frame < 4; ++Frame)
	{
		TestWorld.Tick();
	}
	for (const FIntPoint& Cell : Inside)
	{
		TestEqual(FString::Printf(TEXT("(%d, %d) destroyed"), Cell.X, Cell.Y), Grid->GetCellType(Cell), ERRCellType::Floor);
	}
	int32 OutsideStanding = 0;
	for (const FIntPoint& Cell : Outside)
	{
		OutsideStanding += Grid->IsBlockCell(Cell) ? 1 : 0;
	}
	TestEqual(TEXT("every block outside still stands"), OutsideStanding, Outside.Num());
	TestEqual(TEXT("destruction drops"), CountPickups(TestWorld.Get()), ExpectedPickups);
	TestTrue(TEXT("instance maps consistent"), CheckInstanceMaps(*this, Grid));
	TestEqual(TEXT("second call destroys nothing"), Grid->DestroyCellsInRadius(Center, Radius, ERRTerrainDamageSource::Barrel), 0);
	return true;
}

namespace RRDigTests
{
	/** Floor cell north of the heal ore cluster (147..149, 135..137) of arena_m1_fine: the miner there touches (148, 135). */
	const FIntPoint FineHealOreNorthFloorCell(148, 134);

	/** Spawns the game grid and BP_Miner standing on Cell; returns its drone (null on failure). Weapons stopped. */
	ARRDrone* SpawnArcDigScene(FAutomationTestBase& Test, const FTestWorld& TestWorld, FIntPoint Cell, ARRArenaGrid*& OutGrid, ARRMiner*& OutMiner)
	{
		OutGrid = SpawnFineGrid(Test, TestWorld);
		OutMiner = OutGrid ? SpawnMiner(Test, TestWorld, OutGrid, Cell) : nullptr;
		ARRDrone* Drone = OutMiner ? OutMiner->GetDrone() : nullptr;
		if (!Test.TestNotNull(TEXT("BP_Miner spawned its drone"), Drone))
		{
			return nullptr;
		}
		for (ARRWeapon* Weapon : OutMiner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		return Drone;
	}

	/** Ticks until every cell of Cells is destroyed or MaxFrames pass; returns the frame of the last destruction (INDEX_NONE if one stands). */
	int32 TickUntilDestroyed(const FTestWorld& TestWorld, const ARRArenaGrid* Grid, const TArray<FIntPoint>& Cells, int32 MaxFrames, TArray<int32>& OutFrames)
	{
		OutFrames.Init(INDEX_NONE, Cells.Num());
		for (int32 Frame = 1; Frame <= MaxFrames; ++Frame)
		{
			TestWorld.Tick();
			bool bAllDestroyed = true;
			for (int32 Index = 0; Index < Cells.Num(); ++Index)
			{
				if (OutFrames[Index] == INDEX_NONE && !Grid->IsBlockCell(Cells[Index]))
				{
					OutFrames[Index] = Frame;
				}
				bAllDestroyed &= OutFrames[Index] != INDEX_NONE;
			}
			if (bAllDestroyed)
			{
				return Frame;
			}
		}
		return INDEX_NONE;
	}

	/**
	 * Single dig beam check (task-232-01): LaserMesh visible, from the laser emitter (GetLaserEmitterLocation, task-253-01) to GetDigImpactPoint (within 0.5 uu); the impact point
	 * is the mean of the dug cells' target points projected on the face of the nearest one (Y mean and X of that face for a wall
	 * facing -X, as in these scenes).
	 */
	void CheckDigBeam(FAutomationTestBase& Test, const ARRDrone* Drone, const TCHAR* When)
	{
		const UStaticMeshComponent* Beam = FDroneAccess::Laser(Drone);
		const TArray<FIntPoint>& Cells = Drone->GetDigCells();
		if (!Test.TestTrue(FString::Printf(TEXT("%s: cells dug"), When), Cells.Num() > 0))
		{
			return;
		}
		const float BaseHeight = FDroneAccess::BeamBaseHeight(Drone);
		const FVector HalfBeam = Beam->GetUpVector() * Beam->GetComponentScale().Z * BaseHeight * 0.5;
		const FVector Start = Beam->GetComponentLocation() - HalfBeam;
		const FVector End = Beam->GetComponentLocation() + HalfBeam;
		const FVector Impact = Drone->GetDigImpactPoint();
		FVector Mean = FVector::ZeroVector;
		double FaceX = TNumericLimits<double>::Max();
		for (const FIntPoint& Cell : Cells)
		{
			const FVector Target = Drone->GetDigTargetPoint(Cell);
			Mean += Target / Cells.Num();
			FaceX = FMath::Min(FaceX, Target.X);
		}
		Test.AddInfo(FString::Printf(TEXT("%s: %d cells, one beam ending at %s (%.2f uu from the impact point)"), When, Cells.Num(), *End.ToCompactString(), FVector::Dist(End, Impact)));
		Test.TestTrue(FString::Printf(TEXT("%s: beam visible"), When), Beam->IsVisible());
		// task-253-01: the beam starts at the laser arm's emitter tip, no longer at the drone's centre.
		Test.TestTrue(FString::Printf(TEXT("%s: beam starts at the laser emitter"), When), Start.Equals(Drone->GetLaserEmitterLocation(), 0.5));
		Test.TestTrue(FString::Printf(TEXT("%s: beam ends at the impact point"), When), End.Equals(Impact, 0.5));
		Test.TestTrue(FString::Printf(TEXT("%s: impact point = middle of the target points, on the wall face"), When),
			Impact.Equals(FVector(FaceX, Mean.Y, Mean.Z), 0.01));
	}

	/** True when A and B hold the same cells, in any order. */
	bool SameCells(const TArray<FIntPoint>& A, const TArray<FIntPoint>& B)
	{
		if (A.Num() != B.Num())
		{
			return false;
		}
		for (const FIntPoint& Cell : A)
		{
			if (!B.Contains(Cell))
			{
				return false;
			}
		}
		return true;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigArcContactTest, "RogueRigs.Dig.ArcContactDig",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigArcContactTest::RunTest(const FString& Parameters)
{
	using namespace RRDigTests;

	// Game grid (67 uu): the miner stands on (128, 115), against the rock wall x = 129, with no input. Touching is enough:
	// the drone cuts the 6 blocks of the arc in parallel (two rows: the wall is 2 cells thick), each in the GDD 0.65 s, then
	// goes back to Orbit.
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, FineRockWestFloorCell, Grid, Miner);
	if (!Drone)
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Drone->OnDigBlockStarted.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleDigBlockStarted);
	Drone->OnDigBlockStopped.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleDigBlockStopped);
	TestNearlyEqual(TEXT("mining speed 1"), Drone->GetMiningSpeed(), 1.f);
	TestEqual(TEXT("drone starts in Orbit"), Drone->GetState(), ERRDroneState::Orbit);

	const TArray<FIntPoint> Arc = { FineRockCell, FIntPoint(129, 114), FIntPoint(129, 116), FIntPoint(130, 115), FIntPoint(130, 114), FIntPoint(130, 113) };
	TArray<int32> DestroyFrames;
	TestWorld.Tick();
	TestTrue(TEXT("miner touches rock without input"), Miner->IsTouchingRock());
	TestEqual(TEXT("state Dig"), Drone->GetState(), ERRDroneState::Dig);
	TestTrue(TEXT("arc = the 6 blocks in front, two rows deep"), SameCells(Drone->GetDigCells(), Arc));
	TestEqual(TEXT("nearest block first"), Drone->GetDigCells().Num() > 0 ? Drone->GetDigCells()[0] : FIntPoint(-1, -1), FineRockCell);
	TestTrue(TEXT("arc points east"), Drone->GetDigArcDirection().Equals(FVector::ForwardVector, 1e-3));
	TestEqual(TEXT("6 blocks shaking"), FGridAccess::Shaking(Grid).Num(), 6);
	TestTrue(TEXT("one start event per block"), SameCells(Recorder->GetDigStartedCells(), Arc));
	TestNearlyEqual(TEXT("first frame of damage on every block"), Grid->GetCellHealth(FIntPoint(130, 115)), 0.65f - FrameSeconds, 1e-4f);

	TestWorld.Tick();
	TestTrue(TEXT("laser visible while digging"), FDroneAccess::Laser(Drone)->IsVisible());
	CheckDigBeam(*this, Drone, TEXT("frame 2"));
	for (int32 Frame = 3; Frame <= 25; ++Frame)
	{
		TestWorld.Tick();
	}
	// task-248-01: the drone hovers on the free side of the wall face (facing -X here), DigHoverFaceDistance from the impact point,
	// DigHoverHeight above the floor, never over the rock.
	const FVector DigHover = Drone->GetDigImpactPoint() - FVector::ForwardVector * FDroneAccess::DigHoverFaceDistanceOf(Drone);
	const double HoverError = FVector::Dist2D(Drone->GetActorLocation(), DigHover);
	const double HoverHeight = Drone->GetActorLocation().Z - (Miner->GetActorLocation().Z - Miner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	AddInfo(FString::Printf(TEXT("frame 25: drone %.1f uu from the dig hover point %s, %.1f uu above the floor"), HoverError, *DigHover.ToCompactString(), HoverHeight));
	TestTrue(TEXT("drone hovers on the free side of the face"), HoverError <= 10.0);
	TestTrue(TEXT("drone at the dig hover height"), FMath::Abs(HoverHeight - FDroneAccess::DigHoverHeightOf(Drone)) <= 10.0);
	TestFalse(TEXT("drone not over a block"), Grid->IsBlockCell(Grid->GetCellAtWorld(Drone->GetActorLocation())));
	CheckDigBeam(*this, Drone, TEXT("frame 25"));

	// Frames counted from the first tick: continue to the destruction.
	const int32 LastFrame = TickUntilDestroyed(TestWorld, Grid, Arc, 60, DestroyFrames);
	for (int32 Index = 0; Index < Arc.Num(); ++Index)
	{
		const int32 Frame = DestroyFrames[Index] == INDEX_NONE ? INDEX_NONE : DestroyFrames[Index] + 25;
		AddInfo(FString::Printf(TEXT("block %s destroyed at frame %d"), *Arc[Index].ToString(), Frame));
		TestTrue(FString::Printf(TEXT("block %s destroyed after about 0.65 s"), *Arc[Index].ToString()), Frame >= 38 && Frame <= 40);
	}
	TestTrue(TEXT("all destroyed"), LastFrame != INDEX_NONE);

	TestWorld.Tick();
	TestFalse(TEXT("no block within reach any more"), Miner->IsTouchingRock());
	TestEqual(TEXT("back to Orbit"), Drone->GetState(), ERRDroneState::Orbit);
	TestEqual(TEXT("shake stopped"), FGridAccess::Shaking(Grid).Num(), 0);
	TestFalse(TEXT("laser hidden"), FDroneAccess::Laser(Drone)->IsVisible());
	TestTrue(TEXT("one stop event per block"), SameCells(Recorder->GetDigStoppedCells(), Arc));
	TestFalse(TEXT("every stop was a destruction"), Recorder->GetDigStoppedDestroyed().Contains(false));
	TestTrue(TEXT("rock (129, 113) north of the arc still stands"), Grid->IsBlockCell(FIntPoint(129, 113)));
	TestTrue(TEXT("instance maps consistent"), CheckInstanceMaps(*this, Grid));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigArcLimitsTest, "RogueRigs.Dig.ArcLimits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigArcLimitsTest::RunTest(const FString& Parameters)
{
	using namespace RRDigTests;

	// Arc selection on the game grid around the rock wall x = 129 (floor x <= 128): reach 2.5 cells of the front point,
	// 120 degree cone, at most 8 blocks; the move input steers the arc only while it pushes into the wall.
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, FineRockWestFloorCell, Grid, Miner);
	if (!Drone)
	{
		return false;
	}
	const float CellSize = Grid->GetCellSize();
	const float Radius = Miner->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector At = Grid->GetCellCenter(FineRockWestFloorCell.X, FineRockWestFloorCell.Y);
	TArray<FIntPoint> Cells;

	// Every selected block satisfies the rule.
	const TArray<FVector> Directions = { FVector(1, 0, 0), FVector(1, 1, 0).GetSafeNormal(), FVector(1, -0.5, 0).GetSafeNormal(), FVector(0, 1, 0) };
	int32 RuleErrors = 0;
	for (const FVector& Direction : Directions)
	{
		Drone->SelectArcCells(Grid, At, Direction, Cells);
		RuleErrors += Cells.Num() > 8 ? 1 : 0;
		for (const FIntPoint& Cell : Cells)
		{
			const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
			const double FrontDistance = FVector::Dist2D(Center, At + Direction * Radius);
			const double Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct((Center - At).GetSafeNormal2D(), Direction)));
			RuleErrors += Grid->IsBlockCell(Cell) && FrontDistance <= 2.5 * CellSize + 0.01 && Angle <= 60.01 ? 0 : 1;
		}
	}
	TestEqual(TEXT("selected blocks within 2.5 cells and 60 degrees, at most 8"), RuleErrors, 0);

	// The wall is 2 cells thick here ((130, 116) and x = 131 are floor): the front row and the 3 second-row blocks in reach.
	TestEqual(TEXT("east: 6 blocks"), Drone->SelectArcCells(Grid, At, FVector::ForwardVector, Cells), 6);
	TestTrue(TEXT("east: the 3 wall blocks in front and the 3 behind them in reach"),
		SameCells(Cells, { FineRockCell, FIntPoint(129, 114), FIntPoint(129, 116), FIntPoint(130, 115), FIntPoint(130, 114), FIntPoint(130, 113) }));
	TestEqual(TEXT("west (open floor): nothing"), Drone->SelectArcCells(Grid, At, -FVector::ForwardVector, Cells), 0);
	Drone->SelectArcCells(Grid, At, FVector(0, 1, 0), Cells);
	TestTrue(TEXT("along the wall (+Y): only the block within 60 degrees"), SameCells(Cells, { FIntPoint(129, 116) }));
	TestEqual(TEXT("4 cells from the wall: out of reach"),
		Drone->SelectArcCells(Grid, Grid->GetCellCenter(125, 115), FVector::ForwardVector, Cells), 0);

	FDroneAccess::MaxArcBlocksOf(Drone) = 2;
	TestEqual(TEXT("cap 2: 2 blocks"), Drone->SelectArcCells(Grid, At, FVector::ForwardVector, Cells), 2);
	TestEqual(TEXT("cap 2: nearest first"), Cells.Num() > 0 ? Cells[0] : FIntPoint(-1, -1), FineRockCell);
	FDroneAccess::MaxArcBlocksOf(Drone) = 8;

	// Steering through the miner's move input of the frame (consumed by its dig trigger).
	FVector& MoveInput = FMinerAccess::PendingMoveInputOf(Miner);
	MoveInput = FVector(1, 1, 0).GetSafeNormal();
	TestWorld.Tick();
	TestTrue(TEXT("push into the wall diagonally: the arc follows the input"), Drone->GetDigArcDirection().Equals(FVector(1, 1, 0).GetSafeNormal(), 1e-3));
	TestEqual(TEXT("digging"), Drone->GetState(), ERRDroneState::Dig);
	MoveInput = FVector(-1, 0, 0);
	TestWorld.Tick();
	TestTrue(TEXT("input away from the wall: the arc points at the wall"), Drone->GetDigArcDirection().Equals(FVector::ForwardVector, 1e-3));
	MoveInput = FVector(0.05f, 0, 0);
	TestWorld.Tick();
	TestTrue(TEXT("input below DigMinMoveInput: the arc points at the wall"), Drone->GetDigArcDirection().Equals(FVector::ForwardVector, 1e-3));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigArcSpeedsTest, "RogueRigs.Dig.ArcSpeeds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigArcSpeedsTest::RunTest(const FString& Parameters)
{
	using namespace RRDigTests;

	// Heal ore at mining speed 1: 2 s per block, in parallel (Duskite uses the same 2 s, RogueRigs.Ore.MineDeposit). Since
	// task-088-01 the vein is 2 x 2 (147-148, 135-136): the 8-block arc holds its 4 blocks and 4 rocks, which keep their 0.65 s.
	{
		FTestWorld TestWorld;
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, FineHealOreNorthFloorCell, Grid, Miner);
		if (!Drone)
		{
			return false;
		}
		TestWorld.Tick();
		const TArray<FIntPoint> Arc = Drone->GetDigCells();
		TestEqual(TEXT("heal ore: 8 blocks in the arc"), Arc.Num(), 8);
		int32 HealOreCount = 0;
		for (const FIntPoint& Cell : Arc)
		{
			HealOreCount += Grid->GetCellType(Cell) == ERRCellType::HealOre ? 1 : 0;
		}
		TestEqual(TEXT("heal ore: the 4 blocks of the 2 x 2 vein are in the arc"), HealOreCount, 4);
		TArray<bool> IsHealOre;
		for (const FIntPoint& Cell : Arc)
		{
			IsHealOre.Add(Grid->GetCellType(Cell) == ERRCellType::HealOre);
		}
		TArray<int32> Frames;
		TickUntilDestroyed(TestWorld, Grid, Arc, 150, Frames);
		for (int32 Index = 0; Index < Arc.Num(); ++Index)
		{
			const int32 Frame = Frames[Index] == INDEX_NONE ? INDEX_NONE : Frames[Index] + 1;
			const TCHAR* Kind = IsHealOre[Index] ? TEXT("heal ore") : TEXT("rock");
			AddInfo(FString::Printf(TEXT("%s %s destroyed at frame %d"), Kind, *Arc[Index].ToString(), Frame));
			TestTrue(FString::Printf(TEXT("%s %s destroyed after about %s"), Kind, *Arc[Index].ToString(), IsHealOre[Index] ? TEXT("2 s") : TEXT("0.65 s")),
				IsHealOre[Index] ? Frame >= 119 && Frame <= 121 : Frame >= 38 && Frame <= 40);
		}
	}

	// Rock at mining speed 2: 0.325 s per block.
	{
		FTestWorld TestWorld;
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, FineRockWestFloorCell, Grid, Miner);
		if (!Drone || !TestNotNull(TEXT("miner ASC"), Miner->GetRRAbilitySystemComponent()))
		{
			return false;
		}
		Miner->GetRRAbilitySystemComponent()->SetNumericAttributeBase(URRPlayerAttributeSet::GetMiningSpeedAttribute(), 2.f);
		TestNearlyEqual(TEXT("mining speed 2"), Drone->GetMiningSpeed(), 2.f);
		TArray<int32> Frames;
		TickUntilDestroyed(TestWorld, Grid, { FineRockCell }, 60, Frames);
		AddInfo(FString::Printf(TEXT("rock at mining speed 2 destroyed at frame %d"), Frames[0]));
		TestTrue(TEXT("rock destroyed after about 0.325 s at mining speed 2"), Frames[0] >= 19 && Frames[0] <= 21);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigArcReleaseTest, "RogueRigs.Dig.ArcPersistAndRelease",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigArcReleaseTest::RunTest(const FString& Parameters)
{
	using namespace RRDigTests;

	// The dig stops when the miner moves away (no contact), the damage stays on the blocks, digging resumes on return.
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, FineRockWestFloorCell, Grid, Miner);
	if (!Drone)
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Drone->OnDigBlockStopped.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleDigBlockStopped);

	for (int32 Frame = 0; Frame < 15; ++Frame)
	{
		TestWorld.Tick();
	}
	const float HealthBeforeMove = Grid->GetCellHealth(FineRockCell);
	TestNearlyEqual(TEXT("0.25 s of damage done (0.4 of 0.65 left)"), HealthBeforeMove, 0.4f, 0.02f);
	TestEqual(TEXT("still digging"), Drone->GetState(), ERRDroneState::Dig);
	CheckDigBeam(*this, Drone, TEXT("frame 15"));
	TestEqual(TEXT("6 blocks cut by the one beam before the release"), Drone->GetDigCells().Num(), 6);

	// Three cells west: open floor all around, nothing within reach.
	const FVector Home = Miner->GetActorLocation();
	Miner->SetActorLocation(Home - FVector(3.f * Grid->GetCellSize(), 0.f, 0.f), /*bSweep*/ false, nullptr, ETeleportType::TeleportPhysics);
	TestWorld.Tick();
	AddInfo(FString::Printf(TEXT("health before move %.4f, after release %.4f"), HealthBeforeMove, Grid->GetCellHealth(FineRockCell)));
	TestFalse(TEXT("no contact"), Miner->IsTouchingRock());
	TestEqual(TEXT("released to Orbit"), Drone->GetState(), ERRDroneState::Orbit);
	TestEqual(TEXT("arc cleared"), Drone->GetDigCells().Num(), 0);
	TestEqual(TEXT("shake stopped"), FGridAccess::Shaking(Grid).Num(), 0);
	TestFalse(TEXT("laser hidden"), FDroneAccess::Laser(Drone)->IsVisible());
	TestTrue(TEXT("no impact point after the release"), Drone->GetDigImpactPoint().IsZero());
	TestEqual(TEXT("6 stop events"), Recorder->GetDigStoppedCells().Num(), 6);
	TestFalse(TEXT("none was a destruction"), Recorder->GetDigStoppedDestroyed().Contains(true));
	TestNearlyEqual(TEXT("damage kept, none added after the release"), Grid->GetCellHealth(FineRockCell), HealthBeforeMove, 1e-4f);
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	TestNearlyEqual(TEXT("no damage while away"), Grid->GetCellHealth(FineRockCell), HealthBeforeMove, 1e-4f);
	TestEqual(TEXT("stays in Orbit"), Drone->GetState(), ERRDroneState::Orbit);

	// Back against the wall: the dig resumes where it stopped.
	Miner->SetActorLocation(Home, /*bSweep*/ false, nullptr, ETeleportType::TeleportPhysics);
	TArray<int32> Frames;
	TickUntilDestroyed(TestWorld, Grid, { FineRockCell }, 60, Frames);
	AddInfo(FString::Printf(TEXT("rock destroyed %d frames after the return"), Frames[0]));
	TestTrue(TEXT("resumed: the remaining 0.4 s"), Frames[0] >= 23 && Frames[0] <= 25);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDronePatrolTest, "RogueRigs.Dig.DronePatrol",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDronePatrolTest::RunTest(const FString& Parameters)
{
	using namespace RRDigTests;

	// Patrol movement (task-251-01, replaces the task-247-01 escort check), headless position check only: on the open floor row
	// y = 129 of arena_m1_fine (x 80..151 floor with two clear rows on each side). Rest: the same patrol seed run twice with the
	// miner facing +Y then -Y gives the same drone offsets (no dependence on the facing), the drone stays in the 60-160 uu zone and
	// its patrol point changes at least every 5 s. Walk: the miner walks +X at its max speed for 3 s (the drone lags behind),
	// then rests (the drone catches up into the zone). Every frame: the drone never in a block cell, the patrol point valid.
	const FIntPoint StartCell(84, 129);
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, StartCell, Grid, Miner);
	if (!Drone)
	{
		return false;
	}
	int32 PathBlocks = 0;
	for (int32 Y = StartCell.Y - 2; Y <= StartCell.Y + 2; ++Y)
	{
		for (int32 X = StartCell.X - 2; X <= StartCell.X + 34; ++X)
		{
			PathBlocks += Grid->IsBlockCell(FIntPoint(X, Y)) ? 1 : 0;
		}
	}
	TestEqual(TEXT("walk path is open floor"), PathBlocks, 0);
	UCharacterMovementComponent* MinerMovement = Miner->GetCharacterMovement();
	MinerMovement->bRunPhysicsWithNoController = true;
	MinerMovement->SetDefaultMovementMode();

	const double ZoneMin = FDroneAccess::PatrolMinRadiusOf(Drone);
	const double ZoneMax = FDroneAccess::PatrolMaxRadiusOf(Drone);
	constexpr double ZoneTolerance = 10.0;
	const int32 MaxPointFrames = FMath::CeilToInt32(FDroneAccess::PatrolMaxIntervalOf(Drone) / FrameSeconds) + 1;
	int32 InBlockFrames = 0;
	int32 InvalidPointFrames = 0;
	int32 SampledFrames = 0;
	bool bAlwaysOrbit = true;
	auto Step = [&](const FVector& Direction)
	{
		if (!Direction.IsZero())
		{
			FMinerAccess::PendingMoveInputOf(Miner) = Direction;
			Miner->AddMovementInput(Direction, 1.f);
		}
		TestWorld.Tick();
		++SampledFrames;
		bAlwaysOrbit &= Drone->GetState() == ERRDroneState::Orbit;
		InBlockFrames += Grid->IsBlockCell(Grid->GetCellAtWorld(Drone->GetActorLocation())) ? 1 : 0;
		InvalidPointFrames += !FDroneAccess::PatrolFallbackOf(Drone) && !FDroneAccess::IsPatrolOffsetClearOf(Drone, Grid, FDroneAccess::PatrolOffsetOf(Drone)) ? 1 : 0;
	};
	auto Offset2D = [&]()
	{
		const FVector Offset = Drone->GetActorLocation() - Miner->GetActorLocation();
		return FVector2D(Offset.X, Offset.Y);
	};
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		Step(FVector::ZeroVector);
	}

	// Rest, twice with the same seed and opposite facings: 10 s each, sampled every frame.
	constexpr int32 PatrolSeed = 251;
	constexpr int32 RestFrames = 600;
	struct FRestRun
	{
		TArray<FVector2D> Offsets;
		int32 PointChanges = 0;
		int32 LongestPointFrames = 0;
		int32 OutOfZone = 0;
		double MinDistance = TNumericLimits<double>::Max();
		double MaxDistance = 0.0;
	};
	auto RunRest = [&](float FacingYaw, FRestRun& Run)
	{
		Miner->SetActorRotation(FRotator(0.f, FacingYaw, 0.f));
		FDroneAccess::RestartPatrolOf(Drone, PatrolSeed);
		FVector2D LastPoint = FDroneAccess::PatrolOffsetOf(Drone);
		int32 LastChange = 0;
		for (int32 Frame = 1; Frame <= RestFrames; ++Frame)
		{
			Step(FVector::ZeroVector);
			const FVector2D Offset = Offset2D();
			Run.Offsets.Add(Offset);
			const double Distance = Offset.Size();
			Run.MinDistance = FMath::Min(Run.MinDistance, Distance);
			Run.MaxDistance = FMath::Max(Run.MaxDistance, Distance);
			Run.OutOfZone += Distance < ZoneMin - ZoneTolerance || Distance > ZoneMax + ZoneTolerance ? 1 : 0;
			const FVector2D Point = FDroneAccess::PatrolOffsetOf(Drone);
			if (!Point.Equals(LastPoint, 0.01))
			{
				++Run.PointChanges;
				Run.LongestPointFrames = FMath::Max(Run.LongestPointFrames, Frame - LastChange);
				LastChange = Frame;
				LastPoint = Point;
			}
		}
		Run.LongestPointFrames = FMath::Max(Run.LongestPointFrames, RestFrames - LastChange);
	};
	FRestRun FacingNorth;
	FRestRun FacingSouth;
	RunRest(90.f, FacingNorth);
	RunRest(-90.f, FacingSouth);
	double MaxFacingDifference = 0.0;
	for (int32 Index = 0; Index < RestFrames; ++Index)
	{
		MaxFacingDifference = FMath::Max(MaxFacingDifference, FVector2D::Distance(FacingNorth.Offsets[Index], FacingSouth.Offsets[Index]));
	}
	AddInfo(FString::Printf(TEXT("rest facing +Y: %.1f..%.1f uu from the miner, %d frames out of %.0f-%.0f (+-%.0f), %d point changes, longest point %.2f s"),
		FacingNorth.MinDistance, FacingNorth.MaxDistance, FacingNorth.OutOfZone, ZoneMin, ZoneMax, ZoneTolerance, FacingNorth.PointChanges, FacingNorth.LongestPointFrames * FrameSeconds));
	AddInfo(FString::Printf(TEXT("rest facing -Y: %.1f..%.1f uu, %d out of the zone, %d point changes; largest offset difference between the two facings %.3f uu"),
		FacingSouth.MinDistance, FacingSouth.MaxDistance, FacingSouth.OutOfZone, FacingSouth.PointChanges, MaxFacingDifference));
	TestTrue(TEXT("rest: the drone offset does not depend on the miner facing (same seed, within 0.5 uu)"), MaxFacingDifference <= 0.5);
	TestEqual(TEXT("rest: the drone stays in the patrol zone"), FacingNorth.OutOfZone + FacingSouth.OutOfZone, 0);
	TestTrue(TEXT("rest: the patrol point changes"), FacingNorth.PointChanges >= 2);
	TestTrue(TEXT("rest: the patrol point changes within 5 s"), FacingNorth.LongestPointFrames <= MaxPointFrames);

	// Walk away: 180 frames of +X input; over the last 30 frames the drone must be behind the miner (lag) and keep pace.
	FDroneAccess::RestartPatrolOf(Drone, PatrolSeed);
	const FVector Direction = FVector::ForwardVector;
	constexpr int32 MoveFrames = 180;
	constexpr int32 SettledFrames = 30;
	double MinBehind = TNumericLimits<double>::Max();
	double MaxBehind = TNumericLimits<double>::Lowest();
	double MaxWalkDistance = 0.0;
	double MinSpeed = TNumericLimits<double>::Max();
	for (int32 Frame = 1; Frame <= MoveFrames; ++Frame)
	{
		Step(Direction);
		MaxWalkDistance = FMath::Max(MaxWalkDistance, Offset2D().Size());
		if (Frame > MoveFrames - SettledFrames)
		{
			const double Behind = -FVector2D::DotProduct(Offset2D(), FVector2D(Direction.X, Direction.Y));
			MinBehind = FMath::Min(MinBehind, Behind);
			MaxBehind = FMath::Max(MaxBehind, Behind);
			MinSpeed = FMath::Min(MinSpeed, Miner->GetVelocity().Size2D());
		}
	}
	// Lag: how far the drone trails its own patrol point (which moves with the miner) when the miner stops.
	auto DistanceToPoint = [&]()
	{
		return FVector2D::Distance(Offset2D(), FDroneAccess::PatrolOffsetOf(Drone));
	};
	const double LagAtStop = DistanceToPoint();
	TestTrue(TEXT("miner moves at constant speed"), MinSpeed > 0.9 * MinerMovement->GetMaxSpeed());
	TestTrue(TEXT("walk: the drone lags behind the miner"), MinBehind > 20.0);
	TestTrue(TEXT("walk: the drone lags behind its patrol point (over 100 uu)"), LagAtStop > 100.0);
	TestTrue(TEXT("walk: the drone keeps pace (within 500 uu)"), MaxWalkDistance <= 500.0);

	// Stop: the drone catches up with its point (within 30 uu of it), in the zone, then stays in the zone.
	constexpr int32 StopFrames = 300;
	constexpr double CatchDistance = 30.0;
	int32 CatchFrame = INDEX_NONE;
	int32 OutAfterCatch = 0;
	for (int32 Frame = 1; Frame <= StopFrames; ++Frame)
	{
		Step(FVector::ZeroVector);
		const double Distance = Offset2D().Size();
		if (CatchFrame == INDEX_NONE && DistanceToPoint() <= CatchDistance)
		{
			CatchFrame = Frame;
		}
		// Half a second to settle on the point first (it may still be just outside the zone edge when within 30 uu of it).
		if (CatchFrame != INDEX_NONE && Frame > CatchFrame + 30)
		{
			OutAfterCatch += Distance < ZoneMin - ZoneTolerance || Distance > ZoneMax + ZoneTolerance ? 1 : 0;
		}
	}
	AddInfo(FString::Printf(TEXT("walk at %.0f uu/s (min, last %d frames): drone %.1f..%.1f uu behind the miner, at most %.1f uu from it, %.1f uu from its patrol point at the stop; within %.0f uu of its point %.2f s after the stop, %d frames out of the zone afterwards"),
		MinSpeed, SettledFrames, MinBehind, MaxBehind, MaxWalkDistance, LagAtStop, CatchDistance, CatchFrame * FrameSeconds, OutAfterCatch));
	TestTrue(TEXT("stop: the drone catches up with its point within 2.5 s"), CatchFrame != INDEX_NONE && CatchFrame <= 150);
	TestEqual(TEXT("stop: the drone stays in the zone after catching up"), OutAfterCatch, 0);

	AddInfo(FString::Printf(TEXT("%d frames sampled: %d with the drone in a block cell, %d with an invalid patrol point"), SampledFrames, InBlockFrames, InvalidPointFrames));
	TestTrue(TEXT("drone stays in Orbit"), bAlwaysOrbit);
	TestEqual(TEXT("drone never in a block cell"), InBlockFrames, 0);
	TestEqual(TEXT("patrol point always valid"), InvalidPointFrames, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDroneAvoidsWallsTest, "RogueRigs.Dig.DroneAvoidsWalls",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDroneAvoidsWallsTest::RunTest(const FString& Parameters)
{
	using namespace RRDigTests;

	// task-248-01, headless position check only: with the miner next to a wall of arena_m1_fine, the drone's cell is never a block
	// cell, sampled every frame, in Orbit (rest with the rock pillar x 129-130 inside the patrol zone, walks along it; the patrol
	// point always valid, task-251-01), Dig (against
	// the pillar) and Hack (target inside the rock mass y <= 110, the plain hover point would be in the rock).
	const FIntPoint OrbitCell(127, 112);
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, OrbitCell, Grid, Miner);
	if (!Drone)
	{
		return false;
	}
	TestTrue(TEXT("scene: pillar (129, 113) and mass (125, 110) are rock, (127, 111..117) and (125, 111..112) floor"),
		Grid->IsBlockCell(FIntPoint(129, 113)) && Grid->IsBlockCell(FIntPoint(125, 110)) && !Grid->IsBlockCell(FIntPoint(125, 111))
		&& !Grid->IsBlockCell(FIntPoint(125, 112)) && !Grid->IsBlockCell(FIntPoint(127, 111)) && !Grid->IsBlockCell(FIntPoint(127, 117)));
	UCharacterMovementComponent* MinerMovement = Miner->GetCharacterMovement();
	MinerMovement->bRunPhysicsWithNoController = true;
	MinerMovement->SetDefaultMovementMode();

	struct FPhaseCount
	{
		int32 Frames = 0;
		int32 InBlock = 0;
	};
	FPhaseCount Counts[3];
	int32 InvalidPatrolFrames = 0;
	int32 FallbackFrames = 0;
	auto Sample = [&]()
	{
		FPhaseCount& Count = Counts[static_cast<int32>(Drone->GetState())];
		++Count.Frames;
		Count.InBlock += Grid->IsBlockCell(Grid->GetCellAtWorld(Drone->GetActorLocation())) ? 1 : 0;
		if (Drone->GetState() == ERRDroneState::Orbit)
		{
			// task-251-01: the patrol point is always a valid point (clear line from the miner), or the fallback above the miner.
			const bool bFallback = FDroneAccess::PatrolFallbackOf(Drone);
			FallbackFrames += bFallback ? 1 : 0;
			InvalidPatrolFrames += !bFallback && !FDroneAccess::IsPatrolOffsetClearOf(Drone, Grid, FDroneAccess::PatrolOffsetOf(Drone)) ? 1 : 0;
		}
	};
	auto Walk = [&](const FVector& Direction, int32 Frames)
	{
		for (int32 Frame = 0; Frame < Frames; ++Frame)
		{
			if (!Direction.IsZero())
			{
				FMinerAccess::PendingMoveInputOf(Miner) = Direction;
				Miner->AddMovementInput(Direction, 1.f);
			}
			TestWorld.Tick();
			Sample();
		}
	};

	// Orbit: the pillar lies 2 cells east, inside the 160 uu patrol zone; patrol points toward it must be rejected.
	Drone->SetOwnerMiner(Miner);
	Sample();
	Walk(FVector::ZeroVector, 60);
	Walk(FVector::RightVector, 40);
	Walk(-FVector::RightVector, 30);
	Walk(FVector::ZeroVector, 90);

	// Dig: against the pillar (the arc-dig scene cell).
	Miner->SetActorLocation(Grid->GetCellCenter(FineRockWestFloorCell.X, FineRockWestFloorCell.Y) + FVector(0.0, 0.0, Miner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 2.0),
		/*bSweep*/ false, nullptr, ETeleportType::TeleportPhysics);
	Walk(FVector::ZeroVector, 90);

	// Hack: miner on (125, 112), target in the rock (125, 109); HackHoverDistance toward the miner would end in (125, 110).
	Miner->SetActorLocation(Grid->GetCellCenter(125, 112) + FVector(0.0, 0.0, Miner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 2.0),
		/*bSweep*/ false, nullptr, ETeleportType::TeleportPhysics);
	Walk(FVector::ZeroVector, 30);
	AActor* Target = TestWorld.Spawn<ATargetPoint>(ATargetPoint::StaticClass(), Grid->GetCellCenter(125, 109));
	if (!TestNotNull(TEXT("hack target spawned"), Target))
	{
		return false;
	}
	TestTrue(TEXT("hack started"), Drone->StartHack(Target));
	Walk(FVector::ZeroVector, 90);
	const FIntPoint HackCell = Grid->GetCellAtWorld(Drone->GetActorLocation());
	AddInfo(FString::Printf(TEXT("hack hover cell %s, %.1f uu from the target"), *HackCell.ToString(), FVector::Dist2D(Drone->GetActorLocation(), Target->GetActorLocation())));
	Drone->StopHack(Target);
	Walk(FVector::ZeroVector, 60);

	const TCHAR* Names[3] = { TEXT("Orbit"), TEXT("Dig"), TEXT("Hack") };
	for (int32 Index = 0; Index < 3; ++Index)
	{
		AddInfo(FString::Printf(TEXT("%s: %d frames sampled, %d with the drone in a block cell"), Names[Index], Counts[Index].Frames, Counts[Index].InBlock));
		TestTrue(FString::Printf(TEXT("%s sampled"), Names[Index]), Counts[Index].Frames > 0);
		TestEqual(FString::Printf(TEXT("%s: drone never in a block cell"), Names[Index]), Counts[Index].InBlock, 0);
	}
	AddInfo(FString::Printf(TEXT("Orbit: %d frames with an invalid patrol point, %d on the fallback above the miner"), InvalidPatrolFrames, FallbackFrames));
	TestEqual(TEXT("Orbit: patrol point always valid"), InvalidPatrolFrames, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDroneAliveTest, "RogueRigs.Dig.DroneAlive",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDroneAliveTest::RunTest(const FString& Parameters)
{
	using namespace RRDigTests;

	// task-249-01, headless position check only (no capture): in Orbit the drone height stays inside the 130-190 uu band above the
	// floor (walks, stops and a 10 s rest with idle gestures, sampled every frame), and during Dig its yaw faces the dig target.
	constexpr double MinBand = 130.0;
	constexpr double MaxBand = 190.0;
	constexpr double YawTolerance = 20.0;
	{
		const FIntPoint StartCell(84, 129);
		FTestWorld TestWorld;
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, StartCell, Grid, Miner);
		if (!Drone)
		{
			return false;
		}
		UCharacterMovementComponent* MinerMovement = Miner->GetCharacterMovement();
		MinerMovement->bRunPhysicsWithNoController = true;
		MinerMovement->SetDefaultMovementMode();
		const double FloorZ = Miner->GetActorLocation().Z - Miner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		int32 OrbitFrames = 0;
		int32 OutOfBand = 0;
		int32 Gestures = 0;
		double MinHeight = TNumericLimits<double>::Max();
		double MaxHeight = TNumericLimits<double>::Lowest();
		double MaxTilt = 0.0;
		ERRDroneIdleGesture LastGesture = ERRDroneIdleGesture::None;
		auto Run = [&](const FVector& Direction, int32 Frames)
		{
			for (int32 Frame = 0; Frame < Frames; ++Frame)
			{
				if (!Direction.IsZero())
				{
					FMinerAccess::PendingMoveInputOf(Miner) = Direction;
					Miner->AddMovementInput(Direction, 1.f);
				}
				TestWorld.Tick();
				if (Drone->GetState() != ERRDroneState::Orbit)
				{
					continue;
				}
				++OrbitFrames;
				const double Height = Drone->GetActorLocation().Z - FloorZ;
				MinHeight = FMath::Min(MinHeight, Height);
				MaxHeight = FMath::Max(MaxHeight, Height);
				OutOfBand += Height < MinBand || Height > MaxBand ? 1 : 0;
				const FRotator Rotation = Drone->GetActorRotation();
				MaxTilt = FMath::Max(MaxTilt, static_cast<double>(FMath::Max(FMath::Abs(Rotation.Pitch), FMath::Abs(Rotation.Roll))));
				const ERRDroneIdleGesture Gesture = FDroneAccess::IdleGestureOf(Drone);
				Gestures += Gesture != ERRDroneIdleGesture::None && Gesture != LastGesture ? 1 : 0;
				LastGesture = Gesture;
			}
		};
		Run(FVector::ForwardVector, 120);
		Run(FVector::ZeroVector, 600);
		Run(-FVector::ForwardVector, 60);
		Run(FVector::ForwardVector, 30);
		Run(FVector::ZeroVector, 60);
		AddInfo(FString::Printf(TEXT("Orbit: %d frames, height %.1f..%.1f uu above the floor, %d out of %.0f-%.0f, max pitch or roll %.1f deg, %d idle gestures"),
			OrbitFrames, MinHeight, MaxHeight, OutOfBand, MinBand, MaxBand, MaxTilt, Gestures));
		TestEqual(TEXT("Orbit sampled every frame"), OrbitFrames, 870);
		TestEqual(TEXT("Orbit height inside 130-190 uu"), OutOfBand, 0);
	}
	{
		FTestWorld TestWorld;
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, FineRockWestFloorCell, Grid, Miner);
		if (!Drone)
		{
			return false;
		}
		// Rock wall x = 129. task-253-01: the base turns at BaseYawTurnRate (120 deg/s) at most, so it needs up to 1.5 s to face the
		// target; MiningSpeed 0.25 makes the 6 blocks last 2.6 s (frame about 156), and from frame 100 the yaw must face the impact point.
		Miner->GetRRAbilitySystemComponent()->SetNumericAttributeBase(URRPlayerAttributeSet::GetMiningSpeedAttribute(), 0.25f);
		double MaxError = 0.0;
		double FirstError = 0.0;
		int32 DigFrames = 0;
		for (int32 Frame = 1; Frame <= 115; ++Frame)
		{
			TestWorld.Tick();
			if (Drone->GetState() != ERRDroneState::Dig)
			{
				continue;
			}
			const FVector ToTarget = Drone->GetDigImpactPoint() - Drone->GetActorLocation();
			const double TargetYaw = FMath::RadiansToDegrees(FMath::Atan2(ToTarget.Y, ToTarget.X));
			const double Error = FMath::Abs(FMath::FindDeltaAngleDegrees(Drone->GetActorRotation().Yaw, TargetYaw));
			FirstError = Frame == 1 ? Error : FirstError;
			if (Frame >= 100)
			{
				++DigFrames;
				MaxError = FMath::Max(MaxError, Error);
			}
		}
		AddInfo(FString::Printf(TEXT("Dig: yaw error %.1f deg on frame 1, at most %.1f deg on frames 100-115 (%d frames in Dig)"), FirstError, MaxError, DigFrames));
		TestEqual(TEXT("Dig sampled on frames 100-115"), DigFrames, 16);
		TestTrue(TEXT("Dig: yaw faces the dig target within 20 deg"), MaxError <= YawTolerance);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDronePartsTest, "RogueRigs.Dig.DroneParts",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDronePartsTest::RunTest(const FString& Parameters)
{
	using namespace RRDigTests;

	// task-253-01, headless check only (no capture, no mesh needed): the ArcContactDig scene (miner on (128, 115) against the wall
	// x = 129) with MiningSpeed 0.25, so the 6 blocks last 2.6 s. During Dig the base yaw error to the impact point shrinks without a
	// jump (never more than BaseYawTurnRate per second), the laser arm pitch points its emitter at the impact point within
	// PitchTolerance once the yaw faces it, the antenna turns at AntennaSpinSpeed; back in Orbit the arm rests straight down.
	constexpr double PitchTolerance = 10.0;
	constexpr double ConvergedYawError = 2.0;
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, FineRockWestFloorCell, Grid, Miner);
	if (!Drone)
	{
		return false;
	}
	Miner->GetRRAbilitySystemComponent()->SetNumericAttributeBase(URRPlayerAttributeSet::GetMiningSpeedAttribute(), 0.25f);
	const double MaxYawStep = FDroneAccess::BaseYawTurnRateOf(Drone) * FrameSeconds + 0.01;
	const UStaticMeshComponent* Pivot = FDroneAccess::LaserPivotOf(Drone);
	const UStaticMeshComponent* Antenna = FDroneAccess::AntennaOf(Drone);

	auto YawError = [Drone]()
	{
		const FVector ToTarget = Drone->GetDigImpactPoint() - Drone->GetActorLocation();
		const double TargetYaw = FMath::RadiansToDegrees(FMath::Atan2(ToTarget.Y, ToTarget.X));
		return FMath::Abs(FMath::FindDeltaAngleDegrees(Drone->GetActorRotation().Yaw, TargetYaw));
	};

	int32 DigFrames = 0;
	int32 JumpFrames = 0;
	int32 GrowFrames = 0;
	int32 ConvergedFrame = INDEX_NONE;
	int32 PitchFrames = 0;
	double FirstError = 0.0;
	double MaxYawStepSeen = 0.0;
	double MaxGrowth = 0.0;
	double MaxErrorAfter = 0.0;
	double MaxPitchError = 0.0;
	double MinPitch = TNumericLimits<double>::Max();
	double MaxPitch = TNumericLimits<double>::Lowest();
	double AntennaTurn = 0.0;
	double PreviousError = -1.0;
	double PreviousYaw = Drone->GetActorRotation().Yaw;
	double PreviousAntennaYaw = Antenna->GetRelativeRotation().Yaw;
	constexpr int32 LastFrame = 115;
	constexpr int32 AntennaFirstFrame = 40;
	constexpr int32 AntennaLastFrame = 100;
	for (int32 Frame = 1; Frame <= LastFrame; ++Frame)
	{
		TestWorld.Tick();
		const double Yaw = Drone->GetActorRotation().Yaw;
		const double AntennaYaw = Antenna->GetRelativeRotation().Yaw;
		if (Frame > AntennaFirstFrame && Frame <= AntennaLastFrame)
		{
			AntennaTurn += FMath::FindDeltaAngleDegrees(PreviousAntennaYaw, AntennaYaw);
		}
		PreviousAntennaYaw = AntennaYaw;
		const double YawStep = FMath::Abs(FMath::FindDeltaAngleDegrees(PreviousYaw, Yaw));
		PreviousYaw = Yaw;
		if (Drone->GetState() != ERRDroneState::Dig)
		{
			PreviousError = -1.0;
			continue;
		}
		++DigFrames;
		MaxYawStepSeen = FMath::Max(MaxYawStepSeen, YawStep);
		JumpFrames += YawStep > MaxYawStep ? 1 : 0;
		const double Error = YawError();
		FirstError = DigFrames == 1 ? Error : FirstError;
		if (ConvergedFrame == INDEX_NONE)
		{
			// Until it faces the target, the error may only shrink (the drone's own move shifts the target direction a little).
			if (PreviousError >= 0.0 && Error > PreviousError + 0.25)
			{
				++GrowFrames;
			}
			MaxGrowth = PreviousError >= 0.0 ? FMath::Max(MaxGrowth, Error - PreviousError) : MaxGrowth;
			ConvergedFrame = Error <= ConvergedYawError ? Frame : INDEX_NONE;
		}
		else
		{
			MaxErrorAfter = FMath::Max(MaxErrorAfter, Error);
		}
		PreviousError = Error;

		// Laser arm: from 20 frames after the yaw faces the target (the pitch turns at 240 deg/s), the pivot-to-emitter axis
		// must point along the emitter-to-impact direction.
		if (ConvergedFrame != INDEX_NONE && Frame >= ConvergedFrame + 20)
		{
			++PitchFrames;
			const FVector Emitter = Drone->GetLaserEmitterLocation();
			const FVector ArmAxis = (Emitter - Pivot->GetComponentLocation()).GetSafeNormal();
			const FVector ToImpact = (Drone->GetDigImpactPoint() - Emitter).GetSafeNormal();
			const double PitchError = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(FVector::DotProduct(ArmAxis, ToImpact), -1.0, 1.0)));
			MaxPitchError = FMath::Max(MaxPitchError, PitchError);
			const double Pitch = Pivot->GetRelativeRotation().Pitch;
			MinPitch = FMath::Min(MinPitch, Pitch);
			MaxPitch = FMath::Max(MaxPitch, Pitch);
		}
	}
	CheckDigBeam(*this, Drone, TEXT("frame 115"));
	AddInfo(FString::Printf(TEXT("Dig: %d frames; yaw error %.1f deg on the first, within %.0f deg on frame %d (%.2f s), at most %.1f deg after; largest yaw step %.2f deg (cap %.2f), %d jumps, %d growing frames (largest growth %.2f deg)"),
		DigFrames, FirstError, ConvergedYawError, ConvergedFrame, ConvergedFrame * FrameSeconds, MaxErrorAfter, MaxYawStepSeen, MaxYawStep, JumpFrames, GrowFrames, MaxGrowth));
	AddInfo(FString::Printf(TEXT("Laser arm: %d frames checked, pitch %.1f..%.1f deg from straight down, emitter axis at most %.2f deg off the impact direction"),
		PitchFrames, MinPitch, MaxPitch, MaxPitchError));
	const double ExpectedAntennaTurn = FDroneAccess::AntennaSpinSpeedOf(Drone) * (AntennaLastFrame - AntennaFirstFrame) * FrameSeconds;
	AddInfo(FString::Printf(TEXT("Antenna: turned %.2f deg in %.2f s (expected %.2f)"), AntennaTurn, (AntennaLastFrame - AntennaFirstFrame) * FrameSeconds, ExpectedAntennaTurn));

	TestEqual(TEXT("Dig on every frame"), DigFrames, LastFrame);
	TestEqual(TEXT("base yaw never jumps (at most BaseYawTurnRate per second)"), JumpFrames, 0);
	TestEqual(TEXT("base yaw error never grows before it faces the target"), GrowFrames, 0);
	TestTrue(TEXT("base yaw faces the target before the blocks break"), ConvergedFrame != INDEX_NONE && ConvergedFrame <= 95);
	TestTrue(TEXT("base yaw keeps facing the target"), MaxErrorAfter <= 5.0);
	TestTrue(TEXT("laser arm checked on at least 10 frames"), PitchFrames >= 10);
	TestTrue(TEXT("laser arm aims its emitter at the impact point within 10 deg"), MaxPitchError <= PitchTolerance);
	TestTrue(TEXT("laser arm pitch inside its range"), MinPitch >= FDroneAccess::LaserPitchMinOf(Drone) - 0.01 && MaxPitch <= FDroneAccess::LaserPitchMaxOf(Drone) + 0.01);
	TestTrue(TEXT("antenna turns at AntennaSpinSpeed (within 1 deg)"), FMath::Abs(AntennaTurn - ExpectedAntennaTurn) <= 1.0);

	// Blocks break around frame 156; back in Orbit the arm returns straight down within 0.5 s.
	TArray<int32> DestroyFrames;
	const TArray<FIntPoint> Remaining = Drone->GetDigCells();
	TickUntilDestroyed(TestWorld, Grid, Remaining, 60, DestroyFrames);
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	AddInfo(FString::Printf(TEXT("Orbit: laser arm pitch %.2f deg, beam visible %d"), Pivot->GetRelativeRotation().Pitch, FDroneAccess::Laser(Drone)->IsVisible() ? 1 : 0));
	TestEqual(TEXT("back to Orbit"), Drone->GetState(), ERRDroneState::Orbit);
	TestTrue(TEXT("Orbit: laser arm straight down"), FMath::Abs(Pivot->GetRelativeRotation().Pitch) <= 0.01);
	TestFalse(TEXT("Orbit: beam hidden"), FDroneAccess::Laser(Drone)->IsVisible());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigPickupCollectTest, "RogueRigs.Dig.PickupCollect",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigPickupCollectTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, RockWestFloorCell) : nullptr;
	UClass* HealClass = LoadTestClass<ARRPickup>(*this, HealPickupClassPath);
	if (!Miner || !HealClass)
	{
		return false;
	}

	// Pickups find the miner as the player 0 pawn: a player controller (no local player) possesses it.
	APlayerController* Controller = TestWorld.Spawn<APlayerController>(APlayerController::StaticClass(), Miner->GetActorLocation());
	if (!TestNotNull(TEXT("player controller spawned"), Controller))
	{
		return false;
	}
	Controller->Possess(Miner);
	TestEqual(TEXT("miner is the player 0 pawn"), UGameplayStatics::GetPlayerPawn(TestWorld.Get(), 0), static_cast<APawn*>(Miner));
	// heal-ore (task-089-01): a heal pickup is refused at full health; the miner is damaged so the near one is collected.
	Miner->GetRRAbilitySystemComponent()->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), 100.f);
	Miner->GetRRAbilitySystemComponent()->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 50.f);

	const float Radius = Miner->GetPickupRadius();
	AddInfo(FString::Printf(TEXT("PickupRadius %.1f"), Radius));
	TestTrue(TEXT("PickupRadius loaded from the stats table"), Radius > 200.f);

	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);

	// The pop ejects a pickup PopMinDistance-PopMaxDistance uu (read from the class default, task-195-02: up to 300): spawned under
	// the miner it lands inside the radius; spawned at radius + PopMaxDistance + 100 it lands outside it whatever the pop direction.
	const FFloatProperty* PopMaxDistanceProperty = FindFProperty<FFloatProperty>(ARRPickup::StaticClass(), TEXT("PopMaxDistance"));
	if (!TestNotNull(TEXT("PopMaxDistance property"), PopMaxDistanceProperty))
	{
		return false;
	}
	const float PopMaxDistance = PopMaxDistanceProperty->GetPropertyValue_InContainer(HealClass->GetDefaultObject());
	AddInfo(FString::Printf(TEXT("PopMaxDistance %.1f uu"), PopMaxDistance));
	const FVector MinerFloor = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y);
	ARRPickup* Near = TestWorld.Spawn<ARRPickup>(HealClass, MinerFloor);
	ARRPickup* Far = TestWorld.Spawn<ARRPickup>(HealClass, MinerFloor + FVector(0.f, -(Radius + PopMaxDistance + 100.f), 0.f));
	if (!TestNotNull(TEXT("near pickup"), Near) || !TestNotNull(TEXT("far pickup"), Far))
	{
		return false;
	}
	TWeakObjectPtr<ARRPickup> NearWeak(Near);
	TWeakObjectPtr<ARRPickup> FarWeak(Far);

	// Pop and rest timing read from the spawned pickup (protected tunables, read by reflection so Blueprint overrides count).
	const FFloatProperty* PopDurationProperty = FindFProperty<FFloatProperty>(ARRPickup::StaticClass(), TEXT("PopDuration"));
	const FFloatProperty* AttractDelayProperty = FindFProperty<FFloatProperty>(ARRPickup::StaticClass(), TEXT("AttractDelay"));
	if (!TestNotNull(TEXT("PopDuration property"), PopDurationProperty) || !TestNotNull(TEXT("AttractDelay property"), AttractDelayProperty))
	{
		return false;
	}
	const float PopDuration = PopDurationProperty->GetPropertyValue_InContainer(Near);
	const float AttractDelay = AttractDelayProperty->GetPropertyValue_InContainer(Near);
	const FBoolProperty* AttractDuringPopProperty = FindFProperty<FBoolProperty>(ARRPickup::StaticClass(), TEXT("bAttractDuringPop"));
	const FFloatProperty* PopAttractDelayProperty = FindFProperty<FFloatProperty>(ARRPickup::StaticClass(), TEXT("PopAttractDelay"));
	if (!TestNotNull(TEXT("bAttractDuringPop property"), AttractDuringPopProperty) || !TestNotNull(TEXT("PopAttractDelay property"), PopAttractDelayProperty))
	{
		return false;
	}
	const bool bAttractDuringPop = AttractDuringPopProperty->GetPropertyValue_InContainer(Near);
	const float PopAttractDelay = PopAttractDelayProperty->GetPropertyValue_InContainer(Near);
	AddInfo(FString::Printf(TEXT("PopDuration %.2f s, AttractDelay %.2f s, bAttractDuringPop %d, PopAttractDelay %.2f s"), PopDuration, AttractDelay,
		bAttractDuringPop ? 1 : 0, PopAttractDelay));
	TestTrue(TEXT("AttractDelay is at least 1 s"), AttractDelay >= 1.f);
	// task-195-01: with bAttractDuringPop the wait ends at PopAttractDelay after spawn or AttractDelay after landing, whichever comes first.
	const float EarliestCollect = bAttractDuringPop ? FMath::Min(PopAttractDelay, PopDuration + AttractDelay) : PopDuration + AttractDelay;
	TestTrue(TEXT("earliest collect is at least 1 s"), EarliestCollect >= 1.f);
	// The whole pop and rest plus 2 s for the magnet flight.
	const int32 FrameCount = FMath::Max(240, FMath::CeilToInt32((PopDuration + AttractDelay + 2.f) / FrameSeconds));

	int32 CollectFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= FrameCount; ++Frame)
	{
		TestWorld.Tick();
		if (CollectFrame == INDEX_NONE && Recorder->GetCollectedPickups().Num() > 0)
		{
			CollectFrame = Frame;
		}
	}

	AddInfo(FString::Printf(TEXT("near pickup collected at frame %d (%.4f s); collected %d"), CollectFrame, CollectFrame * FrameSeconds,
		Recorder->GetCollectedPickups().Num()));
	TestEqual(TEXT("OnPickupCollected fired once"), Recorder->GetCollectedPickups().Num(), 1);
	// Frame N ends at N * FrameSeconds; allow one frame of accumulation rounding.
	TestTrue(TEXT("near pickup not collected before the attraction delay"), CollectFrame != INDEX_NONE && CollectFrame * FrameSeconds >= EarliestCollect - FrameSeconds);
	if (Recorder->GetCollectedPickups().Num() > 0)
	{
		TestEqual(TEXT("collected type Heal"), Recorder->GetCollectedPickups()[0], ERRPickupType::Heal);
	}
	TestFalse(TEXT("near pickup destroyed"), NearWeak.IsValid());
	TestTrue(TEXT("far pickup not collected"), FarWeak.IsValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigShakeKeepsCollisionTest, "RogueRigs.Dig.ShakeKeepsCollision",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigShakeKeepsCollisionTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, RockWestFloorCell) : nullptr;
	if (!Miner)
	{
		return false;
	}
	// Collision only: no drone, so touching the rock does not dig it.
	if (IsValid(Miner->GetDrone()))
	{
		Miner->GetDrone()->Destroy();
	}
	for (ARRWeapon* Weapon : Miner->GetWeapons())
	{
		if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
		{
			Weapon->GetAutoFireComponent()->StopFiring();
		}
	}

	// Task-230-07: rock is drawn by the dual-grid corner pieces (no per-cell rendering instance, shake moves to the material in
	// task-230-08); the per-cell collision cube must still never move while the cell shakes.
	const UInstancedStaticMeshComponent* RockCollisionComponent = FGridAccess::CellCollisionComponent(Grid, RockCell);
	const int32 RockIndex = FGridAccess::InstanceIndices(Grid)[RockCell.Y * FGridAccess::Width(Grid) + RockCell.X];
	if (!TestNotNull(TEXT("rock collision ISM"), RockCollisionComponent) || !TestTrue(TEXT("rock instance mapped"), RockIndex != INDEX_NONE))
	{
		return false;
	}
	TestNull(TEXT("rock has no per-cell rendering HISM (dual-grid pieces)"), FGridAccess::CellComponent(Grid, RockCell));
	TestFalse(TEXT("collision ISM is not drawn"), RockCollisionComponent->IsVisible());
	FTransform RestTransform;
	RockCollisionComponent->GetInstanceTransform(RockIndex, RestTransform, /*bWorldSpace*/ false);

	// Part 1: a miner-sized capsule swept east from the floor cell at the rock's west face hits at the same place every frame.
	const UCapsuleComponent* Capsule = Miner->GetCapsuleComponent();
	const float Radius = Capsule->GetScaledCapsuleRadius();
	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const FVector FloorCenter = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y);
	// Object query on WorldStatic: terrain only (the miner standing on the floor cell is a pawn, its drone and weapons are not swept).
	const FVector SweepStart(FloorCenter.X - 0.5f * Grid->GetCellSize() + Radius + 1.f, FloorCenter.Y, FloorCenter.Z + HalfHeight + 2.f);
	const FVector SweepEnd = SweepStart + FVector(Grid->GetCellSize(), 0.f, 0.f);
	const FVector FaceCenter = Grid->GetCellFaceCenter(RockCell, FIntPoint(-1, 0));
	const FCollisionShape Shape = FCollisionShape::MakeCapsule(Radius, HalfHeight);
	const FCollisionObjectQueryParams Objects(ECC_WorldStatic);

	Grid->SetCellShaking(RockCell, true);
	TestTrue(TEXT("rock shaking"), FGridAccess::Shaking(Grid).Contains(RockCell));
	double FirstHitX = 0.0;
	int32 HitFrames = 0;
	int32 MovedHits = 0;
	int32 WrongCellHits = 0;
	float MaxCollisionOffset = 0.f;
	constexpr int32 SweepFrames = 30;
	for (int32 Frame = 0; Frame < SweepFrames; ++Frame)
	{
		TestWorld.Tick();
		FTransform CollisionTransform;
		RockCollisionComponent->GetInstanceTransform(RockIndex, CollisionTransform, /*bWorldSpace*/ false);
		MaxCollisionOffset = FMath::Max(MaxCollisionOffset, static_cast<float>(FVector::Dist(CollisionTransform.GetLocation(), RestTransform.GetLocation())));

		FHitResult Hit;
		if (!TestWorld.Get()->SweepSingleByObjectType(Hit, SweepStart, SweepEnd, FQuat::Identity, Objects, Shape) || Hit.bStartPenetrating)
		{
			continue;
		}
		++HitFrames;
		WrongCellHits += Grid->GetCellFromHit(Hit) == RockCell && Hit.GetComponent() == RockCollisionComponent ? 0 : 1;
		if (HitFrames == 1)
		{
			FirstHitX = Hit.Location.X;
		}
		MovedHits += FMath::Abs(Hit.Location.X - FirstHitX) <= 0.01 ? 0 : 1;
	}
	AddInfo(FString::Printf(TEXT("sweep: %d / %d frames hit, first hit at x %.3f (face x %.1f, radius %.1f), %d moved; collision offset max %.4f uu"),
		HitFrames, SweepFrames, FirstHitX, FaceCenter.X, Radius, MovedHits, MaxCollisionOffset));
	TestTrue(TEXT("the collision never moves"), MaxCollisionOffset < 1e-3f);
	TestEqual(TEXT("the sweep hits the shaking rock every frame"), HitFrames, SweepFrames);
	TestEqual(TEXT("each hit is the rock's collision instance"), WrongCellHits, 0);
	TestEqual(TEXT("the hit position never moves"), MovedHits, 0);
	TestTrue(TEXT("the hit is at the rock's west face"), FMath::Abs(FirstHitX + Radius - FaceCenter.X) <= 1.0);

	// Part 2: the miner walks east into the shaking rock and keeps pushing; it must never be pushed back.
	UCharacterMovementComponent* Movement = Miner->GetCharacterMovement();
	Movement->bRunPhysicsWithNoController = true;
	Movement->SetDefaultMovementMode();
	for (int32 Frame = 0; Frame < 20; ++Frame)
	{
		TestWorld.Tick();
	}
	const double ContactX = FaceCenter.X - Radius;
	double PreviousX = Miner->GetActorLocation().X;
	double MaxX = PreviousX;
	double WorstBackStep = 0.0;
	int32 ContactFrames = 0;
	constexpr int32 PushFrames = 60;
	for (int32 Frame = 0; Frame < PushFrames && Grid->IsBlockCell(RockCell); ++Frame)
	{
		Grid->SetCellShaking(RockCell, true);
		Miner->AddMovementInput(FVector::ForwardVector, 1.f);
		TestWorld.Tick();
		const double X = Miner->GetActorLocation().X;
		WorstBackStep = FMath::Max(WorstBackStep, PreviousX - X);
		MaxX = FMath::Max(MaxX, X);
		ContactFrames += X >= ContactX - 1.0 ? 1 : 0;
		PreviousX = X;
	}
	AddInfo(FString::Printf(TEXT("push: contact x %.2f, max x %.2f, %d frames in contact, worst backward step %.4f uu"), ContactX, MaxX, ContactFrames, WorstBackStep));
	TestTrue(TEXT("the rock still stands"), Grid->IsBlockCell(RockCell));
	TestTrue(TEXT("the miner reached the rock"), ContactFrames >= 30);
	TestTrue(TEXT("the miner never entered the rock"), MaxX <= ContactX + 1.0);
	TestTrue(TEXT("the miner never moved backward"), WorstBackStep <= 0.01);

	Grid->SetCellShaking(RockCell, false);
	TestWorld.Tick();
	TestTrue(TEXT("instance maps consistent after the shake"), CheckInstanceMaps(*this, Grid));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigChunkedInstancesTest, "RogueRigs.Dig.ChunkedInstances",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigChunkedInstancesTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	if (!Grid)
	{
		return false;
	}

	// The 80 x 80 arena splits in several chunks; a destruction only changes the collision chunk of its cell and the piece
	// HISMs of the chunks of its 4 corners (task-230-07); the floor slab is already there.
	TMap<const UInstancedStaticMeshComponent*, int32> CountsBefore;
	int32 RockCollisionChunks = 0;
	for (const FRRGridChunkVisual& Visual : FGridAccess::Visuals(Grid))
	{
		if (Visual.Render)
		{
			CountsBefore.Add(Visual.Render, Visual.Render->GetInstanceCount());
		}
		if (Visual.Collision)
		{
			CountsBefore.Add(Visual.Collision, Visual.Collision->GetInstanceCount());
			RockCollisionChunks += Visual.MapKey == ERRCellType::Rock ? 1 : 0;
		}
	}
	TSet<FIntPoint> PieceChunks;
	for (const FRRGridPieceVisual& Visual : FGridAccess::PieceVisualsOf(Grid))
	{
		if (Visual.Render)
		{
			CountsBefore.Add(Visual.Render, Visual.Render->GetInstanceCount());
			PieceChunks.Add(Visual.Chunk);
		}
	}
	TestTrue(TEXT("rock collision split over several chunk ISMs"), RockCollisionChunks > 1);
	TestTrue(TEXT("rock pieces split over several chunks"), PieceChunks.Num() > 1);

	const UInstancedStaticMeshComponent* RockCollisionChunk = FGridAccess::CellCollisionComponent(Grid, RockCell);
	if (!TestNotNull(TEXT("rock cell collision chunk ISM"), RockCollisionChunk) || !TestTrue(TEXT("destroyed"), Grid->DestroyCell(RockCell, ERRTerrainDamageSource::Drone)))
	{
		return false;
	}
	TestNotNull(TEXT("floor chunk HISM of the destroyed cell"), FGridAccess::CellComponent(Grid, RockCell));

	const int32 Side = FGridAccess::ChunkSide(Grid);
	TSet<FIntPoint> CornerChunks;
	for (int32 DY = 0; DY <= 1; ++DY)
	{
		for (int32 DX = 0; DX <= 1; ++DX)
		{
			CornerChunks.Add(FIntPoint((RockCell.X + DX) / Side, (RockCell.Y + DY) / Side));
		}
	}
	int32 ChangedCells = 0;
	for (const FRRGridChunkVisual& Visual : FGridAccess::Visuals(Grid))
	{
		const UInstancedStaticMeshComponent* const Components[] = { Visual.Render.Get(), Visual.Collision.Get() };
		for (const UInstancedStaticMeshComponent* Component : Components)
		{
			if (!Component)
			{
				continue;
			}
			const int32* Before = CountsBefore.Find(Component);
			const int32 Delta = Component->GetInstanceCount() - (Before ? *Before : 0);
			if (Delta != 0)
			{
				++ChangedCells;
				TestTrue(TEXT("only the rock collision chunk of the cell changed"), Component == RockCollisionChunk && Delta == -1);
			}
		}
	}
	TestEqual(TEXT("one cell component changed (rock collision -1; no floor added)"), ChangedCells, 1);
	int32 ChangedPieces = 0;
	for (const FRRGridPieceVisual& Visual : FGridAccess::PieceVisualsOf(Grid))
	{
		const int32* Before = Visual.Render ? CountsBefore.Find(Visual.Render) : nullptr;
		const int32 Delta = Visual.Render ? Visual.Render->GetInstanceCount() - (Before ? *Before : 0) : 0;
		if (Delta != 0)
		{
			++ChangedPieces;
			TestTrue(TEXT("only piece ISMs of the corner chunks changed"), CornerChunks.Contains(Visual.Chunk));
		}
	}
	AddInfo(FString::Printf(TEXT("%d piece ISM(s) changed in %d corner chunk(s)"), ChangedPieces, CornerChunks.Num()));
	TestTrue(TEXT("the corner pieces changed"), ChangedPieces > 0);
	TestTrue(TEXT("instance maps consistent"), CheckInstanceMaps(*this, Grid));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigRockPiecesTest, "RogueRigs.Dig.RockPieces",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigRockPiecesTest::RunTest(const FString& Parameters)
{
	// Game grid with the BP_ArenaGrid defaults: no piece mesh assigned yet (task-230-11), pieces placed and tracked anyway.
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnFineGrid(*this, TestWorld);
	if (!Grid)
	{
		return false;
	}
	TestFalse(TEXT("blocks stand on the floor top (bBlocksFillFloorSlab false)"), FGridAccess::BlocksFillFloorSlabOf(Grid));
	int32 NonVoid = 0;
	for (int32 Y = 0; Y < Grid->GetGridSize().Y; ++Y)
	{
		for (int32 X = 0; X < Grid->GetGridSize().X; ++X)
		{
			NonVoid += Grid->GetCellType(FIntPoint(X, Y)) != ERRCellType::Void ? 1 : 0;
		}
	}
	TestEqual(TEXT("one floor slab per non-void cell"), FGridAccess::InstanceCount(Grid, ERRCellType::Floor), NonVoid);
	const int32 Pieces = FGridAccess::PieceInstanceCount(Grid);
	AddInfo(FString::Printf(TEXT("%d non-void cells, %d rock pieces"), NonVoid, Pieces));
	TestTrue(TEXT("rock pieces placed without meshes"), Pieces > 0);
	TestTrue(TEXT("instance maps consistent at build"), CheckInstanceMaps(*this, Grid));

	// Ore and heal cells render as rock pieces: every corner of a heal ore cell has a piece.
	const int32 CornerWidth = Grid->GetGridSize().X + 1;
	TestEqual(TEXT("heal ore cell"), Grid->GetCellType(FineHealOreCell), ERRCellType::HealOre);
	for (int32 DY = 0; DY <= 1; ++DY)
	{
		for (int32 DX = 0; DX <= 1; ++DX)
		{
			const int32 CornerIndex = (FineHealOreCell.Y + DY) * CornerWidth + FineHealOreCell.X + DX;
			TestTrue(TEXT("heal ore corner has a rock piece"), FGridAccess::CornerVisuals(Grid)[CornerIndex] != INDEX_NONE);
		}
	}

	// Several variants: two meshes per shape, variant chosen per corner by hash, the same on every rebuild.
	UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMesh* Cylinder = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (!TestNotNull(TEXT("engine cube"), Cube) || !TestNotNull(TEXT("engine cylinder"), Cylinder))
	{
		return false;
	}
	FRRRockPieceMeshes& Meshes = FGridAccess::RockPieceMeshesOf(Grid);
	for (TArray<TObjectPtr<UStaticMesh>>* List : { &Meshes.Full, &Meshes.Edge, &Meshes.Outer, &Meshes.Inner, &Meshes.Diagonal })
	{
		*List = { Cube, Cylinder };
	}
	FGridAccess::Rebuild(Grid);
	TestEqual(TEXT("same piece count with two variants"), FGridAccess::PieceInstanceCount(Grid), Pieces);
	int32 FullVariants[2] = { 0, 0 };
	int32 MeshErrors = 0;
	for (const FRRGridPieceVisual& Visual : FGridAccess::PieceVisualsOf(Grid))
	{
		if (!Visual.Render)
		{
			continue;
		}
		MeshErrors += Visual.Render->GetStaticMesh() == (Visual.Variant == 0 ? Cube : Cylinder) ? 0 : 1;
		if (Visual.Shape == ERRRockPieceShape::Full && Visual.Variant >= 0 && Visual.Variant < 2)
		{
			FullVariants[Visual.Variant] += Visual.Render->GetInstanceCount();
		}
	}
	AddInfo(FString::Printf(TEXT("full pieces: %d variant 0, %d variant 1"), FullVariants[0], FullVariants[1]));
	TestEqual(TEXT("each piece ISM uses the mesh of its variant"), MeshErrors, 0);
	TestTrue(TEXT("both variants used"), FullVariants[0] > 0 && FullVariants[1] > 0);
	TestTrue(TEXT("instance maps consistent with variants"), CheckInstanceMaps(*this, Grid));
	TArray<ERRRockPieceShape> ShapesBefore;
	TArray<int32> VariantsBefore;
	for (const int32 VisualIndex : FGridAccess::CornerVisuals(Grid))
	{
		const FRRGridPieceVisual* Visual = VisualIndex != INDEX_NONE ? &FGridAccess::PieceVisualsOf(Grid)[VisualIndex] : nullptr;
		ShapesBefore.Add(Visual ? Visual->Shape : ERRRockPieceShape::Full);
		VariantsBefore.Add(Visual ? Visual->Variant : -1);
	}
	FGridAccess::Rebuild(Grid);
	int32 Differences = 0;
	for (int32 CornerIndex = 0; CornerIndex < FGridAccess::CornerVisuals(Grid).Num(); ++CornerIndex)
	{
		const int32 VisualIndex = FGridAccess::CornerVisuals(Grid)[CornerIndex];
		const FRRGridPieceVisual* Visual = VisualIndex != INDEX_NONE ? &FGridAccess::PieceVisualsOf(Grid)[VisualIndex] : nullptr;
		Differences += (Visual ? Visual->Variant : -1) != VariantsBefore[CornerIndex] || (Visual && Visual->Shape != ShapesBefore[CornerIndex]) ? 1 : 0;
	}
	TestEqual(TEXT("deterministic variants on rebuild"), Differences, 0);

	// A broken cell changes its 4 corner pieces only.
	TArray<ERRRockPieceShape> Shapes;
	TArray<int32> Present;
	for (const int32 VisualIndex : FGridAccess::CornerVisuals(Grid))
	{
		Shapes.Add(VisualIndex != INDEX_NONE ? FGridAccess::PieceVisualsOf(Grid)[VisualIndex].Shape : ERRRockPieceShape::Full);
		Present.Add(VisualIndex != INDEX_NONE ? 1 : 0);
	}
	if (!TestTrue(TEXT("rock destroyed"), Grid->DestroyCell(FineRockCell, ERRTerrainDamageSource::Drone)))
	{
		return false;
	}
	int32 ChangedCorners = 0;
	int32 ChangedElsewhere = 0;
	for (int32 CornerIndex = 0; CornerIndex < FGridAccess::CornerVisuals(Grid).Num(); ++CornerIndex)
	{
		const int32 VisualIndex = FGridAccess::CornerVisuals(Grid)[CornerIndex];
		const bool bChanged = (VisualIndex != INDEX_NONE ? 1 : 0) != Present[CornerIndex]
			|| (VisualIndex != INDEX_NONE && FGridAccess::PieceVisualsOf(Grid)[VisualIndex].Shape != Shapes[CornerIndex]);
		const FIntPoint Corner(CornerIndex % CornerWidth, CornerIndex / CornerWidth);
		const bool bCellCorner = (Corner.X == FineRockCell.X || Corner.X == FineRockCell.X + 1) && (Corner.Y == FineRockCell.Y || Corner.Y == FineRockCell.Y + 1);
		ChangedCorners += bChanged && bCellCorner ? 1 : 0;
		ChangedElsewhere += bChanged && !bCellCorner ? 1 : 0;
	}
	AddInfo(FString::Printf(TEXT("%d corner piece(s) of (129, 115) changed"), ChangedCorners));
	TestEqual(TEXT("the 4 corner pieces of the broken cell changed"), ChangedCorners, 4);
	TestEqual(TEXT("no other corner changed"), ChangedElsewhere, 0);
	TestTrue(TEXT("instance maps consistent after destruction"), CheckInstanceMaps(*this, Grid));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigFineGridDefaultsTest, "RogueRigs.Dig.FineGridDefaults",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigFineGridDefaultsTest::RunTest(const FString& Parameters)
{
	// BP_ArenaGrid with no override: the game loads arena_m1_fine at 67 uu (task-029-04), same world size as 80 x 80 cells of 200 uu.
	FTestWorld TestWorld;
	UClass* GridClass = LoadTestClass<ARRArenaGrid>(*this, ArenaGridClassPath);
	ARRArenaGrid* Grid = GridClass ? TestWorld.Spawn<ARRArenaGrid>(GridClass, FVector::ZeroVector) : nullptr;
	URRExplosionSubsystem* Explosions = TestWorld.Get()->GetSubsystem<URRExplosionSubsystem>();
	if (!TestNotNull(TEXT("grid spawned"), Grid) || !TestNotNull(TEXT("explosion subsystem"), Explosions))
	{
		return false;
	}
	TestEqual(TEXT("cell size 67 uu"), Grid->GetCellSize(), FineCellSize);
	TestEqual(TEXT("240 x 240 cells"), Grid->GetGridSize(), FIntPoint(240, 240));
	TestNearlyEqual(TEXT("world side within 1 % of 80 x 200 uu"), Grid->GetGridSize().X * Grid->GetCellSize(), 16000.f, 160.f);
	TestEqual(TEXT("spawn cell (121, 121)"), Grid->GetSpawnCell(), FIntPoint(121, 121));

	int32 HealOre = 0;
	for (int32 Y = 0; Y < Grid->GetGridSize().Y; ++Y)
	{
		for (int32 X = 0; X < Grid->GetGridSize().X; ++X)
		{
			HealOre += Grid->GetCellType(FIntPoint(X, Y)) == ERRCellType::HealOre ? 1 : 0;
		}
	}
	TestEqual(TEXT("48 heal ore cells (12 veins of 2 x 2, task-088-01)"), HealOre, 48);
	TestNearlyEqual(TEXT("rock 0.65 s"), Grid->GetCellHealth(FineRockCell), 0.65f);
	TestEqual(TEXT("one pickup per broken ore block"), FGridAccess::DropsOnDestroy(Grid), 1);

	// Explosion radii stay in uu (ExplosionStats.csv rows Barrel, Kamikaze): the barrel clears every block center within its Radius.
	FRRExplosionStatsRow Barrel;
	FRRExplosionStatsRow Kamikaze;
	TestTrue(TEXT("Barrel row"), Explosions->GetExplosionStats(TEXT("Barrel"), Barrel));
	TestTrue(TEXT("Kamikaze row"), Explosions->GetExplosionStats(TEXT("Kamikaze"), Kamikaze));
	TestEqual(TEXT("barrel radius in uu from the CSV"), Barrel.Radius, RRTestStats::ExplosionRow(*this, RRTestStats::BarrelRow).Radius);
	TestEqual(TEXT("kamikaze radius in uu from the CSV"), Kamikaze.Radius, RRTestStats::ExplosionRow(*this, RRTestStats::KamikazeRow).Radius);
	const FVector Center = Grid->GetCellCenter(FineRockCell.X, FineRockCell.Y);
	const int32 Reach = FMath::CeilToInt32(Barrel.Radius / Grid->GetCellSize()) + 1;
	TArray<FIntPoint> Inside;
	for (int32 DY = -Reach; DY <= Reach; ++DY)
	{
		for (int32 DX = -Reach; DX <= Reach; ++DX)
		{
			const FIntPoint Cell(FineRockCell.X + DX, FineRockCell.Y + DY);
			if (Grid->IsBlockCell(Cell) && FVector::Dist2D(Grid->GetCellCenter(Cell.X, Cell.Y), Center) <= Barrel.Radius)
			{
				Inside.Add(Cell);
			}
		}
	}
	AddInfo(FString::Printf(TEXT("%d blocks within %.0f uu of (129, 115)"), Inside.Num(), Barrel.Radius));
	TestTrue(TEXT("several blocks in the radius"), Inside.Num() > 10);
	// Task-187-01: a deposit touched by the radius breaks whole, its blocks outside the radius included.
	TSet<FIntPoint> Expected(Inside);
	for (const FIntPoint& Cell : Inside)
	{
		TArray<FIntPoint> DepositCells;
		Grid->GetDepositCells(Cell, DepositCells);
		Expected.Append(DepositCells);
	}
	AddInfo(FString::Printf(TEXT("%d blocks with the whole deposits"), Expected.Num()));
	TestEqual(TEXT("every block in the 700 uu radius destroyed, touched deposits whole"), Grid->DestroyCellsInRadius(Center, Barrel.Radius, ERRTerrainDamageSource::Barrel), Expected.Num());
	int32 Standing = 0;
	for (const FIntPoint& Cell : Expected)
	{
		Standing += Grid->IsBlockCell(Cell) ? 1 : 0;
	}
	TestEqual(TEXT("none of them left standing"), Standing, 0);
	TestTrue(TEXT("instance maps consistent"), CheckInstanceMaps(*this, Grid));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigRockDamageDataTest, "RogueRigs.Dig.RockDamageData",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigRockDamageDataTest::RunTest(const FString& Parameters)
{
	// Task-230-08 (rock-render-damage): per-quadrant piece custom data, broken set swap, shake and outline as data, ore crystals.
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnFineGrid(*this, TestWorld);
	UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMesh* Cylinder = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (!Grid || !TestNotNull(TEXT("engine cube"), Cube) || !TestNotNull(TEXT("engine cylinder"), Cylinder))
	{
		return false;
	}
	// Distinct meshes per set so the swap shows in the HISM mesh: intact cube, broken cylinder (real meshes: task-230-11).
	FRRRockPieceMeshes& IntactMeshes = FGridAccess::RockPieceMeshesOf(Grid);
	FRRRockPieceMeshes& BrokenMeshes = FGridAccess::RockPieceBrokenMeshesOf(Grid);
	for (TArray<TObjectPtr<UStaticMesh>>* List : { &IntactMeshes.Full, &IntactMeshes.Edge, &IntactMeshes.Outer, &IntactMeshes.Inner, &IntactMeshes.Diagonal })
	{
		*List = { Cube };
	}
	for (TArray<TObjectPtr<UStaticMesh>>* List : { &BrokenMeshes.Full, &BrokenMeshes.Edge, &BrokenMeshes.Outer, &BrokenMeshes.Inner, &BrokenMeshes.Diagonal })
	{
		*List = { Cylinder };
	}
	FGridAccess::Rebuild(Grid);
	const int32 Pieces = FGridAccess::PieceInstanceCount(Grid);
	TestTrue(TEXT("instance maps and piece data consistent at build"), CheckInstanceMaps(*this, Grid));

	auto CountBroken = [Grid]()
	{
		int32 Count = 0;
		for (const FRRGridPieceVisual& Visual : FGridAccess::PieceVisualsOf(Grid))
		{
			Count += Visual.Render && Visual.Set == ERRRockPieceSet::Broken ? Visual.Render->GetInstanceCount() : 0;
		}
		return Count;
	};
	TestEqual(TEXT("no broken piece at build"), CountBroken(), 0);

	const FIntPoint Cell = FineRockCell;
	const FIntPoint Corners[] = { Cell, Cell + FIntPoint(1, 0), Cell + FIntPoint(0, 1), Cell + FIntPoint(1, 1) };
	TestEqual(TEXT("(129, 115) is rock"), Grid->GetCellType(Cell), ERRCellType::Rock);

	// First damage: 0.13 of 0.65 dig-seconds, d = 0.2; the 4 corner pieces switch to the broken set at once.
	Grid->ApplyTerrainDamage(Cell, 0.13f, ERRTerrainDamageSource::Drone);
	TestNearlyEqual(TEXT("display damage 0.2 after 0.13 of 0.65 dig-seconds"), Grid->GetCellDisplayDamage(Cell), 0.2f, 1e-4f);
	TestNearlyEqual(TEXT("empty west neighbour derives the largest solid neighbour damage (provisional)"), Grid->GetCellDisplayDamage(FineRockWestFloorCell), 0.2f, 1e-4f);
	for (const FIntPoint& Corner : Corners)
	{
		const FRRGridPieceVisual* Visual = FGridAccess::CornerPiece(Grid, Corner);
		if (!TestNotNull(*FString::Printf(TEXT("corner (%d, %d) has a piece"), Corner.X, Corner.Y), Visual))
		{
			continue;
		}
		TestEqual(*FString::Printf(TEXT("corner (%d, %d) broken"), Corner.X, Corner.Y), Visual->Set, ERRRockPieceSet::Broken);
		TestTrue(*FString::Printf(TEXT("corner (%d, %d) uses the broken mesh"), Corner.X, Corner.Y), Visual->Render && Visual->Render->GetStaticMesh() == Cylinder);
		TestNearlyEqual(*FString::Printf(TEXT("corner (%d, %d) damage of the dug quadrant"), Corner.X, Corner.Y),
			FGridAccess::PieceData(Grid, Corner, ARRArenaGrid::PieceDamageCustomDataIndex + FGridAccess::QuadrantOf(Corner, Cell)), 0.2f, 1e-4f);
	}
	TestEqual(TEXT("only the 4 corner pieces broke"), CountBroken(), 4);
	TestEqual(TEXT("swap one for one: same piece count"), FGridAccess::PieceInstanceCount(Grid), Pieces);
	TestTrue(TEXT("instance maps and piece data consistent after the swap"), CheckInstanceMaps(*this, Grid));

	// More damage: the floats follow, no further swap.
	Grid->ApplyTerrainDamage(Cell, 0.13f, ERRTerrainDamageSource::Drone);
	TestNearlyEqual(TEXT("damage float 0.4"), FGridAccess::PieceData(Grid, Cell, ARRArenaGrid::PieceDamageCustomDataIndex + 3), 0.4f, 1e-4f);
	TestEqual(TEXT("still 4 broken pieces"), CountBroken(), 4);

	// Shake as custom data (rock-render-top-texture option C): the quadrant float is set, the piece instances never move.
	const FRRGridPieceVisual* CellPiece = FGridAccess::CornerPiece(Grid, Cell);
	const int32 CellPieceInstance = FGridAccess::CornerInstances(Grid)[Cell.Y * (FGridAccess::Width(Grid) + 1) + Cell.X];
	FTransform RestTransform;
	if (CellPiece && CellPiece->Render)
	{
		CellPiece->Render->GetInstanceTransform(CellPieceInstance, RestTransform, /*bWorldSpace*/ false);
	}
	Grid->SetCellShaking(Cell, true);
	for (const FIntPoint& Corner : Corners)
	{
		const float Shake = FGridAccess::PieceData(Grid, Corner, ARRArenaGrid::PieceShakeCustomDataIndex + FGridAccess::QuadrantOf(Corner, Cell));
		TestTrue(*FString::Printf(TEXT("corner (%d, %d) shake float in [1, 2) (%.4f)"), Corner.X, Corner.Y, Shake), Shake >= 1.f && Shake < 2.f);
	}
	TestEqual(TEXT("a still neighbour quadrant has no shake"), FGridAccess::PieceData(Grid, Cell + FIntPoint(1, 0), ARRArenaGrid::PieceShakeCustomDataIndex + 1), 0.f);
	float MaxMove = 0.f;
	for (int32 Frame = 0; Frame < 10; ++Frame)
	{
		TestWorld.Tick();
		FTransform Now;
		if (CellPiece && CellPiece->Render)
		{
			CellPiece->Render->GetInstanceTransform(CellPieceInstance, Now, /*bWorldSpace*/ false);
			MaxMove = FMath::Max(MaxMove, static_cast<float>(FVector::Dist(Now.GetLocation(), RestTransform.GetLocation())));
		}
	}
	TestTrue(TEXT("the shaking cell's piece instance never moves"), MaxMove < 1e-3f);
	Grid->SetCellShaking(Cell, false);
	TestEqual(TEXT("shake float back to 0"), FGridAccess::PieceData(Grid, Cell, ARRArenaGrid::PieceShakeCustomDataIndex + 3), 0.f);

	// Outline as custom data on the marked quadrant of the 4 corner pieces.
	TestTrue(TEXT("marked"), Grid->SetCellMarked(Cell, true));
	TestEqual(TEXT("piece outline data on the 4 corners"), FGridAccess::GetPieceMarkedData(Grid, Cell), FGridAccess::MarkedValueOf(Grid));
	TestTrue(TEXT("unmarked"), Grid->SetCellMarked(Cell, false));
	TestEqual(TEXT("piece outline data cleared"), FGridAccess::GetPieceMarkedData(Grid, Cell), 0.f);

	// Destruction: the remaining corner pieces stay broken (never back); the empty cell derives from undamaged neighbours.
	TestTrue(TEXT("rock destroyed"), Grid->DestroyCell(Cell, ERRTerrainDamageSource::Drone));
	int32 IntactCorners = 0;
	for (const FIntPoint& Corner : Corners)
	{
		const FRRGridPieceVisual* Visual = FGridAccess::CornerPiece(Grid, Corner);
		IntactCorners += Visual && Visual->Set != ERRRockPieceSet::Broken ? 1 : 0;
	}
	TestEqual(TEXT("no corner piece of the destroyed cell went back to intact"), IntactCorners, 0);
	TestTrue(TEXT("broken pieces only at those corners"), CountBroken() <= 4);
	TestNearlyEqual(TEXT("destroyed cell display damage from its undamaged neighbours"), Grid->GetCellDisplayDamage(Cell), 0.f, 1e-6f);
	TestTrue(TEXT("instance maps and piece data consistent after destruction"), CheckInstanceMaps(*this, Grid));

	// Ore crystals: one cluster per ore and heal ore cell in their chunk HISMs, SM_OreChunk_01 by default, on the block top.
	int32 OreCells = 0;
	int32 HealCells = 0;
	for (int32 Y = 0; Y < Grid->GetGridSize().Y; ++Y)
	{
		for (int32 X = 0; X < Grid->GetGridSize().X; ++X)
		{
			OreCells += Grid->GetCellType(FIntPoint(X, Y)) == ERRCellType::Ore ? 1 : 0;
			HealCells += Grid->GetCellType(FIntPoint(X, Y)) == ERRCellType::HealOre ? 1 : 0;
		}
	}
	AddInfo(FString::Printf(TEXT("%d ore and %d heal ore cells"), OreCells, HealCells));
	TestTrue(TEXT("ore cells present"), OreCells > 0);
	TestEqual(TEXT("one crystal per ore cell"), FGridAccess::InstanceCount(Grid, ERRCellType::Ore), OreCells);
	TestEqual(TEXT("one collision or crystal instance per heal ore cell"), FGridAccess::InstanceCount(Grid, ERRCellType::HealOre), HealCells);
	UStaticMesh* CrystalMesh = FGridAccess::OreCrystalMeshOf(Grid);
	TestTrue(TEXT("crystal mesh is SM_OreChunk_01"), CrystalMesh && CrystalMesh->GetName() == TEXT("SM_OreChunk_01"));
	// Heal ore with deposit meshes (BP_ArenaGrid since task-234-04): one formation per heal deposit in the deposit ISMs instead
	// of per-cell clusters (task-237-01). Empty list: one cluster per cell in the heal chunk HISMs.
	if (FGridAccess::UsesDepositCrystalsOf(Grid, ERRCellType::HealOre))
	{
		int32 HealDeposits = 0;
		for (int32 DepositId = 0; DepositId < Grid->GetDepositCount(); ++DepositId)
		{
			HealDeposits += Grid->GetDeposit(DepositId)->CellType == ERRCellType::HealOre ? 1 : 0;
		}
		AddInfo(FString::Printf(TEXT("heal ore on deposit formations: %d heal deposits"), HealDeposits));
		TestEqual(TEXT("one formation per heal deposit"), FGridAccess::DepositCrystalCount(Grid, ERRCellType::HealOre), HealDeposits);
		const int32 HealDepositId = Grid->GetDepositId(FineHealOreCell);
		int32 HealFormation = INDEX_NONE;
		UInstancedStaticMeshComponent* HealFormations = HealDepositId != INDEX_NONE ? FGridAccess::DepositCrystalOf(Grid, HealDepositId, HealFormation) : nullptr;
		if (TestNotNull(TEXT("heal deposit formation ISM"), HealFormations))
		{
			TestEqual(TEXT("heal formation ISM never collides"), HealFormations->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
			TestFalse(TEXT("heal formation casts no shadow (bDepositCrystalsCastShadow false)"), HealFormations->CastShadow);
		}
		TestTrue(TEXT("heal ore destroyed"), Grid->DestroyCell(FineHealOreCell, ERRTerrainDamageSource::Drone));
		TestEqual(TEXT("its formation removed"), FGridAccess::DepositCrystalCount(Grid, ERRCellType::HealOre), HealDeposits - 1);
		TestTrue(TEXT("instance maps consistent after the formation removal"), CheckInstanceMaps(*this, Grid));
	}
	else
	{
		UHierarchicalInstancedStaticMeshComponent* HealCrystals = FGridAccess::CellComponent(Grid, FineHealOreCell);
		const int32 HealIndex = FGridAccess::InstanceIndices(Grid)[FineHealOreCell.Y * FGridAccess::Width(Grid) + FineHealOreCell.X];
		if (TestNotNull(TEXT("heal ore crystal HISM"), HealCrystals) && TestTrue(TEXT("heal crystal mapped"), HealIndex != INDEX_NONE))
		{
			TestTrue(TEXT("heal crystal uses the crystal mesh"), HealCrystals->GetStaticMesh() == CrystalMesh);
			TestEqual(TEXT("crystal HISM never collides"), HealCrystals->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
			FTransform CrystalTransform;
			HealCrystals->GetInstanceTransform(HealIndex, CrystalTransform, /*bWorldSpace*/ false);
			TestTrue(TEXT("crystal stands on the block top"), CrystalTransform.GetLocation().Z >= Grid->GetBlockHeight() - 1.0);
		}
		TestTrue(TEXT("heal ore destroyed"), Grid->DestroyCell(FineHealOreCell, ERRTerrainDamageSource::Drone));
		TestEqual(TEXT("its crystal removed"), FGridAccess::InstanceCount(Grid, ERRCellType::HealOre), HealCells - 1);
		TestTrue(TEXT("instance maps consistent after the crystal removal"), CheckInstanceMaps(*this, Grid));
	}

	// Deposit crystals (task-234-03): with Duskite deposit meshes, one crystal per ore deposit replaces the per-cell clusters
	// (null list entries skipped); heal ore, whose list stays empty, keeps one cluster per cell. Stand-in meshes: SM_OreChunk_01..03.
	TArray<TObjectPtr<UStaticMesh>>& DepositMeshes = FGridAccess::OreDepositMeshesOf(Grid);
	DepositMeshes.Reset();
	for (const TCHAR* Path : { TEXT("/Game/RogueRigs/Art/OreChunk/SM_OreChunk_01.SM_OreChunk_01"), TEXT(""),
		TEXT("/Game/RogueRigs/Art/OreChunk/SM_OreChunk_02.SM_OreChunk_02"), TEXT("/Game/RogueRigs/Art/OreChunk/SM_OreChunk_03.SM_OreChunk_03") })
	{
		DepositMeshes.Add(*Path ? LoadObject<UStaticMesh>(nullptr, Path) : nullptr);
	}
	FGridAccess::HealDepositMeshesOf(Grid).Reset();
	FGridAccess::Rebuild(Grid);
	TestTrue(TEXT("ore uses deposit crystals"), FGridAccess::UsesDepositCrystalsOf(Grid, ERRCellType::Ore));
	TestFalse(TEXT("heal ore keeps per-cell clusters (empty list)"), FGridAccess::UsesDepositCrystalsOf(Grid, ERRCellType::HealOre));
	TArray<int32> OreDeposits;
	for (int32 DepositId = 0; DepositId < Grid->GetDepositCount(); ++DepositId)
	{
		if (Grid->GetDeposit(DepositId)->CellType == ERRCellType::Ore)
		{
			OreDeposits.Add(DepositId);
		}
	}
	AddInfo(FString::Printf(TEXT("%d ore deposits, %d deposit crystal ISMs"), OreDeposits.Num(), FGridAccess::DepositVisualsOf(Grid).Num()));
	TestTrue(TEXT("ore deposits present"), OreDeposits.Num() > 0);
	TestEqual(TEXT("one crystal per ore deposit"), FGridAccess::DepositCrystalCount(Grid, ERRCellType::Ore), OreDeposits.Num());
	TestEqual(TEXT("ore collision still one per cell"), FGridAccess::InstanceCount(Grid, ERRCellType::Ore), OreCells);
	TestEqual(TEXT("heal ore still one crystal per cell"), FGridAccess::InstanceCount(Grid, ERRCellType::HealOre), HealCells);
	TestTrue(TEXT("instance maps consistent with deposit crystals"), CheckInstanceMaps(*this, Grid));
	TSet<UStaticMesh*> UsedMeshes;
	for (const FRRGridDepositVisual& Visual : FGridAccess::DepositVisualsOf(Grid))
	{
		if (Visual.Render)
		{
			UsedMeshes.Add(Visual.Render->GetStaticMesh());
			TestEqual(TEXT("deposit crystal ISM never collides"), Visual.Render->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
			TestFalse(TEXT("deposit crystal component is a plain ISM"), Visual.Render->IsA<UHierarchicalInstancedStaticMeshComponent>());
			TestFalse(TEXT("deposit crystal ISM casts no shadow (bDepositCrystalsCastShadow false)"), Visual.Render->CastShadow);
		}
	}
	TestEqual(TEXT("the 3 non-null meshes are used"), UsedMeshes.Num(), 3);
	TestFalse(TEXT("no null mesh drawn"), UsedMeshes.Contains(nullptr));

	// Every crystal on its deposit: bounding-box centre of its cells, block top, quarter-turn yaw.
	auto CheckDepositCrystals = [this, Grid, &OreDeposits](const TCHAR* When)
	{
		int32 Errors = 0;
		int32 Count = 0;
		for (const int32 DepositId : OreDeposits)
		{
			int32 Instance = INDEX_NONE;
			UInstancedStaticMeshComponent* Crystals = FGridAccess::DepositCrystalOf(Grid, DepositId, Instance);
			if (!Crystals)
			{
				continue;
			}
			++Count;
			FTransform Placed;
			Crystals->GetInstanceTransform(Instance, Placed, /*bWorldSpace*/ false);
			FVector2D Centre = FVector2D::ZeroVector;
			for (const FIntPoint& DepositCell : Grid->GetDeposit(DepositId)->Cells)
			{
				Centre += FVector2D((DepositCell.X + 0.5f) * Grid->GetCellSize(), (DepositCell.Y + 0.5f) * Grid->GetCellSize());
			}
			Centre /= Grid->GetDeposit(DepositId)->Cells.Num();
			const double Yaw = Placed.Rotator().Yaw;
			const bool bQuarter = FMath::IsNearlyZero(FMath::Fmod(FMath::Abs(Yaw) + 0.01, 90.0), 0.05);
			if (!FVector2D(Placed.GetLocation()).Equals(Centre, 1.f) || !FMath::IsNearlyEqual(Placed.GetLocation().Z, Grid->GetBlockHeight(), 0.01) || !bQuarter)
			{
				if (Errors++ < 5)
				{
					AddError(FString::Printf(TEXT("%s: deposit %d crystal at %s yaw %.2f, expected (%.1f, %.1f, %.1f)"), When, DepositId,
						*Placed.GetLocation().ToString(), Yaw, Centre.X, Centre.Y, Grid->GetBlockHeight()));
				}
			}
		}
		return Count;
	};
	TestEqual(TEXT("every ore deposit crystal checked"), CheckDepositCrystals(TEXT("build")), OreDeposits.Num());

	// Shake: the crystal moves while a block of its deposit shakes, back at rest when it stops.
	const int32 ShakenDeposit = OreDeposits[0];
	const FRRGridDeposit* Shaken = Grid->GetDeposit(ShakenDeposit);
	int32 ShakenInstance = INDEX_NONE;
	UInstancedStaticMeshComponent* ShakenCrystals = FGridAccess::DepositCrystalOf(Grid, ShakenDeposit, ShakenInstance);
	if (TestNotNull(TEXT("first ore deposit has a crystal"), ShakenCrystals) && TestTrue(TEXT("its deposit has 4 cells"), Shaken->Cells.Num() == 4))
	{
		const FVector Rest = FGridAccess::DepositCrystalRestOf(Grid, ShakenDeposit).GetLocation();
		Grid->SetCellShaking(Shaken->Cells[0], true);
		Grid->SetCellShaking(Shaken->Cells[1], true);
		float MaxCrystalMove = 0.f;
		for (int32 Frame = 0; Frame < 10; ++Frame)
		{
			TestWorld.Tick();
			FTransform Now;
			ShakenCrystals->GetInstanceTransform(ShakenInstance, Now, /*bWorldSpace*/ false);
			MaxCrystalMove = FMath::Max(MaxCrystalMove, static_cast<float>(FVector::Dist(Now.GetLocation(), Rest)));
		}
		AddInfo(FString::Printf(TEXT("deposit crystal shake: max offset %.2f uu"), MaxCrystalMove));
		TestTrue(TEXT("the deposit crystal moves while dug"), MaxCrystalMove > 0.1f && MaxCrystalMove < 20.f);
		Grid->SetCellShaking(Shaken->Cells[0], false);
		FTransform Still;
		ShakenCrystals->GetInstanceTransform(ShakenInstance, Still, /*bWorldSpace*/ false);
		TestTrue(TEXT("still shaking while another block of the deposit shakes"), FGridAccess::Shaking(Grid).Contains(Shaken->Cells[1]));
		Grid->SetCellShaking(Shaken->Cells[1], false);
		ShakenCrystals->GetInstanceTransform(ShakenInstance, Still, /*bWorldSpace*/ false);
		TestTrue(TEXT("deposit crystal back at rest"), Still.GetLocation().Equals(Rest, 1e-3));

		// Outline: the crystal's marked float follows its blocks.
		const int32 MarkedFloat = ShakenInstance * ShakenCrystals->NumCustomDataFloats + Grid->GetMarkedCustomDataIndex();
		TestTrue(TEXT("deposit block marked"), Grid->SetCellMarked(Shaken->Cells[2], true));
		TestEqual(TEXT("deposit crystal marked"), ShakenCrystals->PerInstanceSMCustomData.IsValidIndex(MarkedFloat) ? ShakenCrystals->PerInstanceSMCustomData[MarkedFloat] : -1.f, FGridAccess::MarkedValueOf(Grid));
		TestTrue(TEXT("deposit block unmarked"), Grid->SetCellMarked(Shaken->Cells[2], false));
		TestEqual(TEXT("deposit crystal unmarked"), ShakenCrystals->PerInstanceSMCustomData.IsValidIndex(MarkedFloat) ? ShakenCrystals->PerInstanceSMCustomData[MarkedFloat] : -1.f, 0.f);

		// Break: the drone breaking one block removes the crystal; an explosion on another deposit too. The swapped instances stay mapped.
		TestTrue(TEXT("deposit block destroyed"), Grid->DestroyCell(Shaken->Cells[0], ERRTerrainDamageSource::Drone));
		int32 RemovedInstance = INDEX_NONE;
		TestNull(TEXT("its crystal removed"), FGridAccess::DepositCrystalOf(Grid, ShakenDeposit, RemovedInstance));
		TestEqual(TEXT("one crystal less"), FGridAccess::DepositCrystalCount(Grid, ERRCellType::Ore), OreDeposits.Num() - 1);
		if (OreDeposits.Num() > 1)
		{
			TestTrue(TEXT("other deposit blown up"), Grid->DestroyCell(Grid->GetDeposit(OreDeposits.Last())->Cells[0], ERRTerrainDamageSource::Barrel));
			TestEqual(TEXT("two crystals less"), FGridAccess::DepositCrystalCount(Grid, ERRCellType::Ore), OreDeposits.Num() - 2);
		}
		TestEqual(TEXT("remaining crystals still on their deposits"), CheckDepositCrystals(TEXT("after breaks")), FGridAccess::DepositCrystalCount(Grid, ERRCellType::Ore));
		TestTrue(TEXT("instance maps consistent after deposit breaks"), CheckInstanceMaps(*this, Grid));
	}

	// Materials: by default the deposit ISMs keep the mesh slot materials; a set per-type material overrides only the slot named
	// DepositCrystalSlotName (SM_OreChunk_01 slots crystal_bright and crystal_dark: name the first).
	for (const FRRGridDepositVisual& Visual : FGridAccess::DepositVisualsOf(Grid))
	{
		TestTrue(TEXT("deposit ISM keeps the mesh materials by default"), !Visual.Render || Visual.Render->OverrideMaterials.FindByPredicate(
			[](const TObjectPtr<UMaterialInterface>& Material) { return Material != nullptr; }) == nullptr);
	}
	UMaterialInterface* OverrideMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/RogueRigs/Art/OreChunk/MI_HealCrystal_Bright.MI_HealCrystal_Bright"));
	FGridAccess::OreDepositMaterialOf(Grid) = OverrideMaterial;
	FGridAccess::DepositCrystalSlotNameOf(Grid) = TEXT("crystal_bright");
	FGridAccess::Rebuild(Grid);
	for (const FRRGridDepositVisual& Visual : FGridAccess::DepositVisualsOf(Grid))
	{
		if (!Visual.Render || Visual.Render->GetStaticMesh()->GetName() != TEXT("SM_OreChunk_01"))
		{
			continue;
		}
		const TArray<FStaticMaterial>& Slots = Visual.Render->GetStaticMesh()->GetStaticMaterials();
		for (int32 Slot = 0; Slot < Slots.Num(); ++Slot)
		{
			const bool bNamed = Slots[Slot].MaterialSlotName == FName(TEXT("crystal_bright"));
			TestEqual(*FString::Printf(TEXT("slot %s overridden only when named"), *Slots[Slot].MaterialSlotName.ToString()),
				Visual.Render->GetMaterial(Slot) == OverrideMaterial, bNamed);
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigDebrisBurstTest, "RogueRigs.Dig.DigDebrisBurst",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigDebrisBurstTest::RunTest(const FString& Parameters)
{
	using namespace RRDigTests;

	// The private empty system below cannot run under a real RHI (engine crash on tick).
	if (FApp::CanEverRender())
	{
		AddInfo(TEXT("SKIPPED under a real RHI: run with -nullrhi."));
		return true;
	}

	// A private system, so the counts hold whatever the shipped asset does (Niagara does not run under -nullrhi). Large cap: no reclaim moves a burst during a check.
	TStrongObjectPtr<UNiagaraSystem> Debris(NewObject<UNiagaraSystem>());
	const auto PrepareDrone = [&Debris](ARRDrone* Drone)
	{
		FDroneAccess::DigDebrisSystemOf(Drone) = Debris.Get();
		FDroneAccess::DigDebrisPoolSizeOf(Drone) = 64;
	};
	const auto FindBurst = [&Debris](URRNiagaraPoolSubsystem* FXPool, const ARRArenaGrid* Grid, FIntPoint Cell) -> const UNiagaraComponent*
	{
		const FRRNiagaraSystemPool* Pool = FNiagaraPoolAccess::PoolOf(FXPool, Debris.Get());
		if (!Pool)
		{
			return nullptr;
		}
		const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
		for (const TArray<TObjectPtr<UNiagaraComponent>>* Components : { &Pool->InUse, &Pool->Free })
		{
			for (const UNiagaraComponent* Component : *Components)
			{
				if (Component && FVector::Dist2D(Component->GetComponentLocation(), Center) < 1.0)
				{
					return Component;
				}
			}
		}
		return nullptr;
	};
	const auto ReadColor = [](const UNiagaraComponent* Component)
	{
		return Component->GetOverrideParameters().GetParameterValueOrDefault<FLinearColor>(FNiagaraVariable(FNiagaraTypeDefinition::GetColorDef(), TEXT("User.ChipColor")), FLinearColor::Transparent);
	};

	// Rock: one burst per broken block, over the block, in the rock colour, thrown toward the side of the miner. Other break sources: none.
	{
		FTestWorld TestWorld;
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, FineRockWestFloorCell, Grid, Miner);
		URRNiagaraPoolSubsystem* FXPool = TestWorld.Get()->GetSubsystem<URRNiagaraPoolSubsystem>();
		if (!Drone || !TestNotNull(TEXT("Niagara pool"), FXPool))
		{
			return false;
		}
		PrepareDrone(Drone);
		const TArray<FIntPoint> Arc = { FineRockCell, FIntPoint(129, 114), FIntPoint(129, 116), FIntPoint(130, 115), FIntPoint(130, 114), FIntPoint(130, 113) };
		TArray<int32> Frames;
		TestTrue(TEXT("arc broken"), TickUntilDestroyed(TestWorld, Grid, Arc, 60, Frames) != INDEX_NONE);
		TestEqual(TEXT("one burst per broken block"), FXPool->GetRequestedCount(Debris.Get()), Arc.Num());
		for (const FIntPoint& Cell : Arc)
		{
			const UNiagaraComponent* Burst = FindBurst(FXPool, Grid, Cell);
			if (TestNotNull(*FString::Printf(TEXT("burst over %s"), *Cell.ToString()), Burst))
			{
				TestTrue(TEXT("rock colour"), ReadColor(Burst).Equals(FDroneAccess::ChipColorOf(Drone, ERRCellType::Rock)));
				const FVector Throw(Burst->GetOverrideParameters().GetParameterValueOrDefault<FVector3f>(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("User.ThrowDirection")), FVector3f::ZeroVector));
				TestTrue(TEXT("thrown toward the side of the miner"), Throw.Equals(-FVector::ForwardVector));
			}
		}

		const int32 RequestsBefore = FXPool->GetRequestedCount(Debris.Get());
		const ERRTerrainDamageSource OtherSources[] = { ERRTerrainDamageSource::Barrel, ERRTerrainDamageSource::Kamikaze, ERRTerrainDamageSource::Shot, ERRTerrainDamageSource::Extractor };
		int32 SourceIndex = 0;
		for (int32 Y = 0; Y < 240 && SourceIndex < UE_ARRAY_COUNT(OtherSources); ++Y)
		{
			for (int32 X = 0; X < 100 && SourceIndex < UE_ARRAY_COUNT(OtherSources); ++X)
			{
				if (Grid->GetCellType(FIntPoint(X, Y)) == ERRCellType::Rock && Grid->DestroyCell(FIntPoint(X, Y), OtherSources[SourceIndex]))
				{
					++SourceIndex;
				}
			}
		}
		TestEqual(TEXT("a block broken by each other source"), SourceIndex, static_cast<int32>(UE_ARRAY_COUNT(OtherSources)));
		TestWorld.Tick();
		TestEqual(TEXT("other break sources: no burst"), FXPool->GetRequestedCount(Debris.Get()), RequestsBefore);
	}

	// Ore types: each block of the vein gets a burst in its colour. A barrel only lights its fuse: no burst over it.
	for (const ERRCellType Type : { ERRCellType::Ore, ERRCellType::HealOre, ERRCellType::SpecialOre, ERRCellType::Barrel })
	{
		const FString TypeName = UEnum::GetValueAsString(Type);
		FTestWorld TestWorld;
		ARRArenaGrid* Grid = SpawnFineGrid(*this, TestWorld);
		URRNiagaraPoolSubsystem* FXPool = TestWorld.Get()->GetSubsystem<URRNiagaraPoolSubsystem>();
		if (!Grid || !TestNotNull(TEXT("Niagara pool"), FXPool))
		{
			return false;
		}
		// A block with open floor on its west, the miner cell free on its north and south, so the arc points at the block.
		FIntPoint Target(INDEX_NONE, INDEX_NONE);
		for (int32 Y = 1; Y < 239 && Target.X == INDEX_NONE; ++Y)
		{
			for (int32 X = 1; X < 240 && Target.X == INDEX_NONE; ++X)
			{
				if (Grid->GetCellType(FIntPoint(X, Y)) == Type && Grid->GetCellType(FIntPoint(X - 1, Y)) == ERRCellType::Floor
					&& Grid->GetCellType(FIntPoint(X - 1, Y - 1)) == ERRCellType::Floor && Grid->GetCellType(FIntPoint(X - 1, Y + 1)) == ERRCellType::Floor)
				{
					Target = FIntPoint(X, Y);
				}
			}
		}
		if (Target.X == INDEX_NONE)
		{
			AddInfo(FString::Printf(TEXT("%s: no reachable cell in this layout, not dug"), *TypeName));
			continue;
		}
		ARRMiner* Miner = SpawnMiner(*this, TestWorld, Grid, Target - FIntPoint(1, 0));
		ARRDrone* Drone = Miner ? Miner->GetDrone() : nullptr;
		if (!TestNotNull(TEXT("BP_Miner spawned its drone"), Drone))
		{
			return false;
		}
		for (ARRWeapon* Weapon : Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		PrepareDrone(Drone);
		TArray<FIntPoint> Vein;
		Grid->GetDepositCells(Target, Vein);
		if (Vein.Num() == 0)
		{
			Vein.Add(Target);
		}
		AddInfo(FString::Printf(TEXT("%s: dug at %s, %d block(s)"), *TypeName, *Target.ToString(), Vein.Num()));
		if (Type == ERRCellType::Barrel)
		{
			for (int32 Frame = 0; Frame < 45; ++Frame)
			{
				TestWorld.Tick();
			}
			TestTrue(TEXT("barrel dug to 0"), Grid->GetCellHealth(Target) <= UE_KINDA_SMALL_NUMBER);
			TestNull(TEXT("no burst over a dug barrel"), FindBurst(FXPool, Grid, Target));
			continue;
		}
		TArray<int32> Frames;
		TestTrue(*FString::Printf(TEXT("%s broken"), *TypeName), TickUntilDestroyed(TestWorld, Grid, Vein, 240, Frames) != INDEX_NONE);
		for (const FIntPoint& Cell : Vein)
		{
			const UNiagaraComponent* Burst = FindBurst(FXPool, Grid, Cell);
			if (TestNotNull(*FString::Printf(TEXT("%s: burst over %s"), *TypeName, *Cell.ToString()), Burst))
			{
				TestTrue(*FString::Printf(TEXT("%s colour"), *TypeName), ReadColor(Burst).Equals(FDroneAccess::ChipColorOf(Drone, Type)));
			}
		}
	}

	// Full pool: its only slot waits to start (tick on, inactive: never reclaimed). The burst is skipped and the dig goes on.
	{
		FTestWorld TestWorld;
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, FineRockWestFloorCell, Grid, Miner);
		URRNiagaraPoolSubsystem* FXPool = TestWorld.Get()->GetSubsystem<URRNiagaraPoolSubsystem>();
		if (!Drone || !TestNotNull(TEXT("Niagara pool"), FXPool))
		{
			return false;
		}
		FDroneAccess::DigDebrisSystemOf(Drone) = Debris.Get();
		FDroneAccess::DigDebrisPoolSizeOf(Drone) = 1;
		TestWorld.Tick();
		TestEqual(TEXT("drone digging"), Drone->GetState(), ERRDroneState::Dig);
		FDroneAccess::PlayDigDebrisOf(Drone, FineRockCell, ERRCellType::Rock);
		FRRNiagaraSystemPool* Pool = FNiagaraPoolAccess::PoolOf(FXPool, Debris.Get());
		if (TestNotNull(TEXT("debris pool"), Pool))
		{
			Pool->InUse.Append(Pool->Free);
			Pool->Free.Reset();
			for (UNiagaraComponent* Busy : Pool->InUse)
			{
				Busy->SetComponentTickEnabled(true);
			}
			const int32 SkippedBefore = FXPool->GetSkippedCount(Debris.Get());
			const int32 CreatedBefore = Pool->CreatedCount;
			FDroneAccess::PlayDigDebrisOf(Drone, FineRockCell, ERRCellType::Rock);
			TestEqual(TEXT("full pool: the burst is skipped"), FXPool->GetSkippedCount(Debris.Get()), SkippedBefore + 1);
			TestEqual(TEXT("full pool: no component created"), Pool->CreatedCount, CreatedBefore);
		}
		TArray<int32> Frames;
		TestTrue(TEXT("full pool: the dig goes on"), TickUntilDestroyed(TestWorld, Grid, { FineRockCell }, 60, Frames) != INDEX_NONE);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigDebrisAssetTest, "RogueRigs.Dig.DigDebrisAsset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigDebrisAssetTest::RunTest(const FString& Parameters)
{
	using namespace RRDigTests;

	FTestWorld TestWorld;
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	ARRDrone* Drone = SpawnArcDigScene(*this, TestWorld, FineRockWestFloorCell, Grid, Miner);
	URRNiagaraPoolSubsystem* FXPool = TestWorld.Get()->GetSubsystem<URRNiagaraPoolSubsystem>();
	UNiagaraSystem* Shipped = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/RogueRigs/FX/Dig/NS_DigDebris.NS_DigDebris"));
	if (!Drone || !TestNotNull(TEXT("Niagara pool"), FXPool) || !TestNotNull(TEXT("NS_DigDebris exists"), Shipped))
	{
		return false;
	}
	TestEqual(TEXT("the drone plays it"), FDroneAccess::DigDebrisSystemOf(Drone).Get(), Shipped);
	TestEqual(TEXT("SetOwnerMiner prewarmed the pool"), FXPool->GetComponentCount(Shipped), FDroneAccess::DigDebrisPoolSizeOf(Drone));

	const TArray<FIntPoint> Arc = { FineRockCell, FIntPoint(129, 114), FIntPoint(129, 116), FIntPoint(130, 115), FIntPoint(130, 114), FIntPoint(130, 113) };
	const int32 CreatedAfterPrewarm = FXPool->GetCreatedCount(Shipped);
	TArray<int32> Frames;
	TestTrue(TEXT("arc broken"), TickUntilDestroyed(TestWorld, Grid, Arc, 60, Frames) != INDEX_NONE);
	TestEqual(TEXT("one burst per broken block"), FXPool->GetRequestedCount(Shipped), Arc.Num());
	TestEqual(TEXT("no component created at the break"), FXPool->GetCreatedCount(Shipped), CreatedAfterPrewarm);

	// Niagara only runs under a real RHI (-RenderOffScreen): each burst plays and comes back to the pool within 0.8 s (one-shot).
	if (!FApp::CanEverRender())
	{
		AddInfo(TEXT("play check SKIPPED: FApp::CanEverRender() is false under -nullrhi; run with -RenderOffScreen."));
		return true;
	}
	TestEqual(TEXT("every burst played"), FXPool->GetPlayedCount(Shipped), Arc.Num());
	int32 Frame = 0;
	while (FXPool->GetInUseCount(Shipped) > 0 && Frame < 600)
	{
		TestWorld.Tick();
		++Frame;
	}
	AddInfo(FString::Printf(TEXT("bursts back in the pool after %d frame(s) of %.4f s"), Frame, FrameSeconds));
	TestEqual(TEXT("one-shot: back in the pool"), FXPool->GetInUseCount(Shipped), 0);
	TestTrue(TEXT("gone within 0.8 s (two frames of margin for the pool callback)"), Frame * FrameSeconds <= 0.8f + 2.f * FrameSeconds);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
