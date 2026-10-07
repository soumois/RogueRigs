// Rogue Rigs - fine grid integration tests with the final Blueprints and the grid placed in L_ArenaM1 (RogueRigs.Integration.Fine*):
// a contact arc dig opens a passage that a shut-in BP_Chaser then uses to reach BP_Miner, and BP_ObjectiveConsole / BP_ExtractionShuttle
// land at the world distances of the 200 uu design from the map's player start at 67 uu.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "Drone/RRDrone.h"
#include "Enemies/RRChaser.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRLandingFootprint.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Player/RRGameMode.h"
#include "Player/RRMiner.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectHash.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRFineGridIntegrationTests
{
	// Test world, frame step, grid and miner spawning: Tests/RRTestHarness.h; protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestHarness;

	/** The game map whose placed grid and player start the tests read. */
	const TCHAR* const ArenaMapPackage = TEXT("/Game/RogueRigs/Maps/L_ArenaM1");

	/** Final Blueprints of the loops. */
	const TCHAR* const ChaserClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Chaser.BP_Chaser_C");
	const TCHAR* const GameModeClassPath = TEXT("/Game/RogueRigs/Blueprints/Player/BP_RRGameMode.BP_RRGameMode_C");
	const TCHAR* const ConsoleClassPath = TEXT("/Game/RogueRigs/Blueprints/Objectives/BP_ObjectiveConsole.BP_ObjectiveConsole_C");
	const TCHAR* const ShuttleClassPath = TEXT("/Game/RogueRigs/Blueprints/Objectives/BP_ExtractionShuttle.BP_ExtractionShuttle_C");

	/**
	 * arena_m1_fine pocket (316 floor cells, x 113..132, y 69..107) shut by the two rock rows y 108..109 above the 3-cell notch
	 * (119..121, 110): the miner walks from MinerStartCell up the middle of the notch into the wall and digs through to the
	 * pocket floor (117..119, 107). The chaser waits deep in the pocket.
	 */
	const FIntPoint MinerStartCell(120, 113);
	const FIntPoint NotchCell(120, 110);
	const FIntPoint PocketFloorCell(119, 107);
	const FIntPoint PocketChaserCell(118, 100);

	/** Grid settings and player start read from L_ArenaM1 as saved on disk. */
	struct FMapSetup
	{
		FString LayoutName;
		float CellSize = 0.f;
		FTransform GridTransform;
		FVector PlayerStart = FVector::ZeroVector;
		bool bHasPlayerStart = false;
	};

	/** Every actor of the map's persistent level, external actor packages included (one file per actor). */
	TArray<AActor*> LoadMapActors(FAutomationTestBase& Test)
	{
		TArray<AActor*> Actors;
		UPackage* MapPackage = LoadPackage(nullptr, ArenaMapPackage, LOAD_None);
		UWorld* MapWorld = MapPackage ? UWorld::FindWorldInPackage(MapPackage) : nullptr;
		if (!Test.TestNotNull(TEXT("L_ArenaM1 loads"), MapWorld) || !Test.TestNotNull(TEXT("L_ArenaM1 level"), MapWorld->PersistentLevel.Get()))
		{
			return Actors;
		}
		ULevel* Level = MapWorld->PersistentLevel;
		for (AActor* Actor : Level->Actors)
		{
			if (Actor)
			{
				Actors.AddUnique(Actor);
			}
		}
#if WITH_EDITOR // external actor packages (one file per actor) exist only in editor builds; a cooked map holds its actors
		if (Level->IsUsingExternalActors())
		{
			for (const FString& ActorPackageName : Level->GetOnDiskExternalActorPackages(/*bTryUsingPackageLoadedPath*/ true))
			{
				if (UPackage* ActorPackage = LoadPackage(nullptr, *ActorPackageName, LOAD_None))
				{
					ForEachObjectWithPackage(ActorPackage, [&Actors](UObject* Object)
					{
						if (AActor* Actor = Cast<AActor>(Object); Actor && !Actor->IsTemplate())
						{
							Actors.AddUnique(Actor);
						}
						return true;
					});
				}
			}
		}
#endif // WITH_EDITOR
		return Actors;
	}

	/** Reads the placed BP_ArenaGrid (layout, cell size, transform) and the player start of L_ArenaM1; false with a failure when absent. */
	bool ReadMap(FAutomationTestBase& Test, FMapSetup& Out)
	{
		TArray<ARRArenaGrid*> Grids;
		for (AActor* Actor : LoadMapActors(Test))
		{
			if (ARRArenaGrid* Grid = Cast<ARRArenaGrid>(Actor))
			{
				Grids.AddUnique(Grid);
			}
			else if (APlayerStart* Start = Cast<APlayerStart>(Actor); Start && !Out.bHasPlayerStart)
			{
				Out.PlayerStart = Start->GetActorLocation();
				Out.bHasPlayerStart = true;
			}
		}
		if (!Test.TestEqual(TEXT("exactly one grid placed in L_ArenaM1"), Grids.Num(), 1))
		{
			return false;
		}
		ARRArenaGrid* Placed = Grids[0];
		Out.LayoutName = RRTestAccess::FGridAccess::LayoutNameOf(Placed);
		Out.CellSize = RRTestAccess::FGridAccess::CellSizeOf(Placed);
		Out.GridTransform = Placed->GetActorTransform();
		Test.AddInfo(FString::Printf(TEXT("placed grid %s: layout %s, %.1f uu, at %s; player start %s"), *Placed->GetName(), *Out.LayoutName,
			Out.CellSize, *Out.GridTransform.GetLocation().ToCompactString(), Out.bHasPlayerStart ? *Out.PlayerStart.ToCompactString() : TEXT("none")));
		Test.TestEqual(TEXT("placed grid uses the game layout"), Out.LayoutName, FString(FineLayoutName));
		Test.TestEqual(TEXT("placed grid uses 67 uu cells"), Out.CellSize, FineCellSize);
		return true;
	}

	/** Spawns BP_ArenaGrid with the map's layout, cell size and transform (set before its construction script and BeginPlay). */
	ARRArenaGrid* SpawnMapGrid(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FMapSetup& Map)
	{
		UClass* GridClass = LoadTestClass<ARRArenaGrid>(Test, ArenaGridClassPath);
		ARRArenaGrid* Grid = GridClass ? TestWorld.Get()->SpawnActorDeferred<ARRArenaGrid>(GridClass, Map.GridTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn) : nullptr;
		if (!Test.TestNotNull(TEXT("grid spawned"), Grid))
		{
			return nullptr;
		}
		RRTestAccess::FGridAccess::LayoutNameOf(Grid) = Map.LayoutName;
		RRTestAccess::FGridAccess::CellSizeOf(Grid) = Map.CellSize;
		Grid->FinishSpawning(Map.GridTransform);
		Test.TestEqual(TEXT("map grid built with its spawn cell"), Grid->GetCellType(Grid->GetSpawnCell()), ERRCellType::Spawn);
		return Grid;
	}

	/** Stops every weapon of the miner. */
	void StopWeapons(const ARRMiner* Miner)
	{
		for (ARRWeapon* Weapon : Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
	}

	/** Cells whose square meets the 2D box Box (inclusive range). */
	void CellRange(const ARRArenaGrid* Grid, const FBox& Box, FIntPoint& OutMin, FIntPoint& OutMax)
	{
		OutMin = Grid->GetCellAtWorld(FVector(Box.Min.X, Box.Min.Y, Box.Min.Z));
		OutMax = Grid->GetCellAtWorld(FVector(Box.Max.X, Box.Max.Y, Box.Min.Z));
	}

	/** Block cells under the components of Actor (2D bounds of every component, visual included: the Blueprints have no collision), logged with Label. */
	int32 CountBlocksUnderActor(FAutomationTestBase& Test, const ARRArenaGrid* Grid, const AActor* Actor, const TCHAR* Label)
	{
		// The body mesh only (inherited component Mesh): floor decals of the Blueprint (task-027-02 shuttle chevrons, console
		// HackRing) lie on the floor around it and are not part of the landing footprint.
		const UStaticMeshComponent* Body = nullptr;
		TArray<UStaticMeshComponent*> Meshes;
		Actor->GetComponents(Meshes);
		for (const UStaticMeshComponent* Candidate : Meshes)
		{
			Body = Candidate && Candidate->GetName() == TEXT("Mesh") ? Candidate : Body;
		}
		const FBox Box = Body ? Body->Bounds.GetBox() : Actor->GetComponentsBoundingBox(/*bNonColliding*/ true);
		if (!Box.IsValid)
		{
			Test.AddInfo(FString::Printf(TEXT("%s: no component bounds"), Label));
			return 0;
		}
		FIntPoint Min;
		FIntPoint Max;
		CellRange(Grid, Box, Min, Max);
		int32 Blocks = 0;
		for (int32 Y = Min.Y; Y <= Max.Y; ++Y)
		{
			for (int32 X = Min.X; X <= Max.X; ++X)
			{
				Blocks += Grid->IsBlockCell(FIntPoint(X, Y)) ? 1 : 0;
			}
		}
		const FVector Size = Box.GetSize();
		Test.AddInfo(FString::Printf(TEXT("%s: footprint %.0f x %.0f uu = cells (%d, %d)..(%d, %d), %d block cell(s) under it"),
			Label, Size.X, Size.Y, Min.X, Min.Y, Max.X, Max.Y, Blocks));
		return Blocks;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRFineContactDigPassageTest, "RogueRigs.Integration.FineContactDigOpensPassage",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRFineContactDigPassageTest::RunTest(const FString& Parameters)
{
	using namespace RRFineGridIntegrationTests;

	// L_ArenaM1 grid settings, final BP_Miner (weapons stopped, walking without controller) and BP_Chaser shut in a pocket.
	FMapSetup Map;
	if (!ReadMap(*this, Map))
	{
		return false;
	}
	FTestWorld TestWorld(TEXT("RRFineDigPassageWorld"));
	ARRArenaGrid* Grid = SpawnMapGrid(*this, TestWorld, Map);
	if (!Grid)
	{
		return false;
	}
	TestEqual(TEXT("miner start is floor"), Grid->GetCellType(MinerStartCell), ERRCellType::Floor);
	TestEqual(TEXT("notch is floor"), Grid->GetCellType(NotchCell), ERRCellType::Floor);
	TestEqual(TEXT("pocket floor"), Grid->GetCellType(PocketFloorCell), ERRCellType::Floor);
	TestTrue(TEXT("wall (120, 109) is a block"), Grid->IsBlockCell(FIntPoint(NotchCell.X, NotchCell.Y - 1)));
	TestTrue(TEXT("wall (119, 108) is a block"), Grid->IsBlockCell(FIntPoint(PocketFloorCell.X, PocketFloorCell.Y + 1)));
	ARRMiner* Miner = SpawnMiner(*this, TestWorld, Grid, MinerStartCell);
	UClass* ChaserClass = LoadTestClass<ARRChaser>(*this, ChaserClassPath);
	URRFlowFieldSubsystem* FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
	if (!Miner || !ChaserClass || !TestNotNull(TEXT("flow field"), FlowField) || !TestNotNull(TEXT("drone"), Miner->GetDrone()))
	{
		return false;
	}
	StopWeapons(Miner);
	UCharacterMovementComponent* MinerMovement = Miner->GetCharacterMovement();
	MinerMovement->bRunPhysicsWithNoController = true;
	MinerMovement->SetDefaultMovementMode();
	const float ChaserHalfHeight = ChaserClass->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	ARRChaser* Chaser = TestWorld.Spawn<ARRChaser>(ChaserClass, Grid->GetCellCenter(PocketChaserCell.X, PocketChaserCell.Y) + FVector(0.f, 0.f, ChaserHalfHeight + 2.f));
	if (!TestNotNull(TEXT("BP_Chaser spawned in the pocket"), Chaser))
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Grid->OnCellDestroyed.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleCellDestroyed);

	// One second at rest: the miner touches nothing, the chaser has no path and stays in the pocket.
	const FVector ChaserStart = Chaser->GetActorLocation();
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	TestFalse(TEXT("miner at rest touches no rock"), Miner->IsTouchingRock());
	TestFalse(TEXT("pocket unreachable before the dig"), FlowField->IsReachable(Chaser->GetActorLocation()));
	const double IdleDrift = FVector::Dist2D(Chaser->GetActorLocation(), ChaserStart);
	TestTrue(FString::Printf(TEXT("chaser stays in the pocket (%.1f uu)"), IdleDrift), Grid->GetCellAtWorld(Chaser->GetActorLocation()).Y <= 107);
	const float StartHealth = Miner->GetHealthSet()->GetHealth();

	// The miner walks north into the wall (move input only, the stick of a player): contact arc dig, no other command.
	const FVector North = (Grid->GetCellCenter(NotchCell.X, NotchCell.Y - 1) - Grid->GetCellCenter(NotchCell.X, NotchCell.Y)).GetSafeNormal2D();
	constexpr int32 MaxPushFrames = 360;
	int32 FirstDigFrame = INDEX_NONE;
	int32 OpenFrame = INDEX_NONE;
	int32 ReachableFrame = INDEX_NONE;
	int32 PushFrames = 0;
	int32 MaxArc = 0;
	int32 DestroyedCount = 0;
	TSet<FIntPoint> ChaserCells;
	int32 BlockedFrames = 0;
	int32 LastDestroyFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= MaxPushFrames; ++Frame)
	{
		RRTestAccess::FMinerAccess::PendingMoveInputOf(Miner) = North;
		Miner->AddMovementInput(North, 1.f);
		TestWorld.Tick();
		PushFrames = Frame;
		const FIntPoint ChaserCell = Grid->GetCellAtWorld(Chaser->GetActorLocation());
		ChaserCells.Add(ChaserCell);
		BlockedFrames += Grid->GetCellType(ChaserCell) == ERRCellType::Floor ? 0 : 1;
		const ARRDrone* Drone = Miner->GetDrone();
		if (Drone->GetState() == ERRDroneState::Dig)
		{
			FirstDigFrame = FirstDigFrame == INDEX_NONE ? Frame : FirstDigFrame;
			MaxArc = FMath::Max(MaxArc, Drone->GetDigCells().Num());
		}
		// The opening is the last destruction before the pocket became reachable.
		if (Recorder->GetDestroyedCells().Num() > DestroyedCount)
		{
			DestroyedCount = Recorder->GetDestroyedCells().Num();
			LastDestroyFrame = Frame;
		}
		if (ReachableFrame == INDEX_NONE && FlowField->IsReachable(Chaser->GetActorLocation()))
		{
			ReachableFrame = Frame;
			OpenFrame = LastDestroyFrame;
		}
		// Stop pushing as soon as the robots have a path.
		if (ReachableFrame != INDEX_NONE)
		{
			break;
		}
	}
	const FIntPoint MinerCell = Grid->GetCellAtWorld(Miner->GetActorLocation());
	const TArray<FIntPoint> Dug = Recorder->GetDestroyedCells();
	AddInfo(FString::Printf(TEXT("dig from frame %d (largest arc %d); pocket opened by the dig at frame %d (%.2f s); pocket reachable at frame %d; %d cells dug in %d frames; miner on (%d, %d)"),
		FirstDigFrame, MaxArc, OpenFrame, OpenFrame * FrameSeconds, ReachableFrame, Dug.Num(), PushFrames, MinerCell.X, MinerCell.Y));
	TestTrue(TEXT("touching the wall started the arc dig"), FirstDigFrame != INDEX_NONE);
	TestTrue(TEXT("arc of several blocks"), MaxArc >= 3);
	TestTrue(TEXT("the contact dig opened the wall"), OpenFrame != INDEX_NONE);
	TestTrue(TEXT("opened within 3 s of pushing"), OpenFrame != INDEX_NONE && OpenFrame <= 180);
	TestTrue(TEXT("pocket reachable within 3 frames of the opening"), ReachableFrame != INDEX_NONE && OpenFrame != INDEX_NONE && ReachableFrame - OpenFrame <= 3);
	TestTrue(TEXT("every dug cell lies in the wall and pocket in front of the miner"), !Recorder->GetDestroyedCells().ContainsByPredicate([](const FIntPoint& Cell) { return Cell.X < 116 || Cell.X > 124 || Cell.Y < 100 || Cell.Y > 110; }));
	TestTrue(TEXT("the miner advanced into the wall"), MinerCell.Y <= 109);

	// The miner backs out of its tunnel for 1.5 s and waits: the chaser must leave the pocket through the dug cells to reach it.
	TSet<FIntPoint> DugSet(Dug);
	int32 ArrivalFrame = INDEX_NONE;
	const float ArrivalDistance = 150.f;
	constexpr int32 BackOffFrames = 90;
	for (int32 Frame = 1; Frame <= 900 && ArrivalFrame == INDEX_NONE; ++Frame)
	{
		if (Frame <= BackOffFrames)
		{
			RRTestAccess::FMinerAccess::PendingMoveInputOf(Miner) = -North;
			Miner->AddMovementInput(-North, 1.f);
		}
		TestWorld.Tick();
		if (!IsValid(Chaser))
		{
			break;
		}
		const FIntPoint Cell = Grid->GetCellAtWorld(Chaser->GetActorLocation());
		ChaserCells.Add(Cell);
		BlockedFrames += Grid->GetCellType(Cell) == ERRCellType::Floor ? 0 : 1;
		const bool bHit = Miner->GetHealthSet()->GetHealth() < StartHealth;
		ArrivalFrame = Frame > BackOffFrames && (bHit || FVector::Dist2D(Chaser->GetActorLocation(), Miner->GetActorLocation()) <= ArrivalDistance) ? Frame : INDEX_NONE;
	}
	int32 DugCellsCrossed = 0;
	for (const FIntPoint& Cell : ChaserCells)
	{
		DugCellsCrossed += DugSet.Contains(Cell) ? 1 : 0;
	}
	AddInfo(FString::Printf(TEXT("chaser reached the miner %d frames (%.2f s) after the opening (miner backed out to (%d, %d)), %.0f uu from its start; crossed %d dug cell(s)"),
		ArrivalFrame, ArrivalFrame * FrameSeconds, Grid->GetCellAtWorld(Miner->GetActorLocation()).X, Grid->GetCellAtWorld(Miner->GetActorLocation()).Y, IsValid(Chaser) ? FVector::Dist2D(ChaserStart, Chaser->GetActorLocation()) : -1.0, DugCellsCrossed));
	TestTrue(TEXT("chaser reaches the miner within 15 s"), ArrivalFrame != INDEX_NONE);
	TestTrue(TEXT("chaser went through the dug passage"), DugCellsCrossed > 0);
	TestEqual(TEXT("chaser centre never on a block or void cell"), BlockedFrames, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRFineConsoleShuttlePlacementTest, "RogueRigs.Integration.FineConsoleShuttlePlacement",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRFineConsoleShuttlePlacementTest::RunTest(const FString& Parameters)
{
	using namespace RRFineGridIntegrationTests;

	// L_ArenaM1 grid and player start, BP_RRGameMode run flow config: the console and shuttle Blueprints land at the world
	// distances of the 200 uu design (12..20 and >= 25 cells of 200 uu) on cells the miner and robots can walk to.
	FMapSetup Map;
	if (!ReadMap(*this, Map) || !TestTrue(TEXT("L_ArenaM1 has a player start"), Map.bHasPlayerStart))
	{
		return false;
	}
	UClass* GameModeClass = LoadTestClass<ARRGameMode>(*this, GameModeClassPath);
	UClass* ConsoleClass = LoadTestClass<ARRConsole>(*this, ConsoleClassPath);
	UClass* ShuttleClass = LoadTestClass<ARRShuttle>(*this, ShuttleClassPath);
	if (!GameModeClass || !ConsoleClass || !ShuttleClass)
	{
		return false;
	}
	const FRRRunFlowConfig& Config = GameModeClass->GetDefaultObject<ARRGameMode>()->GetRunFlowConfig();
	TestTrue(TEXT("BP_RRGameMode console is BP_ObjectiveConsole"), Config.ConsoleClass.Get() == ConsoleClass);
	TestTrue(TEXT("BP_RRGameMode shuttle is BP_ExtractionShuttle"), Config.ShuttleClass.Get() == ShuttleClass);
	TestEqual(TEXT("console ring 36 cells"), Config.ConsoleMinDistanceCells, 36.f);
	TestEqual(TEXT("console ring 60 cells"), Config.ConsoleMaxDistanceCells, 60.f);
	TestEqual(TEXT("shuttle 37 cells min"), Config.ShuttleMinDistanceCells, 37.f);
	TestEqual(TEXT("shuttle 50 cells max"), Config.ShuttleMaxDistanceCells, 50.f);
	TestEqual(TEXT("board radius 3 cells"), Config.ShuttleBoardRadiusCells, 3.f);

	for (const int32 Seed : { 1, 20261003, 777, 4242, 90210 })
	{
		FTestWorld TestWorld(TEXT("RRFinePlacementWorld"));
		ARRArenaGrid* Grid = SpawnMapGrid(*this, TestWorld, Map);
		if (!Grid)
		{
			return false;
		}
		const float CellSize = Grid->GetCellSize();
		const FIntPoint StartCell = Grid->GetCellAtWorld(Map.PlayerStart);
		TestTrue(*FString::Printf(TEXT("seed %d: player start on open floor"), Seed), Grid->GetCellType(StartCell) == ERRCellType::Floor || Grid->GetCellType(StartCell) == ERRCellType::Spawn);
		const FIntPoint SpawnCell = Grid->GetSpawnCell();
		const float StartToSpawn = FVector::Dist2D(Map.PlayerStart, Grid->GetCellCenter(SpawnCell.X, SpawnCell.Y));
		TestTrue(*FString::Printf(TEXT("seed %d: player start within 2 cells of the layout spawn (%.0f uu)"), Seed, StartToSpawn), StartToSpawn <= 2.f * CellSize);
		ARRMiner* Miner = SpawnMiner(*this, TestWorld, Grid, StartCell);
		URRRunFlowSubsystem* RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>();
		URRFlowFieldSubsystem* FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
		if (!Miner || !TestNotNull(TEXT("run flow"), RunFlow) || !TestNotNull(TEXT("flow field"), FlowField))
		{
			return false;
		}
		StopWeapons(Miner);
		RunFlow->SetConfig(Config);
		RunFlow->SetRandomSeed(Seed);
		TestWorld.Tick();

		// Console: Blueprint, floor cell at the cell centre, 36..60 cells = 2412..4020 uu (2400..4000 at 200 uu), reachable.
		const FVector MinerAt = Miner->GetActorLocation();
		ARRConsole* Console = RunFlow->SkipToConsole();
		if (!TestNotNull(*FString::Printf(TEXT("seed %d: console dropped"), Seed), Console))
		{
			return false;
		}
		const FIntPoint ConsoleCell = Console->GetCell();
		const double ConsoleDistance = FVector::Dist2D(Console->GetActorLocation(), MinerAt);
		AddInfo(FString::Printf(TEXT("seed %d: console on (%d, %d), %.0f uu (%.1f cells), %d marked rocks within %.0f uu"), Seed, ConsoleCell.X, ConsoleCell.Y,
			ConsoleDistance, ConsoleDistance / CellSize, Console->GetMarkedRockCount(), 6.f * CellSize));
		TestTrue(*FString::Printf(TEXT("seed %d: console is BP_ObjectiveConsole"), Seed), Console->IsA(ConsoleClass));
		TestEqual(*FString::Printf(TEXT("seed %d: console on floor"), Seed), Grid->GetCellType(ConsoleCell), ERRCellType::Floor);
		TestTrue(*FString::Printf(TEXT("seed %d: console at its 2 x 2 footprint centre"), Seed), Console->GetFootprintCells() == FIntPoint(2, 2)
			&& Console->GetActorLocation().Equals(RRLandingFootprint::GetCenter(Grid, ConsoleCell, Console->GetFootprintCells()), 0.5));
		TestTrue(*FString::Printf(TEXT("seed %d: console 2400..4000 uu away (one cell of slack)"), Seed), ConsoleDistance >= 2400.0 - CellSize && ConsoleDistance <= 4000.0 + CellSize);
		TestTrue(*FString::Printf(TEXT("seed %d: console reachable"), Seed), FlowField->GetCellDistance(ConsoleCell) >= 0);
		TestTrue(*FString::Printf(TEXT("seed %d: rocks marked around the console"), Seed), Console->GetMarkedRockCount() > 0);
		TestTrue(*FString::Printf(TEXT("seed %d: console zone 9 cells = 603 uu"), Seed), FMath::IsNearlyEqual(Console->GetZoneRadius(), 9.f * CellSize, 0.5f));
		TestEqual(*FString::Printf(TEXT("seed %d: no block under the console mesh"), Seed),
			CountBlocksUnderActor(*this, Grid, Console, *FString::Printf(TEXT("seed %d: console"), Seed)), 0);

		// Shuttle: Blueprint, floor cell, 37 to 50 cells = 2479 to 3350 uu, reachable, board radius 3 cells = 201 uu.
		TestTrue(*FString::Printf(TEXT("seed %d: skip to extraction"), Seed), RunFlow->SkipToExtraction());
		ARRShuttle* Shuttle = RunFlow->GetShuttle();
		if (!TestNotNull(*FString::Printf(TEXT("seed %d: shuttle landed"), Seed), Shuttle))
		{
			return false;
		}
		const FIntPoint ShuttleCell = Shuttle->GetCell();
		const double ShuttleDistance = FVector::Dist2D(Shuttle->GetActorLocation(), MinerAt);
		int32 BoardCells = 0;
		const int32 Span = FMath::CeilToInt32(Config.ShuttleBoardRadiusCells);
		for (int32 DY = -Span; DY <= Span; ++DY)
		{
			for (int32 DX = -Span; DX <= Span; ++DX)
			{
				const FIntPoint Near(ShuttleCell.X + DX, ShuttleCell.Y + DY);
				const bool bInside = FVector::Dist2D(Grid->GetCellCenter(Near.X, Near.Y), Shuttle->GetActorLocation()) <= Shuttle->GetBoardRadius();
				BoardCells += bInside && Grid->GetCellType(Near) == ERRCellType::Floor && FlowField->GetCellDistance(Near) >= 0 ? 1 : 0;
			}
		}
		AddInfo(FString::Printf(TEXT("seed %d: shuttle on (%d, %d), %.0f uu (%.1f cells), board radius %.0f uu, %d reachable floor cells to board from"),
			Seed, ShuttleCell.X, ShuttleCell.Y, ShuttleDistance, ShuttleDistance / CellSize, Shuttle->GetBoardRadius(), BoardCells));
		TestTrue(*FString::Printf(TEXT("seed %d: shuttle is BP_ExtractionShuttle"), Seed), Shuttle->IsA(ShuttleClass));
		TestEqual(*FString::Printf(TEXT("seed %d: shuttle on floor"), Seed), Grid->GetCellType(ShuttleCell), ERRCellType::Floor);
		TestTrue(*FString::Printf(TEXT("seed %d: shuttle at its 4 x 3 footprint centre"), Seed), Shuttle->GetFootprintCells() == FIntPoint(4, 3)
			&& Shuttle->GetActorLocation().Equals(RRLandingFootprint::GetCenter(Grid, ShuttleCell, Shuttle->GetFootprintCells()), 1.0));
		TestTrue(*FString::Printf(TEXT("seed %d: shuttle 37 to 50 cells away (one cell of slack)"), Seed),
			ShuttleDistance >= 37.0 * CellSize - CellSize && ShuttleDistance <= 50.0 * CellSize + CellSize);
		TestTrue(*FString::Printf(TEXT("seed %d: shuttle reachable"), Seed), FlowField->GetCellDistance(ShuttleCell) >= 0);
		TestTrue(*FString::Printf(TEXT("seed %d: board radius 3 cells = 201 uu"), Seed), FMath::IsNearlyEqual(Shuttle->GetBoardRadius(), 3.f * CellSize, 0.5f));
		TestTrue(*FString::Printf(TEXT("seed %d: several floor cells to board from"), Seed), BoardCells >= 2);
		TestEqual(*FString::Printf(TEXT("seed %d: no block under the shuttle mesh"), Seed),
			CountBlocksUnderActor(*this, Grid, Shuttle, *FString::Printf(TEXT("seed %d: shuttle"), Seed)), 0);
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
