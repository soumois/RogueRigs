// Rogue Rigs - automation tests of robot recycling (RogueRigs.Spawner.Recycle*): far off-screen robots relocated into the spawn band after the seen (3 s) or unseen (6 s) delay, same robot, class and health, state reset, no death, kill, explosion or new slot; a miner running for 3 min 30 keeps getting robots in its front sector.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Algo/AllOf.h"
#include "Algo/AnyOf.h"
#include "Algo/Reverse.h"
#include "Enemies/RRChaser.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRHeavy.h"
#include "Enemies/RRKamikaze.h"
#include "Enemies/RRShooter.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "GAS/RRHealthAttributeSet.h"
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
	constexpr float RecycleUnseenDelayDefault = 6.f;

	/** Puts a heavy in a given state (protected EnterState). */
	struct FHeavyStateAccess : public ARRHeavy
	{
		static void Enter(ARRHeavy* Heavy, ERRHeavyState NewState) { (Heavy->*(&FHeavyStateAccess::EnterState))(NewState); }
	};

	/** Puts a kamikaze in a given state (protected EnterState). */
	struct FKamikazeStateAccess : public ARRKamikaze
	{
		static void Enter(ARRKamikaze* Kamikaze, ERRKamikazeState NewState) { (Kamikaze->*(&FKamikazeStateAccess::EnterState))(NewState); }
	};

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
		Out.Spawner->SetSpawnMaxFrames(0); // task-091-01: robots delivered at once (these checks read spawn results in the same frame); spread covered by RogueRigs.RobotPool.
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

	/** Arrival counts of one 30 s window of the runner test (arrival = a new robot or a relocated one). */
	struct FRunnerWindow
	{
		int32 Spawned = 0;
		int32 Relocated = 0;
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
		// Last location of each robot: a jump of more than RelocationJump in one frame is a relocation (robots walk 10 uu a frame at most).
		constexpr double RelocationJump = 500.0;
		TMap<TObjectKey<ARREnemy>, FVector2D> LastLocations;
		TMap<TObjectKey<ARREnemy>, int32> LastLives;
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
				const FVector2D RobotLocation(Robot->GetActorLocation());
				// A robot reused from the actor pool (task-091-01) is the same actor in a new life: a new spawn, not a relocation.
				const int32* LastLife = LastLives.Find(Robot);
				FVector2D* Last = LastLife && *LastLife == Robot->GetPoolLifeCount() ? LastLocations.Find(Robot) : nullptr;
				const bool bNew = !Last;
				const bool bRelocated = Last && FVector2D::Distance(*Last, RobotLocation) > RelocationJump;
				if (bNew || bRelocated)
				{
					Window.Spawned += bNew ? 1 : 0;
					Window.Relocated += bRelocated ? 1 : 0;
					Window.Front += Setup.Spawner->GetSpawnSector(RobotLocation) == ERRSpawnSector::Front ? 1 : 0;
				}
				LastLocations.Add(Robot, RobotLocation);
				LastLives.Add(Robot, Robot->GetPoolLifeCount());
			}
			Window.Recycled = Setup.Spawner->GetRecycledCount();
		}
		for (int32 Index = 0; Index < OutWindows.Num(); ++Index)
		{
			const FRunnerWindow& Window = OutWindows[Index];
			Test.AddInfo(FString::Printf(TEXT("%s, %3d-%3d s: %3d spawned, %3d relocated, %3d arrivals in front, max %3d alive, %4d recycled so far"),
				bRecycle ? TEXT("recycling") : TEXT("no recycling"), Index * 30, Index * 30 + 30, Window.Spawned, Window.Relocated, Window.Front, Window.MaxAlive, Window.Recycled));
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
	TestEqual(TEXT("default RecycleDelay 3 s (seen once)"), FSpawnerAccess::RecycleDelayOf(Setup.Spawner), RecycleDelayDefault);
	TestEqual(TEXT("default RecycleUnseenDelay 6 s (never seen)"), FSpawnerAccess::RecycleUnseenDelayOf(Setup.Spawner), RecycleUnseenDelayDefault);
	TestEqual(TEXT("default RecycleOffScreenMargin 0 uu (off screen = recyclable)"), FSpawnerAccess::RecycleMarginOf(Setup.Spawner), RecycleMarginDefault);
	Setup.Spawner->SetSpawningEnabled(false);
	const FVector MinerLocation = Setup.Miner->GetActorLocation();
	const FVector2D Center(MinerLocation);
	const FBox2D View(Center - RecycleViewHalfExtent, Center + RecycleViewHalfExtent);
	Setup.Spawner->SetViewRectangleOverride(View);
	const float BandMin = FSpawnerAccess::BandMinOf(Setup.Spawner);
	const float BandMax = FSpawnerAccess::BandMaxOf(Setup.Spawner);

	ARRKamikaze* Kamikaze = Cast<ARRKamikaze>(Setup.Spawner->TrySpawnEnemyOfClass(ARRKamikaze::StaticClass()));
	ARREnemy* Shooter = Setup.Spawner->TrySpawnEnemyOfClass(ARRShooter::StaticClass());
	ARRHeavy* Heavy = Cast<ARRHeavy>(Setup.Spawner->TrySpawnEnemyOfClass(ARRHeavy::StaticClass()));
	ARREnemy* Chaser = Setup.Spawner->TrySpawnEnemyOfClass(ARRChaser::StaticClass());
	ARREnemy* Unseen = Setup.Spawner->TrySpawnEnemyOfClass(ARRChaser::StaticClass());
	if (!TestTrue(TEXT("five robots spawned"), Kamikaze && Shooter && Heavy && Chaser && Unseen))
	{
		return false;
	}
	TestNull(TEXT("heavy cap 1 reached: no second heavy"), Setup.Spawner->TrySpawnEnemyOfClass(ARRHeavy::StaticClass()));
	const TWeakObjectPtr<ARREnemy> KamikazeRef(Kamikaze), ShooterRef(Shooter), HeavyRef(Heavy), ChaserRef(Chaser), UnseenRef(Unseen);
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
			for (const TWeakObjectPtr<ARREnemy>& Robot : { KamikazeRef, ShooterRef, HeavyRef, ChaserRef, UnseenRef })
			{
				bSeenDead |= Robot.IsValid() && Robot->IsDead();
			}
			bSeenDead |= Explosions && Explosions->GetPendingCount() > 0;
		}
	};
	auto InBand = [&View, BandMin, BandMax](const ARREnemy* Robot)
	{
		const float Distance = ARREnemySpawner::GetDistanceToRectangle(View, FVector2D(Robot->GetActorLocation()));
		return Distance >= BandMin - 1.f && Distance <= BandMax + 1.f;
	};

	// Kamikaze, shooter, heavy and chaser are on screen for one frame (seen once); the unseen chaser never is.
	PlaceFrozen(Kamikaze, MinerLocation, FVector2D(300.f, 0.f));
	PlaceFrozen(Shooter, MinerLocation, FVector2D(-300.f, 0.f));
	PlaceFrozen(Heavy, MinerLocation, FVector2D(0.f, 300.f));
	PlaceFrozen(Chaser, MinerLocation, FVector2D(0.f, -300.f));
	PlaceFrozen(Unseen, MinerLocation, FVector2D(2500.f, 2500.f));
	TickSeconds(RRTestHarness::FrameSeconds);
	TestTrue(TEXT("kamikaze seen on screen"), Setup.Spawner->HasBeenOnScreen(Kamikaze));
	TestFalse(TEXT("far chaser never seen"), Setup.Spawner->HasBeenOnScreen(Unseen));

	// Wounded, armed kamikaze and telegraphing heavy: health kept, state reset by the relocation.
	const float KamikazeHealth = Kamikaze->GetHealthSet()->GetMaxHealth() * 0.5f;
	Kamikaze->GetAbilitySystemComponent()->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), KamikazeHealth);
	FKamikazeStateAccess::Enter(Kamikaze, ERRKamikazeState::Armed);
	FHeavyStateAccess::Enter(Heavy, ERRHeavyState::Telegraph);

	// Kamikaze 3500 uu ahead (1900 beyond the view), shooter 3000 behind, heavy 3000 aside (2000 beyond): far off screen.
	// Chaser off screen (500 beyond the view edge) but only 1500 uu away, within RecycleDistance: never relocated.
	PlaceFrozen(Kamikaze, MinerLocation, FVector2D(3500.f, 0.f));
	PlaceFrozen(Shooter, MinerLocation, FVector2D(-3000.f, 0.f));
	PlaceFrozen(Heavy, MinerLocation, FVector2D(0.f, 3000.f));
	PlaceFrozen(Chaser, MinerLocation, FVector2D(0.f, -1500.f));
	const FVector ChaserLocation = Chaser->GetActorLocation();
	const FVector UnseenLocation = Unseen->GetActorLocation();

	TickSeconds(2.f);
	AddInfo(FString::Printf(TEXT("after 2 s: kamikaze %.2f s, heavy %.2f s, chaser %.2f s, unseen %.2f s off screen"),
		Setup.Spawner->GetRecycleSeconds(Kamikaze), Setup.Spawner->GetRecycleSeconds(Heavy), Setup.Spawner->GetRecycleSeconds(Chaser),
		Setup.Spawner->GetRecycleSeconds(Unseen)));
	TestNearlyEqual(TEXT("kamikaze off screen counts 2 s"), Setup.Spawner->GetRecycleSeconds(Kamikaze), 2.f, 0.05f);
	TestNearlyEqual(TEXT("chaser within RecycleDistance counts its off-screen time too"), Setup.Spawner->GetRecycleSeconds(Chaser), 2.f, 0.05f);
	TestEqual(TEXT("nothing recycled before 3 s"), Setup.Spawner->GetRecycledCount(), 0);

	// The heavy steps on screen for one frame: its timer restarts.
	PlaceFrozen(Heavy, MinerLocation, FVector2D(0.f, 900.f));
	TickSeconds(RRTestHarness::FrameSeconds);
	TestEqual(TEXT("heavy on screen: timer reset"), Setup.Spawner->GetRecycleSeconds(Heavy), 0.f);
	PlaceFrozen(Heavy, MinerLocation, FVector2D(0.f, 3000.f));

	TickSeconds(1.1f);
	TestFalse(TEXT("kamikaze kept (relocated, not destroyed)"), IsGone(KamikazeRef));
	TestFalse(TEXT("shooter kept (relocated, not destroyed)"), IsGone(ShooterRef));
	TestTrue(TEXT("kamikaze relocated into the spawn band 3 s after it left the screen"), InBand(Kamikaze));
	TestTrue(TEXT("shooter relocated into the spawn band 3 s after it left the screen"), InBand(Shooter));
	TestEqual(TEXT("kamikaze health kept"), Kamikaze->GetHealthSet()->GetHealth(), KamikazeHealth);
	TestTrue(TEXT("armed kamikaze back to Chase"), Kamikaze->GetState() == ERRKamikazeState::Chase);
	TestFalse(TEXT("relocated kamikaze counts as never seen"), Setup.Spawner->HasBeenOnScreen(Kamikaze));
	TestTrue(TEXT("relocated kamikaze off-screen timer restarted"), Setup.Spawner->GetRecycleSeconds(Kamikaze) < 0.5f);
	TestFalse(TEXT("heavy not relocated yet (timer restarted at 2 s)"), InBand(Heavy));
	TestTrue(TEXT("chaser within RecycleDistance not moved"), Chaser->GetActorLocation().Equals(ChaserLocation, 1.f));
	TestTrue(TEXT("unseen chaser not moved before 6 s"), Unseen->GetActorLocation().Equals(UnseenLocation, 1.f));
	TestEqual(TEXT("two recycled"), Setup.Spawner->GetRecycledCount(), 2);
	TestEqual(TEXT("five alive: no slot freed, no new spawn"), Setup.Spawner->GetAliveCount(), 5);
	TestEqual(TEXT("shooter still counted"), Setup.Spawner->GetAliveCountOfClass(ARRShooter::StaticClass()), 1);
	TestEqual(TEXT("kamikaze still counted"), Setup.Spawner->GetAliveCountOfClass(ARRKamikaze::StaticClass()), 1);

	TickSeconds(2.f);
	TestTrue(TEXT("heavy relocated into the spawn band 3 s after it left the screen"), InBand(Heavy));
	TestTrue(TEXT("telegraphing heavy back to Chase"), Heavy->GetState() == ERRHeavyState::Chase);
	TestTrue(TEXT("unseen chaser not moved at 5.1 s"), Unseen->GetActorLocation().Equals(UnseenLocation, 1.f));
	TestEqual(TEXT("three recycled"), Setup.Spawner->GetRecycledCount(), 3);
	TestNull(TEXT("heavy cap 1 still reached: the relocated heavy keeps its slot"), Setup.Spawner->TrySpawnEnemyOfClass(ARRHeavy::StaticClass()));

	TickSeconds(1.f);
	TestTrue(TEXT("never-seen chaser relocated 6 s after spawning"), InBand(Unseen));
	TestEqual(TEXT("four recycled"), Setup.Spawner->GetRecycledCount(), 4);
	TestTrue(TEXT("chaser within RecycleDistance still not moved after 6.1 s"), Chaser->GetActorLocation().Equals(ChaserLocation, 1.f));
	TestEqual(TEXT("still five alive"), Setup.Spawner->GetAliveCount(), 5);
	TestFalse(TEXT("no recycled robot died, no explosion queued"), bSeenDead);
	TestEqual(TEXT("no kill counted"), RunFlow ? RunFlow->GetKillCount() : 0, KillsBefore);

	// Off: nothing is relocated.
	FSpawnerAccess::RecycleEnabledOf(Setup.Spawner) = false;
	PlaceFrozen(Shooter, MinerLocation, FVector2D(-3500.f, 0.f));
	const FVector FarLocation = Shooter->GetActorLocation();
	TickSeconds(6.5f);
	TestTrue(TEXT("recycling off: far shooter not moved"), Shooter->GetActorLocation().Equals(FarLocation, 1.f));
	TestEqual(TEXT("recycling off: count unchanged"), Setup.Spawner->GetRecycledCount(), 4);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerRecycleRunnerTest, "RogueRigs.Spawner.RecycleKeepsSpawnsAhead",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerRecycleRunnerTest::RunTest(const FString& Parameters)
{
	using namespace RRRecyclingTests;

	// The third play test: running nonstop from 2-3 min, no robot popped ahead (150 alive cap filled by robots left behind).
	// Relocation (task-036-01): robots left behind come back ahead, so arrivals count new and relocated robots.
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
	TestTrue(TEXT("relocations seen as jumps while running"), Algo::AnyOf(WithRecycling, [](const FRunnerWindow& Window) { return Window.Relocated > 0; }));
	TestTrue(TEXT("no relocation without recycling"), Algo::AllOf(WithoutRecycling, [](const FRunnerWindow& Window) { return Window.Relocated == 0; }));
	for (int32 Index = 4; Index < 7; ++Index)
	{
		TestTrue(*FString::Printf(TEXT("%d-%d s: at least 20 arrivals (spawned or relocated) in the front sector"), Index * 30, Index * 30 + 30), WithRecycling[Index].Front >= 20);
	}
	TestTrue(TEXT("the last 30 s get more front arrivals with recycling than without"), WithRecycling.Last().Front > WithoutRecycling.Last().Front);
	TestTrue(TEXT("never above 150 alive"), Algo::AllOf(WithRecycling, [](const FRunnerWindow& Window) { return Window.MaxAlive <= 150; }));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
