// Rogue Rigs - integration tests with the final Blueprints (RogueRigs.Integration.*): BP_Chaser reaching and hitting BP_Miner around rocks, the starting rifle killing a chaser, five rifle kills whose XP cubes are collected up to level 2, miner death and game over, a 50-chaser headless timing run, crowd packing with and without robot-robot collision, and the explosion loop (barrel chain from a rifle shot with BP_ExplosionFX, BP_Kamikaze reaching the miner, point-blank barrel, rifle-killed kamikaze after 10 non-critical hits), and the level-up upgrade loop (real game mode pause, BP_RRPlayerController, BP_EnemySpawner, WBP_HUD cards, reroll, damage card, resume).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "AI/Navigation/AvoidanceManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Debug/RRCheatSettings.h"
#include "Debug/RRCheats.h"
#include "Kismet/GameplayStatics.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRKamikaze.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"
#include "Explosions/RRExplosionFX.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "GAS/RRHealthAttributeSet.h"
#include "HAL/PlatformTime.h"
#include "Math/RandomStream.h"
#include "Pickups/RRPickup.h"
#include "Projectiles/RRProjectile.h"
#include "Player/RRGameMode.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "UI/RRHUDWidget.h"
#include "UObject/StrongObjectPtr.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRIntegrationTests
{
	using namespace RRTestHarness;

	/** XP cubes burst by one chaser (task-027-01: its 1 XP split over 3 cubes) and by one kamikaze (4 cubes). */
	constexpr int32 ChaserXPCubes = 3;
	constexpr int32 KamikazeXPCubes = 4;

	/** Final Blueprints of the loop. */
	const TCHAR* const ChaserClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Chaser.BP_Chaser_C");
	const TCHAR* const GameModeClassPath = TEXT("/Game/RogueRigs/Blueprints/Player/BP_RRGameMode.BP_RRGameMode_C");

	/** Open floor cell of arena_m1 the miner stands on in the chase tests (flow field tests). */
	const FIntPoint ChaseMinerCell(40, 40);

	/** Floor cell 8.5 cells from ChaseMinerCell in a straight line, 15 BFS steps around the rocks (28-31 x 29-36, 34-38 x 35-40). */
	const FIntPoint WindingStartCell(34, 34);

	/** Open row 42 of arena_m1: the armed miner on the west end, the chaser 9 cells east (1800 uu, inside the rifle Range 2000). */
	const FIntPoint ArmedMinerCell(32, 42);
	const FIntPoint StraightStartCell(41, 42);

	/** Rocks destroyed during the timing run to force flow-field recomputes (all touch the main floor). */
	const FIntPoint PerfRockCells[] = { FIntPoint(43, 38), FIntPoint(39, 36), FIntPoint(46, 40) };

	/** One hit received by the miner. */
	struct FHit
	{
		double Time = 0.0;
		float Damage = 0.f;
	};

	/** Spawns BP_Chaser with its feet on the floor of Cell. */
	ARRChaser* SpawnChaser(FAutomationTestBase& Test, const FTestWorld& TestWorld, UClass* ChaserClass, const ARRArenaGrid* Grid, FIntPoint Cell)
	{
		const float HalfHeight = ChaserClass->GetDefaultObject<ARRChaser>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(Grid->GetCellCenter(Cell.X, Cell.Y) + FVector(0.f, 0.f, HalfHeight + 2.f));
		ARRChaser* Chaser = TestWorld.Get()->SpawnActorDeferred<ARRChaser>(ChaserClass, SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Test.TestNotNull(FString::Printf(TEXT("chaser spawned on (%d, %d)"), Cell.X, Cell.Y), Chaser))
		{
			Chaser->FinishSpawning(SpawnTransform);
		}
		return Chaser;
	}

	/** Stops every weapon of the miner (they would shoot the chasers). */
	void StopWeapons(ARRMiner* Miner)
	{
		for (ARRWeapon* Weapon : Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
	}

	/** Records every hit of the miner (time, damage) on its health set. Remove the returned handle when done. */
	FDelegateHandle RecordHits(ARRMiner* Miner, TArray<FHit>& Hits)
	{
		UWorld* World = Miner->GetWorld();
		return Miner->GetHealthSet()->OnDamageReceived.AddLambda([&Hits, World](float Damage, bool, float)
		{
			Hits.Add({ World->GetTimeSeconds(), Damage });
		});
	}

	/** Current Health of an actor with an ASC. */
	float GetHealth(const IAbilitySystemInterface* Actor)
	{
		return Actor->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Live XP pickups in the world. */
	int32 CountXPPickups(UWorld* World)
	{
		int32 Count = 0;
		for (TActorIterator<ARRPickup> It(World); It; ++It)
		{
			Count += IsValid(*It) && !It->IsActorBeingDestroyed() && It->GetPickupType() == ERRPickupType::XP ? 1 : 0;
		}
		return Count;
	}

	/** Aims the miner along a world direction with the stick input (of the four cardinal inputs, the one closest to Direction). */
	void AimAlong(ARRMiner* Miner, const FVector& Direction)
	{
		const FVector2D Inputs[] = { FVector2D(1.f, 0.f), FVector2D(-1.f, 0.f), FVector2D(0.f, 1.f), FVector2D(0.f, -1.f) };
		FVector2D Best = Inputs[0];
		float BestDot = -2.f;
		for (const FVector2D& Input : Inputs)
		{
			Miner->ApplyStickAimInput(Input);
			const float Dot = FVector::DotProduct(Miner->GetAimDirection(), Direction.GetSafeNormal2D());
			if (Dot > BestDot)
			{
				BestDot = Dot;
				Best = Input;
			}
		}
		Miner->ApplyStickAimInput(Best);
	}

	/** Grid, Blueprint chaser class and flow field of one test world, with a miner on Cell. */
	struct FLoopSetup
	{
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		UClass* ChaserClass = nullptr;
		URRFlowFieldSubsystem* FlowField = nullptr;
	};

	/**
	 * Spawns BP_ArenaGrid and BP_Miner on Cell (weapons stopped unless bArmed; the shotgun always stopped), loads BP_Chaser, ticks one frame.
	 * bFineGrid: the game grid (arena_m1_fine, 67 uu) instead of the legacy 200 uu grid, for loops whose rules count cells (objectives).
	 */
	bool SetUp(FAutomationTestBase& Test, const FTestWorld& TestWorld, FIntPoint Cell, bool bArmed, FLoopSetup& Out, bool bFineGrid = false)
	{
		Out.ChaserClass = LoadTestClass<ARRChaser>(Test, ChaserClassPath);
		Out.Grid = Out.ChaserClass ? (bFineGrid ? SpawnFineGrid(Test, TestWorld) : SpawnGrid(Test, TestWorld)) : nullptr;
		Out.Miner = Out.Grid ? SpawnMiner(Test, TestWorld, Out.Grid, Cell, M1TestLoadout) : nullptr;
		Out.FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
		if (!Out.Miner || !Test.TestNotNull(TEXT("flow field subsystem"), Out.FlowField))
		{
			return false;
		}
		if (!bArmed)
		{
			StopWeapons(Out.Miner);
		}
		else
		{
			// Rifle loops: the M1 loadout keeps its shotgun, stopped so only the rifle hits (its kill times and hit counts were measured alone).
			StopShotgun(Out.Miner);
		}
		TestWorld.Tick();
		return Test.TestEqual(TEXT("field goal cell is the miner cell"), Out.FlowField->GetGoalCell(), Cell);
	}

	/**
	 * Spawns Count BP_Chaser one after the other on StartCell, each once the previous one is dead; each must be killed by the
	 * armed miner's rifle within 10 s and before reaching attack range. Appends " time after spawn / distance;" per kill to OutKills.
	 */
	bool KillChasersOneByOne(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FLoopSetup& Setup, FIntPoint StartCell, int32 Count, FString& OutKills)
	{
		for (int32 Index = 0; Index < Count; ++Index)
		{
			ARRChaser* Chaser = SpawnChaser(Test, TestWorld, Setup.ChaserClass, Setup.Grid, StartCell);
			if (!Chaser)
			{
				return false;
			}
			TWeakObjectPtr<ARRChaser> Weak(Chaser);
			const double SpawnTime = TestWorld.Get()->GetTimeSeconds();
			bool bDead = false;
			for (int32 Frame = 0; Frame < 600 && !bDead; ++Frame)
			{
				TestWorld.Tick();
				bDead = Weak.IsValid() && Weak->IsDead();
			}
			if (!Test.TestTrue(FString::Printf(TEXT("chaser %d killed by the rifle within 10 s"), Index + 1), bDead))
			{
				return false;
			}
			const float Distance = FVector::Dist2D(Weak->GetActorLocation(), Setup.Miner->GetActorLocation());
			OutKills += FString::Printf(TEXT(" %.3f s / %.0f uu;"), TestWorld.Get()->GetTimeSeconds() - SpawnTime, Distance);
			Test.TestTrue(FString::Printf(TEXT("chaser %d killed before reaching attack range"), Index + 1), Distance > Chaser->GetStats().AttackRange);
		}
		return true;
	}

	/**
	 * Walks the miner over the drop zone until Recorder counts Count collected pickups (10 s max): whenever no XP cube lies within
	 * its PickupRadius, the miner steps onto the nearest one; cubes in range fly to it. Returns the number of steps; OutSeconds
	 * gets the collection time. Stops on the tick that collects the last cube.
	 */
	int32 CollectXPCubes(const FTestWorld& TestWorld, ARRMiner* Miner, const URRTestEventRecorder* Recorder, int32 Count, double& OutSeconds)
	{
		const double Start = TestWorld.Get()->GetTimeSeconds();
		int32 Moves = 0;
		for (int32 Frame = 0; Frame < 600 && Recorder->GetCollectedPickups().Num() < Count; ++Frame)
		{
			bool bInRange = false;
			const ARRPickup* Nearest = nullptr;
			float NearestDistance = TNumericLimits<float>::Max();
			for (TActorIterator<ARRPickup> It(TestWorld.Get()); It; ++It)
			{
				if (IsValid(*It) && !It->IsActorBeingDestroyed() && It->GetPickupType() == ERRPickupType::XP)
				{
					const float Distance = FVector::Dist2D(It->GetActorLocation(), Miner->GetActorLocation());
					bInRange |= Distance <= Miner->GetPickupRadius();
					if (Distance < NearestDistance)
					{
						NearestDistance = Distance;
						Nearest = *It;
					}
				}
			}
			if (!bInRange && Nearest)
			{
				Miner->SetActorLocation(FVector(Nearest->GetActorLocation().X, Nearest->GetActorLocation().Y, Miner->GetActorLocation().Z));
				++Moves;
			}
			TestWorld.Tick();
		}
		OutSeconds = TestWorld.Get()->GetTimeSeconds() - Start;
		return Moves;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationChaserReachesTest, "RogueRigs.Integration.ChaserReachesAndHitsMiner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationChaserReachesTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationTests;

	FTestWorld TestWorld(TEXT("RRIntegrationReach"));
	FLoopSetup Setup;
	if (!SetUp(*this, TestWorld, ChaseMinerCell, /*bArmed*/ false, Setup))
	{
		return false;
	}
	const float Straight = FVector2D(WindingStartCell - ChaseMinerCell).Size();
	const int32 Steps = Setup.FlowField->GetCellDistance(WindingStartCell);
	AddInfo(FString::Printf(TEXT("start (%d, %d): straight %.2f cells, path %d steps"), WindingStartCell.X, WindingStartCell.Y, Straight, Steps));
	TestTrue(TEXT("start about 8 cells away"), Straight >= 7.5f && Straight <= 9.f);
	TestTrue(TEXT("winding path: BFS steps >= 1.5 x straight distance"), Steps >= 1.5f * Straight);

	ARRChaser* Chaser = SpawnChaser(*this, TestWorld, Setup.ChaserClass, Setup.Grid, WindingStartCell);
	if (!Chaser)
	{
		return false;
	}
	ARRMiner* Miner = Setup.Miner;
	const float StartHealth = GetHealth(Miner);
	const float AttackRange = Chaser->GetStats().AttackRange;
	TArray<FHit> Hits;
	const FDelegateHandle Handle = RecordHits(Miner, Hits);

	// Up to 15 s: the chaser must stay on walkable cells, reach attack range, then land a hit.
	const double StartTime = TestWorld.Get()->GetTimeSeconds();
	double ReachTime = -1.0;
	int32 OffFloorFrames = 0;
	for (int32 Frame = 0; Frame < 900 && Hits.Num() == 0; ++Frame)
	{
		TestWorld.Tick();
		OffFloorFrames += Setup.FlowField->IsWalkableCell(Setup.Grid->GetCellAtWorld(Chaser->GetActorLocation())) ? 0 : 1;
		if (ReachTime < 0.0 && FVector::Dist2D(Chaser->GetActorLocation(), Miner->GetActorLocation()) <= AttackRange)
		{
			ReachTime = TestWorld.Get()->GetTimeSeconds() - StartTime;
		}
	}
	Miner->GetHealthSet()->OnDamageReceived.Remove(Handle);

	const double HitTime = Hits.Num() > 0 ? Hits[0].Time - StartTime : -1.0;
	AddInfo(FString::Printf(TEXT("attack range %.0f uu reached at %.3f s, first hit at %.3f s (%d damage), miner Health %.0f -> %.0f"),
		AttackRange, ReachTime, HitTime, Hits.Num() > 0 ? FMath::RoundToInt(Hits[0].Damage) : 0, StartHealth, GetHealth(Miner)));
	TestEqual(TEXT("chaser center never on a non-walkable cell"), OffFloorFrames, 0);
	TestTrue(TEXT("chaser reached attack range within 15 s"), ReachTime >= 0.0);
	if (!TestEqual(TEXT("chaser hit the miner"), Hits.Num(), 1))
	{
		return false;
	}
	TestEqual(TEXT("hit deals the CSV Damage"), Hits[0].Damage, Chaser->GetStats().Damage);
	TestEqual(TEXT("miner Health lowered by one hit"), GetHealth(Miner), StartHealth - Chaser->GetStats().Damage);
	TestTrue(TEXT("hit lands after the wind-up that follows the arrival"), HitTime >= ReachTime + Chaser->GetStats().WindUp - FrameSeconds - 1e-3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationRifleKillsTest, "RogueRigs.Integration.RifleKillsChaser",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationRifleKillsTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationTests;

	FTestWorld TestWorld(TEXT("RRIntegrationRifle"));
	FLoopSetup Setup;
	if (!SetUp(*this, TestWorld, ArmedMinerCell, /*bArmed*/ true, Setup))
	{
		return false;
	}
	ARRMiner* Miner = Setup.Miner;
	const FVector Toward = Setup.Grid->GetCellCenter(StraightStartCell.X, StraightStartCell.Y) - Miner->GetActorLocation();
	AimAlong(Miner, Toward);
	TestTrue(TEXT("miner aims at the chaser start"), FVector::DotProduct(Miner->GetAimDirection(), Toward.GetSafeNormal2D()) > 0.99f);
	bool bFiring = false;
	for (const ARRWeapon* Weapon : Miner->GetWeapons())
	{
		bFiring |= IsValid(Weapon) && Weapon->GetAutoFireComponent()->IsFiring();
	}
	TestTrue(TEXT("starting weapon firing"), bFiring);

	ARRChaser* Chaser = SpawnChaser(*this, TestWorld, Setup.ChaserClass, Setup.Grid, StraightStartCell);
	if (!Chaser)
	{
		return false;
	}
	TWeakObjectPtr<ARRChaser> WeakChaser(Chaser);
	const float StartHealth = GetHealth(Miner);
	const double StartTime = TestWorld.Get()->GetTimeSeconds();
	double DeathTime = -1.0;
	float DeathDistance = 0.f;
	for (int32 Frame = 0; Frame < 600 && DeathTime < 0.0; ++Frame)
	{
		TestWorld.Tick();
		if (WeakChaser.IsValid() && WeakChaser->IsDead())
		{
			DeathTime = TestWorld.Get()->GetTimeSeconds() - StartTime;
			DeathDistance = FVector::Dist2D(WeakChaser->GetActorLocation(), Miner->GetActorLocation());
		}
	}
	AddInfo(FString::Printf(TEXT("chaser killed at %.3f s, %.0f uu from the miner; miner Health %.0f -> %.0f"),
		DeathTime, DeathDistance, StartHealth, GetHealth(Miner)));
	if (!TestTrue(TEXT("chaser killed within 10 s"), DeathTime >= 0.0))
	{
		return false;
	}
	TestTrue(TEXT("killed before reaching attack range"), DeathDistance > Chaser->GetStats().AttackRange);
	TestEqual(TEXT("miner untouched"), GetHealth(Miner), StartHealth);

	// Shrink, then BP_PickupXP dropped and the chaser destroyed.
	for (int32 Frame = 0; Frame < 15; ++Frame)
	{
		TestWorld.Tick();
	}
	TestFalse(TEXT("chaser destroyed after the shrink"), WeakChaser.IsValid() && !WeakChaser->IsActorBeingDestroyed());
	TestEqual(TEXT("one chaser burst of XP cubes dropped"), CountXPPickups(TestWorld.Get()), ChaserXPCubes);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationXPLevelUpTest, "RogueRigs.Integration.RifleKillsFiveChasersLevelUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationXPLevelUpTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationTests;

	FTestWorld TestWorld(TEXT("RRIntegrationXPLevel"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FLoopSetup Setup;
	if (!SetUp(*this, TestWorld, ArmedMinerCell, /*bArmed*/ true, Setup))
	{
		return false;
	}
	ARRMiner* Miner = Setup.Miner;
	URRProgressionComponent* Progression = Miner->GetProgression();
	if (!TestNotNull(TEXT("miner has a progression component"), Progression))
	{
		return false;
	}
	TestEqual(TEXT("starts at level 1"), Progression->GetLevel(), 1);
	TestEqual(TEXT("starts with 0 XP"), Progression->GetXP(), 0.f);
	TestEqual(TEXT("level 1 needs 5 XP"), Progression->GetXPToNextLevel(), 5.f);

	// Pickups find the miner as the player 0 pawn: a player controller (no local player) possesses it.
	APlayerController* Controller = TestWorld.Spawn<APlayerController>(APlayerController::StaticClass(), Miner->GetActorLocation());
	if (!TestNotNull(TEXT("player controller spawned"), Controller))
	{
		return false;
	}
	Controller->Possess(Miner);
	Progression->OnXPChanged.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleXPChanged);
	Progression->OnLevelUp.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleLevelUp);
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);
	const FVector Toward = Setup.Grid->GetCellCenter(StraightStartCell.X, StraightStartCell.Y) - Miner->GetActorLocation();
	AimAlong(Miner, Toward);
	TestTrue(TEXT("miner aims down the corridor"), FVector::DotProduct(Miner->GetAimDirection(), Toward.GetSafeNormal2D()) > 0.99f);
	const float StartHealth = GetHealth(Miner);

	// Five BP_Chaser one after the other from the east end of row 42, each spawned once the previous one is dead.
	const int32 ChaserCount = 5;
	FString Kills;
	if (!KillChasersOneByOne(*this, TestWorld, Setup, StraightStartCell, ChaserCount, Kills))
	{
		return false;
	}

	// Pop and rest: the cubes lie 1000 uu or more from the miner, outside its PickupRadius, so none is collected yet.
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	TArray<FVector> Cubes;
	for (TActorIterator<ARRPickup> It(TestWorld.Get()); It; ++It)
	{
		if (IsValid(*It) && !It->IsActorBeingDestroyed() && It->GetPickupType() == ERRPickupType::XP)
		{
			Cubes.Add(It->GetActorLocation());
		}
	}
	TestEqual(TEXT("five bursts of XP cubes dropped"), Cubes.Num(), ChaserCount * ChaserXPCubes);
	TestEqual(TEXT("no cube collected from afar"), Recorder->GetCollectedPickups().Num(), 0);
	TestEqual(TEXT("still level 1 before collecting"), Progression->GetLevel(), 1);
	if (Cubes.Num() != ChaserCount * ChaserXPCubes)
	{
		return false;
	}

	// The kills spread over about 600 uu (the first shots land mid-burst): the miner walks the drop zone.
	double CollectTime = 0.0;
	const int32 Moves = CollectXPCubes(TestWorld, Miner, Recorder.Get(), ChaserCount * ChaserXPCubes, CollectTime);
	TestWorld.Tick();

	AddInfo(FString::Printf(TEXT("kills (time after spawn / distance):%s %d miner move(s) (PickupRadius %.0f); %d collected in %.3f s; level %d, XP %.0f / %.0f; %d level-up(s), %d XP event(s); miner Health %.0f -> %.0f"),
		*Kills, Moves, Miner->GetPickupRadius(), Recorder->GetCollectedPickups().Num(), CollectTime, Progression->GetLevel(), Progression->GetXP(),
		Progression->GetXPToNextLevel(), Recorder->GetLevelUps().Num(), Recorder->GetXPChanges().Num(), StartHealth, GetHealth(Miner)));
	TestEqual(TEXT("every cube collected"), Recorder->GetCollectedPickups().Num(), ChaserCount * ChaserXPCubes);
	TestEqual(TEXT("no XP cube left"), CountXPPickups(TestWorld.Get()), 0);
	TestEqual(TEXT("level 2"), Progression->GetLevel(), 2);
	TestEqual(TEXT("0 XP carried"), Progression->GetXP(), 0.f);
	TestEqual(TEXT("level 2 needs the XPCurve.csv row 2"), Progression->GetXPToNextLevel(), Progression->GetXPToNextLevelAt(2));
	if (TestEqual(TEXT("exactly one level-up event"), Recorder->GetLevelUps().Num(), 1))
	{
		TestEqual(TEXT("level-up to 2"), Recorder->GetLevelUps()[0], 2);
	}
	if (TestEqual(TEXT("one XP event per cube"), Recorder->GetXPChanges().Num(), ChaserCount * ChaserXPCubes))
	{
		TestEqual(TEXT("last XP event (0, need of level 2, 2)"), Recorder->GetXPChanges().Last(), FVector(0.f, Progression->GetXPToNextLevelAt(2), 2.f));
	}
	TestEqual(TEXT("miner untouched"), GetHealth(Miner), StartHealth);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationGameOverTest, "RogueRigs.Integration.ChasersKillMinerGameOver",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationGameOverTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationTests;

	UClass* GameModeClass = LoadTestClass<ARRGameMode>(*this, GameModeClassPath);
	FTestWorld TestWorld(TEXT("RRIntegrationGameOver"));
	FLoopSetup Setup;
	if (!GameModeClass || !SetUp(*this, TestWorld, ChaseMinerCell, /*bArmed*/ false, Setup))
	{
		return false;
	}
	ARRMiner* Miner = Setup.Miner;
	ARRGameMode* GameMode = TestWorld.Spawn<ARRGameMode>(GameModeClass, FVector::ZeroVector);
	if (!TestNotNull(TEXT("game mode spawned"), GameMode))
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	GameMode->OnGameOver.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleGameOver);
	// No authority game mode in the test world: register as ARRMiner::BeginPlay does in a run. Never travel.
	GameMode->RegisterMiner(Miner);
	const float RestartDelay = GameMode->GetRestartDelay();

	ARRChaser* Chaser = SpawnChaser(*this, TestWorld, Setup.ChaserClass, Setup.Grid, ChaseMinerCell + FIntPoint(3, 0));
	if (!Chaser)
	{
		return false;
	}
	const float StartHealth = GetHealth(Miner);
	const int32 ExpectedHits = FMath::CeilToInt(StartHealth / Chaser->GetStats().Damage);
	TArray<FHit> Hits;
	const FDelegateHandle Handle = RecordHits(Miner, Hits);
	const double StartTime = TestWorld.Get()->GetTimeSeconds();
	for (int32 Frame = 0; Frame < 1800 && !Miner->IsDead(); ++Frame)
	{
		TestWorld.Tick();
	}
	const double DeathTime = TestWorld.Get()->GetTimeSeconds() - StartTime;
	AddInfo(FString::Printf(TEXT("miner Health %.0f, dead %d at %.3f s after %d hits (%d attacks); game over %d"),
		StartHealth, Miner->IsDead() ? 1 : 0, DeathTime, Hits.Num(), Chaser->GetAttackCount(), Recorder->GetGameOverCount()));
	if (!TestTrue(TEXT("miner died within 30 s"), Miner->IsDead()))
	{
		Miner->GetHealthSet()->OnDamageReceived.Remove(Handle);
		return false;
	}
	TestEqual(TEXT("died after Health / Damage hits"), Hits.Num(), ExpectedHits);
	TestEqual(TEXT("Health 0"), GetHealth(Miner), 0.f);
	TestEqual(TEXT("game over fired once"), Recorder->GetGameOverCount(), 1);
	TestTrue(TEXT("IsGameOver"), GameMode->IsGameOver());
	TestEqual(TEXT("restart armed with the Blueprint delay"), GameMode->GetRestartTimeRemaining(), RestartDelay, 1.e-4f);

	// One more second (below the restart delay: no travel): no further hit, no second game over.
	const int32 AttacksAtDeath = Chaser->GetAttackCount();
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	Miner->GetHealthSet()->OnDamageReceived.Remove(Handle);
	AddInfo(FString::Printf(TEXT("1 s after death: %d hits, %d attacks, chaser state %s, restart in %.3f s"),
		Hits.Num(), Chaser->GetAttackCount(), *UEnum::GetValueAsString(Chaser->GetState()), GameMode->GetRestartTimeRemaining()));
	TestTrue(TEXT("restart delay longer than the extra second"), RestartDelay > 1.f + 2.f * FrameSeconds);
	TestEqual(TEXT("no hit on the dead miner"), Hits.Num(), ExpectedHits);
	TestTrue(TEXT("at most one attack in flight after death"), Chaser->GetAttackCount() <= AttacksAtDeath + 1);
	TestEqual(TEXT("game over still fired once"), Recorder->GetGameOverCount(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationPerfTest, "RogueRigs.Integration.Perf50Chasers",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationPerfTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationTests;

	constexpr int32 ChaserCount = 50;
	constexpr int32 Frames = 600;

	FTestWorld TestWorld(TEXT("RRIntegrationPerf"));
	FLoopSetup Setup;
	if (!SetUp(*this, TestWorld, ChaseMinerCell, /*bArmed*/ false, Setup))
	{
		return false;
	}
	// The miner must outlive the run so the chasers keep chasing: huge health (test only).
	UAbilitySystemComponent* MinerASC = Setup.Miner->GetAbilitySystemComponent();
	MinerASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), 1.e9f);
	MinerASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 1.e9f);

	// Reachable cells 6 to 20 BFS steps from the miner, drawn with a fixed seed.
	TArray<FIntPoint> Candidates;
	const FIntPoint Size = Setup.Grid->GetGridSize();
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const int32 Steps = Setup.FlowField->GetCellDistance(FIntPoint(X, Y));
			if (Steps >= 6 && Steps <= 20)
			{
				Candidates.Add(FIntPoint(X, Y));
			}
		}
	}
	if (!TestTrue(TEXT("enough reachable cells"), Candidates.Num() >= ChaserCount))
	{
		return false;
	}
	FRandomStream Stream(9007);
	for (int32 Index = Candidates.Num() - 1; Index > 0; --Index)
	{
		Candidates.Swap(Index, Stream.RandRange(0, Index));
	}
	TArray<ARRChaser*> Chasers;
	for (int32 Index = 0; Index < ChaserCount; ++Index)
	{
		if (ARRChaser* Chaser = SpawnChaser(*this, TestWorld, Setup.ChaserClass, Setup.Grid, Candidates[Index]))
		{
			Chasers.Add(Chaser);
		}
	}
	if (!TestEqual(TEXT("50 chasers spawned"), Chasers.Num(), ChaserCount))
	{
		return false;
	}

	// 600 frames at 1/60 s; destroy a rock at frames 150, 300, 450 to force recomputes.
	const int32 OpenedBefore = Setup.FlowField->GetOpenedCellCount();
	TArray<double> FrameMs;
	FrameMs.Reserve(Frames);
	TArray<float> ComputeMs;
	int32 RocksDestroyed = 0;
	for (int32 Frame = 0; Frame < Frames; ++Frame)
	{
		const int32 WorkFramesBefore = Setup.FlowField->GetWorkFrameCount();
		if (Frame > 0 && Frame % 150 == 0 && RocksDestroyed < UE_ARRAY_COUNT(PerfRockCells))
		{
			RocksDestroyed += Setup.Grid->DestroyCell(PerfRockCells[RocksDestroyed], ERRTerrainDamageSource::Barrel) ? 1 : 0;
		}
		const double Start = FPlatformTime::Seconds();
		TestWorld.Tick();
		FrameMs.Add((FPlatformTime::Seconds() - Start) * 1000.0);
		if (Setup.FlowField->GetWorkFrameCount() != WorkFramesBefore)
		{
			ComputeMs.Add(Setup.FlowField->GetLastFrameWorkMilliseconds());
		}
	}

	double Sum = 0.0;
	double Max = 0.0;
	for (const double Ms : FrameMs)
	{
		Sum += Ms;
		Max = FMath::Max(Max, Ms);
	}
	TArray<double> Sorted = FrameMs;
	Sorted.Sort();
	float ComputeSum = 0.f;
	float ComputeMax = 0.f;
	FString ComputeList;
	for (const float Ms : ComputeMs)
	{
		ComputeSum += Ms;
		ComputeMax = FMath::Max(ComputeMax, Ms);
		ComputeList += FString::Printf(TEXT(" %.3f"), Ms);
	}
	int32 Alive = 0;
	int32 InContact = 0;
	for (const ARRChaser* Chaser : Chasers)
	{
		if (IsValid(Chaser) && !Chaser->IsDead())
		{
			++Alive;
			InContact += Chaser->GetState() != ERRChaserState::Chase ? 1 : 0;
		}
	}
	const FString Summary = FString::Printf(
		TEXT("PERF 50 BP_Chaser, %d frames at 1/60 s, headless -nullrhi (no rendering): world tick avg %.3f ms, median %.3f ms, p95 %.3f ms, max %.3f ms; ")
		TEXT("flow field %d work frames (%d rocks destroyed): avg %.3f ms, max %.3f ms [%s ]; %d alive, %d out of Chase at the end"),
		Frames, Sum / Frames, Sorted[Frames / 2], Sorted[FMath::Min(Frames - 1, FMath::FloorToInt(Frames * 0.95))], Max,
		ComputeMs.Num(), RocksDestroyed, ComputeMs.Num() > 0 ? ComputeSum / ComputeMs.Num() : 0.f, ComputeMax, *ComputeList, Alive, InContact);
	AddInfo(Summary);
	UE_LOG(LogTemp, Display, TEXT("%s"), *Summary);
	TestEqual(TEXT("three rocks destroyed"), RocksDestroyed, 3);
	TestEqual(TEXT("each destroyed rock opened incrementally"), Setup.FlowField->GetOpenedCellCount() - OpenedBefore, RocksDestroyed);
	TestEqual(TEXT("every chaser alive (miner unarmed)"), Alive, ChaserCount);
	return true;
}

