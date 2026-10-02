// Rogue Rigs - integration tests with the final Blueprints (RogueRigs.Integration.*): BP_Chaser reaching and hitting BP_Miner around rocks, the starting rifle killing a chaser, miner death and game over, and a 50-chaser headless timing run.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Enemies/RRChaser.h"
#include "EngineUtils.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GAS/RRHealthAttributeSet.h"
#include "HAL/PlatformTime.h"
#include "Math/RandomStream.h"
#include "Pickups/RRPickup.h"
#include "Player/RRGameMode.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRIntegrationTests
{
	using namespace RRTestHarness;

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

	/** Spawns BP_ArenaGrid and BP_Miner on Cell (weapons stopped unless bArmed), loads BP_Chaser, ticks one frame. */
	bool SetUp(FAutomationTestBase& Test, const FTestWorld& TestWorld, FIntPoint Cell, bool bArmed, FLoopSetup& Out)
	{
		Out.ChaserClass = LoadTestClass<ARRChaser>(Test, ChaserClassPath);
		Out.Grid = Out.ChaserClass ? SpawnGrid(Test, TestWorld) : nullptr;
		Out.Miner = Out.Grid ? SpawnMiner(Test, TestWorld, Out.Grid, Cell) : nullptr;
		Out.FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
		if (!Out.Miner || !Test.TestNotNull(TEXT("flow field subsystem"), Out.FlowField))
		{
			return false;
		}
		if (!bArmed)
		{
			StopWeapons(Out.Miner);
		}
		TestWorld.Tick();
		return Test.TestEqual(TEXT("field goal cell is the miner cell"), Out.FlowField->GetGoalCell(), Cell);
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
	TestEqual(TEXT("one XP pickup dropped"), CountXPPickups(TestWorld.Get()), 1);
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
	const int32 ComputesBefore = Setup.FlowField->GetComputeCount();
	TArray<double> FrameMs;
	FrameMs.Reserve(Frames);
	TArray<float> ComputeMs;
	int32 RocksDestroyed = 0;
	for (int32 Frame = 0; Frame < Frames; ++Frame)
	{
		const int32 ComputesBeforeFrame = Setup.FlowField->GetComputeCount();
		if (Frame > 0 && Frame % 150 == 0 && RocksDestroyed < UE_ARRAY_COUNT(PerfRockCells))
		{
			RocksDestroyed += Setup.Grid->DestroyCell(PerfRockCells[RocksDestroyed], ERRTerrainDamageSource::Barrel) ? 1 : 0;
		}
		const double Start = FPlatformTime::Seconds();
		TestWorld.Tick();
		FrameMs.Add((FPlatformTime::Seconds() - Start) * 1000.0);
		if (Setup.FlowField->GetComputeCount() != ComputesBeforeFrame)
		{
			ComputeMs.Add(Setup.FlowField->GetLastComputeMilliseconds());
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
		TEXT("flow field %d recomputes (%d rocks destroyed): avg %.3f ms, max %.3f ms [%s ]; %d alive, %d out of Chase at the end"),
		Frames, Sum / Frames, Sorted[Frames / 2], Sorted[FMath::Min(Frames - 1, FMath::FloorToInt(Frames * 0.95))], Max,
		ComputeMs.Num(), RocksDestroyed, ComputeMs.Num() > 0 ? ComputeSum / ComputeMs.Num() : 0.f, ComputeMax, *ComputeList, Alive, InContact);
	AddInfo(Summary);
	UE_LOG(LogTemp, Display, TEXT("%s"), *Summary);
	TestEqual(TEXT("three rocks destroyed"), RocksDestroyed, 3);
	TestTrue(TEXT("a recompute per destroyed rock"), Setup.FlowField->GetComputeCount() - ComputesBefore >= RocksDestroyed);
	TestEqual(TEXT("every chaser alive (miner unarmed)"), Alive, ChaserCount);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
