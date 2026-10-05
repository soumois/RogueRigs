// Rogue Rigs - automation tests of the dig rules (RogueRigs.Dig.*): grid damage and destruction, ore drops, radius destruction, drone dig, pickup collection, render-only shake.

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
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
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

	/** Heal ore cell of arena_m1 used by the tests. */
	const FIntPoint HealOreCell(49, 45);

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
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	if (!Grid)
	{
		return false;
	}

	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Grid->OnCellDestroyed.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleCellDestroyed);

	TestEqual(TEXT("(43, 38) is rock"), Grid->GetCellType(RockCell), ERRCellType::Rock);
	TestEqual(TEXT("(42, 38) is floor"), Grid->GetCellType(RockWestFloorCell), ERRCellType::Floor);
	TestNearlyEqual(TEXT("rock health 1.5"), Grid->GetCellHealth(RockCell), 1.5f);
	CheckInstanceMaps(*this, Grid);

	const int32 RockBefore = FGridAccess::InstanceCount(Grid, ERRCellType::Rock);
	const int32 FloorBefore = FGridAccess::InstanceCount(Grid, ERRCellType::Floor);

	// One ApplyTerrainDamage(dt) per frame, as the drone does at mining speed 1.
	int32 DestroyFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= 120 && DestroyFrame == INDEX_NONE; ++Frame)
	{
		if (Grid->ApplyTerrainDamage(RockCell, FrameSeconds, ERRTerrainDamageSource::Drone))
		{
			DestroyFrame = Frame;
		}
		TestWorld.Tick();
	}

	const float DestroySeconds = DestroyFrame * FrameSeconds;
	AddInfo(FString::Printf(TEXT("rock destroyed at frame %d (%.4f s)"), DestroyFrame, DestroySeconds));
	TestTrue(TEXT("rock destroyed within 1.5 s +- 1 frame"), DestroyFrame >= 89 && DestroyFrame <= 91);
	TestEqual(TEXT("cell is floor after destruction"), Grid->GetCellType(RockCell), ERRCellType::Floor);
	TestFalse(TEXT("cell is no longer a block"), Grid->IsBlockCell(RockCell));
	TestEqual(TEXT("rock HISM lost one instance"), FGridAccess::InstanceCount(Grid, ERRCellType::Rock), RockBefore - 1);
	TestEqual(TEXT("floor HISM gained one instance"), FGridAccess::InstanceCount(Grid, ERRCellType::Floor), FloorBefore + 1);
	TestTrue(TEXT("instance maps consistent after destruction"), CheckInstanceMaps(*this, Grid));
	TestEqual(TEXT("OnCellDestroyed fired once"), Recorder->GetDestroyedCells().Num(), 1);
	TestEqual(TEXT("rock drops nothing"), CountPickups(TestWorld.Get()), 0);
	TestFalse(TEXT("damage on a destroyed cell is ignored"), Grid->ApplyTerrainDamage(RockCell, 1.f, ERRTerrainDamageSource::Drone));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigHealOreDropsTest, "RogueRigs.Dig.HealOreDrops",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigHealOreDropsTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
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

	TestEqual(TEXT("(49, 45) is heal ore"), Grid->GetCellType(HealOreCell), ERRCellType::HealOre);
	TestNearlyEqual(TEXT("heal ore health 4.5"), Grid->GetCellHealth(HealOreCell), 4.5f);
	TestEqual(TEXT("no pickup at start"), CountPickups(TestWorld.Get()), 0);

	int32 FirstDropFrame = INDEX_NONE;
	int32 SecondDropFrame = INDEX_NONE;
	int32 DestroyFrame = INDEX_NONE;
	int32 PickupsAtDestroy = 0;
	for (int32 Frame = 1; Frame <= 300 && DestroyFrame == INDEX_NONE; ++Frame)
	{
		const int32 Before = CountPickups(TestWorld.Get());
		if (Grid->ApplyTerrainDamage(HealOreCell, FrameSeconds, ERRTerrainDamageSource::Drone))
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
		if (Frame == 181)
		{
			TestEqual(TEXT("2 pickups by 3.0 s (+1 frame)"), After, 2);
		}
		TestWorld.Tick();
	}

	AddInfo(FString::Printf(TEXT("drops at frames %d and %d (%.4f s, %.4f s); destroyed at frame %d (%.4f s); %d pickups on destruction"),
		FirstDropFrame, SecondDropFrame, FirstDropFrame * FrameSeconds, SecondDropFrame * FrameSeconds, DestroyFrame, DestroyFrame * FrameSeconds,
		PickupsAtDestroy));
	TestTrue(TEXT("first drop at 1.5 s +- 1 frame"), FirstDropFrame >= 89 && FirstDropFrame <= 91);
	TestTrue(TEXT("second drop at 3.0 s +- 1 frame"), SecondDropFrame >= 179 && SecondDropFrame <= 181);
	TestTrue(TEXT("destroyed at 4.5 s +- 1 frame"), DestroyFrame >= 269 && DestroyFrame <= 271);
	TestEqual(TEXT("destruction drops DestroyDropCount"), PickupsAtDestroy, FGridAccess::DropsOnDestroy(Grid));

	const int32 Total = CountPickups(TestWorld.Get());
	AddInfo(FString::Printf(TEXT("total pickups: %d"), Total));
	TestEqual(TEXT("5 pickups in total"), Total, 5);
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigDroneDigTest, "RogueRigs.Dig.DroneDig",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigDroneDigTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, RockWestFloorCell) : nullptr;
	ARRDrone* Drone = Miner ? Miner->GetDrone() : nullptr;
	if (!TestNotNull(TEXT("BP_Miner spawned its drone"), Drone))
	{
		return false;
	}
	TestEqual(TEXT("miner found the grid"), Miner->GetArenaGrid(), Grid);
	TestNearlyEqual(TEXT("mining speed 1"), Drone->GetMiningSpeed(), 1.f);
	for (int32 Frame = 0; Frame < 10; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("drone starts in Orbit"), Drone->GetState(), ERRDroneState::Orbit);

	// Same call as the miner's dig trigger when pushing east into the rock: the miner is on the west side.
	TestTrue(TEXT("RequestDig accepted"), Drone->RequestDig(Grid, RockCell, FIntPoint(-1, 0)));
	TestEqual(TEXT("state Dig"), Drone->GetState(), ERRDroneState::Dig);
	TestEqual(TEXT("dig cell"), Drone->GetDigCell(), RockCell);
	TestTrue(TEXT("rock shaking"), FGridAccess::Shaking(Grid).Contains(RockCell));
	TestTrue(TEXT("grid ticks while shaking"), Grid->IsActorTickEnabled());

	int32 DestroyFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= 180 && DestroyFrame == INDEX_NONE; ++Frame)
	{
		TestWorld.Tick();
		if (Frame == 2)
		{
			TestTrue(TEXT("laser visible while digging"), FDroneAccess::Laser(Drone)->IsVisible());
		}
		if (!Grid->IsBlockCell(RockCell))
		{
			DestroyFrame = Frame;
		}
	}

	const float DestroySeconds = DestroyFrame * FrameSeconds;
	AddInfo(FString::Printf(TEXT("drone dig destroyed the rock at frame %d (%.4f s); drone at %s"), DestroyFrame, DestroySeconds,
		*Drone->GetActorLocation().ToCompactString()));
	// Damage starts on the first Dig tick (fly-in does not delay it): 90 frames, 1 frame tolerance plus 2 for the fly-in.
	TestTrue(TEXT("rock destroyed after about 1.5 s"), DestroyFrame >= 89 && DestroyFrame <= 93);
	TestEqual(TEXT("back to Orbit"), Drone->GetState(), ERRDroneState::Orbit);
	TestEqual(TEXT("shake stopped"), FGridAccess::Shaking(Grid).Num(), 0);
	TestFalse(TEXT("grid tick disabled"), Grid->IsActorTickEnabled());
	TestFalse(TEXT("laser hidden"), FDroneAccess::Laser(Drone)->IsVisible());
	TestTrue(TEXT("instance maps consistent"), CheckInstanceMaps(*this, Grid));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRDigDroneCancelTest, "RogueRigs.Dig.DroneDigCancel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRDigDroneCancelTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, RockWestFloorCell) : nullptr;
	ARRDrone* Drone = Miner ? Miner->GetDrone() : nullptr;
	if (!TestNotNull(TEXT("BP_Miner spawned its drone"), Drone))
	{
		return false;
	}

	TestTrue(TEXT("RequestDig accepted"), Drone->RequestDig(Grid, RockCell, FIntPoint(-1, 0)));
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	const float HealthBeforeMove = Grid->GetCellHealth(RockCell);
	TestNearlyEqual(TEXT("0.5 s of damage done"), HealthBeforeMove, 1.0f, 0.02f);
	TestEqual(TEXT("still digging"), Drone->GetState(), ERRDroneState::Dig);

	// Two cells further west: 3 cells (600 uu) from the block center, beyond DigCancelDistanceCells (1.5).
	const FVector Away = Miner->GetActorLocation() - FVector(2.f * Grid->GetCellSize(), 0.f, 0.f);
	Miner->SetActorLocation(Away, /*bSweep*/ false, nullptr, ETeleportType::TeleportPhysics);
	TestWorld.Tick();

	AddInfo(FString::Printf(TEXT("health before move %.4f, after cancel %.4f"), HealthBeforeMove, Grid->GetCellHealth(RockCell)));
	TestEqual(TEXT("cancelled to Orbit"), Drone->GetState(), ERRDroneState::Orbit);
	TestEqual(TEXT("shake stopped"), FGridAccess::Shaking(Grid).Num(), 0);
	TestFalse(TEXT("laser hidden"), FDroneAccess::Laser(Drone)->IsVisible());
	TestTrue(TEXT("block still stands"), Grid->IsBlockCell(RockCell));
	TestNearlyEqual(TEXT("damage kept, none added by the cancel tick"), Grid->GetCellHealth(RockCell), HealthBeforeMove, 1e-4f);

	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	TestNearlyEqual(TEXT("no damage after the cancel"), Grid->GetCellHealth(RockCell), HealthBeforeMove, 1e-4f);
	TestEqual(TEXT("stays in Orbit"), Drone->GetState(), ERRDroneState::Orbit);
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

	const float Radius = Miner->GetPickupRadius();
	AddInfo(FString::Printf(TEXT("PickupRadius %.1f"), Radius));
	TestTrue(TEXT("PickupRadius loaded from the stats table"), Radius > 200.f);

	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);

	// The pop moves a pickup by up to 80 uu per axis: 100 uu stays inside the radius, radius + 300 stays outside.
	const FVector MinerFloor = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y);
	ARRPickup* Near = TestWorld.Spawn<ARRPickup>(HealClass, MinerFloor + FVector(0.f, 100.f, 0.f));
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

#endif // WITH_DEV_AUTOMATION_TESTS
