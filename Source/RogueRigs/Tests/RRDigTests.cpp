// Rogue Rigs - automation tests of the dig rules (RogueRigs.Dig.*): grid damage and destruction, ore drops, radius destruction, drone arc dig (contact, arc limits, speeds, persistence and release), pickup collection, render-only shake.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Drone/RRDrone.h"
#include "Engine/Engine.h"
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
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
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
			Count += IsValid(*It) ? 1 : 0;
		}
		return Count;
	}

	/**
	 * Checks the grid's instance maps against its HISMs: each map has one entry per instance, every instance maps back
	 * to a cell whose index points at it, of the map's type, and the instance sits on that cell's center; each block type's
	 * collision ISM has the same instances in the same order, at rest; every non-void cell has exactly one instance.
	 * Returns true when consistent.
	 */
	bool CheckInstanceMaps(FAutomationTestBase& Test, const ARRArenaGrid* Grid)
	{
		const int32 Width = FGridAccess::Width(Grid);
		const int32 Height = FGridAccess::Height(Grid);
		const float CellSize = Grid->GetCellSize();
		const TArray<int32>& Indices = FGridAccess::InstanceIndices(Grid);
		int32 Errors = 0;
		int32 MappedInstances = 0;

		const int32 ChunkSide = FGridAccess::ChunkSide(Grid);
		for (const FRRGridChunkVisual& Visual : FGridAccess::Visuals(Grid))
		{
			const UHierarchicalInstancedStaticMeshComponent* Component = Visual.Render;
			const TArray<FIntPoint>& InstanceCells = Visual.InstanceCells;
			if (!Component)
			{
				if (InstanceCells.Num() > 0)
				{
					Test.AddError(FString::Printf(TEXT("chunk (%d, %d) %s has %d entries and no HISM"), Visual.Chunk.X, Visual.Chunk.Y,
						*UEnum::GetValueAsString(Visual.MapKey), InstanceCells.Num()));
					++Errors;
				}
				continue;
			}
			if (Component->GetInstanceCount() != InstanceCells.Num())
			{
				Test.AddError(FString::Printf(TEXT("chunk (%d, %d) map %s has %d entries, HISM has %d instances"), Visual.Chunk.X, Visual.Chunk.Y,
					*UEnum::GetValueAsString(Visual.MapKey), InstanceCells.Num(), Component->GetInstanceCount()));
				++Errors;
				continue;
			}
			const UInstancedStaticMeshComponent* Collision = Visual.Collision;
			if (Visual.MapKey != ERRCellType::Floor && (!Collision || Collision->GetInstanceCount() != InstanceCells.Num()))
			{
				Test.AddError(FString::Printf(TEXT("chunk (%d, %d) map %s has %d entries, collision ISM has %d instances"), Visual.Chunk.X, Visual.Chunk.Y,
					*UEnum::GetValueAsString(Visual.MapKey), InstanceCells.Num(), Collision ? Collision->GetInstanceCount() : -1));
				++Errors;
				continue;
			}
			// No grid component affects navigation (no per-instance navigation bounds update on add or remove).
			if (Component->CanEverAffectNavigation() || (Collision && Collision->CanEverAffectNavigation()))
			{
				Test.AddError(FString::Printf(TEXT("chunk (%d, %d) %s is navigation relevant"), Visual.Chunk.X, Visual.Chunk.Y, *UEnum::GetValueAsString(Visual.MapKey)));
				++Errors;
			}
			const ERRCellType MapKey = Visual.MapKey;

			for (int32 Instance = 0; Instance < InstanceCells.Num(); ++Instance)
			{
				const FIntPoint Cell = InstanceCells[Instance];
				const ERRCellType Type = Grid->GetCellType(Cell);
				const bool bTypeMatches = (MapKey == ERRCellType::Floor ? (Type == ERRCellType::Floor || Type == ERRCellType::Spawn) : Type == MapKey)
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
					if (Errors < 10)
					{
						Test.AddError(FString::Printf(TEXT("%s instance %d -> cell (%d, %d): type %s, cell index %d, on cell %d"),
							*UEnum::GetValueAsString(MapKey), Instance, Cell.X, Cell.Y, *UEnum::GetValueAsString(Type),
							Indices.IsValidIndex(CellIndex) ? Indices[CellIndex] : -2, bOnCell ? 1 : 0));
					}
					++Errors;
				}
				++MappedInstances;
			}
		}

		int32 NonVoidCells = 0;
		for (int32 Y = 0; Y < Height; ++Y)
		{
			for (int32 X = 0; X < Width; ++X)
			{
				if (Grid->GetCellType(FIntPoint(X, Y)) != ERRCellType::Void)
				{
					++NonVoidCells;
					if (Indices[Y * Width + X] == INDEX_NONE)
					{
						Test.AddError(FString::Printf(TEXT("cell (%d, %d) has no instance"), X, Y));
						++Errors;
					}
				}
			}
		}
		Test.TestEqual(TEXT("one instance per non-void cell"), MappedInstances, NonVoidCells);
		return Errors == 0 && MappedInstances == NonVoidCells;
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
	TestNearlyEqual(TEXT("rock health 0.5 (drone gdd: 0.5 s per 67 uu block)"), Grid->GetCellHealth(FineRockCell), 0.5f);
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
	TestTrue(TEXT("rock destroyed within 0.5 s +- 1 frame"), DestroyFrame >= 29 && DestroyFrame <= 31);
	TestEqual(TEXT("cell is floor after destruction"), Grid->GetCellType(FineRockCell), ERRCellType::Floor);
	TestFalse(TEXT("cell is no longer a block"), Grid->IsBlockCell(FineRockCell));
	TestEqual(TEXT("rock HISM lost one instance"), FGridAccess::InstanceCount(Grid, ERRCellType::Rock), RockBefore - 1);
	TestEqual(TEXT("floor HISM gained one instance"), FGridAccess::InstanceCount(Grid, ERRCellType::Floor), FloorBefore + 1);
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
	// Game grid (67 uu): heal ore takes 1.5 s per block and drops one pickup when broken (ore-blocks, drone gdd).
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
	TestNearlyEqual(TEXT("heal ore health 1.5"), Grid->GetCellHealth(FineHealOreCell), 1.5f);
	TestNearlyEqual(TEXT("drop interval 1.5 s: no multiple strictly below the health, so no dig drop"), FGridAccess::DropIntervalOf(Grid), 1.5f);
	TestEqual(TEXT("one pickup per broken block"), FGridAccess::DropsOnDestroy(Grid), 1);
	TestEqual(TEXT("no pickup at start"), CountPickups(TestWorld.Get()), 0);

	int32 FirstDropFrame = INDEX_NONE;
	int32 SecondDropFrame = INDEX_NONE;
	int32 DestroyFrame = INDEX_NONE;
	int32 PickupsAtDestroy = 0;
	for (int32 Frame = 1; Frame <= 300 && DestroyFrame == INDEX_NONE; ++Frame)
	{
		const int32 Before = CountPickups(TestWorld.Get());
		if (Grid->ApplyTerrainDamage(FineHealOreCell, FrameSeconds, ERRTerrainDamageSource::Drone))
		{
			DestroyFrame = Frame;
			PickupsAtDestroy = CountPickups(TestWorld.Get()) - Before;
		}
		const int32 After = CountPickups(TestWorld.Get());
		if (DestroyFrame == INDEX_NONE && After > Before)
		{
			TestEqual(FString::Printf(TEXT("one dig drop at frame %d"), Frame), After - Before, 1);
			(FirstDropFrame == INDEX_NONE ? FirstDropFrame : SecondDropFrame) = Frame;
		}
		TestWorld.Tick();
	}

	AddInfo(FString::Printf(TEXT("drops at frames %d and %d (%.4f s, %.4f s); destroyed at frame %d (%.4f s); %d pickups on destruction"),
		FirstDropFrame, SecondDropFrame, FirstDropFrame * FrameSeconds, SecondDropFrame * FrameSeconds, DestroyFrame, DestroyFrame * FrameSeconds,
		PickupsAtDestroy));
	TestEqual(TEXT("no dig drop before the destruction"), FirstDropFrame, static_cast<int32>(INDEX_NONE));
	TestTrue(TEXT("destroyed at 1.5 s +- 1 frame"), DestroyFrame >= 89 && DestroyFrame <= 91);
	TestEqual(TEXT("destruction drops DestroyDropCount"), PickupsAtDestroy, FGridAccess::DropsOnDestroy(Grid));

	const int32 Total = CountPickups(TestWorld.Get());
	AddInfo(FString::Printf(TEXT("total pickups: %d"), Total));
	TestEqual(TEXT("1 pickup in total"), Total, 1);
	for (TActorIterator<ARRPickup> It(TestWorld.Get()); It; ++It)
	{
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

	/** Number of visible beams in the drone's laser pool. */
	int32 CountVisibleBeams(const ARRDrone* Drone)
	{
		int32 Count = 0;
		for (const UStaticMeshComponent* Beam : FDroneAccess::Beams(Drone))
		{
			Count += (Beam && Beam->IsVisible()) ? 1 : 0;
		}
		return Count;
	}

	/** Laser fan check: one visible beam per dug cell, beam i from the drone to GetDigTargetPoint(DigCells[i]) (within 0.5 uu), the rest hidden. */
	void CheckLaserFan(FAutomationTestBase& Test, const ARRDrone* Drone, const TCHAR* When)
	{
		const TArray<TObjectPtr<UStaticMeshComponent>>& Beams = FDroneAccess::Beams(Drone);
		const TArray<FIntPoint>& Cells = Drone->GetDigCells();
		Test.TestTrue(FString::Printf(TEXT("%s: pool holds at least one beam per dug cell"), When), Beams.Num() >= Cells.Num());
		Test.TestEqual(FString::Printf(TEXT("%s: visible beams = dug cells"), When), CountVisibleBeams(Drone), Cells.Num());
		const float BaseHeight = FDroneAccess::BeamBaseHeight(Drone);
		for (int32 Index = 0; Index < Cells.Num() && Index < Beams.Num(); ++Index)
		{
			const UStaticMeshComponent* Beam = Beams[Index];
			const FVector HalfBeam = Beam->GetUpVector() * Beam->GetComponentScale().Z * BaseHeight * 0.5;
			const FVector Start = Beam->GetComponentLocation() - HalfBeam;
			const FVector End = Beam->GetComponentLocation() + HalfBeam;
			const FVector Target = Drone->GetDigTargetPoint(Cells[Index]);
			Test.AddInfo(FString::Printf(TEXT("%s: beam %d on %s ends %.2f uu from its target point"), When, Index, *Cells[Index].ToString(), FVector::Dist(End, Target)));
			Test.TestTrue(FString::Printf(TEXT("%s: beam %d visible"), When, Index), Beam->IsVisible());
			Test.TestTrue(FString::Printf(TEXT("%s: beam %d starts at the drone"), When, Index), Start.Equals(Drone->GetActorLocation(), 0.5));
			Test.TestTrue(FString::Printf(TEXT("%s: beam %d ends at the target point"), When, Index), End.Equals(Target, 0.5));
		}
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
	// the drone cuts the 6 blocks of the arc in parallel (two rows: the wall is 2 cells thick), each in the GDD 0.5 s, then
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
	TestNearlyEqual(TEXT("first frame of damage on every block"), Grid->GetCellHealth(FIntPoint(130, 115)), 0.5f - FrameSeconds, 1e-4f);

	TestWorld.Tick();
	TestTrue(TEXT("laser visible while digging"), FDroneAccess::Laser(Drone)->IsVisible());
	TestTrue(TEXT("LaserMesh is beam 0"), FDroneAccess::Beams(Drone).Num() > 0 && FDroneAccess::Beams(Drone)[0] == FDroneAccess::Laser(Drone));
	CheckLaserFan(*this, Drone, TEXT("frame 2"));
	for (int32 Frame = 3; Frame <= 25; ++Frame)
	{
		TestWorld.Tick();
	}
	FVector ArcCentre = FVector::ZeroVector;
	for (const FIntPoint& Cell : Arc)
	{
		ArcCentre += Grid->GetCellCenter(Cell.X, Cell.Y) / Arc.Num();
	}
	const double HoverError = FVector::Dist2D(Drone->GetActorLocation(), ArcCentre);
	AddInfo(FString::Printf(TEXT("frame 25: drone %.1f uu from the arc centre %s"), HoverError, *ArcCentre.ToCompactString()));
	TestTrue(TEXT("drone hovers over the arc centre"), HoverError <= 10.0);
	CheckLaserFan(*this, Drone, TEXT("frame 25"));

	// Frames counted from the first tick: continue to the destruction.
	const int32 LastFrame = TickUntilDestroyed(TestWorld, Grid, Arc, 60, DestroyFrames);
	for (int32 Index = 0; Index < Arc.Num(); ++Index)
	{
		const int32 Frame = DestroyFrames[Index] == INDEX_NONE ? INDEX_NONE : DestroyFrames[Index] + 25;
		AddInfo(FString::Printf(TEXT("block %s destroyed at frame %d"), *Arc[Index].ToString(), Frame));
		TestTrue(FString::Printf(TEXT("block %s destroyed after about 0.5 s"), *Arc[Index].ToString()), Frame >= 29 && Frame <= 31);
	}
	TestTrue(TEXT("all destroyed"), LastFrame != INDEX_NONE);

	TestWorld.Tick();
	TestFalse(TEXT("no block within reach any more"), Miner->IsTouchingRock());
	TestEqual(TEXT("back to Orbit"), Drone->GetState(), ERRDroneState::Orbit);
	TestEqual(TEXT("shake stopped"), FGridAccess::Shaking(Grid).Num(), 0);
	TestFalse(TEXT("laser hidden"), FDroneAccess::Laser(Drone)->IsVisible());
	TestEqual(TEXT("every beam hidden"), CountVisibleBeams(Drone), 0);
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

	// Heal ore at mining speed 1: 1.5 s per block, in parallel (gold uses the same 1.5 s, RogueRigs.Ore.MineDeposit). Since
	// task-088-01 the vein is 2 x 2 (147-148, 135-136): the 8-block arc holds its 4 blocks and 4 rocks, which keep their 0.5 s.
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
		TickUntilDestroyed(TestWorld, Grid, Arc, 120, Frames);
		for (int32 Index = 0; Index < Arc.Num(); ++Index)
		{
			const int32 Frame = Frames[Index] == INDEX_NONE ? INDEX_NONE : Frames[Index] + 1;
			const TCHAR* Kind = IsHealOre[Index] ? TEXT("heal ore") : TEXT("rock");
			AddInfo(FString::Printf(TEXT("%s %s destroyed at frame %d"), Kind, *Arc[Index].ToString(), Frame));
			TestTrue(FString::Printf(TEXT("%s %s destroyed after about %s"), Kind, *Arc[Index].ToString(), IsHealOre[Index] ? TEXT("1.5 s") : TEXT("0.5 s")),
				IsHealOre[Index] ? Frame >= 89 && Frame <= 91 : Frame >= 29 && Frame <= 31);
		}
	}

	// Rock at mining speed 2: 0.25 s per block.
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
		TestTrue(TEXT("rock destroyed after about 0.25 s at mining speed 2"), Frames[0] >= 14 && Frames[0] <= 16);
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
	TestNearlyEqual(TEXT("0.25 s of damage done"), HealthBeforeMove, 0.25f, 0.02f);
	TestEqual(TEXT("still digging"), Drone->GetState(), ERRDroneState::Dig);
	CheckLaserFan(*this, Drone, TEXT("frame 15"));
	TestEqual(TEXT("6 beams before the release"), CountVisibleBeams(Drone), 6);

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
	TestEqual(TEXT("every beam hidden on release"), CountVisibleBeams(Drone), 0);
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
	TestTrue(TEXT("resumed: the remaining 0.25 s"), Frames[0] >= 14 && Frames[0] <= 16);
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

	// The pop ejects a pickup 80-220 uu (task-027-01): spawned under the miner it stays inside the radius, radius + 300 stays outside.
	const FVector MinerFloor = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y);
	ARRPickup* Near = TestWorld.Spawn<ARRPickup>(HealClass, MinerFloor);
	ARRPickup* Far = TestWorld.Spawn<ARRPickup>(HealClass, MinerFloor + FVector(0.f, -(Radius + 300.f), 0.f));
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
	AddInfo(FString::Printf(TEXT("PopDuration %.2f s, AttractDelay %.2f s"), PopDuration, AttractDelay));
	TestTrue(TEXT("AttractDelay is at least 1 s"), AttractDelay >= 1.f);
	const float EarliestCollect = PopDuration + AttractDelay;

	int32 CollectFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= 240; ++Frame)
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
	TestTrue(TEXT("near pickup not collected before pop + AttractDelay"), CollectFrame != INDEX_NONE && CollectFrame * FrameSeconds >= EarliestCollect - FrameSeconds);
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

	const UHierarchicalInstancedStaticMeshComponent* RockRender = FGridAccess::CellComponent(Grid, RockCell);
	const UInstancedStaticMeshComponent* RockCollisionComponent = FGridAccess::CellCollisionComponent(Grid, RockCell);
	const int32 RockIndex = FGridAccess::InstanceIndices(Grid)[RockCell.Y * FGridAccess::Width(Grid) + RockCell.X];
	if (!TestNotNull(TEXT("rock collision ISM"), RockCollisionComponent) || !TestTrue(TEXT("rock instance mapped"), RockIndex != INDEX_NONE))
	{
		return false;
	}
	TestEqual(TEXT("rock HISM renders without collision"), RockRender->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
	TestFalse(TEXT("collision ISM is not drawn"), RockCollisionComponent->IsVisible());
	FTransform RestTransform;
	RockRender->GetInstanceTransform(RockIndex, RestTransform, /*bWorldSpace*/ false);

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
	float MaxRenderOffset = 0.f;
	float MaxCollisionOffset = 0.f;
	constexpr int32 SweepFrames = 30;
	for (int32 Frame = 0; Frame < SweepFrames; ++Frame)
	{
		TestWorld.Tick();
		FTransform RenderTransform;
		FTransform CollisionTransform;
		RockRender->GetInstanceTransform(RockIndex, RenderTransform, /*bWorldSpace*/ false);
		RockCollisionComponent->GetInstanceTransform(RockIndex, CollisionTransform, /*bWorldSpace*/ false);
		MaxRenderOffset = FMath::Max(MaxRenderOffset, static_cast<float>(FVector::Dist(RenderTransform.GetLocation(), RestTransform.GetLocation())));
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
	AddInfo(FString::Printf(TEXT("sweep: %d / %d frames hit, first hit at x %.3f (face x %.1f, radius %.1f), %d moved; render offset max %.2f uu, collision offset max %.4f uu"),
		HitFrames, SweepFrames, FirstHitX, FaceCenter.X, Radius, MovedHits, MaxRenderOffset, MaxCollisionOffset));
	TestTrue(TEXT("the rendering shakes"), MaxRenderOffset > 1.f);
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

	// The 80 x 80 arena splits in several rock chunks; a destruction only changes the chunk of its cell.
	TMap<const UHierarchicalInstancedStaticMeshComponent*, int32> CountsBefore;
	int32 RockChunks = 0;
	for (const FRRGridChunkVisual& Visual : FGridAccess::Visuals(Grid))
	{
		if (Visual.Render)
		{
			CountsBefore.Add(Visual.Render, Visual.Render->GetInstanceCount());
			RockChunks += Visual.MapKey == ERRCellType::Rock ? 1 : 0;
		}
	}
	TestTrue(TEXT("rock split over several chunk HISMs"), RockChunks > 1);

	UHierarchicalInstancedStaticMeshComponent* RockChunk = FGridAccess::CellComponent(Grid, RockCell);
	if (!TestNotNull(TEXT("rock cell chunk HISM"), RockChunk) || !TestTrue(TEXT("destroyed"), Grid->DestroyCell(RockCell, ERRTerrainDamageSource::Drone)))
	{
		return false;
	}
	UHierarchicalInstancedStaticMeshComponent* FloorChunk = FGridAccess::CellComponent(Grid, RockCell);
	TestNotNull(TEXT("floor chunk HISM of the destroyed cell"), FloorChunk);

	int32 Changed = 0;
	for (const FRRGridChunkVisual& Visual : FGridAccess::Visuals(Grid))
	{
		if (!Visual.Render)
		{
			continue;
		}
		const int32* Before = CountsBefore.Find(Visual.Render);
		const int32 Delta = Visual.Render->GetInstanceCount() - (Before ? *Before : 0);
		if (Delta != 0)
		{
			++Changed;
			TestTrue(TEXT("only the destroyed cell's chunk changed"), Visual.Chunk.X == RockCell.X / FGridAccess::ChunkSide(Grid)
				&& Visual.Chunk.Y == RockCell.Y / FGridAccess::ChunkSide(Grid));
			TestEqual(TEXT("rock chunk -1, floor chunk +1"), Delta, Visual.Render == RockChunk ? -1 : 1);
		}
	}
	TestEqual(TEXT("two chunk HISMs changed (rock and floor)"), Changed, 2);
	TestTrue(TEXT("instance maps consistent"), CheckInstanceMaps(*this, Grid));
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
	TestNearlyEqual(TEXT("rock 0.5 s"), Grid->GetCellHealth(FineRockCell), 0.5f);
	TestEqual(TEXT("one pickup per broken ore block"), FGridAccess::DropsOnDestroy(Grid), 1);

	// Explosion radii stay in uu (ExplosionStats.csv): the barrel clears every block center within 350 uu, about 5.2 cells of 67 uu.
	FRRExplosionStatsRow Barrel;
	FRRExplosionStatsRow Kamikaze;
	TestTrue(TEXT("Barrel row"), Explosions->GetExplosionStats(TEXT("Barrel"), Barrel));
	TestTrue(TEXT("Kamikaze row"), Explosions->GetExplosionStats(TEXT("Kamikaze"), Kamikaze));
	TestEqual(TEXT("barrel radius 350 uu"), Barrel.Radius, 350.f);
	TestEqual(TEXT("kamikaze radius 312 uu"), Kamikaze.Radius, 312.f);
	const FVector Center = Grid->GetCellCenter(FineRockCell.X, FineRockCell.Y);
	TArray<FIntPoint> Inside;
	for (int32 DY = -7; DY <= 7; ++DY)
	{
		for (int32 DX = -7; DX <= 7; ++DX)
		{
			const FIntPoint Cell(FineRockCell.X + DX, FineRockCell.Y + DY);
			if (Grid->IsBlockCell(Cell) && FVector::Dist2D(Grid->GetCellCenter(Cell.X, Cell.Y), Center) <= Barrel.Radius)
			{
				Inside.Add(Cell);
			}
		}
	}
	AddInfo(FString::Printf(TEXT("%d blocks within 350 uu of (129, 115)"), Inside.Num()));
	TestTrue(TEXT("several blocks in the radius"), Inside.Num() > 10);
	TestEqual(TEXT("every block in the 350 uu radius destroyed"), Grid->DestroyCellsInRadius(Center, Barrel.Radius, ERRTerrainDamageSource::Barrel), Inside.Num());
	TestTrue(TEXT("instance maps consistent"), CheckInstanceMaps(*this, Grid));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
