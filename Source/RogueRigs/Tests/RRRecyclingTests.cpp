// Rogue Rigs - automation tests of robot recycling (RogueRigs.Spawner.Recycle*): far off-screen robots removed after the delay without death, kill or explosion, slots and class caps freed, timer reset on screen; a miner running for 3 min 30 keeps getting spawns in its front sector.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Algo/AllOf.h"
#include "Algo/Reverse.h"
#include "Enemies/RRChaser.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRHeavy.h"
#include "Enemies/RRKamikaze.h"
#include "Enemies/RRShooter.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RRGameplayTags.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRRecyclingTests
{
	/** Miner cell on arena_m1_fine (open floor; fine cell of the legacy (40, 40)), as in the spawner tests. */
	const FIntPoint RecycleMinerCell = RRTestHarness::FineCell(FIntPoint(40, 40));

	/** Half extent (uu) of the fixed view rectangle of the unit test, centered on the miner. */
	const FVector2D RecycleViewHalfExtent(1600.f, 1000.f);

	/** Recycling defaults (enemies-index, owner 2026-10-06 fourth test, provisional). */
	constexpr float RecycleDistanceDefault = 1800.f;
	constexpr float RecycleDelayDefault = 3.f;
	constexpr float RecycleMarginDefault = 0.f;

	/** World with the fine grid, an unarmed invulnerable miner and a native spawner (chaser, kamikaze, shooter, heavy capped at HeavyCap). */
	struct FRecycleSetup
	{
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARREnemySpawner* Spawner = nullptr;
	};

	FRRWeightedEnemyClass RecycleEntry(TSubclassOf<ARREnemy> Class, float Weight, int32 MaxAlive = 0)
	{
		FRRWeightedEnemyClass Entry;
		Entry.EnemyClass = Class;
		Entry.Weight = Weight;
		Entry.MaxAlive = MaxAlive;
		return Entry;
	}

	bool MakeRecycleSetup(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, int32 HeavyCap, FRecycleSetup& Out)
	{
		using namespace RRTestAccess;
		Out.Grid = RRTestHarness::SpawnFineGrid(Test, TestWorld);
		Out.Miner = Out.Grid ? RRTestHarness::SpawnMiner(Test, TestWorld, Out.Grid, RecycleMinerCell) : nullptr;
		if (!Out.Miner)
		{
			return false;
		}
		for (ARRWeapon* Weapon : Out.Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		Out.Miner->GetAbilitySystemComponent()->AddLooseGameplayTag(RRGameplayTags::State_Invulnerable);
		Out.Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
		if (!Test.TestNotNull(TEXT("spawner spawned"), Out.Spawner))
		{
			return false;
		}
		FSpawnerAccess::WeightsOf(Out.Spawner) = { RecycleEntry(ARRChaser::StaticClass(), 65.f), RecycleEntry(ARRKamikaze::StaticClass(), 12.f),
			RecycleEntry(ARRShooter::StaticClass(), 15.f), RecycleEntry(ARRHeavy::StaticClass(), 8.f, HeavyCap) };
		return true;
	}

	/** Freezes Robot at MinerLocation + Offset (same height): no behavior tick, no movement, so only the test moves it. */
	void PlaceFrozen(ARREnemy* Robot, const FVector& MinerLocation, const FVector2D& Offset)
	{
		Robot->SetActorTickEnabled(false);
		Robot->GetCharacterMovement()->SetComponentTickEnabled(false);
		Robot->GetEnemyMovement()->SetMovementEnabled(false);
		Robot->GetEnemyMovement()->SetComponentTickEnabled(false);
		Robot->SetActorLocation(FVector(MinerLocation.X + Offset.X, MinerLocation.Y + Offset.Y, Robot->GetActorLocation().Z), false, nullptr, ETeleportType::TeleportPhysics);
	}

	/** True when Robot is gone or being destroyed. */
	bool IsGone(const TWeakObjectPtr<ARREnemy>& Robot)
	{
		return !Robot.IsValid() || Robot->IsActorBeingDestroyed();
	}

	/** Walkable for the runner path: floor or spawn cells. */
	bool IsRunnerFloor(const ARRArenaGrid* Grid, const FIntPoint& Cell)
	{
		const ERRCellType Type = Grid->GetCellType(Cell);
		return Type == ERRCellType::Floor || Type == ERRCellType::Spawn;
	}

	/** Shortest 8-neighbour path of floor cells (no corner cutting) from Start to Goal, Start excluded; empty when unreachable. */
	TArray<FIntPoint> FindRunnerPath(const ARRArenaGrid* Grid, const FIntPoint& Start, const FIntPoint& Goal, int32 Width, int32 Height)
	{
		TArray<int32> Parent;
		Parent.Init(INDEX_NONE, Width * Height);
		auto IndexOf = [Width](const FIntPoint& Cell) { return Cell.Y * Width + Cell.X; };
		TArray<FIntPoint> Queue;
		Queue.Add(Start);
		Parent[IndexOf(Start)] = IndexOf(Start);
		for (int32 Head = 0; Head < Queue.Num(); ++Head)
		{
			const FIntPoint Cell = Queue[Head];
			if (Cell == Goal)
			{
				break;
			}
			for (int32 DY = -1; DY <= 1; ++DY)
			{
				for (int32 DX = -1; DX <= 1; ++DX)
				{
					const FIntPoint Next(Cell.X + DX, Cell.Y + DY);
					if ((DX == 0 && DY == 0) || Next.X < 0 || Next.Y < 0 || Next.X >= Width || Next.Y >= Height
						|| Parent[IndexOf(Next)] != INDEX_NONE || !IsRunnerFloor(Grid, Next)
						|| (DX != 0 && DY != 0 && (!IsRunnerFloor(Grid, FIntPoint(Cell.X + DX, Cell.Y)) || !IsRunnerFloor(Grid, FIntPoint(Cell.X, Cell.Y + DY)))))
					{
						continue;
					}
					Parent[IndexOf(Next)] = IndexOf(Cell);
					Queue.Add(Next);
				}
			}
		}
		TArray<FIntPoint> Path;
		if (Parent[IndexOf(Goal)] == INDEX_NONE)
		{
			return Path;
		}
		for (int32 Index = IndexOf(Goal); Index != IndexOf(Start); Index = Parent[Index])
		{
			Path.Add(FIntPoint(Index % Width, Index / Width));
		}
		Algo::Reverse(Path);
		return Path;
	}

	/** Spawn counts of one 30 s window of the runner test. */
	struct FRunnerWindow
	{
		int32 Spawned = 0;
		int32 Front = 0;
		int32 MaxAlive = 0;
		int32 Recycled = 0;
	};

	/**
	 * Runs the miner at 600 uu/s along floor paths between random far waypoints for Seconds with the default director (miner
	 * camera view, run heading measured); fills one FRunnerWindow per 30 s. Recycling on or off.
	 */
	bool RunRunner(FAutomationTestBase& Test, const TCHAR* WorldName, bool bRecycle, float Seconds, TArray<FRunnerWindow>& OutWindows)
	{
		using namespace RRTestAccess;
		RRTestHarness::FTestWorld TestWorld(WorldName);
		FRecycleSetup Setup;
		if (!MakeRecycleSetup(Test, TestWorld, 5, Setup))
		{
			return false;
		}
		FSpawnerAccess::RecycleEnabledOf(Setup.Spawner) = bRecycle;
		Setup.Spawner->SeedRandomStream(20261006);
		Setup.Spawner->SetSpawningEnabled(true);
		const int32 Width = FGridAccess::Width(Setup.Grid);
		const int32 Height = FGridAccess::Height(Setup.Grid);
		const float MinerZ = Setup.Miner->GetActorLocation().Z;

		// Every floor cell reachable from the start: waypoint pool.
		TArray<FIntPoint> Reachable;
		{
			TBitArray<> Seen(false, Width * Height);
			TArray<FIntPoint> Queue = { RecycleMinerCell };
			Seen[RecycleMinerCell.Y * Width + RecycleMinerCell.X] = true;
			for (int32 Head = 0; Head < Queue.Num(); ++Head)
			{
				const FIntPoint Cell = Queue[Head];
				for (const FIntPoint Step : { FIntPoint(1, 0), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(0, -1) })
				{
					const FIntPoint Next = Cell + Step;
					if (Next.X >= 0 && Next.Y >= 0 && Next.X < Width && Next.Y < Height && !Seen[Next.Y * Width + Next.X] && IsRunnerFloor(Setup.Grid, Next))
					{
						Seen[Next.Y * Width + Next.X] = true;
						Queue.Add(Next);
					}
				}
			}
			Reachable = MoveTemp(Queue);
		}
		FRandomStream RouteStream(4242);
		FIntPoint Current = RecycleMinerCell;
		TArray<FIntPoint> Path;
		int32 PathCursor = 0;
		auto NextRoute = [&]()
		{
			for (int32 Try = 0; Try < 50; ++Try)
			{
				const FIntPoint Goal = Reachable[RouteStream.RandHelper(Reachable.Num())];
				if (FVector2D::Distance(FVector2D(Goal), FVector2D(Current)) < 60.f)
				{
					continue;
				}
				Path = FindRunnerPath(Setup.Grid, Current, Goal, Width, Height);
				PathCursor = 0;
				if (Path.Num() > 0)
				{
					return;
				}
			}
		};

		constexpr float RunSpeed = 600.f;
		const int32 Frames = FMath::RoundToInt(Seconds / RRTestHarness::FrameSeconds);
		const int32 FramesPerWindow = FMath::RoundToInt(30.f / RRTestHarness::FrameSeconds);
		TSet<TObjectKey<ARREnemy>> Seen;
		OutWindows.Reset();
		for (int32 Frame = 0; Frame < Frames; ++Frame)
		{
			// Move 10 uu along the path (600 uu/s at 60 frames per second).
			float Budget = RunSpeed * RRTestHarness::FrameSeconds;
			FVector Location = Setup.Miner->GetActorLocation();
			while (Budget > 0.f)
			{
				if (PathCursor >= Path.Num())
				{
					NextRoute();
					if (Path.IsEmpty())
					{
						break;
					}
				}
				const FVector Target = Setup.Grid->GetCellCenter(Path[PathCursor].X, Path[PathCursor].Y);
				const FVector2D Delta = FVector2D(Target) - FVector2D(Location);
				const float Length = Delta.Size();
				if (Length <= Budget)
				{
					Location = FVector(Target.X, Target.Y, MinerZ);
					Budget -= Length;
					Current = Path[PathCursor++];
				}
				else
				{
					Location += FVector(Delta / Length * Budget, 0.f);
					Budget = 0.f;
				}
			}
			Setup.Miner->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
			TestWorld.Tick();

			if (Frame % FramesPerWindow == 0)
			{
				OutWindows.AddDefaulted();
			}
			FRunnerWindow& Window = OutWindows.Last();
			const TArray<ARREnemy*> Alive = Setup.Spawner->GetAliveEnemies();
			Window.MaxAlive = FMath::Max(Window.MaxAlive, Alive.Num());
			for (ARREnemy* Robot : Alive)
			{
				bool bAlreadySeen = false;
				Seen.Add(Robot, &bAlreadySeen);
				if (!bAlreadySeen)
				{
					++Window.Spawned;
					Window.Front += Setup.Spawner->GetSpawnSector(FVector2D(Robot->GetActorLocation())) == ERRSpawnSector::Front ? 1 : 0;
				}
			}
			Window.Recycled = Setup.Spawner->GetRecycledCount();
		}
		for (int32 Index = 0; Index < OutWindows.Num(); ++Index)
		{
			const FRunnerWindow& Window = OutWindows[Index];
			Test.AddInfo(FString::Printf(TEXT("%s, %3d-%3d s: %3d spawned, %3d in front, max %3d alive, %4d recycled so far"),
				bRecycle ? TEXT("recycling") : TEXT("no recycling"), Index * 30, Index * 30 + 30, Window.Spawned, Window.Front, Window.MaxAlive, Window.Recycled));
		}
		return true;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerRecycleFarRobotTest, "RogueRigs.Spawner.RecycleFarRobot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerRecycleFarRobotTest::RunTest(const FString& Parameters)
{
	using namespace RRRecyclingTests;
	using namespace RRTestAccess;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRSpawnerRecycleFarRobot"));
	FRecycleSetup Setup;
	if (!MakeRecycleSetup(*this, TestWorld, 1, Setup))
	{
		return false;
	}
	TestTrue(TEXT("recycling on by default"), FSpawnerAccess::RecycleEnabledOf(Setup.Spawner));
	TestEqual(TEXT("default RecycleDistance 1800 uu"), FSpawnerAccess::RecycleDistanceOf(Setup.Spawner), RecycleDistanceDefault);
	TestEqual(TEXT("default RecycleDelay 3 s"), FSpawnerAccess::RecycleDelayOf(Setup.Spawner), RecycleDelayDefault);
	TestEqual(TEXT("default RecycleOffScreenMargin 0 uu (off screen = recyclable)"), FSpawnerAccess::RecycleMarginOf(Setup.Spawner), RecycleMarginDefault);
	Setup.Spawner->SetSpawningEnabled(false);
	const FVector MinerLocation = Setup.Miner->GetActorLocation();
	const FVector2D Center(MinerLocation);
	Setup.Spawner->SetViewRectangleOverride(FBox2D(Center - RecycleViewHalfExtent, Center + RecycleViewHalfExtent));

	ARREnemy* Kamikaze = Setup.Spawner->TrySpawnEnemyOfClass(ARRKamikaze::StaticClass());
	ARREnemy* Shooter = Setup.Spawner->TrySpawnEnemyOfClass(ARRShooter::StaticClass());
	ARREnemy* Heavy = Setup.Spawner->TrySpawnEnemyOfClass(ARRHeavy::StaticClass());
	ARREnemy* Chaser = Setup.Spawner->TrySpawnEnemyOfClass(ARRChaser::StaticClass());
	if (!TestTrue(TEXT("four robots spawned"), Kamikaze && Shooter && Heavy && Chaser))
	{
		return false;
	}
	TestNull(TEXT("heavy cap 1 reached: no second heavy"), Setup.Spawner->TrySpawnEnemyOfClass(ARRHeavy::StaticClass()));
	// Kamikaze 3500 uu ahead (1900 beyond the view), shooter 3000 behind, heavy 3000 aside (2000 beyond): far off screen.
	// Chaser off screen (500 beyond the view edge) but only 1500 uu away, within RecycleDistance: never recycled.
	PlaceFrozen(Kamikaze, MinerLocation, FVector2D(3500.f, 0.f));
	PlaceFrozen(Shooter, MinerLocation, FVector2D(-3000.f, 0.f));
	PlaceFrozen(Heavy, MinerLocation, FVector2D(0.f, 3000.f));
	PlaceFrozen(Chaser, MinerLocation, FVector2D(0.f, -1500.f));
	const TWeakObjectPtr<ARREnemy> KamikazeRef(Kamikaze), ShooterRef(Shooter), HeavyRef(Heavy), ChaserRef(Chaser);
	URRRunFlowSubsystem* RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>();
	URRExplosionSubsystem* Explosions = TestWorld.Get()->GetSubsystem<URRExplosionSubsystem>();
	const int32 KillsBefore = RunFlow ? RunFlow->GetKillCount() : 0;
	bool bSeenDead = false;
	auto TickSeconds = [&](float Seconds)
	{
		const int32 Frames = FMath::RoundToInt(Seconds / RRTestHarness::FrameSeconds);
		for (int32 Frame = 0; Frame < Frames; ++Frame)
		{
			TestWorld.Tick();
			for (const TWeakObjectPtr<ARREnemy>& Robot : { KamikazeRef, ShooterRef, HeavyRef, ChaserRef })
			{
				bSeenDead |= Robot.IsValid() && Robot->IsDead();
			}
			bSeenDead |= Explosions && Explosions->GetPendingCount() > 0;
		}
	};

	TickSeconds(2.f);
	AddInfo(FString::Printf(TEXT("after 2 s: kamikaze %.2f s, heavy %.2f s, chaser %.2f s recyclable"),
		Setup.Spawner->GetRecycleSeconds(Kamikaze), Setup.Spawner->GetRecycleSeconds(Heavy), Setup.Spawner->GetRecycleSeconds(Chaser)));
	TestNearlyEqual(TEXT("kamikaze far off screen counts 2 s"), Setup.Spawner->GetRecycleSeconds(Kamikaze), 2.f, 0.05f);
	TestEqual(TEXT("chaser within RecycleDistance does not count"), Setup.Spawner->GetRecycleSeconds(Chaser), 0.f);
	TestEqual(TEXT("nothing recycled before 3 s"), Setup.Spawner->GetRecycledCount(), 0);

	// The heavy steps on screen for one frame: its timer restarts.
	PlaceFrozen(Heavy, MinerLocation, FVector2D(0.f, 900.f));
	TickSeconds(RRTestHarness::FrameSeconds);
	TestEqual(TEXT("heavy on screen: timer reset"), Setup.Spawner->GetRecycleSeconds(Heavy), 0.f);
	PlaceFrozen(Heavy, MinerLocation, FVector2D(0.f, 3000.f));

	TickSeconds(1.1f);
	TestTrue(TEXT("kamikaze recycled after 3 s"), IsGone(KamikazeRef));
	TestTrue(TEXT("shooter recycled after 3 s"), IsGone(ShooterRef));
	TestFalse(TEXT("heavy kept (timer restarted at 2 s)"), IsGone(HeavyRef));
	TestFalse(TEXT("chaser within RecycleDistance kept"), IsGone(ChaserRef));
	TestEqual(TEXT("two recycled"), Setup.Spawner->GetRecycledCount(), 2);
	TestEqual(TEXT("two alive"), Setup.Spawner->GetAliveCount(), 2);
	TestEqual(TEXT("shooter slot freed (shooter cap count 0)"), Setup.Spawner->GetAliveCountOfClass(ARRShooter::StaticClass()), 0);
	TestEqual(TEXT("kamikaze slot freed"), Setup.Spawner->GetAliveCountOfClass(ARRKamikaze::StaticClass()), 0);

	TickSeconds(2.5f);
	TestTrue(TEXT("heavy recycled 3 s after it left the screen"), IsGone(HeavyRef));
	TestEqual(TEXT("heavy slot freed"), Setup.Spawner->GetAliveCountOfClass(ARRHeavy::StaticClass()), 0);
	TestNotNull(TEXT("heavy cap 1 has room again"), Setup.Spawner->TrySpawnEnemyOfClass(ARRHeavy::StaticClass()));
	TestFalse(TEXT("chaser within RecycleDistance still kept after 5.6 s"), IsGone(ChaserRef));
	TestFalse(TEXT("no recycled robot died, no explosion queued"), bSeenDead);
	TestEqual(TEXT("no kill counted"), RunFlow ? RunFlow->GetKillCount() : 0, KillsBefore);

	// Off: nothing is recycled.
	FSpawnerAccess::RecycleEnabledOf(Setup.Spawner) = false;
	const TWeakObjectPtr<ARREnemy> Far(Setup.Spawner->TrySpawnEnemyOfClass(ARRChaser::StaticClass()));
	if (TestTrue(TEXT("one more chaser"), Far.IsValid()))
	{
		PlaceFrozen(Far.Get(), MinerLocation, FVector2D(-3500.f, 0.f));
		TickSeconds(4.5f);
		TestFalse(TEXT("recycling off: far chaser kept"), IsGone(Far));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerRecycleRunnerTest, "RogueRigs.Spawner.RecycleKeepsSpawnsAhead",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerRecycleRunnerTest::RunTest(const FString& Parameters)
{
	using namespace RRRecyclingTests;

	// The third play test: running nonstop from 2-3 min, no robot popped ahead (150 alive cap filled by robots left behind).
	constexpr float RunSeconds = 210.f;
	TArray<FRunnerWindow> WithRecycling;
	if (!RunRunner(*this, TEXT("RRSpawnerRecycleRunner"), true, RunSeconds, WithRecycling) || !TestEqual(TEXT("seven windows"), WithRecycling.Num(), 7))
	{
		return false;
	}
	TArray<FRunnerWindow> WithoutRecycling;
	if (!RunRunner(*this, TEXT("RRSpawnerRecycleRunnerOff"), false, RunSeconds, WithoutRecycling) || !TestEqual(TEXT("seven windows (off)"), WithoutRecycling.Num(), 7))
	{
		return false;
	}
	TestTrue(TEXT("robots recycled while running"), WithRecycling.Last().Recycled > 0);
	for (int32 Index = 4; Index < 7; ++Index)
	{
		TestTrue(*FString::Printf(TEXT("%d-%d s: at least 20 spawns in the front sector"), Index * 30, Index * 30 + 30), WithRecycling[Index].Front >= 20);
	}
	TestTrue(TEXT("the last 30 s get more front spawns with recycling than without"), WithRecycling.Last().Front > WithoutRecycling.Last().Front);
	TestTrue(TEXT("never above 150 alive"), Algo::AllOf(WithRecycling, [](const FRunnerWindow& Window) { return Window.MaxAlive <= 150; }));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
