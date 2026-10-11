// Rogue Rigs - automation tests of robots from the actor pool (RogueRigs.RobotPool.*, task-091-01): a recycled robot behaves like a fresh one, pending robots count against MaxAlive, a wave is created within 20 frames and reused at once.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Damage/RRGE_IFrames.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRKamikaze.h"
#include "Enemies/RRRobotSeparationSubsystem.h"
#include "Enemies/RRShieldChaser.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RREnemyAttributeSet.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRMovementAttributeSet.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRRobotPoolTests
{
	using namespace RRTestHarness;
	using namespace RRTestAccess;

	/** Miner cell on arena_m1_fine (open floor, as the spawner tests). */
	const FIntPoint MinerCell = FineCell(FIntPoint(40, 40));

	/** World with the fine grid, an invulnerable unarmed miner and a spawner of Class (fixed view rectangle, timers off, recycling off). */
	struct FSetup
	{
		ARRMiner* Miner = nullptr;
		ARREnemySpawner* Spawner = nullptr;
		URRActorPoolSubsystem* Pool = nullptr;
	};

	bool MakeSetup(FAutomationTestBase& Test, const FTestWorld& TestWorld, UClass* Class, int32 MaxAlive, FSetup& Out)
	{
		ARRArenaGrid* Grid = SpawnFineGrid(Test, TestWorld);
		Out.Miner = Grid ? SpawnMiner(Test, TestWorld, Grid, MinerCell) : nullptr;
		Out.Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
		if (!Out.Miner || !Test.TestNotNull(TEXT("actor pool in the game world"), Out.Pool))
		{
			return false;
		}
		Out.Miner->GetAbilitySystemComponent()->AddLooseGameplayTag(RRGameplayTags::State_Invulnerable);
		for (ARRWeapon* Weapon : Out.Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		Out.Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
		if (!Test.TestNotNull(TEXT("spawner spawned"), Out.Spawner))
		{
			return false;
		}
		Test.TestEqual(TEXT("default SpawnMaxFrames 20"), Out.Spawner->GetSpawnMaxFrames(), 20);
		FSpawnerAccess::EnemyClassOf(Out.Spawner) = Class;
		FSpawnerAccess::MaxAliveOf(Out.Spawner) = MaxAlive;
		FSpawnerAccess::RecycleEnabledOf(Out.Spawner) = false;
		Out.Spawner->SetSpawningEnabled(false);
		const FVector MinerLocation = Out.Miner->GetActorLocation();
		const FVector2D Center(MinerLocation.X, MinerLocation.Y);
		Out.Spawner->SetViewRectangleOverride(FBox2D(Center - FVector2D(1600.f, 1000.f), Center + FVector2D(1600.f, 1000.f)));
		Out.Pool->ResetStats();
		return true;
	}

	/** Kills Robot with a flat damage from the miner. */
	void Kill(const FSetup& Setup, ARREnemy* Robot)
	{
		UAbilitySystemComponent* Source = Setup.Miner->GetAbilitySystemComponent();
		FGameplayEffectSpecHandle Spec = Source->MakeOutgoingSpec(URRGE_DamageFlat::StaticClass(), 1.f, Source->MakeEffectContext());
		Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, 100000.f);
		Source->ApplyGameplayEffectSpecToTarget(*Spec.Data, Robot->GetAbilitySystemComponent());
	}

	/** Ticks until Robot is parked idle in the pool (at most MaxTicks); true when it is. */
	bool TickUntilIdle(const FTestWorld& TestWorld, const ARREnemy* Robot, int32 MaxTicks)
	{
		for (int32 Tick = 0; Tick < MaxTicks && !URRActorPoolSubsystem::IsPooledIdle(Robot); ++Tick)
		{
			TestWorld.Tick();
		}
		return URRActorPoolSubsystem::IsPooledIdle(Robot);
	}

	/** Gets one robot from the spawner: queued then delivered by the next tick when the pool has none, at once when it has one. */
	ARREnemy* SpawnOne(const FTestWorld& TestWorld, const FSetup& Setup)
	{
		if (ARREnemy* Robot = Setup.Spawner->TrySpawnEnemy())
		{
			return Robot;
		}
		TestWorld.Tick();
		const TArray<ARREnemy*> Alive = Setup.Spawner->GetAliveEnemies();
		return Alive.Num() > 0 ? Alive.Last() : nullptr;
	}

	/** Per-life state of a robot compared between a fresh and a recycled one. */
	struct FLifeState
	{
		float Health = 0.f, MaxHealth = 0.f, Shield = 0.f, MoveSpeed = 0.f, MaxWalkSpeed = 0.f, AttackPower = 0.f;
		int32 ActiveEffects = 0;
		bool bDead = false, bHidden = false, bCollision = false;
		FVector Scale = FVector::ZeroVector;
		FVector Velocity = FVector::ZeroVector;
		FVector PendingLaunch = FVector::ZeroVector;
		bool bFar = false;
		bool bMovementOn = false;
	};

	FLifeState ReadLife(const ARREnemy* Robot)
	{
		FLifeState State;
		const UAbilitySystemComponent* ASC = Robot->GetAbilitySystemComponent();
		State.Health = ASC->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
		State.MaxHealth = ASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
		State.Shield = ASC->GetNumericAttribute(URRHealthAttributeSet::GetShieldAttribute());
		State.MoveSpeed = ASC->GetNumericAttribute(URRMovementAttributeSet::GetMoveSpeedAttribute());
		State.AttackPower = ASC->GetNumericAttribute(URREnemyAttributeSet::GetAttackPowerAttribute());
		State.ActiveEffects = ASC->GetActiveEffects(FGameplayEffectQuery()).Num();
		State.MaxWalkSpeed = Robot->GetCharacterMovement()->MaxWalkSpeed;
		State.bDead = Robot->IsDead();
		State.bHidden = Robot->IsHidden();
		State.bCollision = Robot->GetActorEnableCollision();
		State.Scale = Robot->GetActorScale3D();
		State.Velocity = Robot->GetVelocity();
		State.PendingLaunch = Robot->GetCharacterMovement()->PendingLaunchVelocity;
		State.bFar = Robot->IsFarMovementActive();
		State.bMovementOn = Robot->GetCharacterMovement()->MovementMode != MOVE_None;
		return State;
	}

	void CompareLife(FAutomationTestBase& Test, const FLifeState& Fresh, const FLifeState& Reused, const TCHAR* Label)
	{
		Test.TestEqual(*FString::Printf(TEXT("%s: health"), Label), Reused.Health, Fresh.Health);
		Test.TestEqual(*FString::Printf(TEXT("%s: max health"), Label), Reused.MaxHealth, Fresh.MaxHealth);
		Test.TestEqual(*FString::Printf(TEXT("%s: shield"), Label), Reused.Shield, Fresh.Shield);
		Test.TestEqual(*FString::Printf(TEXT("%s: move speed"), Label), Reused.MoveSpeed, Fresh.MoveSpeed);
		Test.TestEqual(*FString::Printf(TEXT("%s: max walk speed"), Label), Reused.MaxWalkSpeed, Fresh.MaxWalkSpeed);
		Test.TestEqual(*FString::Printf(TEXT("%s: attack power"), Label), Reused.AttackPower, Fresh.AttackPower);
		Test.TestEqual(*FString::Printf(TEXT("%s: no active effect"), Label), Reused.ActiveEffects, Fresh.ActiveEffects);
		Test.TestEqual(*FString::Printf(TEXT("%s: alive"), Label), Reused.bDead, Fresh.bDead);
		Test.TestEqual(*FString::Printf(TEXT("%s: shown"), Label), Reused.bHidden, Fresh.bHidden);
		Test.TestEqual(*FString::Printf(TEXT("%s: collision"), Label), Reused.bCollision, Fresh.bCollision);
		Test.TestTrue(*FString::Printf(TEXT("%s: death shrink undone (scale %s)"), Label, *Reused.Scale.ToString()), Reused.Scale.Equals(Fresh.Scale));
		Test.TestTrue(*FString::Printf(TEXT("%s: no velocity"), Label), Reused.Velocity.IsNearlyZero());
		Test.TestTrue(*FString::Printf(TEXT("%s: no pending knockback"), Label), Reused.PendingLaunch.IsNearlyZero());
		Test.TestEqual(*FString::Printf(TEXT("%s: far movement off"), Label), Reused.bFar, Fresh.bFar);
		Test.TestEqual(*FString::Printf(TEXT("%s: moving"), Label), Reused.bMovementOn, Fresh.bMovementOn);
	}

	/**
	 * Spawns a robot of the setup class, reads its fresh state, dirties it (i-frames effect, knockback, far movement, damage),
	 * kills it, waits for its release and spawns again: the same actor must come back at once with the fresh state.
	 */
	ARREnemy* RecycleOnce(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FSetup& Setup, const TCHAR* Label, FLifeState& OutFresh,
		TFunctionRef<void(ARREnemy*)> Dirty)
	{
		ARREnemy* Robot = SpawnOne(TestWorld, Setup);
		if (!Test.TestNotNull(*FString::Printf(TEXT("%s: first robot"), Label), Robot))
		{
			return nullptr;
		}
		Test.TestEqual(*FString::Printf(TEXT("%s: first life"), Label), Robot->GetPoolLifeCount(), 1);
		OutFresh = ReadLife(Robot);

		Robot->ApplyKnockback(FVector(500.f, 0.f, 0.f));
		Robot->SetFarMovement(true);
		Dirty(Robot);
		Kill(Setup, Robot);
		Test.TestTrue(*FString::Printf(TEXT("%s: dead"), Label), Robot->IsDead());
		// A lasting effect left from the last life (i-frames would have refused the kill, so it comes after it).
		UAbilitySystemComponent* ASC = Robot->GetAbilitySystemComponent();
		ASC->ApplyGameplayEffectToSelf(GetDefault<URRGE_IFrames>(), 1.f, ASC->MakeEffectContext());
		Test.TestTrue(*FString::Printf(TEXT("%s: an effect is active before the reuse"), Label), ASC->GetActiveEffects(FGameplayEffectQuery()).Num() > 0);
		if (!Test.TestTrue(*FString::Printf(TEXT("%s: released to the pool after the shrink"), Label), TickUntilIdle(TestWorld, Robot, 120)))
		{
			return nullptr;
		}
		Test.TestTrue(*FString::Printf(TEXT("%s: idle robot still counts as dead"), Label), Robot->IsDead());
		Test.TestEqual(*FString::Printf(TEXT("%s: idle robot not alive for the spawner"), Label), Setup.Spawner->GetAliveCount(), 0);
		Test.TestTrue(*FString::Printf(TEXT("%s: idle robot hidden"), Label), Robot->IsHidden());
		Test.TestTrue(*FString::Printf(TEXT("%s: death shrink undone on release"), Label), Robot->GetActorScale3D().Equals(FVector::OneVector));

		ARREnemy* Reused = Setup.Spawner->TrySpawnEnemy();
		Test.TestTrue(*FString::Printf(TEXT("%s: the released robot is reused at once"), Label), Reused == Robot);
		Test.TestEqual(*FString::Printf(TEXT("%s: second life"), Label), Robot->GetPoolLifeCount(), 2);
		Test.TestEqual(*FString::Printf(TEXT("%s: alive again for the spawner"), Label), Setup.Spawner->GetAliveCount(), 1);
		CompareLife(Test, OutFresh, ReadLife(Robot), Label);
		return Reused;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRRobotPoolResetTest, "RogueRigs.RobotPool.RecycledRobotIsFresh",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRRobotPoolResetTest::RunTest(const FString& Parameters)
{
	using namespace RRRobotPoolTests;

	// Chaser: stats, effects, death state, movement, state machine and counters.
	{
		FTestWorld TestWorld(TEXT("RRRobotPoolResetChaser"));
		FSetup Setup;
		if (!MakeSetup(*this, TestWorld, ARRChaser::StaticClass(), 10, Setup))
		{
			return false;
		}
		const URRRobotSeparationSubsystem* Separation = TestWorld.Get()->GetSubsystem<URRRobotSeparationSubsystem>();
		FLifeState Fresh;
		ARREnemy* Robot = RecycleOnce(*this, TestWorld, Setup, TEXT("chaser"), Fresh, [](ARREnemy*) {});
		ARRChaser* Chaser = Cast<ARRChaser>(Robot);
		if (!TestNotNull(TEXT("chaser reused"), Chaser))
		{
			return false;
		}
		TestEqual(TEXT("chaser back in Chase"), Chaser->GetState(), ERRChaserState::Chase);
		TestEqual(TEXT("chaser attack count cleared"), Chaser->GetAttackCount(), 0);
		TestEqual(TEXT("one robot in the separation"), Separation ? Separation->GetRobotCount() : -1, 1);
		const FRRActorPoolStats Stats = Setup.Pool->GetStats();
		TestEqual(TEXT("one robot created"), Stats.TotalCreated, 1);
		TestEqual(TEXT("one robot reused"), Stats.TotalReused, 1);

		// The recycled robot lives normally: it walks and dies again.
		for (int32 Tick = 0; Tick < 30; ++Tick)
		{
			TestWorld.Tick();
		}
		TestTrue(TEXT("recycled chaser walks"), Chaser->GetVelocity().Size2D() > 1.f || Chaser->GetState() != ERRChaserState::Chase);
		Kill(Setup, Chaser);
		TestTrue(TEXT("recycled chaser dies again"), Chaser->IsDead() && TickUntilIdle(TestWorld, Chaser, 120));
	}

	// Kamikaze: fuse, explosion source and flare state (its explosion fires again in the next life).
	{
		FTestWorld TestWorld(TEXT("RRRobotPoolResetKamikaze"));
		FSetup Setup;
		if (!MakeSetup(*this, TestWorld, ARRKamikaze::StaticClass(), 10, Setup))
		{
			return false;
		}
		const URRExplosionSubsystem* Explosions = TestWorld.Get()->GetSubsystem<URRExplosionSubsystem>();
		FLifeState Fresh;
		ARRKamikaze* Kamikaze = Cast<ARRKamikaze>(RecycleOnce(*this, TestWorld, Setup, TEXT("kamikaze"), Fresh, [](ARREnemy*) {}));
		if (!TestNotNull(TEXT("kamikaze reused"), Kamikaze) || !TestNotNull(TEXT("explosion subsystem"), Explosions))
		{
			return false;
		}
		TestEqual(TEXT("kamikaze back in Chase"), Kamikaze->GetState(), ERRKamikazeState::Chase);
		TestFalse(TEXT("kamikaze not detonated"), Kamikaze->HasDetonated());
		TestFalse(TEXT("kamikaze explosion source forgotten"), Explosions->IsSourceSpent(Kamikaze));
		// task-096-01: the first life ended by a kill fuse (flat damage = player side); nothing of it is left.
		TestFalse(TEXT("kamikaze kill fuse cleared"), Kamikaze->IsKillFuseRunning());
		TestEqual(TEXT("kamikaze hit capsule takes shots again"), Kamikaze->GetHitCapsule()->GetCollisionEnabled(), ECollisionEnabled::QueryOnly);
		Kill(Setup, Kamikaze);
		TestTrue(TEXT("killed again: kill fuse running"), Kamikaze->IsKillFuseRunning());
		TestFalse(TEXT("killed again: no explosion before the fuse"), Explosions->IsSourceSpent(Kamikaze));
		for (int32 Tick = 0; Tick < 45; ++Tick)
		{
			TestWorld.Tick();
		}
		TestTrue(TEXT("killed again: its explosion is queued again after the fuse"), Explosions->IsSourceSpent(Kamikaze));
	}

	// Shield chaser: shield back to full after a break, break count cleared.
	{
		FTestWorld TestWorld(TEXT("RRRobotPoolResetShield"));
		FSetup Setup;
		if (!MakeSetup(*this, TestWorld, ARRShieldChaser::StaticClass(), 10, Setup))
		{
			return false;
		}
		FLifeState Fresh;
		ARRShieldChaser* Shield = Cast<ARRShieldChaser>(RecycleOnce(*this, TestWorld, Setup, TEXT("shield chaser"), Fresh, [&Setup](ARREnemy* Robot)
		{
			// Breaks the shell first (a hit equal to the shield).
			UAbilitySystemComponent* Source = Setup.Miner->GetAbilitySystemComponent();
			FGameplayEffectSpecHandle Spec = Source->MakeOutgoingSpec(URRGE_DamageFlat::StaticClass(), 1.f, Source->MakeEffectContext());
			Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, Cast<ARRShieldChaser>(Robot)->GetMaxShield());
			Source->ApplyGameplayEffectSpecToTarget(*Spec.Data, Robot->GetAbilitySystemComponent());
		}));
		if (!TestNotNull(TEXT("shield chaser reused"), Shield))
		{
			return false;
		}
		TestTrue(TEXT("shield chaser has a shield (CSV)"), Shield->GetMaxShield() > 0.f);
		TestEqual(TEXT("shield back to full"), Shield->GetShield(), Shield->GetMaxShield());
		TestEqual(TEXT("shield break count cleared"), Shield->GetShieldBreakCount(), 0);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRRobotPoolPendingCapTest, "RogueRigs.RobotPool.PendingRobotsCountAgainstMaxAlive",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRRobotPoolPendingCapTest::RunTest(const FString& Parameters)
{
	using namespace RRRobotPoolTests;

	FTestWorld TestWorld(TEXT("RRRobotPoolPendingCap"));
	FSetup Setup;
	constexpr int32 Cap = 10;
	if (!MakeSetup(*this, TestWorld, ARRChaser::StaticClass(), Cap, Setup))
	{
		return false;
	}
	// Empty pool: the pack is queued (nothing delivered yet) but takes its slots.
	const TArray<ARREnemy*> Pack = Setup.Spawner->TrySpawnPack();
	TestEqual(TEXT("no robot delivered at once from an empty pool"), Pack.Num(), 0);
	const int32 Pending = Setup.Spawner->GetPendingCount();
	TestTrue(*FString::Printf(TEXT("pack queued (%d pending, 8 to 10)"), Pending), Pending >= 8 && Pending <= Cap);
	for (int32 Try = 0; Try < 2 * Cap; ++Try)
	{
		Setup.Spawner->TrySpawnEnemy();
	}
	TestEqual(TEXT("lone spawns fill only the free slots"), Setup.Spawner->GetOccupiedCount(), Cap);
	TestEqual(TEXT("a wave at the cap requests nothing"), Setup.Spawner->TrySpawnDirectorWave({ 0 }).Num(), 0);
	TestEqual(TEXT("nothing alive before the pool tick"), Setup.Spawner->GetAliveCount(), 0);

	int32 MostAlive = 0;
	for (int32 Tick = 0; Tick < 25; ++Tick)
	{
		TestWorld.Tick();
		Setup.Spawner->TrySpawnEnemy();
		MostAlive = FMath::Max(MostAlive, Setup.Spawner->GetAliveCount());
		TestTrue(*FString::Printf(TEXT("tick %d: alive + pending within the cap"), Tick), Setup.Spawner->GetOccupiedCount() <= Cap);
	}
	TestEqual(TEXT("every pending robot delivered"), Setup.Spawner->GetPendingCount(), 0);
	TestEqual(TEXT("cap reached, never exceeded"), MostAlive, Cap);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRRobotPoolSpreadTest, "RogueRigs.RobotPool.WaveCreatedWithin20Frames",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRRobotPoolSpreadTest::RunTest(const FString& Parameters)
{
	using namespace RRRobotPoolTests;

	FTestWorld TestWorld(TEXT("RRRobotPoolSpread"));
	FSetup Setup;
	constexpr int32 WaveSize = 17; // the largest wave of task-068-01 (17 robots in one frame)
	if (!MakeSetup(*this, TestWorld, ARRChaser::StaticClass(), 100, Setup))
	{
		return false;
	}
	FSpawnerAccess::PackSizeMinOf(Setup.Spawner) = WaveSize;
	FSpawnerAccess::PackSizeMaxOf(Setup.Spawner) = WaveSize;
	Setup.Spawner->TrySpawnPack();
	TestEqual(TEXT("wave queued"), Setup.Spawner->GetPendingCount(), WaveSize);

	int32 Ticks = 0;
	int32 MostPerTick = 0;
	int32 Created = 0;
	while (Setup.Spawner->GetPendingCount() > 0 && Ticks < 40)
	{
		TestWorld.Tick();
		++Ticks;
		const int32 Total = Setup.Pool->GetStats().TotalCreated;
		MostPerTick = FMath::Max(MostPerTick, Total - Created);
		Created = Total;
	}
	TestTrue(*FString::Printf(TEXT("wave delivered within 20 frames (%d)"), Ticks), Ticks <= 20);
	TestEqual(TEXT("at most ceil(17 / 20) = 1 robot created per frame"), MostPerTick, 1);
	TestEqual(TEXT("all robots alive"), Setup.Spawner->GetAliveCount(), WaveSize);
	TestTrue(TEXT("longest wait at most 20 frames"), Setup.Pool->GetStats().LongestWaitFrames <= 20);

	// Same wave from released robots: delivered at once, nothing created.
	for (ARREnemy* Robot : Setup.Spawner->GetAliveEnemies())
	{
		Kill(Setup, Robot);
	}
	for (int32 Tick = 0; Tick < 60 && Setup.Pool->GetStats(ARRChaser::StaticClass()).Idle < WaveSize; ++Tick)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("every dead robot back in the pool"), Setup.Pool->GetStats(ARRChaser::StaticClass()).Idle, WaveSize);
	const int32 CreatedBefore = Setup.Pool->GetStats().TotalCreated;
	const TArray<ARREnemy*> Reused = Setup.Spawner->TrySpawnPack();
	TestEqual(TEXT("reused wave delivered at once"), Reused.Num(), WaveSize);
	TestEqual(TEXT("nothing pending"), Setup.Spawner->GetPendingCount(), 0);
	TestEqual(TEXT("nothing created"), Setup.Pool->GetStats().TotalCreated, CreatedBefore);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
