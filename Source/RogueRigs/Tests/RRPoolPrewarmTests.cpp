// Rogue Rigs - automation tests of the level-start pool prewarm (RogueRigs.PoolPrewarm.*, task-111-01): per-class counts and frame spread, classes absent from the level, MaxAlive caps, prewarmed actors idle and reused by the first spawn, start delay.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Enemies/RRChaser.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRHeavy.h"
#include "Enemies/RRKamikaze.h"
#include "Enemies/RRShieldChaser.h"
#include "Enemies/RRShooter.h"
#include "HAL/IConsoleManager.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "Tests/RRTestPooledActor.h"

namespace RRPoolPrewarmTests
{
	using namespace RRTestHarness;
	using namespace RRTestAccess;

	/** Ticks the world until the pool has nothing pending (at most MaxTicks); records the most creations of Class in one tick. */
	int32 TickPrewarm(const FTestWorld& TestWorld, URRActorPoolSubsystem* Pool, TSubclassOf<AActor> Class, int32 MaxTicks, int32& OutTicks)
	{
		int32 MostPerTick = 0;
		OutTicks = 0;
		while (OutTicks < MaxTicks && Pool->GetStats().Pending > 0)
		{
			TestWorld.Tick();
			++OutTicks;
			MostPerTick = FMath::Max(MostPerTick, Pool->GetStats(Class).CreatedThisFrame);
		}
		return MostPerTick;
	}

	/** Restores the class-default XP pickup of a native robot on scope exit. */
	struct FScopedXPClass
	{
		ARREnemy* Defaults = nullptr;
		TSubclassOf<ARRPickup> Previous;

		FScopedXPClass(UClass* RobotClass, TSubclassOf<ARRPickup> XPClass)
			: Defaults(RobotClass->GetDefaultObject<ARREnemy>())
		{
			Previous = FEnemyAccess::XPPickupClassOf(Defaults);
			FEnemyAccess::XPPickupClassOf(Defaults) = XPClass;
		}