namespace RRIntegrationCrowdTests
{
	using namespace RRIntegrationTests;

	/** Measures of one crowd run around the miner. */
	struct FCrowdResult
	{
		int32 Spawned = 0;
		int32 WithinAttack = 0;
		int32 Within200 = 0;
		int32 Within300 = 0;
		int32 OutOfChase = 0;
		int32 Attacks = 0;
		int32 AllIgnoring = 0;
		float MinerDamage = 0.f;
		float MinMinerDistance = TNumericLimits<float>::Max();
		float MinPairDistance = TNumericLimits<float>::Max();
		float MeanDistance = 0.f;
	};

	/**
	 * Spawns Count BP_Chaser on reachable cells 6 to 20 BFS steps from an unarmed miner (huge health), seed 9007, runs Frames at
	 * 1/60 s and measures the crowd. bLegacyCrowd restores the pre-018-02 settings on every robot (robot-robot collision, RVO
	 * consideration 500 uu, manager radius expansion 1.5 and lock 0.2 s) right after the spawn.
	 */
	bool RunCrowd(FAutomationTestBase& Test, bool bLegacyCrowd, int32 Count, int32 Frames, FCrowdResult& Out)
	{
		FTestWorld TestWorld(bLegacyCrowd ? TEXT("RRCrowdLegacy") : TEXT("RRCrowdPacked"));
		FLoopSetup Setup;
		if (!SetUp(Test, TestWorld, ChaseMinerCell, /*bArmed*/ false, Setup))
		{
			return false;
		}
		UAbilitySystemComponent* MinerASC = Setup.Miner->GetAbilitySystemComponent();
		// 1e6 (not 1e9): a 10-damage hit must stay visible in float precision.
		MinerASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), 1.e6f);
		MinerASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 1.e6f);

		TArray<FIntPoint> Candidates;
		const FIntPoint Size = Setup.Grid->GetGridSize();
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			for (int32 X = 0; X < Size.X; ++X)
			{
				const int32 Steps = Setup.FlowField->GetCellDistance(FIntPoint(X, Y));
				if (Steps >= 6 && Steps <= 20)
				{
					Candidates.Add(FIntPoint(X, Y));
				}
			}
		}
		if (!Test.TestTrue(TEXT("enough reachable cells"), Candidates.Num() >= Count))
		{
			return false;
		}
		FRandomStream Stream(9007);
		for (int32 Index = Candidates.Num() - 1; Index > 0; --Index)
		{
			Candidates.Swap(Index, Stream.RandRange(0, Index));
		}
		TArray<ARRChaser*> Chasers;
		for (int32 Index = 0; Index < Count; ++Index)
		{
			if (ARRChaser* Chaser = SpawnChaser(Test, TestWorld, Setup.ChaserClass, Setup.Grid, Candidates[Index]))
			{
				Chasers.Add(Chaser);
				Out.AllIgnoring += Chaser->IsIgnoringOtherRobots() ? 1 : 0;
				if (bLegacyCrowd)
				{
					Chaser->SetIgnoreOtherRobots(false);
					Chaser->GetCharacterMovement()->AvoidanceConsiderationRadius = 500.f;
				}
			}
		}
		Out.Spawned = Chasers.Num();
		if (bLegacyCrowd)
		{
			if (UAvoidanceManager* Manager = TestWorld.Get()->GetAvoidanceManager())
			{
				Manager->ArtificialRadiusExpansion = 1.5f;
				Manager->LockTimeAfterAvoid = 0.2f;
			}
		}

		for (int32 Frame = 0; Frame < Frames; ++Frame)
		{
			TestWorld.Tick();
			for (const ARRChaser* Chaser : Chasers)
			{
				Out.MinMinerDistance = FMath::Min(Out.MinMinerDistance, FVector::Dist2D(Chaser->GetActorLocation(), Setup.Miner->GetActorLocation()));
			}
		}
		Out.MinerDamage = 1.e6f - MinerASC->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
		const float AttackRange = Chasers.Num() > 0 ? Chasers[0]->GetStats().AttackRange : 0.f;
		for (int32 Index = 0; Index < Chasers.Num(); ++Index)
		{
			const ARRChaser* Chaser = Chasers[Index];
			const float Distance = FVector::Dist2D(Chaser->GetActorLocation(), Setup.Miner->GetActorLocation());
			Out.WithinAttack += Distance <= AttackRange ? 1 : 0;
			Out.Within200 += Distance <= 200.f ? 1 : 0;
			Out.Within300 += Distance <= 300.f ? 1 : 0;
			Out.OutOfChase += Chaser->GetState() != ERRChaserState::Chase ? 1 : 0;
			Out.Attacks += Chaser->GetAttackCount();
			Out.MeanDistance += Distance / Chasers.Num();
			for (int32 Other = Index + 1; Other < Chasers.Num(); ++Other)
			{
				Out.MinPairDistance = FMath::Min(Out.MinPairDistance, FVector::Dist2D(Chaser->GetActorLocation(), Chasers[Other]->GetActorLocation()));
			}
		}
		const FString Summary = FString::Printf(
			TEXT("CROWD %s, %d BP_Chaser, %.1f s: %d within attack range (%.0f uu), %d within 200 uu, %d within 300 uu, mean %.1f uu; ")
			TEXT("%d out of Chase, %d attacks, miner damage %.0f; closest to the miner %.1f uu, closest pair %.1f uu"),
			bLegacyCrowd ? TEXT("legacy (robot collision, RVO 500 / 1.5 / 0.2)") : TEXT("packed (robots ignore robots, RVO defaults of ARREnemy)"),
			Out.Spawned, Frames * FrameSeconds, Out.WithinAttack, AttackRange, Out.Within200, Out.Within300, Out.MeanDistance, Out.OutOfChase,
			Out.Attacks, Out.MinerDamage, Out.MinMinerDistance, Out.MinPairDistance);
		Test.AddInfo(Summary);
		UE_LOG(LogTemp, Display, TEXT("%s"), *Summary);
		return true;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationCrowdPackingTest, "RogueRigs.Integration.CrowdPacking",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationCrowdPackingTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationCrowdTests;

	constexpr int32 ChaserCount = 40;
	constexpr int32 Frames = 600;
	FCrowdResult Legacy;
	FCrowdResult Packed;
	if (!RunCrowd(*this, /*bLegacyCrowd*/ true, ChaserCount, Frames, Legacy) || !RunCrowd(*this, /*bLegacyCrowd*/ false, ChaserCount, Frames, Packed))
	{
		return false;
	}
	TestEqual(TEXT("40 chasers spawned per run"), Packed.Spawned, ChaserCount);
	// Both robot Blueprints inherit the ARREnemy crowd defaults (no override of the avoidance values).
	for (const TCHAR* Path : { ChaserClassPath, TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Kamikaze.BP_Kamikaze_C") })
	{
		if (UClass* RobotClass = LoadTestClass<ARREnemy>(*this, Path))
		{
			const ARREnemy* Robot = RobotClass->GetDefaultObject<ARREnemy>();
			TestEqual(FString::Printf(TEXT("%s RVO consideration radius 250 uu"), *RobotClass->GetName()), Robot->GetCharacterMovement()->AvoidanceConsiderationRadius, 250.f);
			TestTrue(FString::Printf(TEXT("%s uses RVO"), *RobotClass->GetName()), Robot->GetCharacterMovement()->bUseRVOAvoidance != 0);
		}
	}
	TestEqual(TEXT("every BP_Chaser ignores other robots by default (inherited from ARREnemy)"), Packed.AllIgnoring, ChaserCount);
	// Miner capsule 34 + chaser capsule 24: robots never pass through the miner.
	TestTrue(TEXT("packed: no robot inside the miner"), Packed.MinMinerDistance >= 58.f - 1.f);
	TestTrue(TEXT("packed: robots still attack"), Packed.Attacks > 0);
	TestTrue(TEXT("packed: robots still damage the miner"), Packed.MinerDamage > 0.f);
	TestTrue(TEXT("packed: at least as many robots within 200 uu as legacy"), Packed.Within200 >= Legacy.Within200);
	TestTrue(TEXT("packed: at least as many robots within attack range as legacy"), Packed.WithinAttack >= Legacy.WithinAttack);
	return true;
}

namespace RRIntegrationExplosionTests
{
	using namespace RRIntegrationTests;

	/** Final Blueprints of the explosion loop. */
	const TCHAR* const KamikazeClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Kamikaze.BP_Kamikaze_C");
	const TCHAR* const ExplosionFXClassPath = TEXT("/Game/RogueRigs/Blueprints/FX/BP_ExplosionFX.BP_ExplosionFX_C");

	/** arena_m1 barrel pair 200 uu apart, the shot one with two floor cells to its west; the miner shoots from ChainShooterCell (400 uu away: outside both radii). */
	const FIntPoint ChainShotBarrelCell(24, 49);
	const FIntPoint ChainPartnerCell(24, 48);
	const FIntPoint ChainShooterCell(22, 49);

	/** arena_m1 barrel pair 200 uu apart with the miner on the floor cell east of the first (200 and 283 uu: point blank for both). */
	const FIntPoint PointBlankBarrelCell(35, 65);
	const FIntPoint PointBlankPartnerCell(35, 66);
	const FIntPoint PointBlankMinerCell(36, 65);

	/** Open row 44 of arena_m1 running along the rocks (37-41, 45): armed miner on the west end, kamikaze 11 cells east
	 * (slow and tough: 240 uu/s, 100 HP; measured in task-030-01: killed at 4.4 s, about 1190 uu from the miner, above the rocks). */
	const FIntPoint KamikazeKillMinerCell(32, 44);
	const FIntPoint KamikazeKillStartCell(43, 44);

	// Protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestAccess;

	/** Spawns an enemy Blueprint of class Class (native base T) with its feet on the floor of Cell. */
	template <typename T>
	T* SpawnEnemy(FAutomationTestBase& Test, const FTestWorld& TestWorld, UClass* Class, const ARRArenaGrid* Grid, FIntPoint Cell)
	{
		const float HalfHeight = Class->GetDefaultObject<T>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(Grid->GetCellCenter(Cell.X, Cell.Y) + FVector(0.f, 0.f, HalfHeight + 2.f));
		T* Enemy = TestWorld.Get()->SpawnActorDeferred<T>(Class, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Test.TestNotNull(FString::Printf(TEXT("%s spawned on (%d, %d)"), *Class->GetName(), Cell.X, Cell.Y), Enemy))
		{
			Enemy->FinishSpawning(SpawnTransform);
		}
		return Enemy;
	}

	/** Block cells whose center lies within Radius (2D) of Center. */
	TArray<FIntPoint> GetBlocksWithin(const ARRArenaGrid* Grid, const FVector& Center, float Radius)
	{
		TArray<FIntPoint> Result;
		const FIntPoint Size = Grid->GetGridSize();
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			for (int32 X = 0; X < Size.X; ++X)
			{
				if (Grid->IsBlockCell(FIntPoint(X, Y)) && FVector::Dist2D(Grid->GetCellCenter(X, Y), Center) <= Radius)
				{
					Result.Add(FIntPoint(X, Y));
				}
			}
		}
		return Result;
	}

	/** Number of Cells that are still blocks. */
	int32 CountBlocks(const ARRArenaGrid* Grid, const TArray<FIntPoint>& Cells)
	{
		int32 Count = 0;
		for (const FIntPoint& Cell : Cells)
		{
			Count += Grid->IsBlockCell(Cell) ? 1 : 0;
		}
		return Count;
	}

	/** Explosion subsystem of the world with Recorder bound to OnExplosion. */
	URRExplosionSubsystem* BindExplosions(FAutomationTestBase& Test, const FTestWorld& TestWorld, URRTestEventRecorder* Recorder)
	{
		URRExplosionSubsystem* Subsystem = TestWorld.Get()->GetSubsystem<URRExplosionSubsystem>();
		if (Test.TestNotNull(TEXT("explosion subsystem exists"), Subsystem))
		{
			Subsystem->OnExplosion.AddDynamic(Recorder, &URRTestEventRecorder::HandleExplosion);
		}
		return Subsystem;
	}

	/** Lifetime record of one explosion visual. */
	struct FFXRecord
	{
		TWeakObjectPtr<ARRExplosionFX> FX;
		UClass* Class = nullptr;
		float Radius = 0.f;
		float Duration = 0.f;
		double SeenTime = 0.0;
		double GoneTime = -1.0;
	};

	/** Adds the explosion visuals that appeared this frame and stamps the ones destroyed since. */
	void TrackFX(UWorld* World, TArray<FFXRecord>& Records)
	{
		const double Now = World->GetTimeSeconds();
		for (FFXRecord& Record : Records)
		{
			if (Record.GoneTime < 0.0 && (!Record.FX.IsValid() || Record.FX->IsActorBeingDestroyed()))
			{
				Record.GoneTime = Now;
			}
		}
		for (TActorIterator<ARRExplosionFX> It(World); It; ++It)
		{
			ARRExplosionFX* FX = *It;
			if (IsValid(FX) && !FX->IsActorBeingDestroyed() && !Records.ContainsByPredicate([FX](const FFXRecord& Record) { return Record.FX.Get() == FX; }))
			{
				Records.Add({ FX, FX->GetClass(), FX->GetRadius(), FExplosionFXAccess::DurationOf(FX), Now, -1.0 });
			}
		}
	}

	/** Appends the time (relative to Start) of every explosion the recorder received since the last call. */
	void StampExplosions(const UWorld* World, const URRTestEventRecorder* Recorder, double Start, TArray<double>& Times)
	{
		while (Times.Num() < Recorder->GetExplosionRows().Num())
		{
			Times.Add(World->GetTimeSeconds() - Start);
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationBarrelChainTest, "RogueRigs.Integration.ShotBarrelChainOpensWall",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationBarrelChainTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationExplosionTests;

	FTestWorld TestWorld(TEXT("RRIntegrationBarrelChain"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	UClass* FXClass = LoadTestClass<ARRExplosionFX>(*this, ExplosionFXClassPath);
	FLoopSetup Setup;
	if (!FXClass || !SetUp(*this, TestWorld, ChainShooterCell, /*bArmed*/ true, Setup))
	{
		return false;
	}
	URRExplosionSubsystem* Explosions = BindExplosions(*this, TestWorld, Recorder.Get());
	FRRExplosionStatsRow Stats;
	if (!Explosions || !TestTrue(TEXT("Barrel row loads"), Explosions->GetExplosionStats(TEXT("Barrel"), Stats)))
	{
		return false;
	}
	ARRArenaGrid* Grid = Setup.Grid;
	ARRMiner* Miner = Setup.Miner;
	TestEqual(TEXT("(24, 49) is a barrel"), Grid->GetCellType(ChainShotBarrelCell), ERRCellType::Barrel);
	TestEqual(TEXT("(24, 48) is a barrel"), Grid->GetCellType(ChainPartnerCell), ERRCellType::Barrel);
	TestEqual(TEXT("(23, 49) is floor (line of fire)"), Grid->GetCellType(ChainShooterCell + FIntPoint(1, 0)), ERRCellType::Floor);
	const FVector ShotCenter = Grid->GetCellCenter(ChainShotBarrelCell.X, ChainShotBarrelCell.Y);
	const FVector PartnerCenter = Grid->GetCellCenter(ChainPartnerCell.X, ChainPartnerCell.Y);
	TestTrue(TEXT("partner inside the shot barrel's radius"), FVector::Dist2D(ShotCenter, PartnerCenter) <= Stats.Radius);
	TestTrue(TEXT("miner outside both radii"), FVector::Dist2D(Miner->GetActorLocation(), ShotCenter) > Stats.Radius
		&& FVector::Dist2D(Miner->GetActorLocation(), PartnerCenter) > Stats.Radius);

	// Every block in both radii, the barrels included, before the shot.
	TArray<FIntPoint> Blocks = GetBlocksWithin(Grid, ShotCenter, Stats.Radius);
	for (const FIntPoint& Cell : GetBlocksWithin(Grid, PartnerCenter, Stats.Radius))
	{
		Blocks.AddUnique(Cell);
	}

	const FVector Toward = ShotCenter - Miner->GetActorLocation();
	AimAlong(Miner, Toward);
	TestTrue(TEXT("miner aims at the barrel"), FVector::DotProduct(Miner->GetAimDirection(), Toward.GetSafeNormal2D()) > 0.99f);
	const float StartHealth = GetHealth(Miner);

	TArray<double> Times;
	TArray<FFXRecord> FXRecords;
	const double Start = TestWorld.Get()->GetTimeSeconds();
	for (int32 Frame = 0; Frame < 300 && (Times.Num() < 2 || FXRecords.ContainsByPredicate([](const FFXRecord& Record) { return Record.GoneTime < 0.0; })); ++Frame)
	{
		TestWorld.Tick();
		StampExplosions(TestWorld.Get(), Recorder.Get(), Start, Times);
		TrackFX(TestWorld.Get(), FXRecords);
		if (Times.Num() == 1)
		{
			TestEqual(TEXT("first explosion destroyed the partner"), Grid->GetCellType(ChainPartnerCell), ERRCellType::Floor);
		}
	}
	// A few more frames: nothing else explodes.
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}

	if (!TestEqual(TEXT("two explosions"), Recorder->GetExplosionRows().Num(), 2) || Times.Num() != 2)
	{
		return false;
	}
	const double Gap = Times[1] - Times[0];
	const int32 BlocksLeft = CountBlocks(Grid, Blocks);
	AddInfo(FString::Printf(TEXT("shot barrel exploded at %.4f s, partner at %.4f s, gap %.4f s (chain delay %.2f); %d blocks in both radii, %d left; miner Health %.0f -> %.0f"),
		Times[0], Times[1], Gap, Explosions->GetChainDelay(), Blocks.Num(), BlocksLeft, StartHealth, GetHealth(Miner)));
	TestEqual(TEXT("first row Barrel"), Recorder->GetExplosionRows()[0], FName(TEXT("Barrel")));
	TestEqual(TEXT("second row Barrel"), Recorder->GetExplosionRows()[1], FName(TEXT("Barrel")));
	TestTrue(TEXT("first at the shot barrel"), Recorder->GetExplosionCenters()[0].Equals(ShotCenter));
	TestTrue(TEXT("second at the partner"), Recorder->GetExplosionCenters()[1].Equals(PartnerCenter));
	TestTrue(TEXT("partner 0.08 s later, within one frame"), Gap >= Explosions->GetChainDelay() - 1e-4 && Gap < Explosions->GetChainDelay() + FrameSeconds);
	TestTrue(TEXT("blocks around (not only the barrels)"), Blocks.Num() > 2);
	TestEqual(TEXT("every block in both radii destroyed"), BlocksLeft, 0);
	TestEqual(TEXT("miner outside the radii untouched"), GetHealth(Miner), StartHealth);

	// One BP_ExplosionFX per explosion, at the radius, gone after its Duration.
	if (TestEqual(TEXT("one explosion visual per explosion"), FXRecords.Num(), 2))
	{
		for (int32 Index = 0; Index < FXRecords.Num(); ++Index)
		{
			const FFXRecord& Record = FXRecords[Index];
			const double Seen = Record.SeenTime - Start;
			const double Life = Record.GoneTime - Record.SeenTime;
			AddInfo(FString::Printf(TEXT("FX %d (%s): seen at %.4f s, radius %.0f, destroyed %.4f s later (Duration %.2f)"),
				Index, Record.Class ? *Record.Class->GetName() : TEXT("none"), Seen, Record.Radius, Life, Record.Duration));
			TestTrue(TEXT("FX is BP_ExplosionFX"), Record.Class == FXClass);
			TestNearlyEqual(TEXT("FX radius = Barrel radius"), Record.Radius, Stats.Radius);
			TestNearlyEqual(TEXT("FX spawned with its explosion"), Seen, Times[Index], 1e-4);
			TestTrue(TEXT("FX destroyed itself"), Record.GoneTime >= 0.0);
			TestTrue(TEXT("FX lifetime = Duration, within one frame"), Life >= Record.Duration - FrameSeconds - 1e-4 && Life <= Record.Duration + FrameSeconds + 1e-4);
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationKamikazeReachesTest, "RogueRigs.Integration.KamikazeReachesAndHurtsMiner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationKamikazeReachesTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationExplosionTests;

	FTestWorld TestWorld(TEXT("RRIntegrationKamikazeReach"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	UClass* KamikazeClass = LoadTestClass<ARRKamikaze>(*this, KamikazeClassPath);
	FLoopSetup Setup;
	if (!KamikazeClass || !SetUp(*this, TestWorld, ChaseMinerCell, /*bArmed*/ false, Setup))
	{
		return false;
	}
	URRExplosionSubsystem* Explosions = BindExplosions(*this, TestWorld, Recorder.Get());
	FRRExplosionStatsRow Stats;
	if (!Explosions || !TestTrue(TEXT("Kamikaze row loads"), Explosions->GetExplosionStats(TEXT("Kamikaze"), Stats)))
	{
		return false;
	}
	const int32 Steps = Setup.FlowField->GetCellDistance(WindingStartCell);
	ARRKamikaze* Kamikaze = SpawnEnemy<ARRKamikaze>(*this, TestWorld, KamikazeClass, Setup.Grid, WindingStartCell);
	if (!Kamikaze)
	{
		return false;
	}
	TWeakObjectPtr<ARRKamikaze> Weak(Kamikaze);
	ARRMiner* Miner = Setup.Miner;
	const float StartHealth = GetHealth(Miner);
	TArray<FHit> Hits;
	const FDelegateHandle Handle = RecordHits(Miner, Hits);

	// Until the fuse explodes it (max 15 s): the center must stay on walkable cells while it moves.
	const double StartTime = TestWorld.Get()->GetTimeSeconds();
	int32 OffFloorFrames = 0;
	for (int32 Frame = 0; Frame < 900 && Recorder->GetExplosionRows().Num() == 0; ++Frame)
	{
		TestWorld.Tick();
		if (Weak.IsValid() && !Weak->IsDead())
		{
			OffFloorFrames += Setup.FlowField->IsWalkableCell(Setup.Grid->GetCellAtWorld(Weak->GetActorLocation())) ? 0 : 1;
		}
	}
	const double ExplosionTime = TestWorld.Get()->GetTimeSeconds() - StartTime;
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	Miner->GetHealthSet()->OnDamageReceived.Remove(Handle);

	const double ArmTime = Kamikaze->GetArmTime() - StartTime;
	const double Fuse = Kamikaze->GetExplosionQueuedTime() - Kamikaze->GetArmTime();
	AddInfo(FString::Printf(TEXT("kamikaze from (%d, %d), %d BFS steps: armed at %.3f s, fuse %.3f s, exploded at %.3f s; %d hit(s) of %.0f, miner Health %.0f -> %.0f"),
		WindingStartCell.X, WindingStartCell.Y, Steps, ArmTime, Fuse, ExplosionTime, Hits.Num(), Hits.Num() > 0 ? Hits[0].Damage : 0.f, StartHealth, GetHealth(Miner)));
	TestEqual(TEXT("kamikaze center never on a non-walkable cell"), OffFloorFrames, 0);
	if (!TestTrue(TEXT("kamikaze armed"), Kamikaze->GetArmTime() >= 0.0) || !TestEqual(TEXT("one explosion"), Recorder->GetExplosionRows().Num(), 1))
	{
		return false;
	}
	TestTrue(TEXT("exploded by its fuse"), Kamikaze->HasDetonated());
	TestTrue(TEXT("fuse 0.5 s (within one frame)"), Fuse >= 0.5 - 1e-3 && Fuse <= 0.5 + FrameSeconds + 1e-3);
	TestEqual(TEXT("row Kamikaze"), Recorder->GetExplosionRows()[0], FName(TEXT("Kamikaze")));
	if (TestEqual(TEXT("miner hit once"), Hits.Num(), 1))
	{
		TestEqual(TEXT("hit = Kamikaze explosion damage (30)"), Hits[0].Damage, Stats.Damage);
	}
	TestEqual(TEXT("explosion damage is 30"), Stats.Damage, 30.f);
	TestEqual(TEXT("miner Health lowered by 30"), GetHealth(Miner), StartHealth - Stats.Damage);
	TestFalse(TEXT("miner alive"), Miner->IsDead());
	TestFalse(TEXT("kamikaze destroyed"), Weak.IsValid() && !Weak->IsActorBeingDestroyed());
	TestEqual(TEXT("no XP after a fuse explosion"), CountXPPickups(TestWorld.Get()), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationPointBlankBarrelTest, "RogueRigs.Integration.MinerSurvivesPointBlankBarrel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationPointBlankBarrelTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationExplosionTests;

	FTestWorld TestWorld(TEXT("RRIntegrationPointBlank"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FLoopSetup Setup;
	if (!SetUp(*this, TestWorld, PointBlankMinerCell, /*bArmed*/ false, Setup))
	{
		return false;
	}
	URRExplosionSubsystem* Explosions = BindExplosions(*this, TestWorld, Recorder.Get());
	FRRExplosionStatsRow Stats;
	if (!Explosions || !TestTrue(TEXT("Barrel row loads"), Explosions->GetExplosionStats(TEXT("Barrel"), Stats)))
	{
		return false;
	}
	ARRArenaGrid* Grid = Setup.Grid;
	ARRMiner* Miner = Setup.Miner;
	TestEqual(TEXT("(35, 65) is a barrel"), Grid->GetCellType(PointBlankBarrelCell), ERRCellType::Barrel);
	TestEqual(TEXT("(35, 66) is a barrel"), Grid->GetCellType(PointBlankPartnerCell), ERRCellType::Barrel);
	const float MaxHealth = Miner->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	const float StartHealth = GetHealth(Miner);
	TestEqual(TEXT("miner at full health"), StartHealth, MaxHealth);
	const float NearDistance = FVector::Dist2D(Miner->GetActorLocation(), Grid->GetCellCenter(PointBlankBarrelCell.X, PointBlankBarrelCell.Y));
	const float PartnerDistance = FVector::Dist2D(Miner->GetActorLocation(), Grid->GetCellCenter(PointBlankPartnerCell.X, PointBlankPartnerCell.Y));
	TestTrue(TEXT("miner inside both radii"), NearDistance <= Stats.Radius && PartnerDistance <= Stats.Radius);

	TArray<FHit> Hits;
	const FDelegateHandle Handle = RecordHits(Miner, Hits);
	TestTrue(TEXT("shot damage destroys the barrel"), Grid->ApplyShotDamage(PointBlankBarrelCell, Grid->GetCellShotHealth(PointBlankBarrelCell)));
	TArray<double> Times;
	const double Start = TestWorld.Get()->GetTimeSeconds();
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
		StampExplosions(TestWorld.Get(), Recorder.Get(), Start, Times);
	}
	Miner->GetHealthSet()->OnDamageReceived.Remove(Handle);

	const float EndHealth = GetHealth(Miner);
	AddInfo(FString::Printf(TEXT("miner %.0f / %.0f uu from the barrels; %d explosions (gap %.4f s); %d hit(s) of %.0f; Health %.0f -> %.0f"),
		NearDistance, PartnerDistance, Times.Num(), Times.Num() == 2 ? Times[1] - Times[0] : 0.0, Hits.Num(), Hits.Num() > 0 ? Hits[0].Damage : 0.f, StartHealth, EndHealth));
	TestEqual(TEXT("both barrels exploded (chain)"), Times.Num(), 2);
	TestFalse(TEXT("miner alive"), Miner->IsDead());
	TestTrue(TEXT("miner keeps at least 1 Health"), EndHealth >= 1.f);
	if (TestEqual(TEXT("one hit: the chained link falls in the i-frames"), Hits.Num(), 1))
	{
		TestEqual(TEXT("hit = min(Barrel damage, Health - 1)"), Hits[0].Damage, FMath::Min(Stats.Damage, StartHealth - 1.f));
	}
	TestEqual(TEXT("Health after one capped explosion"), EndHealth, StartHealth - FMath::Min(Stats.Damage, StartHealth - 1.f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationRifleKillsKamikazeTest, "RogueRigs.Integration.RifleKillsKamikazeExplodes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationRifleKillsKamikazeTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationExplosionTests;

	FTestWorld TestWorld(TEXT("RRIntegrationKamikazeKill"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	UClass* KamikazeClass = LoadTestClass<ARRKamikaze>(*this, KamikazeClassPath);
	FLoopSetup Setup;
	if (!KamikazeClass || !SetUp(*this, TestWorld, KamikazeKillMinerCell, /*bArmed*/ true, Setup))
	{
		return false;
	}
	URRExplosionSubsystem* Explosions = BindExplosions(*this, TestWorld, Recorder.Get());
	FRRExplosionStatsRow Stats;
	if (!Explosions || !TestTrue(TEXT("Kamikaze row loads"), Explosions->GetExplosionStats(TEXT("Kamikaze"), Stats)))
	{
		return false;
	}
	ARRArenaGrid* Grid = Setup.Grid;
	ARRMiner* Miner = Setup.Miner;
	const FVector Toward = Grid->GetCellCenter(KamikazeKillStartCell.X, KamikazeKillStartCell.Y) - Miner->GetActorLocation();
	AimAlong(Miner, Toward);
	TestTrue(TEXT("miner aims at the kamikaze start"), FVector::DotProduct(Miner->GetAimDirection(), Toward.GetSafeNormal2D()) > 0.99f);

	ARRKamikaze* Kamikaze = SpawnEnemy<ARRKamikaze>(*this, TestWorld, KamikazeClass, Grid, KamikazeKillStartCell);
	if (!Kamikaze)
	{
		return false;
	}
	TWeakObjectPtr<ARRKamikaze> Weak(Kamikaze);
	const float StartHealth = GetHealth(Miner);

	// Crits off on the starting weapon so every rifle hit deals the CSV Damage; count the hits the kamikaze takes until death.
	// A shot captures CritChance in its spec when fired: the shots already in flight (fired during the set-up) are removed.
	for (ARRWeapon* Weapon : Miner->GetWeapons())
	{
		if (IsValid(Weapon) && Weapon->GetAbilitySystemComponent())
		{
			Weapon->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);
		}
	}
	for (TActorIterator<ARRProjectile> It(TestWorld.Get()); It; ++It)
	{
		It->Destroy();
	}
	const float KamikazeMaxHealth = Kamikaze->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	TestEqual(TEXT("kamikaze at full health (100)"), GetHealth(Kamikaze), KamikazeMaxHealth);
	TestEqual(TEXT("kamikaze MaxHealth 100"), KamikazeMaxHealth, 100.f);
	TArray<FHit> RifleHits;
	int32 CriticalHits = 0;
	UWorld* World = TestWorld.Get();
	const FDelegateHandle RifleHitHandle = Kamikaze->GetHealthSet()->OnDamageReceived.AddLambda([&RifleHits, &CriticalHits, World](float Damage, bool bIsCritical, float)
	{
		RifleHits.Add({ World->GetTimeSeconds(), Damage });
		CriticalHits += bIsCritical ? 1 : 0;
	});
	const double StartTime = TestWorld.Get()->GetTimeSeconds();
	double DeathTime = -1.0;
	FVector DeathLocation = FVector::ZeroVector;
	// Blocks before the run (rifle shots never break blocks; the explosion may resolve on the death frame itself).
	const TArray<FIntPoint> BlocksBefore = GetBlocksWithin(Grid, Grid->GetCellCenter(KamikazeKillStartCell.X, KamikazeKillStartCell.Y), 1.e6f);
	for (int32 Frame = 0; Frame < 600 && DeathTime < 0.0; ++Frame)
	{
		TestWorld.Tick();
		if (Weak.IsValid() && Weak->IsDead())
		{
			DeathTime = TestWorld.Get()->GetTimeSeconds() - StartTime;
			DeathLocation = Weak->GetActorLocation();
		}
	}
	if (Weak.IsValid())
	{
		Weak->GetHealthSet()->OnDamageReceived.Remove(RifleHitHandle);
	}
	if (!TestTrue(TEXT("kamikaze killed within 10 s"), DeathTime >= 0.0))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("kamikaze took %d rifle hit(s) (%d critical) before dying, first %.0f, Health %.0f -> 0"),
		RifleHits.Num(), CriticalHits, RifleHits.Num() > 0 ? RifleHits[0].Damage : 0.f, KamikazeMaxHealth));
	TestEqual(TEXT("10 rifle hits to kill the 100 HP kamikaze"), RifleHits.Num(), 10);
	TestEqual(TEXT("no critical hit"), CriticalHits, 0);
	for (const FHit& Hit : RifleHits)
	{
		TestEqual(TEXT("every rifle hit deals 10"), Hit.Damage, 10.f);
	}
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TArray<FIntPoint> Blocks = BlocksBefore.FilterByPredicate([Grid, &DeathLocation, &Stats](const FIntPoint& Cell)
	{
		return FVector::Dist2D(Grid->GetCellCenter(Cell.X, Cell.Y), DeathLocation) <= Stats.Radius;
	});
	const int32 BlocksLeft = CountBlocks(Grid, Blocks);
	const float DeathDistance = FVector::Dist2D(DeathLocation, Miner->GetActorLocation());
	AddInfo(FString::Printf(TEXT("kamikaze killed at %.3f s, %.0f uu from the miner, detonated %d; %d explosion(s); %d blocks within %.0f uu, %d left; miner Health %.0f -> %.0f; %d XP"),
		DeathTime, DeathDistance, Kamikaze->HasDetonated() ? 1 : 0, Recorder->GetExplosionRows().Num(), Blocks.Num(), Stats.Radius, BlocksLeft,
		StartHealth, GetHealth(Miner), CountXPPickups(TestWorld.Get())));
	TestFalse(TEXT("killed, not detonated"), Kamikaze->HasDetonated());
	TestTrue(TEXT("killed before arming"), Kamikaze->GetArmTime() < 0.0);
	if (TestEqual(TEXT("one explosion"), Recorder->GetExplosionRows().Num(), 1))
	{
		TestEqual(TEXT("row Kamikaze"), Recorder->GetExplosionRows()[0], FName(TEXT("Kamikaze")));
		TestTrue(TEXT("at the kamikaze death location"), FVector::Dist2D(Recorder->GetExplosionCenters()[0], DeathLocation) < 1.f);
	}
	TestTrue(TEXT("blocks within the radius at death"), Blocks.Num() > 0);
	TestEqual(TEXT("every nearby block destroyed"), BlocksLeft, 0);
	TestTrue(TEXT("miner outside the radius"), DeathDistance > Stats.Radius);
	TestEqual(TEXT("miner untouched"), GetHealth(Miner), StartHealth);
	TestEqual(TEXT("one kamikaze burst of XP cubes dropped"), CountXPPickups(TestWorld.Get()), KamikazeXPCubes);
	return true;
}

namespace RRIntegrationUpgradeTests
{
	using namespace RRIntegrationTests;

	/** Final Blueprints of the level-up loop besides the miner, grid and chaser. */
	const TCHAR* const PlayerControllerClassPath = TEXT("/Game/RogueRigs/Blueprints/Player/BP_RRPlayerController.BP_RRPlayerController_C");
	const TCHAR* const SpawnerClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_EnemySpawner.BP_EnemySpawner_C");
	const TCHAR* const HUDClassPath = TEXT("/Game/RogueRigs/UI/WBP_HUD.WBP_HUD_C");

	/** Seed of the upgrade draws: with it, the rerolled offer holds a damage card for the rifle. */
	constexpr int32 UpgradeSeed = 6;

	/** Half extent (uu) of the fixed spawner view rectangle around the miner (spawner tests). */
	const FVector2D SpawnerViewHalfExtent(1600.f, 1000.f);

	// Protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestAccess;

	/** Game instance of a test world. Declare it before the FTestWorld: it outlives the world, then drops its context pointer. */
	struct FTestGameInstance
	{
		/** Clears the (destroyed) world context pointer of the game instance and releases it. */
		~FTestGameInstance()
		{
			if (GameInstance.IsValid())
			{
				FGameInstanceAccess::WorldContextOf(GameInstance.Get()) = nullptr;
			}
		}

		/** The game instance, kept alive for the test. */
		TStrongObjectPtr<UGameInstance> GameInstance;
	};

	/**
	 * Gives the test world a game instance (wired to the world context, so widgets created on it find the world) and makes
	 * GameModeClass its authority game mode, so APlayerController::SetPause works (the bare harness world has none).
	 * Call before spawning the miner and the controller (miner registration, player state).
	 */
	AGameModeBase* StartGameMode(FAutomationTestBase& Test, const FTestWorld& TestWorld, UClass* GameModeClass, FTestGameInstance& OutGameInstance)
	{
		UWorld* World = TestWorld.Get();
		FWorldContext* Context = GEngine->GetWorldContextFromWorld(World);
		if (!Test.TestNotNull(TEXT("test world has a world context"), Context))
		{
			return nullptr;
		}
		UGameInstance* GameInstance = NewObject<UGameInstance>(GEngine);
		OutGameInstance.GameInstance.Reset(GameInstance);
		Context->OwningGameInstance = GameInstance;
		FGameInstanceAccess::WorldContextOf(GameInstance) = Context;
		World->SetGameInstance(GameInstance);
		World->GetWorldSettings()->DefaultGameMode = GameModeClass;
		World->SetGameMode(FURL());
		AGameModeBase* GameMode = World->GetAuthGameMode();
		Test.TestTrue(TEXT("authority game mode is BP_RRGameMode"), GameMode && GameMode->IsA(GameModeClass));
		return GameMode;
	}

	/** Visibility of the widget named Name in Widget (Hidden when missing, with a test failure). */
	ESlateVisibility GetWidgetVisibility(FAutomationTestBase& Test, UUserWidget* Widget, const TCHAR* Name)
	{
		const UWidget* Child = Widget->GetWidgetFromName(Name);
		Test.TestNotNull(FString::Printf(TEXT("WBP_HUD has %s"), Name), Child);
		return Child ? Child->GetVisibility() : ESlateVisibility::Hidden;
	}

	/** Checks that WBP_HUD shows Cards: UpgradeScreen visible, RerollButton enabled as bCanReroll, CardName0..2 = display names. */
	void CheckHUDCards(FAutomationTestBase& Test, UUserWidget* HUD, const TArray<FRRUpgradeCard>& Cards, bool bCanReroll, const TCHAR* Label)
	{
		Test.TestTrue(FString::Printf(TEXT("%s: UpgradeScreen visible"), Label), GetWidgetVisibility(Test, HUD, TEXT("UpgradeScreen")) == ESlateVisibility::Visible);
		const UWidget* Reroll = HUD->GetWidgetFromName(TEXT("RerollButton"));
		if (Test.TestNotNull(TEXT("WBP_HUD has RerollButton"), Reroll))
		{
			Test.TestTrue(FString::Printf(TEXT("%s: RerollButton enabled = %d"), Label, bCanReroll ? 1 : 0), Reroll->GetIsEnabled() == bCanReroll);
		}
		for (int32 Index = 0; Index < Cards.Num(); ++Index)
		{
			const FString Name = FString::Printf(TEXT("CardName%d"), Index);
			const UTextBlock* Text = Cast<UTextBlock>(HUD->GetWidgetFromName(*Name));
			if (Test.TestNotNull(FString::Printf(TEXT("WBP_HUD has %s"), *Name), Text))
			{
				Test.TestEqual(FString::Printf(TEXT("%s: %s shows the card name"), Label, *Name), Text->GetText().ToString(), Cards[Index].DisplayName.ToString());
			}
		}
	}

	/** Checks Cards form a valid offer for Miner: 3 cards, no shared stat and target, weapon cards on all weapons or an owned weapon. */
	void CheckOfferValid(FAutomationTestBase& Test, const ARRMiner* Miner, const TArray<FRRUpgradeCard>& Cards, const TCHAR* Label)
	{
		Test.TestEqual(FString::Printf(TEXT("%s: 3 cards"), Label), Cards.Num(), 3);
		for (int32 Index = 0; Index < Cards.Num(); ++Index)
		{
			const FRRUpgradeCard& Card = Cards[Index];
			Test.TestTrue(FString::Printf(TEXT("%s: card %d has a value"), Label, Index), Card.Value > 0.f);
			Test.TestFalse(FString::Printf(TEXT("%s: card %d has a name"), Label, Index), Card.DisplayName.IsEmpty());
			Test.TestTrue(FString::Printf(TEXT("%s: card %d is not a magazine card"), Label, Index), Card.Stat != ERRUpgradeStat::MagazineSize);
			if (Card.Target == ERRUpgradeTarget::Weapon)
			{
				Test.TestTrue(FString::Printf(TEXT("%s: card %d targets an owned weapon"), Label, Index), Miner->GetWeapons().Contains(Card.TargetWeapon.Get()));
			}
			for (int32 Other = Index + 1; Other < Cards.Num(); ++Other)
			{
				Test.TestFalse(FString::Printf(TEXT("%s: cards %d and %d differ"), Label, Index, Other), Card.IsSameChoice(Cards[Other]));
			}
		}
	}

	/** "Stat/Target/Value" of each card, for the log. */
	FString DescribeCards(const TArray<FRRUpgradeCard>& Cards)
	{
		FString Text;
		for (const FRRUpgradeCard& Card : Cards)
		{
			Text += FString::Printf(TEXT(" [%s %s %s]"), *Card.DisplayName.ToString(), *Card.TargetText.ToString(), *Card.ValueText.ToString());
		}
		return Text;
	}

	/** Number of robots (alive or dying) in World. */
	int32 CountEnemies(UWorld* World)
	{
		int32 Count = 0;
		for (TActorIterator<ARREnemy> It(World); It; ++It)
		{
			Count += IsValid(*It) && !It->IsActorBeingDestroyed() ? 1 : 0;
		}
		return Count;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationUpgradeLoopTest, "RogueRigs.Integration.LevelUpUpgradeLoop",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationUpgradeLoopTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationTests;
	using namespace RRIntegrationUpgradeTests;

	// Declared before the world: released after it.
	FTestGameInstance GameInstance;
	FTestWorld TestWorld(TEXT("RRIntegrationUpgrade"));
	UWorld* World = TestWorld.Get();
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	UClass* GameModeClass = LoadTestClass<AGameModeBase>(*this, GameModeClassPath);
	UClass* ControllerClass = LoadTestClass<APlayerController>(*this, PlayerControllerClassPath);
	UClass* SpawnerClass = LoadTestClass<ARREnemySpawner>(*this, SpawnerClassPath);
	UClass* HUDClass = LoadTestClass<URRHUDWidget>(*this, HUDClassPath);
	if (!GameModeClass || !ControllerClass || !SpawnerClass || !HUDClass || !StartGameMode(*this, TestWorld, GameModeClass, GameInstance))
	{
		return false;
	}

	// Real Blueprints: armed BP_Miner possessed by BP_RRPlayerController (no local player: no HUD of its own, so WBP_HUD is
	// created here and bound like the controller does), BP_EnemySpawner held off until the level-up.
	FLoopSetup Setup;
	if (!SetUp(*this, TestWorld, ArmedMinerCell, /*bArmed*/ true, Setup))
	{
		return false;
	}
	ARRMiner* Miner = Setup.Miner;
	URRProgressionComponent* Progression = Miner->GetProgression();
	URRLevelUpComponent* LevelUp = Miner->GetLevelUp();
	URRUpgradeComponent* Upgrades = Miner->GetUpgrades();
	ARRWeapon* Rifle = Miner->GetWeapons().Num() > 0 ? Miner->GetWeapons()[0].Get() : nullptr;
	if (!TestNotNull(TEXT("progression"), Progression) || !TestNotNull(TEXT("level-up flow"), LevelUp)
		|| !TestNotNull(TEXT("upgrades"), Upgrades) || !TestNotNull(TEXT("starting rifle"), Rifle))
	{
		return false;
	}
	APlayerController* Controller = TestWorld.Spawn<APlayerController>(ControllerClass, Miner->GetActorLocation());
	if (!TestNotNull(TEXT("BP_RRPlayerController spawned"), Controller) || !TestNotNull(TEXT("controller has a player state"), Controller->PlayerState.Get()))
	{
		return false;
	}
	Controller->Possess(Miner);
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(SpawnerClass, FVector::ZeroVector);
	if (!TestNotNull(TEXT("BP_EnemySpawner spawned"), Spawner))
	{
		return false;
	}
	Spawner->SetSpawningEnabled(false);
	// Older timers (director off): this test needs a spawn within 2 s of the resume, the director is calm for 10 s.
	RRTestAccess::FSpawnerAccess::DirectorOf(Spawner).bEnabled = false;
	URRHUDWidget* HUD = CreateWidget<URRHUDWidget>(World, HUDClass);
	if (!TestNotNull(TEXT("WBP_HUD created"), HUD))
	{
		return false;
	}
	// The world tick may run a garbage collection: keep the unreferenced widget alive.
	TStrongObjectPtr<URRHUDWidget> HUDRoot(HUD);
	HUD->BindToMiner(Miner);
	TestTrue(TEXT("UpgradeScreen collapsed at start"), GetWidgetVisibility(*this, HUD, TEXT("UpgradeScreen")) == ESlateVisibility::Collapsed);

	Upgrades->SetRandomSeed(UpgradeSeed);
	LevelUp->OnLevelUpEffect.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleLevelUpEffect);
	LevelUp->OnUpgradeOfferShown.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleUpgradeOfferShown);
	LevelUp->OnUpgradeOfferClosed.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleUpgradeOfferClosed);
	Progression->OnLevelUp.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleLevelUp);
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);
	Rifle->GetAutoFireComponent()->OnShotFired.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleShotFired);
	const FVector Toward = Setup.Grid->GetCellCenter(StraightStartCell.X, StraightStartCell.Y) - Miner->GetActorLocation();
	AimAlong(Miner, Toward);

	// Kill five robots with the rifle, wound the miner, collect the five cubes: level 2 on the last one.
	const int32 ChaserCount = 5;
	FString Kills;
	if (!KillChasersOneByOne(*this, TestWorld, Setup, StraightStartCell, ChaserCount, Kills))
	{
		return false;
	}
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	const FGameplayAttribute Health = URRHealthAttributeSet::GetHealthAttribute();
	const FGameplayAttribute Damage = URRWeaponAttributeSet::GetDamageAttribute();
	UAbilitySystemComponent* MinerASC = Miner->GetAbilitySystemComponent();
	UAbilitySystemComponent* RifleASC = Rifle->GetAbilitySystemComponent();
	MinerASC->SetNumericAttributeBase(Health, 50.f);
	double CollectTime = 0.0;
	const int32 Moves = CollectXPCubes(TestWorld, Miner, Recorder.Get(), ChaserCount * ChaserXPCubes, CollectTime);
	AddInfo(FString::Printf(TEXT("kills:%s %d miner move(s); %d cubes collected in %.3f s"), *Kills, Moves, Recorder->GetCollectedPickups().Num(), CollectTime));
	if (!TestEqual(TEXT("level 2 after the five cubes"), Progression->GetLevel(), 2) || !TestEqual(TEXT("one level-up"), Recorder->GetLevelUps().Num(), 1))
	{
		return false;
	}

	// Heal and feedback at once, game still running during the choice delay.
	TestEqual(TEXT("level-up heals 20 % of max: 50 -> 70"), MinerASC->GetNumericAttribute(Health), 70.f, 1e-3f);
	TestEqual(TEXT("one level-up effect"), Recorder->GetLevelUpEffects().Num(), 1);
	TestTrue(TEXT("choice delay running"), LevelUp->IsChoiceDelayActive());
	TestFalse(TEXT("world not paused at the level-up"), World->IsPaused());
	TestEqual(TEXT("no offer at the level-up"), Recorder->GetOfferCardCounts().Num(), 0);
	Spawner->SetViewRectangleOverride(FBox2D(FVector2D(Miner->GetActorLocation()) - SpawnerViewHalfExtent, FVector2D(Miner->GetActorLocation()) + SpawnerViewHalfExtent));
	Spawner->SetSpawningEnabled(true);
	const float RifleDamageBefore = RifleASC->GetNumericAttribute(Damage);

	// Wait the delay frame by frame: the world pauses on the frame the screen opens.
	const double LevelUpTime = World->GetTimeSeconds();
	int32 PauseFrame = -1;
	bool bPausedEarly = false;
	for (int32 Frame = 1; Frame <= 60 && PauseFrame < 0; ++Frame)
	{
		TestWorld.Tick();
		if (LevelUp->IsAwaitingChoice())
		{
			PauseFrame = Frame;
		}
		bPausedEarly |= PauseFrame < 0 && World->IsPaused();
	}
	const double DelaySeconds = World->GetTimeSeconds() - LevelUpTime;
	AddInfo(FString::Printf(TEXT("screen opened %d frames (%.3f s game time) after the level-up"), PauseFrame, DelaySeconds));
	TestFalse(TEXT("no pause before the screen"), bPausedEarly);
	if (!TestTrue(TEXT("screen opened"), PauseFrame > 0))
	{
		return false;
	}
	TestTrue(TEXT("screen opens after the 0.8 s delay (within one frame)"), DelaySeconds >= LevelUp->GetChoiceDelay() - FrameSeconds - 1e-3 && DelaySeconds <= LevelUp->GetChoiceDelay() + FrameSeconds + 1e-3);
	TestTrue(TEXT("world paused by the game mode"), World->IsPaused());
	TestTrue(TEXT("controller reports the pause"), Controller->IsPaused());
	TestTrue(TEXT("cursor shown on the screen"), Controller->ShouldShowMouseCursor());
	if (!TestEqual(TEXT("one offer shown"), Recorder->GetOfferCardCounts().Num(), 1))
	{
		return false;
	}
	TestTrue(TEXT("reroll offered"), Recorder->GetOfferRerollFlags()[0]);
	const TArray<FRRUpgradeCard> FirstCards = Recorder->GetLastOfferCards();
	CheckOfferValid(*this, Miner, FirstCards, TEXT("first offer"));
	CheckHUDCards(*this, HUD, FirstCards, true, TEXT("first offer"));

	// Frozen: game time, spawner, rifle and position hold over 1 s of frames.
	const double PausedTime = World->GetTimeSeconds();
	const float PausedRunTime = Spawner->GetRunTime();
	const int32 PausedShots = Recorder->GetShotIndices().Num();
	const int32 PausedEnemies = CountEnemies(World);
	const FVector PausedLocation = Miner->GetActorLocation();
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("game time frozen while paused"), World->GetTimeSeconds(), PausedTime);
	TestEqual(TEXT("spawner frozen while paused"), Spawner->GetRunTime(), PausedRunTime);
	TestEqual(TEXT("no robot spawned while paused"), CountEnemies(World), PausedEnemies);
	TestEqual(TEXT("rifle silent while paused"), Recorder->GetShotIndices().Num(), PausedShots);
	TestTrue(TEXT("miner still while paused"), Miner->GetActorLocation().Equals(PausedLocation, 0.01f));
	TestTrue(TEXT("still awaiting the choice"), LevelUp->IsAwaitingChoice());

	// Reroll through the HUD (the Relancer button): new cards, no second reroll.
	TestTrue(TEXT("HUD reroll accepted"), HUD->RequestReroll());
	if (!TestEqual(TEXT("offer shown again"), Recorder->GetOfferCardCounts().Num(), 2))
	{
		return false;
	}
	TestFalse(TEXT("no second reroll offered"), Recorder->GetOfferRerollFlags()[1]);
	TestFalse(TEXT("second HUD reroll refused"), HUD->RequestReroll());
	const TArray<FRRUpgradeCard> Cards = Recorder->GetLastOfferCards();
	CheckOfferValid(*this, Miner, Cards, TEXT("rerolled offer"));
	CheckHUDCards(*this, HUD, Cards, false, TEXT("rerolled offer"));
	for (const FRRUpgradeCard& Card : Cards)
	{
		for (const FRRUpgradeCard& Old : FirstCards)
		{
			TestFalse(TEXT("rerolled card shares no stat and target with the first offer"), Card.IsSameChoice(Old));
		}
	}
	AddInfo(FString::Printf(TEXT("first offer:%s; rerolled:%s"), *DescribeCards(FirstCards), *DescribeCards(Cards)));

	// Pick the damage card for the rifle through the HUD (a card button).
	const int32 DamageIndex = Cards.IndexOfByPredicate([Rifle](const FRRUpgradeCard& Card)
	{
		return Card.Stat == ERRUpgradeStat::Damage && (Card.Target == ERRUpgradeTarget::AllWeapons || Card.TargetWeapon == Rifle);
	});
	if (!TestTrue(TEXT("rerolled offer holds a rifle damage card (seeded)"), DamageIndex != INDEX_NONE))
	{
		return false;
	}
	const FRRUpgradeCard& DamageCard = Cards[DamageIndex];
	const float ExpectedDamage = DamageCard.Operation == ERRUpgradeOperation::MultiplyAdditive
		? RifleDamageBefore * (1.f + DamageCard.Value) : RifleDamageBefore + DamageCard.Value;
	TestTrue(TEXT("HUD pick accepted"), HUD->ChooseUpgrade(DamageIndex));
	AddInfo(FString::Printf(TEXT("picked card %d (%s %s %s): rifle Damage %.3f -> %.3f"), DamageIndex, *DamageCard.DisplayName.ToString(),
		*DamageCard.TargetText.ToString(), *DamageCard.ValueText.ToString(), RifleDamageBefore, RifleASC->GetNumericAttribute(Damage)));
	TestEqual(TEXT("rifle damage raised by the card"), RifleASC->GetNumericAttribute(Damage), ExpectedDamage, 1e-3f);
	TestTrue(TEXT("rifle damage rose"), RifleASC->GetNumericAttribute(Damage) > RifleDamageBefore);
	TestEqual(TEXT("one upgrade picked"), Upgrades->GetPickedUpgrades().Num(), 1);
	TestEqual(TEXT("screen closed once"), Recorder->GetOfferClosedCount(), 1);
	TestTrue(TEXT("UpgradeScreen collapsed after the pick"), GetWidgetVisibility(*this, HUD, TEXT("UpgradeScreen")) == ESlateVisibility::Collapsed);
	TestFalse(TEXT("no choice left"), LevelUp->IsAwaitingChoice());
	TestFalse(TEXT("world resumed"), World->IsPaused());
	TestFalse(TEXT("controller resumed"), Controller->IsPaused());

	// Resumed: time, spawner and rifle run again (the spawner reaches its 2 s interval and spawns).
	const double ResumeTime = World->GetTimeSeconds();
	for (int32 Frame = 0; Frame < 90; ++Frame)
	{
		TestWorld.Tick();
	}
	AddInfo(FString::Printf(TEXT("after resume: %.3f s game time, spawner run time %.3f s, %d shot(s), %d robot(s)"),
		World->GetTimeSeconds() - ResumeTime, Spawner->GetRunTime(), Recorder->GetShotIndices().Num() - PausedShots, CountEnemies(World)));
	TestEqual(TEXT("game time runs again (1.5 s)"), World->GetTimeSeconds() - ResumeTime, 90.0 * FrameSeconds, 1e-3);
	TestTrue(TEXT("spawner runs again"), Spawner->GetRunTime() > PausedRunTime + 1.f);
	TestTrue(TEXT("spawner spawned after the resume"), CountEnemies(World) > PausedEnemies);
	TestTrue(TEXT("rifle fires again"), Recorder->GetShotIndices().Num() > PausedShots);
	HUD->BindToMiner(nullptr);
	return true;
}

namespace RRIntegrationObjectiveTests
{
	using namespace RRIntegrationTests;
	using namespace RRIntegrationUpgradeTests;

	/** Final objective Blueprints, expected in the RunFlowConfig of BP_RRGameMode. */
	const TCHAR* const ConsoleClassPath = TEXT("/Game/RogueRigs/Blueprints/Objectives/BP_ObjectiveConsole.BP_ObjectiveConsole_C");
	const TCHAR* const ShuttleClassPath = TEXT("/Game/RogueRigs/Blueprints/Objectives/BP_ExtractionShuttle.BP_ExtractionShuttle_C");

	/** Cheat alert speed of the loops: the 240 s bar fills in 6 s (console at 2.4 s, extraction 3.6 s after the hack). */
	constexpr float AlertSpeed = 40.f;

	/** Seed of the console and shuttle placement, the spawner and the upgrade draws. */
	constexpr int32 RunSeed = 20261003;

	/** Text of a TextBlock of WBP_HUD (empty with a test failure when missing). */
	FString GetWidgetText(FAutomationTestBase& Test, UUserWidget* Widget, const TCHAR* Name)
	{
		const UTextBlock* Text = Cast<UTextBlock>(Widget->GetWidgetFromName(Name));
		Test.TestNotNull(FString::Printf(TEXT("WBP_HUD has TextBlock %s"), Name), Text);
		return Text ? Text->GetText().ToString() : FString();
	}

	/** Percent of a ProgressBar of WBP_HUD (-1 with a test failure when missing). */
	float GetWidgetPercent(FAutomationTestBase& Test, UUserWidget* Widget, const TCHAR* Name)
	{
		const UProgressBar* Bar = Cast<UProgressBar>(Widget->GetWidgetFromName(Name));
		Test.TestNotNull(FString::Printf(TEXT("WBP_HUD has ProgressBar %s"), Name), Bar);
		return Bar ? Bar->GetPercent() : -1.f;
	}

	/** Moves the miner (teleport) to the XY of Target, standing on the floor of the cell below it. */
	void TeleportMiner(ARRMiner* Miner, const ARRArenaGrid* Grid, const FVector& Target)
	{
		const FIntPoint Cell = Grid->GetCellAtWorld(Target);
		const float FloorZ = Grid->GetCellCenter(Cell.X, Cell.Y).Z;
		const float HalfHeight = Miner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		Miner->SetActorLocation(FVector(Target.X, Target.Y, FloorZ + HalfHeight + 2.f), false, nullptr, ETeleportType::TeleportPhysics);
	}

	/** Resets the cheats the loops use (alert speed, shuttle countdown, invulnerability). Declare after the world: runs before its teardown. */
	struct FCheatGuard
	{
		explicit FCheatGuard(UWorld* InWorld) : World(InWorld) { Reset(); }
		~FCheatGuard()
		{
			RRCheats::SetInvulnerable(World, false, TEXT("RogueRigs.Integration"));
			Reset();
		}

		/** Neutral values of the persistent cheat properties. */
		static void Reset()
		{
			URRCheatSettings* Settings = GetMutableDefault<URRCheatSettings>();
			Settings->AlertSpeedMultiplier = 1.f;
			Settings->ShuttleCountdownOverride = 0.f;
			Settings->bInvulnerable = false;
		}

		/** World whose miner was made invulnerable. */
		UWorld* World = nullptr;
	};

	/** Actors and widgets of one objective loop: real game mode, controller, spawner and HUD around an armed, invulnerable miner. */
	struct FRunLoop
	{
		/** Unbinds the HUD (the world is destroyed after this). */
		~FRunLoop()
		{
			if (HUD)
			{
				HUD->BindToMiner(nullptr);
				HUD->BindToRunFlow(nullptr);
			}
		}

		FLoopSetup Setup;
		AGameModeBase* GameMode = nullptr;
		APlayerController* Controller = nullptr;
		ARREnemySpawner* Spawner = nullptr;
		URRHUDWidget* HUD = nullptr;
		/** Keeps WBP_HUD alive: the world tick may run a garbage collection and nothing else references the widget. */
		TStrongObjectPtr<URRHUDWidget> HUDRoot;
		URRRunFlowSubsystem* RunFlow = nullptr;
		UClass* ConsoleClass = nullptr;
		UClass* ShuttleClass = nullptr;
		/** Level-up cards picked through the HUD while the loop ran. */
		int32 UpgradeChoices = 0;
	};

	/**
	 * Starts BP_RRGameMode (StartGameMode), BP_ArenaGrid, an armed BP_Miner on ChaseMinerCell possessed by BP_RRPlayerController,
	 * BP_EnemySpawner (seeded, enabled, view rectangle around the miner), WBP_HUD bound to the miner and the run flow; the miner is
	 * made invulnerable (cheat) and the alert runs at AlertSpeed. Checks the run flow reads BP_ObjectiveConsole and BP_ExtractionShuttle.
	 */
	bool StartRun(FAutomationTestBase& Test, const FTestWorld& TestWorld, FTestGameInstance& GameInstance, FRunLoop& Out)
	{
		UWorld* World = TestWorld.Get();
		UClass* GameModeClass = LoadTestClass<AGameModeBase>(Test, GameModeClassPath);
		UClass* ControllerClass = LoadTestClass<APlayerController>(Test, PlayerControllerClassPath);
		UClass* SpawnerClass = LoadTestClass<ARREnemySpawner>(Test, SpawnerClassPath);
		UClass* HUDClass = LoadTestClass<URRHUDWidget>(Test, HUDClassPath);
		Out.ConsoleClass = LoadTestClass<ARRConsole>(Test, ConsoleClassPath);
		Out.ShuttleClass = LoadTestClass<ARRShuttle>(Test, ShuttleClassPath);
		if (!GameModeClass || !ControllerClass || !SpawnerClass || !HUDClass || !Out.ConsoleClass || !Out.ShuttleClass)
		{
			return false;
		}
		Out.GameMode = StartGameMode(Test, TestWorld, GameModeClass, GameInstance);
		Out.RunFlow = World->GetSubsystem<URRRunFlowSubsystem>();
		if (!Out.GameMode || !Test.TestNotNull(TEXT("run flow subsystem"), Out.RunFlow))
		{
			return false;
		}
		Out.RunFlow->SetRandomSeed(RunSeed);
		// Game grid: console and shuttle distances are cell counts tuned for 67 uu cells.
		if (!SetUp(Test, TestWorld, FineCell(ChaseMinerCell), /*bArmed*/ true, Out.Setup, /*bFineGrid*/ true))
		{
			return false;
		}
		ARRMiner* Miner = Out.Setup.Miner;
		const FRRRunFlowConfig& Config = Out.RunFlow->GetConfig();
		Test.TestTrue(TEXT("run flow reads BP_ObjectiveConsole from BP_RRGameMode"), Config.ConsoleClass.Get() == Out.ConsoleClass);
		Test.TestTrue(TEXT("run flow reads BP_ExtractionShuttle from BP_RRGameMode"), Config.ShuttleClass.Get() == Out.ShuttleClass);

		Out.Controller = TestWorld.Spawn<APlayerController>(ControllerClass, Miner->GetActorLocation());
		if (!Test.TestNotNull(TEXT("BP_RRPlayerController spawned"), Out.Controller))
		{
			return false;
		}
		Out.Controller->Possess(Miner);
		Out.Spawner = TestWorld.Spawn<ARREnemySpawner>(SpawnerClass, FVector::ZeroVector);
		Out.HUD = CreateWidget<URRHUDWidget>(World, HUDClass);
		Out.HUDRoot.Reset(Out.HUD);
		if (!Test.TestNotNull(TEXT("BP_EnemySpawner spawned"), Out.Spawner) || !Test.TestNotNull(TEXT("WBP_HUD created"), Out.HUD))
		{
			return false;
		}
		Out.Spawner->SeedRandomStream(RunSeed);
		Out.Spawner->SetSpawningEnabled(true);
		Out.HUD->BindToMiner(Miner);
		Out.HUD->BindToRunFlow(Out.RunFlow);
		Miner->GetUpgrades()->SetRandomSeed(RunSeed);
		Test.TestTrue(TEXT("EndScreen collapsed at start"), GetWidgetVisibility(Test, Out.HUD, TEXT("EndScreen")) == ESlateVisibility::Collapsed);

		GetMutableDefault<URRCheatSettings>()->AlertSpeedMultiplier = AlertSpeed;
		Test.TestEqual(TEXT("miner made invulnerable"), RRCheats::SetInvulnerable(World, true, TEXT("RogueRigs.Integration")), 1);
		return true;
	}

	/**
	 * Ticks the loop up to MaxFrames frames, stopping on the frame Done returns true; returns that frame (1-based) or -1. Before each
	 * frame the spawner view follows the miner, and an open level-up screen (world paused) is answered with card 0 through the HUD.
	 */
	int32 TickRun(FAutomationTestBase& Test, const FTestWorld& TestWorld, FRunLoop& Loop, int32 MaxFrames, TFunctionRef<bool()> Done)
	{
		ARRMiner* Miner = Loop.Setup.Miner;
		URRLevelUpComponent* LevelUp = Miner->GetLevelUp();
		for (int32 Frame = 1; Frame <= MaxFrames; ++Frame)
		{
			const FVector2D MinerXY(Miner->GetActorLocation());
			Loop.Spawner->SetViewRectangleOverride(FBox2D(MinerXY - SpawnerViewHalfExtent, MinerXY + SpawnerViewHalfExtent));
			if (LevelUp->IsAwaitingChoice())
			{
				Test.TestTrue(TEXT("level-up screen: world paused"), TestWorld.Get()->IsPaused());
				Test.TestTrue(TEXT("level-up screen: UpgradeScreen visible"), GetWidgetVisibility(Test, Loop.HUD, TEXT("UpgradeScreen")) == ESlateVisibility::Visible);
				Test.TestTrue(TEXT("level-up screen: HUD pick accepted"), Loop.HUD->ChooseUpgrade(0));
				++Loop.UpgradeChoices;
			}
			TestWorld.Tick();
			if (Done())
			{
				return Frame;
			}
		}
		return -1;
	}

	/** Checks the end state: run ended with bSuccess, real pause, UI input, WBP_HUD end screen with Title, nothing moves for 2 s. */
	void CheckEndScreen(FAutomationTestBase& Test, const FTestWorld& TestWorld, FRunLoop& Loop, const URRTestEventRecorder* Recorder, bool bSuccess, const TCHAR* Title)
	{
		UWorld* World = TestWorld.Get();
		ARRMiner* Miner = Loop.Setup.Miner;
		URRLevelUpComponent* LevelUp = Miner->GetLevelUp();
		Test.TestTrue(TEXT("run ended"), Loop.RunFlow->IsRunEnded());
		Test.TestTrue(TEXT("phase Ended"), Loop.RunFlow->GetPhase() == ERRRunPhase::Ended);
		Test.TestEqual(TEXT("run success flag"), Loop.RunFlow->WasRunSuccessful(), bSuccess);
		if (Test.TestEqual(TEXT("OnRunEnded once"), Recorder->GetRunEndedResults().Num(), 1))
		{
			Test.TestEqual(TEXT("OnRunEnded result"), Recorder->GetRunEndedResults()[0], bSuccess);
		}
		Test.TestFalse(TEXT("miner alive at the end"), Miner->IsDead());
		const ARRGameMode* RunGameMode = Cast<ARRGameMode>(Loop.GameMode);
		Test.TestTrue(TEXT("no game over"), RunGameMode && !RunGameMode->IsGameOver());

		// Real pause through the game mode, UI input with the cursor.
		Test.TestTrue(TEXT("world paused by the end"), World->IsPaused());
		Test.TestTrue(TEXT("controller reports the pause"), Loop.Controller->IsPaused());
		Test.TestTrue(TEXT("cursor shown on the end screen"), Loop.Controller->ShouldShowMouseCursor());

		// WBP_HUD end screen; the level-up screen lost to it.
		Test.TestTrue(TEXT("EndScreen visible"), GetWidgetVisibility(Test, Loop.HUD, TEXT("EndScreen")) == ESlateVisibility::Visible);
		Test.TestEqual(TEXT("EndTitle"), GetWidgetText(Test, Loop.HUD, TEXT("EndTitle")), FString(Title));
		Test.TestTrue(TEXT("ShuttleText hidden at the end"), GetWidgetVisibility(Test, Loop.HUD, TEXT("ShuttleText")) == ESlateVisibility::Collapsed);
		Test.TestTrue(TEXT("UpgradeScreen collapsed at the end"), GetWidgetVisibility(Test, Loop.HUD, TEXT("UpgradeScreen")) == ESlateVisibility::Collapsed);
		Test.TestTrue(TEXT("level-up choices blocked by the end"), LevelUp->AreChoicesBlocked());
		Test.TestFalse(TEXT("no level-up choice pending at the end"), LevelUp->IsAwaitingChoice());

		// Frozen: 2 s of frames change nothing.
		const double EndTime = World->GetTimeSeconds();
		const int32 EndEnemies = CountEnemies(World);
		const FVector EndLocation = Miner->GetActorLocation();
		const float EndCountdown = Loop.RunFlow->GetShuttle() ? Loop.RunFlow->GetShuttle()->GetRemainingSeconds() : -1.f;
		const int32 EndOffers = Recorder->GetOfferCardCounts().Num();
		for (int32 Frame = 0; Frame < 120; ++Frame)
		{
			TestWorld.Tick();
		}
		Test.TestEqual(TEXT("game time frozen after the end"), World->GetTimeSeconds(), EndTime);
		Test.TestEqual(TEXT("no robot spawned or removed after the end"), CountEnemies(World), EndEnemies);
		Test.TestTrue(TEXT("miner still after the end"), Miner->GetActorLocation().Equals(EndLocation, 0.01f));
		Test.TestEqual(TEXT("shuttle countdown frozen after the end"), Loop.RunFlow->GetShuttle() ? Loop.RunFlow->GetShuttle()->GetRemainingSeconds() : -1.f, EndCountdown);
		Test.TestEqual(TEXT("no level-up offer after the end"), Recorder->GetOfferCardCounts().Num(), EndOffers);
		Test.TestTrue(TEXT("EndScreen still visible"), GetWidgetVisibility(Test, Loop.HUD, TEXT("EndScreen")) == ESlateVisibility::Visible);
		Test.TestFalse(TEXT("second EndRun refused"), Loop.RunFlow->EndRun(!bSuccess));
		Test.TestEqual(TEXT("still one OnRunEnded"), Recorder->GetRunEndedResults().Num(), 1);

		// Rejouer: clicking the button reaches RestartLevel, which asks the engine to reopen this map (travel cleared before it runs).
		UButton* Restart = Cast<UButton>(Loop.HUD->GetWidgetFromName(TEXT("RestartButton")));
		FWorldContext* Context = GEngine->GetWorldContextFromWorld(World);
		if (Test.TestNotNull(TEXT("WBP_HUD has RestartButton"), Restart) && Test.TestNotNull(TEXT("world context"), Context))
		{
			Test.TestTrue(TEXT("RestartButton visible (UMG property: no Slate widget headless)"), Restart->GetVisibility() == ESlateVisibility::Visible || Restart->GetVisibility() == ESlateVisibility::SelfHitTestInvisible);
			Test.TestTrue(TEXT("RestartButton enabled"), Restart->GetIsEnabled());
			Test.TestTrue(TEXT("RestartButton click bound"), Restart->OnClicked.IsBound());
			Test.TestTrue(TEXT("no travel before the click"), Context->TravelURL.IsEmpty());
			Restart->OnClicked.Broadcast();
			Test.TestEqual(TEXT("Rejouer reopens the current map"), Context->TravelURL, UGameplayStatics::GetCurrentLevelName(World, true));
			Context->TravelURL.Empty();
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationConsoleExtractionWinTest, "RogueRigs.Integration.ConsoleExtractionWin",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationConsoleExtractionWinTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationObjectiveTests;

	// Declared in this order: the loop unbinds, the cheats reset, then the world and the game instance go.
	FTestGameInstance GameInstance;
	FTestWorld TestWorld(TEXT("RRIntegrationExtractionWin"));
	UWorld* World = TestWorld.Get();
	FCheatGuard CheatGuard(World);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FRunLoop Loop;
	if (!StartRun(*this, TestWorld, GameInstance, Loop))
	{
		return false;
	}
	ARRMiner* Miner = Loop.Setup.Miner;
	ARRArenaGrid* Grid = Loop.Setup.Grid;
	URRRunFlowSubsystem* RunFlow = Loop.RunFlow;
	URRLevelUpComponent* LevelUp = Miner->GetLevelUp();
	const FRRRunFlowConfig& Config = RunFlow->GetConfig();
	RunFlow->OnExtractionReady.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleExtractionReady);
	RunFlow->OnRunEnded.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleRunEnded);
	RunFlow->OnShuttleCountdown.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleShuttleCountdown);
	LevelUp->OnUpgradeOfferShown.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleUpgradeOfferShown);

	// 1. The alert (time and kills, x AlertSpeed) reaches the threshold: BP_ObjectiveConsole drops with marked rocks, alert paused.
	const double StartTime = World->GetTimeSeconds();
	const int32 DropFrame = TickRun(*this, TestWorld, Loop, 600, [RunFlow]() { return RunFlow->GetConsole() != nullptr; });
	if (!TestTrue(TEXT("console dropped"), DropFrame > 0))
	{
		return false;
	}
	ARRConsole* Console = RunFlow->GetConsole();
	Console->OnConsoleStateChanged.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleConsoleStateChanged);
	AddInfo(FString::Printf(TEXT("console dropped on (%d, %d) after %.3f s game time, %d kill(s), %d marked rock(s)"), Console->GetCell().X, Console->GetCell().Y,
		World->GetTimeSeconds() - StartTime, RunFlow->GetKillCount(), Console->GetInitialMarkedRockCount()));
	TestTrue(TEXT("console is BP_ObjectiveConsole"), Console->IsA(Loop.ConsoleClass));
	TestEqual(TEXT("alert clamped at the console threshold"), RunFlow->GetAlert(), Config.ConsoleAlertThreshold, 1e-4f);
	TestTrue(TEXT("phase ObjectivePending"), RunFlow->GetPhase() == ERRRunPhase::ObjectivePending);
	TestTrue(TEXT("console Locked"), Console->GetConsoleState() == ERRConsoleState::Locked);
	const TArray<FIntPoint> MarkedRocks = Console->GetMarkedRocks();
	if (!TestTrue(TEXT("console marked rocks (seeded drop prefers cells with rocks)"), MarkedRocks.Num() > 0))
	{
		return false;
	}
	for (const FIntPoint& Cell : MarkedRocks)
	{
		TestTrue(FString::Printf(TEXT("rock (%d, %d) marked on the grid"), Cell.X, Cell.Y), Grid->IsCellMarked(Cell));
	}
	TestTrue(TEXT("ObjectiveText visible"), GetWidgetVisibility(*this, Loop.HUD, TEXT("ObjectiveText")) == ESlateVisibility::Visible);
	TestTrue(TEXT("ObjectiveText asks to clear the marked rocks"), GetWidgetText(*this, Loop.HUD, TEXT("ObjectiveText")).Contains(TEXT("roches marquées")));
	TestEqual(TEXT("AlertBar at the threshold"), GetWidgetPercent(*this, Loop.HUD, TEXT("AlertBar")), Config.ConsoleAlertThreshold, 1e-4f);
	TestEqual(TEXT("AlertLabel shows the pause"), GetWidgetText(*this, Loop.HUD, TEXT("AlertLabel")), FString(TEXT("ALERTE (pause)")));

	// The alert holds while the console is pending.
	TickRun(*this, TestWorld, Loop, 60, []() { return false; });
	TestEqual(TEXT("alert holds while the console is pending"), RunFlow->GetAlert(), Config.ConsoleAlertThreshold, 1e-4f);
	TestFalse(TEXT("no extraction while the console is pending"), RunFlow->IsExtractionReady());

	// 2. The marked rocks are destroyed: unmarked, console hackable (Idle: the miner is 36+ cells away).
	for (const FIntPoint& Cell : MarkedRocks)
	{
		Grid->DestroyCell(Cell, ERRTerrainDamageSource::Drone);
	}
	TickRun(*this, TestWorld, Loop, 1, []() { return true; });
	TestEqual(TEXT("no marked rock left"), Console->GetMarkedRockCount(), 0);
	for (const FIntPoint& Cell : MarkedRocks)
	{
		TestFalse(FString::Printf(TEXT("cell (%d, %d) destroyed and unmarked"), Cell.X, Cell.Y), Grid->IsCellMarked(Cell) || Grid->GetCellType(Cell) != ERRCellType::Floor);
	}
	TestTrue(TEXT("console Idle once cleared"), Console->GetConsoleState() == ERRConsoleState::Idle);
	TestTrue(TEXT("ObjectiveText asks to hack"), GetWidgetText(*this, Loop.HUD, TEXT("ObjectiveText")).Contains(TEXT("Pirate la console")));

	// 3. The miner stands at the console: the drone hacks it in HackDuration of game time.
	TeleportMiner(Miner, Grid, Console->GetActorLocation());
	const double HackStart = World->GetTimeSeconds();
	const int32 HackFrames = TickRun(*this, TestWorld, Loop, 1800, [Console]() { return Console->IsComplete(); });
	const double HackSeconds = World->GetTimeSeconds() - HackStart;
	AddInfo(FString::Printf(TEXT("hack completed in %.3f s game time (%d frames), %d kill(s), %d upgrade choice(s) so far"), HackSeconds, HackFrames,
		RunFlow->GetKillCount(), Loop.UpgradeChoices));
	if (!TestTrue(TEXT("hack completed"), HackFrames > 0))
	{
		return false;
	}
	TestTrue(TEXT("hack lasted at least HackDuration (within one frame)"), HackSeconds >= Console->GetHackDuration() - FrameSeconds - 1e-3);
	TestTrue(TEXT("console states went through Hacking"), Recorder->GetConsoleStates().Contains(ERRConsoleState::Hacking));
	TestTrue(TEXT("console ends Complete"), Recorder->GetConsoleStates().Num() > 0 && Recorder->GetConsoleStates().Last() == ERRConsoleState::Complete);
	TestFalse(TEXT("objective no longer pending"), RunFlow->HasPendingObjective());
	TestTrue(TEXT("HackBar hidden once hacked"), GetWidgetVisibility(*this, Loop.HUD, TEXT("HackBar")) == ESlateVisibility::Collapsed);

	// 4. The alert resumes to 1: extraction ready once, BP_ExtractionShuttle lands far away with its countdown.
	const double ResumeTime = World->GetTimeSeconds();
	const int32 LandFrame = TickRun(*this, TestWorld, Loop, 900, [RunFlow]() { return RunFlow->GetShuttle() != nullptr; });
	if (!TestTrue(TEXT("shuttle landed"), LandFrame > 0))
	{
		return false;
	}
	ARRShuttle* Shuttle = RunFlow->GetShuttle();
	const float LandDistanceCells = FVector::Dist2D(Shuttle->GetActorLocation(), Miner->GetActorLocation()) / Grid->GetCellSize();
	AddInfo(FString::Printf(TEXT("shuttle landed on (%d, %d) %.1f cells away, %.3f s game time after the hack"), Shuttle->GetCell().X, Shuttle->GetCell().Y,
		LandDistanceCells, World->GetTimeSeconds() - ResumeTime));
	TestTrue(TEXT("shuttle is BP_ExtractionShuttle"), Shuttle->IsA(Loop.ShuttleClass));
	TestEqual(TEXT("extraction ready once"), Recorder->GetExtractionReadyCount(), 1);
	TestEqual(TEXT("alert full"), RunFlow->GetAlert(), 1.f, 1e-4f);
	TestTrue(TEXT("shuttle at least ShuttleMinDistanceCells away (within a cell)"), LandDistanceCells >= Config.ShuttleMinDistanceCells - 1.f);
	TestEqual(TEXT("countdown from the config"), Shuttle->GetCountdownSeconds(), Config.ShuttleCountdownSeconds, 1e-4f);
	TestTrue(TEXT("ObjectiveText says to reach the shuttle"), GetWidgetText(*this, Loop.HUD, TEXT("ObjectiveText")).Contains(TEXT("navette")));
	TestTrue(TEXT("ShuttleText visible"), GetWidgetVisibility(*this, Loop.HUD, TEXT("ShuttleText")) == ESlateVisibility::Visible);
	TestTrue(TEXT("ShuttleText shows the countdown"), GetWidgetText(*this, Loop.HUD, TEXT("ShuttleText")).StartsWith(TEXT("Navette")));
	TickRun(*this, TestWorld, Loop, 60, []() { return false; });
	TestTrue(TEXT("countdown running"), Shuttle->GetRemainingSeconds() < Config.ShuttleCountdownSeconds - 0.5f);
	TestFalse(TEXT("run not ended while the miner is away"), RunFlow->IsRunEnded());

	// 5. A level-up starts its choice delay, then the miner boards: the run ends won and the end screen wins over the offer.
	TestTrue(TEXT("XP granted"), RRCheats::GrantXP(World, 500.f, TEXT("RogueRigs.Integration")));
	TestTrue(TEXT("level-up choice delay running before boarding"), LevelUp->IsChoiceDelayActive());
	TeleportMiner(Miner, Grid, Shuttle->GetActorLocation());
	TestEqual(TEXT("run ends on the boarding tick"), TickRun(*this, TestWorld, Loop, 1, [RunFlow]() { return RunFlow->IsRunEnded(); }), 1);
	TestTrue(TEXT("shuttle extracted the miner"), Shuttle->IsFinished() && Shuttle->WasExtracted());
	AddInfo(FString::Printf(TEXT("won with %.2f s left; %d kill(s), %d upgrade choice(s) during the run"), Shuttle->GetRemainingSeconds(), RunFlow->GetKillCount(), Loop.UpgradeChoices));
	CheckEndScreen(*this, TestWorld, Loop, Recorder.Get(), /*bSuccess*/ true, TEXT("EXTRACTION RÉUSSIE"));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationShuttleTimeoutLossTest, "RogueRigs.Integration.ShuttleTimeoutLoss",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationShuttleTimeoutLossTest::RunTest(const FString& Parameters)
{
	using namespace RRIntegrationObjectiveTests;

	FTestGameInstance GameInstance;
	FTestWorld TestWorld(TEXT("RRIntegrationExtractionLoss"));
	UWorld* World = TestWorld.Get();
	FCheatGuard CheatGuard(World);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FRunLoop Loop;
	if (!StartRun(*this, TestWorld, GameInstance, Loop))
	{
		return false;
	}
	URRRunFlowSubsystem* RunFlow = Loop.RunFlow;
	RunFlow->OnRunEnded.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleRunEnded);
	Loop.Setup.Miner->GetLevelUp()->OnUpgradeOfferShown.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleUpgradeOfferShown);
	constexpr float ShortCountdown = 3.f;
	GetMutableDefault<URRCheatSettings>()->ShuttleCountdownOverride = ShortCountdown;

	// Accelerated alert to the console; the hack (proven by ConsoleExtractionWin) is completed by the cheat; the alert fills.
	if (!TestTrue(TEXT("console dropped"), TickRun(*this, TestWorld, Loop, 600, [RunFlow]() { return RunFlow->GetConsole() != nullptr; }) > 0))
	{
		return false;
	}
	TestTrue(TEXT("console is BP_ObjectiveConsole"), RunFlow->GetConsole()->IsA(Loop.ConsoleClass));
	TestTrue(TEXT("hack completed by the cheat"), RRCheats::CompleteConsoleHack(World, TEXT("RogueRigs.Integration")));
	if (!TestTrue(TEXT("shuttle landed"), TickRun(*this, TestWorld, Loop, 900, [RunFlow]() { return RunFlow->GetShuttle() != nullptr; }) > 0))
	{
		return false;
	}
	ARRShuttle* Shuttle = RunFlow->GetShuttle();
	TestTrue(TEXT("shuttle is BP_ExtractionShuttle"), Shuttle->IsA(Loop.ShuttleClass));
	TestEqual(TEXT("countdown from the cheat override"), Shuttle->GetCountdownSeconds(), ShortCountdown, 1e-4f);

	// The miner stays where it is (75+ cells away): the countdown runs out in game time, the run is lost.
	const double LandTime = World->GetTimeSeconds();
	const int32 EndFrame = TickRun(*this, TestWorld, Loop, 600, [RunFlow]() { return RunFlow->IsRunEnded(); });
	const double CountdownSeconds = World->GetTimeSeconds() - LandTime;
	AddInfo(FString::Printf(TEXT("run lost %.3f s game time after landing (%d frames), %d kill(s), %d upgrade choice(s)"), CountdownSeconds, EndFrame,
		RunFlow->GetKillCount(), Loop.UpgradeChoices));
	if (!TestTrue(TEXT("run ended"), EndFrame > 0))
	{
		return false;
	}
	TestEqual(TEXT("lost when the countdown runs out (within one frame)"), CountdownSeconds, double(ShortCountdown), double(FrameSeconds) + 1e-3);
	TestTrue(TEXT("shuttle missed"), Shuttle->IsFinished() && !Shuttle->WasExtracted());
	CheckEndScreen(*this, TestWorld, Loop, Recorder.Get(), /*bSuccess*/ false, TEXT("RUN PERDUE"));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
