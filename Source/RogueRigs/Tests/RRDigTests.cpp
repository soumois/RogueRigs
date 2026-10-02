// Rogue Rigs - automation tests of the dig rules (RogueRigs.Dig.*): grid damage and destruction, ore drops, radius destruction, drone dig, pickup collection.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Drone/RRDrone.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "Grid/RRArenaGrid.h"
#include "Kismet/GameplayStatics.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"

namespace RRDigTests
{
	// Test world, frame step, grid and miner spawning, RockCell and RockWestFloorCell: Tests/RRTestHarness.h.
	using namespace RRTestHarness;

	/** Blueprint class of the heal pickup. */
	const TCHAR* const HealPickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal.BP_PickupHeal_C");

	/** Heal ore cell of arena_m1 used by the tests. */
	const FIntPoint HealOreCell(49, 45);

	/** Read access to protected ARRArenaGrid state, through pointers to members formed in a derived class. Never instantiated. */
	struct FGridAccess : public ARRArenaGrid
	{
		static const TMap<ERRCellType, FRRInstanceCellMap>& Maps(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::InstanceCellMaps); }
		static const TArray<int32>& InstanceIndices(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::CellInstanceIndex); }
		static const TMap<FIntPoint, float>& Shaking(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::ShakingCells); }
		static int32 Width(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::GridWidth); }
		static int32 Height(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::GridHeight); }
		static int32 DropsOnDestroy(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::DestroyDropCount); }
		static TSubclassOf<ARRPickup>& HealClass(ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::HealPickupClass); }
		static TSubclassOf<ARRPickup> OreClass(const ARRArenaGrid* Grid) { return Grid->*(&FGridAccess::OrePickupClass); }

		static UHierarchicalInstancedStaticMeshComponent* Component(const ARRArenaGrid* Grid, ERRCellType Key)
		{
			switch (Key)
			{
			case ERRCellType::Floor:      return Grid->*(&FGridAccess::FloorInstances);
			case ERRCellType::Rock:       return Grid->*(&FGridAccess::RockInstances);
			case ERRCellType::Ore:        return Grid->*(&FGridAccess::OreInstances);
			case ERRCellType::HealOre:    return Grid->*(&FGridAccess::HealOreInstances);
			case ERRCellType::SpecialOre: return Grid->*(&FGridAccess::SpecialOreInstances);
			case ERRCellType::Barrel:     return Grid->*(&FGridAccess::BarrelInstances);
			default:                      return nullptr;
			}
		}
	};

	/** Read access to the drone's protected laser mesh. Never instantiated. */
	struct FDroneAccess : public ARRDrone
	{
		static const UStaticMeshComponent* Laser(const ARRDrone* Drone) { return Drone->*(&FDroneAccess::LaserMesh); }
	};

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
	 * to a cell whose index points at it, of the map's type, and the instance sits on that cell's center; every non-void
	 * cell has exactly one instance. Returns true when consistent.
	 */
	bool CheckInstanceMaps(FAutomationTestBase& Test, const ARRArenaGrid* Grid)
	{
		const int32 Width = FGridAccess::Width(Grid);
		const int32 Height = FGridAccess::Height(Grid);
		const float CellSize = Grid->GetCellSize();
		const TArray<int32>& Indices = FGridAccess::InstanceIndices(Grid);
		int32 Errors = 0;
		int32 MappedInstances = 0;

		for (const TPair<ERRCellType, FRRInstanceCellMap>& Pair : FGridAccess::Maps(Grid))
		{
			const UHierarchicalInstancedStaticMeshComponent* Component = FGridAccess::Component(Grid, Pair.Key);
			const TArray<FIntPoint>& InstanceCells = Pair.Value.InstanceCells;
			if (!Component || Component->GetInstanceCount() != InstanceCells.Num())
			{
				Test.AddError(FString::Printf(TEXT("map %s has %d entries, HISM has %d instances"), *UEnum::GetValueAsString(Pair.Key),
					InstanceCells.Num(), Component ? Component->GetInstanceCount() : -1));
				++Errors;
				continue;
			}

			for (int32 Instance = 0; Instance < InstanceCells.Num(); ++Instance)
			{
				const FIntPoint Cell = InstanceCells[Instance];
				const ERRCellType Type = Grid->GetCellType(Cell);
				const bool bTypeMatches = Pair.Key == ERRCellType::Floor ? (Type == ERRCellType::Floor || Type == ERRCellType::Spawn) : Type == Pair.Key;
				const int32 CellIndex = Cell.Y * Width + Cell.X;
				FTransform InstanceTransform;
				Component->GetInstanceTransform(Instance, InstanceTransform, /*bWorldSpace*/ false);
				const FVector2D Expected((Cell.X + 0.5f) * CellSize, (Cell.Y + 0.5f) * CellSize);
				const bool bOnCell = FVector2D(InstanceTransform.GetLocation()).Equals(Expected, 1.f);
				if (!bTypeMatches || !Indices.IsValidIndex(CellIndex) || Indices[CellIndex] != Instance || !bOnCell)
				{
					if (Errors < 10)
					{
						Test.AddError(FString::Printf(TEXT("%s instance %d -> cell (%d, %d): type %s, cell index %d, on cell %d"),
							*UEnum::GetValueAsString(Pair.Key), Instance, Cell.X, Cell.Y, *UEnum::GetValueAsString(Type),
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

	const int32 RockBefore = FGridAccess::Component(Grid, ERRCellType::Rock)->GetInstanceCount();
	const int32 FloorBefore = FGridAccess::Component(Grid, ERRCellType::Floor)->GetInstanceCount();

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
	TestEqual(TEXT("rock HISM lost one instance"), FGridAccess::Component(Grid, ERRCellType::Rock)->GetInstanceCount(), RockBefore - 1);
	TestEqual(TEXT("floor HISM gained one instance"), FGridAccess::Component(Grid, ERRCellType::Floor)->GetInstanceCount(), FloorBefore + 1);
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

#endif // WITH_DEV_AUTOMATION_TESTS