		~FScopedXPClass() { FEnemyAccess::XPPickupClassOf(Defaults) = Previous; }
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPoolPrewarmSpreadTest, "RogueRigs.PoolPrewarm.PoolSpread",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPoolPrewarmSpreadTest::RunTest(const FString& Parameters)
{
	using namespace RRPoolPrewarmTests;
	struct FCase { int32 Count; int32 MaxFrames; };
	const FCase Cases[] = { { 30, 20 }, { 5, 60 }, { 3, 60 }, { 60, 60 }, { 130, 60 } };
	for (const FCase& Case : Cases)
	{
		FTestWorld TestWorld;
		URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
		if (!TestNotNull(TEXT("pool"), Pool))
		{
			return false;
		}
		const TSubclassOf<AActor> Class = ARRTestPooledActor::StaticClass();
		Pool->Prewarm(Class, Case.Count, Case.MaxFrames);
		int32 Ticks = 0;
		const int32 Most = TickPrewarm(TestWorld, Pool, Class, Case.MaxFrames + 5, Ticks);
		const int32 Cap = FMath::DivideAndRoundUp(Case.Count, Case.MaxFrames);
		AddInfo(FString::Printf(TEXT("Prewarm(%d, %d): %d ticks, at most %d per tick"), Case.Count, Case.MaxFrames, Ticks, Most));
		TestTrue(FString::Printf(TEXT("Prewarm(%d, %d): at most ceil = %d per frame"), Case.Count, Case.MaxFrames, Cap), Most <= Cap);
		TestEqual(FString::Printf(TEXT("Prewarm(%d, %d): spread over exactly MaxFrames ticks"), Case.Count, Case.MaxFrames), Ticks, Case.MaxFrames);
		TestEqual(FString::Printf(TEXT("Prewarm(%d, %d): all idle"), Case.Count, Case.MaxFrames), Pool->GetStats(Class).Idle, Case.Count);
	}

	// Two batches with the same MaxFrames together: each keeps its own rate (staggered deadlines interleave them).
	FTestWorld TestWorld;
	URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
	Pool->Prewarm(ARRTestPooledActor::StaticClass(), 30, 20);
	Pool->Prewarm(ARRTestPickupXP::StaticClass(), 30, 20);
	int32 MostA = 0, MostB = 0, Ticks = 0;
	while (Ticks < 25 && Pool->GetStats().Pending > 0)
	{
		TestWorld.Tick();
		++Ticks;
		MostA = FMath::Max(MostA, Pool->GetStats(ARRTestPooledActor::StaticClass()).CreatedThisFrame);
		MostB = FMath::Max(MostB, Pool->GetStats(ARRTestPickupXP::StaticClass()).CreatedThisFrame);
	}
	TestTrue(FString::Printf(TEXT("two 30/20 batches: %d and %d per frame, at most 2 each"), MostA, MostB), MostA <= 2 && MostB <= 2);
	TestEqual(TEXT("two 30/20 batches spread over 20 ticks"), Ticks, 20);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPoolPrewarmSpawnerTest, "RogueRigs.PoolPrewarm.SpawnerOrders",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPoolPrewarmSpawnerTest::RunTest(const FString& Parameters)
{
	using namespace RRPoolPrewarmTests;
	const FScopedXPClass ChaserXP(ARRChaser::StaticClass(), ARRTestPickupXP::StaticClass());
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnFineGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, FineCell(FIntPoint(40, 40))) : nullptr;
	URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	if (!Miner || !TestNotNull(TEXT("pool"), Pool) || !TestNotNull(TEXT("spawner"), Spawner))
	{
		return false;
	}
	Spawner->SetSpawningEnabled(false);
	FSpawnerAccess::RecycleEnabledOf(Spawner) = false;
	const FVector2D Center(Miner->GetActorLocation().X, Miner->GetActorLocation().Y);
	Spawner->SetViewRectangleOverride(FBox2D(Center - FVector2D(1600.f, 1000.f), Center + FVector2D(1600.f, 1000.f)));
	// Default director (every robot class), heavy capped at 3 like BP_EnemySpawner.
	FSpawnerAccess::WeightsOf(Spawner) = { FRRWeightedEnemyClass{ ARRHeavy::StaticClass(), 8.f, 3 } };

	TestEqual(TEXT("chaser: 30"), Spawner->GetPoolPrewarmCount(ARRChaser::StaticClass()), 30);
	TestEqual(TEXT("heavy: min(5, cap 3) = 3"), Spawner->GetPoolPrewarmCount(ARRHeavy::StaticClass()), 3);
	TestEqual(TEXT("kamikaze: 5"), Spawner->GetPoolPrewarmCount(ARRKamikaze::StaticClass()), 5);
	TestEqual(TEXT("shield chaser (director only): 5"), Spawner->GetPoolPrewarmCount(ARRShieldChaser::StaticClass()), 5);
	TestEqual(TEXT("shooter: 5"), Spawner->GetPoolPrewarmCount(ARRShooter::StaticClass()), 5);
	TestEqual(TEXT("XP cubes of the robots: 30"), Spawner->GetPoolPrewarmCount(ARRTestPickupXP::StaticClass()), 30);

	TArray<FRRPoolPrewarmOrder> Orders;
	Spawner->BuildPoolPrewarmOrders(Orders);
	Pool->ResetStats();
	TestTrue(TEXT("prewarm queued"), Spawner->QueuePoolPrewarm());
	TestFalse(TEXT("queued once"), Spawner->QueuePoolPrewarm());
	TMap<UClass*, int32> MostPerTick;
	int32 Ticks = 0;
	while (Ticks < 70 && Pool->GetStats().Pending > 0)
	{
		TestWorld.Tick();
		++Ticks;
		for (const FRRPoolPrewarmOrder& Order : Orders)
		{
			int32& Most = MostPerTick.FindOrAdd(Order.Class.Get());
			Most = FMath::Max(Most, Pool->GetStats(Order.Class).CreatedThisFrame);
		}
	}
	TestEqual(TEXT("prewarm spread over 60 ticks (the longest window)"), Ticks, 60);
	for (const FRRPoolPrewarmOrder& Order : Orders)
	{
		const FRRActorPoolStats Stats = Pool->GetStats(Order.Class);
		const int32 Cap = FMath::DivideAndRoundUp(Order.Count, Order.MaxFrames);
		const FString Name = GetNameSafe(Order.Class.Get());
		AddInfo(FString::Printf(TEXT("%s: %d over %d frames, at most %d per frame"), *Name, Order.Count, Order.MaxFrames, MostPerTick.FindRef(Order.Class.Get())));
		TestEqual(*FString::Printf(TEXT("%s: all idle"), *Name), Stats.Idle, Order.Count);
		TestEqual(*FString::Printf(TEXT("%s: none acquired"), *Name), Stats.Acquired, 0);
		TestTrue(*FString::Printf(TEXT("%s: at most ceil(%d / %d) = %d per frame"), *Name, Order.Count, Order.MaxFrames, Cap), MostPerTick.FindRef(Order.Class.Get()) <= Cap);
	}
	TestEqual(TEXT("prewarmed robots are not alive"), Spawner->GetAliveCount(), 0);
	TestEqual(TEXT("prewarmed robots are not pending"), Spawner->GetPendingCount(), 0);
	TestEqual(TEXT("heavy cap count unaffected"), Spawner->GetAliveCountOfClass(ARRHeavy::StaticClass()), 0);

	// The first spawn reuses a prewarmed chaser at once: nothing created, delivered synchronously, alive.
	const int32 CreatedBefore = Pool->GetStats().TotalCreated;
	ARREnemy* First = Spawner->TrySpawnEnemyOfClass(ARRChaser::StaticClass());
	TestNotNull(TEXT("first spawn delivered at once from the prewarm"), First);
	TestEqual(TEXT("no creation for the first spawn"), Pool->GetStats().TotalCreated, CreatedBefore);
	TestEqual(TEXT("one reuse"), Pool->GetStats(ARRChaser::StaticClass()).TotalReused, 1);
	TestEqual(TEXT("29 chasers left idle"), Pool->GetStats(ARRChaser::StaticClass()).Idle, 29);
	TestEqual(TEXT("one robot alive"), Spawner->GetAliveCount(), 1);
	TestFalse(TEXT("the delivered robot is not pooled idle"), URRActorPoolSubsystem::IsPooledIdle(First));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPoolPrewarmAbsentTest, "RogueRigs.PoolPrewarm.AbsentClass",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPoolPrewarmAbsentTest::RunTest(const FString& Parameters)
{
	using namespace RRPoolPrewarmTests;
	FTestWorld TestWorld;
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	if (!TestNotNull(TEXT("spawner"), Spawner))
	{
		return false;
	}
	Spawner->SetSpawningEnabled(false);
	// Director off: the weighted mix decides; the heavy at weight 0 is not part of the level.
	FSpawnerAccess::DirectorOf(Spawner).bEnabled = false;
	FSpawnerAccess::WeightsOf(Spawner) = { FRRWeightedEnemyClass{ ARRChaser::StaticClass(), 1.f, 0 }, FRRWeightedEnemyClass{ ARRHeavy::StaticClass(), 0.f, 3 },
		FRRWeightedEnemyClass{ ARRShooter::StaticClass(), 1.f, 2 } };
	TestEqual(TEXT("chaser in the mix: 30"), Spawner->GetPoolPrewarmCount(ARRChaser::StaticClass()), 30);
	TestEqual(TEXT("heavy weight 0: 0"), Spawner->GetPoolPrewarmCount(ARRHeavy::StaticClass()), 0);
	TestEqual(TEXT("shooter cap 2: 2"), Spawner->GetPoolPrewarmCount(ARRShooter::StaticClass()), 2);
	TestEqual(TEXT("kamikaze not listed: 0"), Spawner->GetPoolPrewarmCount(ARRKamikaze::StaticClass()), 0);

	// Director on without heavy packs or heavy horde members: the heavy is absent even with a positive weight entry.
	FRRWaveDirectorConfig& Config = FSpawnerAccess::DirectorOf(Spawner);
	Config = FRRWaveDirectorConfig::MakeDefault();
	for (FRRDirectorPhase& Phase : Config.Phases)
	{
		Phase.PackWeights.RemoveAll([](const FRRDirectorPackWeight& Weight) { return Weight.PackType.ToString().Contains(TEXT("Heav")); });
	}
	auto NoHeavy = [](FRRDirectorHorde& Horde) { Horde.Members.RemoveAll([](const FRRDirectorPackMember& Member) { return Member.RobotClass == ARRHeavy::StaticClass(); }); };
	for (FRRDirectorHorde& Horde : Config.Hordes)
	{
		NoHeavy(Horde);
	}
	NoHeavy(Config.Overtime.Horde);
	FSpawnerAccess::WeightsOf(Spawner) = { FRRWeightedEnemyClass{ ARRHeavy::StaticClass(), 8.f, 3 } };
	TestEqual(TEXT("director without heavies: 0 heavies"), Spawner->GetPoolPrewarmCount(ARRHeavy::StaticClass()), 0);
	TestEqual(TEXT("director without heavies: chasers still 30"), Spawner->GetPoolPrewarmCount(ARRChaser::StaticClass()), 30);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPoolPrewarmBlueprintTest, "RogueRigs.PoolPrewarm.BlueprintSpawner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPoolPrewarmBlueprintTest::RunTest(const FString& Parameters)
{
	using namespace RRPoolPrewarmTests;
	UClass* SpawnerClass = LoadTestClass<ARREnemySpawner>(*this, TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_EnemySpawner.BP_EnemySpawner_C"));
	const ARREnemySpawner* Defaults = SpawnerClass ? SpawnerClass->GetDefaultObject<ARREnemySpawner>() : nullptr;
	if (!TestNotNull(TEXT("BP_EnemySpawner defaults"), Defaults))
	{
		return false;
	}
	TArray<TSubclassOf<ARREnemy>> Classes;
	Defaults->GetSpawnableClasses(Classes);
	TSubclassOf<AActor> XPClass;
	for (const TSubclassOf<ARREnemy>& Class : Classes)
	{
		int32 Expected = 5;
		if (Class->IsChildOf(ARRHeavy::StaticClass()))
		{
			Expected = 3; // MaxAlive 3 on the Blueprint entry
		}
		else if (Class->IsChildOf(ARRChaser::StaticClass()) && !Class->IsChildOf(ARRShieldChaser::StaticClass()))
		{
			Expected = 30;
			XPClass = Class->GetDefaultObject<ARREnemy>()->GetXPPickupClass();
		}
		const int32 Count = Defaults->GetPoolPrewarmCount(Class);
		AddInfo(FString::Printf(TEXT("%s: %d"), *Class->GetName(), Count));
		TestEqual(*FString::Printf(TEXT("%s prewarm"), *Class->GetName()), Count, Expected);
	}
	if (TestNotNull(TEXT("BP_Chaser drops an XP class"), XPClass.Get()))
	{
		TestEqual(TEXT("XP cubes: 30"), Defaults->GetPoolPrewarmCount(XPClass), 30);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPoolPrewarmStartTest, "RogueRigs.PoolPrewarm.StartDelay",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPoolPrewarmStartTest::RunTest(const FString& Parameters)
{
	using namespace RRPoolPrewarmTests;
	FTestWorld TestWorld;
	IConsoleVariable* Prewarm = IConsoleManager::Get().FindConsoleVariable(TEXT("rr.Pool.Prewarm"));
	if (!TestNotNull(TEXT("rr.Pool.Prewarm exists"), Prewarm))
	{
		return false;
	}
	TestEqual(TEXT("the harness turns the automatic prewarm off"), Prewarm->GetInt(), 0);
	Prewarm->Set(1, ECVF_SetByCode);
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	Spawner->SetSpawningEnabled(false);
	FSpawnerAccess::RecycleEnabledOf(Spawner) = false;
	TestFalse(TEXT("nothing queued at BeginPlay (loading frame)"), Spawner->HasQueuedPoolPrewarm());
	TestWorld.Tick();
	TestFalse(TEXT("nothing queued on the first tick"), Spawner->HasQueuedPoolPrewarm());
	TestWorld.Tick();
	TestTrue(TEXT("queued on the second tick (PrewarmStartDelayFrames 1)"), Spawner->HasQueuedPoolPrewarm());
	URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
	TestTrue(TEXT("creations started"), Pool && Pool->GetStats().PoolSize > 0);
	Prewarm->Set(0, ECVF_SetByCode);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
