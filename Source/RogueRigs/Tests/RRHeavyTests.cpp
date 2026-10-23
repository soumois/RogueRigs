// Rogue Rigs - automation tests of the heavy (RogueRigs.Heavy.*): CSV stats and capsule, slam telegraph and cadence, shockwave rules (miner only, no terrain, no robot, dodgeable), one-cell corridor crossing, spawner share and per-class alive cap.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRHeavy.h"
#include "Enemies/RRKamikaze.h"
#include "Enemies/RRShooter.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRHeavyTests
{
	using namespace RRTestHarness;
	using namespace RRTestAccess;

	/** Open row 43 of arena_m1 (x 28 to 51 floor): miner on the west, heavy east of it. */
	const FIntPoint OpenMinerCell(30, 43);

	/** One-cell corridor of arena_m1: column x 36, rows 66 to 68 (barrel and rock on the west, rock on the east). */
	constexpr int32 CorridorX = 36;
	constexpr int32 CorridorMinY = 66;
	constexpr int32 CorridorMaxY = 68;

	/** Spawns a native robot of class T with its feet on the floor at FloorLocation (XP drop: ARRTestPickupXP). */
	template <typename T>
	T* SpawnRobot(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FVector& FloorLocation)
	{
		const float HalfHeight = GetDefault<T>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(FloorLocation + FVector(0.f, 0.f, HalfHeight + 2.f));
		T* Robot = TestWorld.Get()->SpawnActorDeferred<T>(T::StaticClass(), SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Test.TestNotNull(TEXT("robot spawned"), Robot))
		{
			FEnemyAccess::XPPickupClassOf(Robot) = ARRTestPickupXP::StaticClass();
			Robot->FinishSpawning(SpawnTransform);
		}
		return Robot;
	}

	/** Health of Actor (health set on its ASC). */
	float Health(const IAbilitySystemInterface* Actor)
	{
		return Actor->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Grid and an unarmed miner on StartCell; false when something failed. */
	bool MakeArena(FAutomationTestBase& Test, const FTestWorld& TestWorld, FIntPoint StartCell, ARRArenaGrid*& OutGrid, ARRMiner*& OutMiner)
	{
		OutGrid = SpawnGrid(Test, TestWorld);
		OutMiner = OutGrid ? SpawnMiner(Test, TestWorld, OutGrid, StartCell) : nullptr;
		if (!OutMiner)
		{
			return false;
		}
		for (ARRWeapon* Weapon : OutMiner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		return true;
	}

	/** Weighted spawner entry. */
	FRRWeightedEnemyClass Entry(TSubclassOf<ARREnemy> Class, float Weight, int32 MaxAlive = 0)
	{
		FRRWeightedEnemyClass Result;
		Result.EnemyClass = Class;
		Result.Weight = Weight;
		Result.MaxAlive = MaxAlive;
		return Result;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHeavyStatsTest, "RogueRigs.Heavy.StatsFromCSV",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHeavyStatsTest::RunTest(const FString& Parameters)
{
	using namespace RRHeavyTests;

	FRREnemyStatsRow Row;
	if (TestTrue(TEXT("EnemyStats.csv row Heavy read"), ARREnemy::LoadStatsRow(TEXT("Heavy"), Row)))
	{
		TestEqual(TEXT("MaxHealth 300"), Row.MaxHealth, 300.f);
		TestEqual(TEXT("MoveSpeed 153"), Row.MoveSpeed, 153.f);
		TestEqual(TEXT("Damage 25 (slam)"), Row.Damage, 25.f);
		TestEqual(TEXT("AttackRate 0.5 (one slam every 2 s)"), Row.AttackRate, 0.5f);
		TestEqual(TEXT("AttackRange 361 (slam radius)"), Row.AttackRange, 361.f);
		TestEqual(TEXT("WindUp 1.0 (telegraph)"), Row.WindUp, 1.f);
		TestEqual(TEXT("XPValue 5"), Row.XPValue, 5);
		TestEqual(TEXT("KnockbackResistance 0.8"), Row.KnockbackResistance, 0.8f);
	}

	FTestWorld TestWorld(TEXT("RRHeavyStats"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	if (!Grid)
	{
		return false;
	}
	ARRHeavy* Heavy = SpawnRobot<ARRHeavy>(*this, TestWorld, Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y));
	if (!Heavy)
	{
		return false;
	}
	TestEqual(TEXT("Health 300"), Health(Heavy), 300.f);
	TestEqual(TEXT("MaxWalkSpeed 153"), Heavy->GetCharacterMovement()->MaxWalkSpeed, 153.f);
	TestEqual(TEXT("knockback resistance 0.8"), Heavy->GetKnockbackResistance(), 0.8f);
	TestEqual(TEXT("slam radius 361"), Heavy->GetSlamRadius(), 361.f);
	TestEqual(TEXT("telegraph 1.0 s"), Heavy->GetTelegraphDuration(), 1.f);
	TestEqual(TEXT("arrival 144.5 uu (half the slam radius)"), Heavy->GetEnemyMovement()->GetArrivalDistance(), 144.5f);
	const float Radius = Heavy->GetBodyRadius();
	TestTrue(TEXT("body radius at most 80 uu"), Radius <= 80.f);
	TestEqual(TEXT("body 2x chaser (48 x 124)"), Radius, 48.f);
	TestEqual(TEXT("hit capsule radius 48"), Heavy->GetHitCapsule()->GetScaledCapsuleRadius(), 48.f);
	TestEqual(TEXT("terrain capsule radius 24 (chaser)"), Heavy->GetCapsuleComponent()->GetScaledCapsuleRadius(), 24.f);
	TestEqual(TEXT("capsule half height 124"), Heavy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), 124.f);
	TestEqual(TEXT("starts in Chase"), Heavy->GetState(), ERRHeavyState::Chase);
	TestEqual(TEXT("no telegraph progress at rest"), Heavy->GetTelegraphProgress(), 0.f);
	TestTrue(TEXT("robots pass through robots"), Heavy->IsIgnoringOtherRobots());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHeavyCadenceTest, "RogueRigs.Heavy.SlamCadence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHeavyCadenceTest::RunTest(const FString& Parameters)
{
	using namespace RRHeavyTests;

	FTestWorld TestWorld(TEXT("RRHeavyCadence"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!MakeArena(*this, TestWorld, OpenMinerCell, Grid, Miner))
	{
		return false;
	}
	const FVector MinerFloor = Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y);
	ARRHeavy* Heavy = SpawnRobot<ARRHeavy>(*this, TestWorld, MinerFloor + FVector(600.f, 0.f, 0.f));
	if (!Heavy)
	{
		return false;
	}
	const float HealthBefore = Health(Miner);

	TArray<double> TelegraphStarts;
	TArray<double> SlamTimes;
	TArray<float> HealthAfterSlams;
	float FirstTelegraphDistance = -1.f;
	float MaxProgress = 0.f;
	bool bStoppedWhileTelegraphing = true;
	bool bProgressInRange = true;
	for (int32 Frame = 0; Frame < 60 * 12 && SlamTimes.Num() < 3; ++Frame)
	{
		const ERRHeavyState Previous = Heavy->GetState();
		const int32 SlamsBefore = Heavy->GetSlamCount();
		TestWorld.Tick();
		const double Now = TestWorld.Get()->GetTimeSeconds();
		if (Previous != ERRHeavyState::Telegraph && Heavy->IsTelegraphing())
		{
			TelegraphStarts.Add(Now);
			if (FirstTelegraphDistance < 0.f)
			{
				FirstTelegraphDistance = FVector::Dist2D(Heavy->GetActorLocation(), Miner->GetActorLocation());
			}
		}
		if (Heavy->IsTelegraphing())
		{
			bStoppedWhileTelegraphing &= Heavy->GetVelocity().Size2D() < 1.f;
			const float Progress = Heavy->GetTelegraphProgress();
			bProgressInRange &= Progress >= 0.f && Progress <= 1.f;
			MaxProgress = FMath::Max(MaxProgress, Progress);
		}
		if (Heavy->GetSlamCount() > SlamsBefore)
		{
			SlamTimes.Add(Heavy->GetLastSlamTime());
			HealthAfterSlams.Add(Health(Miner));
		}
	}
	AddInfo(FString::Printf(TEXT("telegraphs at %s; slams at %s; first telegraph at %.0f uu"),
		*FString::JoinBy(TelegraphStarts, TEXT(", "), [](double Time) { return FString::Printf(TEXT("%.3f"), Time); }),
		*FString::JoinBy(SlamTimes, TEXT(", "), [](double Time) { return FString::Printf(TEXT("%.3f"), Time); }),
		FirstTelegraphDistance));

	if (!TestEqual(TEXT("three slams within 12 s"), SlamTimes.Num(), 3) || !TestTrue(TEXT("three telegraphs"), TelegraphStarts.Num() >= 3))
	{
		return false;
	}
	TestTrue(TEXT("first telegraph only after walking in (beyond the trigger at spawn)"), TelegraphStarts[0] > 0.5);
	TestTrue(TEXT("telegraph starts within 252.7 uu (361 x 0.7)"), FirstTelegraphDistance > 0.f && FirstTelegraphDistance <= 252.7f + 1.f);
	for (int32 Index = 0; Index < 3; ++Index)
	{
		TestTrue(FString::Printf(TEXT("slam %d lands 1.0 s after its telegraph (within one frame)"), Index + 1),
			FMath::Abs(SlamTimes[Index] - TelegraphStarts[Index] - 1.0) <= FrameSeconds + 1e-3);
		TestEqual(FString::Printf(TEXT("slam %d hits for 25"), Index + 1), HealthAfterSlams[Index], HealthBefore - 25.f * (Index + 1));
	}
	TestTrue(TEXT("slam 2 lands 2 s after slam 1 (within one frame)"), FMath::Abs(SlamTimes[1] - SlamTimes[0] - 2.0) <= FrameSeconds + 1e-3);
	TestTrue(TEXT("slam 3 lands 2 s after slam 2 (within one frame)"), FMath::Abs(SlamTimes[2] - SlamTimes[1] - 2.0) <= FrameSeconds + 1e-3);
	TestTrue(TEXT("stopped while telegraphing"), bStoppedWhileTelegraphing);
	TestTrue(TEXT("telegraph progress in [0, 1]"), bProgressInRange);
	TestTrue(TEXT("telegraph progress reaches 0.95"), MaxProgress >= 0.95f);
	TestEqual(TEXT("three hits counted"), Heavy->GetSlamHitCount(), 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHeavyShockwaveTest, "RogueRigs.Heavy.ShockwaveRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHeavyShockwaveTest::RunTest(const FString& Parameters)
{
	using namespace RRHeavyTests;

	// Heavy on (42, 38): rock (43, 38) 200 uu east, a frozen chaser on (42, 39) 200 uu south, the miner 150 uu west.
	FTestWorld TestWorld(TEXT("RRHeavyShockwave"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!MakeArena(*this, TestWorld, RockWestFloorCell, Grid, Miner))
	{
		return false;
	}
	const FVector HeavyFloor = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y);
	Miner->SetActorLocation(FVector(HeavyFloor.X - 150.f, HeavyFloor.Y, Miner->GetActorLocation().Z));
	ARRHeavy* Heavy = SpawnRobot<ARRHeavy>(*this, TestWorld, HeavyFloor);
	ARRChaser* Chaser = SpawnRobot<ARRChaser>(*this, TestWorld, Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y + 1));
	if (!Heavy || !Chaser || !TestEqual(TEXT("rock east of the heavy"), Grid->GetCellType(RockCell), ERRCellType::Rock))
	{
		return false;
	}
	Chaser->SetActorTickEnabled(false);
	Chaser->GetCharacterMovement()->SetComponentTickEnabled(false);
	Chaser->GetEnemyMovement()->SetComponentTickEnabled(false);
	const float RockHealth = Grid->GetCellHealth(RockCell);
	const float MinerHealth = Health(Miner);
	const float ChaserHealth = Health(Chaser);
	const float HeavyHealth = Health(Heavy);

	// 1. Miner within the radius: one hit of 25; rock and robots untouched.
	for (int32 Frame = 0; Frame < 90 && Heavy->GetSlamCount() < 1; ++Frame)
	{
		TestWorld.Tick();
	}
	if (!TestEqual(TEXT("first slam within 1.5 s"), Heavy->GetSlamCount(), 1))
	{
		return false;
	}
	TestEqual(TEXT("miner hit for 25"), Health(Miner), MinerHealth - 25.f);
	TestEqual(TEXT("chaser 200 uu away unharmed"), Health(Chaser), ChaserHealth);
	TestEqual(TEXT("heavy unharmed"), Health(Heavy), HeavyHealth);
	TestEqual(TEXT("rock 200 uu away still standing"), Grid->GetCellType(RockCell), ERRCellType::Rock);
	TestEqual(TEXT("rock health unchanged"), Grid->GetCellHealth(RockCell), RockHealth);

	// 2. Second telegraph: the miner walks out (teleported to (39, 38), 600 uu west, beyond 361 uu) during it; the slam lands and misses.
	for (int32 Frame = 0; Frame < 60 * 4 && !Heavy->IsTelegraphing(); ++Frame)
	{
		TestWorld.Tick();
	}
	if (!TestTrue(TEXT("second telegraph"), Heavy->IsTelegraphing()))
	{
		return false;
	}
	const FVector Escape = Grid->GetCellCenter(39, 38);
	Miner->SetActorLocation(FVector(Escape.X, Escape.Y, Miner->GetActorLocation().Z));
	const float HealthBeforeDodge = Health(Miner);
	for (int32 Frame = 0; Frame < 90 && Heavy->GetSlamCount() < 2; ++Frame)
	{
		TestWorld.Tick();
	}
	const float DodgeDistance = FVector::Dist2D(Heavy->GetActorLocation(), Miner->GetActorLocation());
	AddInfo(FString::Printf(TEXT("dodged slam at %.0f uu"), DodgeDistance));
	TestEqual(TEXT("second slam landed (committed telegraph)"), Heavy->GetSlamCount(), 2);
	TestTrue(TEXT("miner beyond the radius"), DodgeDistance > 361.f);
	TestEqual(TEXT("dodged slam deals nothing"), Health(Miner), HealthBeforeDodge);
	TestEqual(TEXT("one hit in total"), Heavy->GetSlamHitCount(), 1);
	TestEqual(TEXT("chaser still unharmed"), Health(Chaser), ChaserHealth);
	TestEqual(TEXT("rock still standing after two slams"), Grid->GetCellHealth(RockCell), RockHealth);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHeavyCorridorTest, "RogueRigs.Heavy.CrossesOneCellCorridor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHeavyCorridorTest::RunTest(const FString& Parameters)
{
	using namespace RRHeavyTests;

	// Miner north of the corridor on (36, 61); heavy south of it on (36, 69): the shortest path is the corridor x 36, rows 66-68.
	FTestWorld TestWorld(TEXT("RRHeavyCorridor"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!MakeArena(*this, TestWorld, FIntPoint(CorridorX, 61), Grid, Miner))
	{
		return false;
	}
	const URRFlowFieldSubsystem* FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
	TestWorld.Tick(); // The flow field binds the grid on its first tick.
	for (int32 Y = CorridorMinY; Y <= CorridorMaxY; ++Y)
	{
		TestTrue(FString::Printf(TEXT("corridor cell (%d, %d) walkable"), CorridorX, Y), FlowField->IsWalkableCell(FIntPoint(CorridorX, Y)));
		TestFalse(TEXT("west side blocked"), FlowField->IsWalkableCell(FIntPoint(CorridorX - 1, Y)));
		TestFalse(TEXT("east side blocked"), FlowField->IsWalkableCell(FIntPoint(CorridorX + 1, Y)));
	}
	ARRHeavy* Heavy = SpawnRobot<ARRHeavy>(*this, TestWorld, Grid->GetCellCenter(CorridorX, 69));
	if (!Heavy)
	{
		return false;
	}
	TestTrue(TEXT("capsule radius at most 80 uu"), Heavy->GetCapsuleComponent()->GetScaledCapsuleRadius() <= 80.f);

	TSet<int32> CorridorRowsVisited;
	bool bAlwaysWalkable = true;
	double ExitTime = -1.0;
	for (int32 Frame = 0; Frame < 60 * 15 && ExitTime < 0.0; ++Frame)
	{
		TestWorld.Tick();
		const FIntPoint Cell = Grid->GetCellAtWorld(Heavy->GetActorLocation());
		bAlwaysWalkable &= FlowField->IsWalkableCell(Cell);
		if (Cell.X == CorridorX && Cell.Y >= CorridorMinY && Cell.Y <= CorridorMaxY)
		{
			CorridorRowsVisited.Add(Cell.Y);
		}
		if (Cell.Y < CorridorMinY - 1)
		{
			ExitTime = TestWorld.Get()->GetTimeSeconds();
		}
	}
	AddInfo(FString::Printf(TEXT("left the corridor north at t=%.3f s, %d corridor rows crossed"), ExitTime, CorridorRowsVisited.Num()));
	TestTrue(TEXT("came out north of the corridor within 15 s"), ExitTime > 0.0);
	TestEqual(TEXT("crossed the three corridor cells"), CorridorRowsVisited.Num(), 3);
	TestTrue(TEXT("center always on a walkable cell"), bAlwaysWalkable);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHeavySpawnerCapTest, "RogueRigs.Heavy.SpawnerShareAndCap",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHeavySpawnerCapTest::RunTest(const FString& Parameters)
{
	using namespace RRHeavyTests;

	TestEqual(TEXT("entry cap defaults to 0 (none)"), FRRWeightedEnemyClass().MaxAlive, 0);

	FTestWorld TestWorld(TEXT("RRHeavySpawnerCap"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!MakeArena(*this, TestWorld, FIntPoint(40, 40), Grid, Miner))
	{
		return false;
	}
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	if (!TestNotNull(TEXT("spawner spawned"), Spawner))
	{
		return false;
	}
	Spawner->SetSpawnMaxFrames(0); // task-091-01: robots delivered at once (these checks read spawn results in the same frame); spread covered by RogueRigs.RobotPool.
	Spawner->SetSpawningEnabled(false);
	const FVector MinerLocation = Miner->GetActorLocation();
	Spawner->SetViewRectangleOverride(FBox2D(FVector2D(MinerLocation.X - 1600.f, MinerLocation.Y - 1000.f),
		FVector2D(MinerLocation.X + 1600.f, MinerLocation.Y + 1000.f)));

	// 1. Owner mix chaser 65 / kamikaze 12 / shooter 15 / heavy 8 (cap 5, nothing alive): seeded share.
	FSpawnerAccess::WeightsOf(Spawner) = { Entry(ARRChaser::StaticClass(), 65.f), Entry(ARRKamikaze::StaticClass(), 12.f),
		Entry(ARRShooter::StaticClass(), 15.f), Entry(ARRHeavy::StaticClass(), 8.f, 5) };
	FRandomStream Stream(20261004);
	constexpr int32 Picks = 4000;
	int32 Counts[4] = { 0, 0, 0, 0 };
	for (int32 Index = 0; Index < Picks; ++Index)
	{
		const TSubclassOf<ARREnemy> Picked = Spawner->PickEnemyClass(Stream);
		Counts[0] += Picked == ARRChaser::StaticClass() ? 1 : 0;
		Counts[1] += Picked == ARRKamikaze::StaticClass() ? 1 : 0;
		Counts[2] += Picked == ARRShooter::StaticClass() ? 1 : 0;
		Counts[3] += Picked == ARRHeavy::StaticClass() ? 1 : 0;
	}
	AddInfo(FString::Printf(TEXT("%d chasers, %d kamikazes, %d shooters, %d heavies over %d seeded picks"), Counts[0], Counts[1], Counts[2],
		Counts[3], Picks));
	TestEqual(TEXT("every pick is one of the four classes"), Counts[0] + Counts[1] + Counts[2] + Counts[3], Picks);
	TestTrue(TEXT("62 to 68 % chasers"), Counts[0] >= 2480 && Counts[0] <= 2720);
	TestTrue(TEXT("6.5 to 9.5 % heavies"), Counts[3] >= 260 && Counts[3] <= 380);

	// 2. Chaser 1 / heavy 1 capped at 5: 30 spawns never hold more than 5 heavies alive.
	FSpawnerAccess::WeightsOf(Spawner) = { Entry(ARRChaser::StaticClass(), 1.f), Entry(ARRHeavy::StaticClass(), 1.f, 5) };
	Spawner->SeedRandomStream(20261004);
	int32 MaxHeavies = 0;
	int32 Spawned = 0;
	TArray<ARRHeavy*> Heavies;
	for (int32 Index = 0; Index < 30; ++Index)
	{
		ARREnemy* Robot = Spawner->TrySpawnEnemy();
		Spawned += Robot ? 1 : 0;
		if (ARRHeavy* Heavy = Cast<ARRHeavy>(Robot))
		{
			Heavies.Add(Heavy);
		}
		MaxHeavies = FMath::Max(MaxHeavies, Spawner->GetAliveCountOfClass(ARRHeavy::StaticClass()));
	}
	AddInfo(FString::Printf(TEXT("%d robots spawned, %d heavies"), Spawned, Heavies.Num()));
	TestEqual(TEXT("30 robots spawned"), Spawned, 30);
	TestEqual(TEXT("never more than 5 heavies alive"), MaxHeavies, 5);
	TestEqual(TEXT("exactly 5 heavies spawned"), Heavies.Num(), 5);
	int32 HeavyPicks = 0;
	for (int32 Index = 0; Index < 200; ++Index)
	{
		HeavyPicks += Spawner->PickEnemyClass(Stream) == ARRHeavy::StaticClass() ? 1 : 0;
	}
	TestEqual(TEXT("no heavy drawn at the cap"), HeavyPicks, 0);

	// 3. A destroyed heavy frees a heavy slot.
	if (Heavies.Num() == 5)
	{
		Heavies[0]->Destroy();
		TestEqual(TEXT("4 heavies alive after a destroy"), Spawner->GetAliveCountOfClass(ARRHeavy::StaticClass()), 4);
		for (int32 Index = 0; Index < 200; ++Index)
		{
			HeavyPicks += Spawner->PickEnemyClass(Stream) == ARRHeavy::StaticClass() ? 1 : 0;
		}
		TestTrue(TEXT("heavies drawn again below the cap"), HeavyPicks > 0);
	}

	// 4. Heavy-only list: one more heavy fills the cap, then nothing spawns (no fallback to EnemyClass).
	FSpawnerAccess::EnemyClassOf(Spawner) = ARRChaser::StaticClass();
	FSpawnerAccess::WeightsOf(Spawner) = { Entry(ARRHeavy::StaticClass(), 1.f, 5) };
	TestTrue(TEXT("heavy spawned below the cap"), Cast<ARRHeavy>(Spawner->TrySpawnEnemy()) != nullptr);
	TestNull(TEXT("nothing spawns once every entry is capped"), Spawner->TrySpawnEnemy());
	TestEqual(TEXT("5 heavies alive"), Spawner->GetAliveCountOfClass(ARRHeavy::StaticClass()), 5);

	TArray<TSubclassOf<ARREnemy>> Classes;
	Spawner->GetSpawnableClasses(Classes);
	TestTrue(TEXT("heavy listed as spawnable"), Classes.Contains(ARRHeavy::StaticClass()));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
