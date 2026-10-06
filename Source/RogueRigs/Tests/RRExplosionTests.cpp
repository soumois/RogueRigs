// Rogue Rigs - automation tests of the explosion system (RogueRigs.Explosions.*): blocks in the radius, characters hit once, kamikaze reach (312 uu), chain delay, single explosion per source, miner no-one-shot.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Enemies/RRChaser.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRExplosionTests
{
	using namespace RRTestHarness;

	/** Health the tests give the miner (miner gdd base value). */
	constexpr float TestMaxHealth = 100.f;

	/** Explosion subsystem of the test world, with a recorder bound to OnExplosion. */
	URRExplosionSubsystem* GetSubsystem(FAutomationTestBase& Test, const FTestWorld& TestWorld, URRTestEventRecorder* Recorder)
	{
		URRExplosionSubsystem* Subsystem = TestWorld.Get()->GetSubsystem<URRExplosionSubsystem>();
		if (Test.TestNotNull(TEXT("explosion subsystem exists"), Subsystem) && Recorder)
		{
			Subsystem->OnExplosion.AddDynamic(Recorder, &URRTestEventRecorder::HandleExplosion);
		}
		return Subsystem;
	}

	/** Current value of a health attribute on Actor's ASC. */
	float Attribute(UAbilitySystemComponent* ASC, const FGameplayAttribute& Attr)
	{
		return ASC->GetNumericAttribute(Attr);
	}

	/** Spawns BP_Miner on Cell with its weapons stopped and Health = MaxHealth = TestMaxHealth. */
	ARRMiner* SpawnTestMiner(FAutomationTestBase& Test, const FTestWorld& TestWorld, const ARRArenaGrid* Grid, FIntPoint Cell)
	{
		ARRMiner* Miner = SpawnMiner(Test, TestWorld, Grid, Cell);
		if (!Miner)
		{
			return nullptr;
		}
		for (ARRWeapon* Weapon : Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		UAbilitySystemComponent* ASC = Miner->GetAbilitySystemComponent();
		ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), TestMaxHealth);
		ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), TestMaxHealth);
		return Miner;
	}

	/** Ticks until the queue is empty (at most MaxFrames); returns the frames ticked. */
	int32 TickUntilIdle(const FTestWorld& TestWorld, const URRExplosionSubsystem* Subsystem, int32 MaxFrames = 120)
	{
		int32 Frames = 0;
		while (Subsystem->GetPendingCount() > 0 && Frames < MaxFrames)
		{
			TestWorld.Tick();
			++Frames;
		}
		return Frames;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExplosionBlocksInRadiusTest, "RogueRigs.Explosions.BlocksInRadius",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExplosionBlocksInRadiusTest::RunTest(const FString& Parameters)
{
	using namespace RRExplosionTests;

	FTestWorld TestWorld(TEXT("RRExplosionBlocksInRadius"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, Recorder.Get()) : nullptr;
	if (!Subsystem)
	{
		return false;
	}
	FRRExplosionStatsRow Stats;
	if (!TestTrue(TEXT("Barrel row loads"), Subsystem->GetExplosionStats(TEXT("Barrel"), Stats)))
	{
		return false;
	}
	TestEqual(TEXT("Barrel radius 350"), Stats.Radius, 350.f);
	TestEqual(TEXT("Barrel damage 40"), Stats.Damage, 40.f);
	TestTrue(TEXT("Barrel terrain source"), Stats.TerrainSource == ERRTerrainDamageSource::Barrel);
	FRRExplosionStatsRow KamikazeStats;
	TestTrue(TEXT("Kamikaze row loads"), Subsystem->GetExplosionStats(TEXT("Kamikaze"), KamikazeStats));
	TestEqual(TEXT("Kamikaze radius 312"), KamikazeStats.Radius, 312.f);
	TestEqual(TEXT("Kamikaze damage 30"), KamikazeStats.Damage, 30.f);

	// Blocks around the floor cell west of the test rock, before the explosion.
	const FIntPoint CenterCell = RockWestFloorCell;
	const FVector Center = Grid->GetCellCenter(CenterCell.X, CenterCell.Y);
	TArray<FIntPoint> Inside;
	TArray<FIntPoint> Outside;
	for (int32 Y = CenterCell.Y - 4; Y <= CenterCell.Y + 4; ++Y)
	{
		for (int32 X = CenterCell.X - 4; X <= CenterCell.X + 4; ++X)
		{
			const FIntPoint Cell(X, Y);
			if (Grid->IsBlockCell(Cell))
			{
				const float Distance = FVector::Dist2D(Grid->GetCellCenter(X, Y), Center);
				(Distance <= Stats.Radius ? Inside : Outside).Add(Cell);
			}
		}
	}
	TestTrue(TEXT("blocks inside the radius"), Inside.Num() > 0);
	TestTrue(TEXT("blocks outside the radius"), Outside.Num() > 0);
	AddInfo(FString::Printf(TEXT("blocks inside %d, outside %d"), Inside.Num(), Outside.Num()));

	TestTrue(TEXT("explosion queued"), Subsystem->QueueExplosion(Center, TEXT("Barrel"), nullptr, nullptr));
	TestTrue(TEXT("not synchronous: rock still there"), Grid->IsBlockCell(RockCell));
	TestEqual(TEXT("one pending"), Subsystem->GetPendingCount(), 1);

	TestWorld.Tick();
	TestEqual(TEXT("resolved on the next tick"), Subsystem->GetPendingCount(), 0);
	TestEqual(TEXT("OnExplosion once"), Recorder->GetExplosionRows().Num(), 1);
	if (Recorder->GetExplosionRows().Num() == 1)
	{
		TestEqual(TEXT("OnExplosion row"), Recorder->GetExplosionRows()[0], FName(TEXT("Barrel")));
		TestTrue(TEXT("OnExplosion center"), Recorder->GetExplosionCenters()[0].Equals(Center));
	}
	for (const FIntPoint& Cell : Inside)
	{
		TestFalse(FString::Printf(TEXT("block (%d, %d) inside destroyed"), Cell.X, Cell.Y), Grid->IsBlockCell(Cell));
	}
	for (const FIntPoint& Cell : Outside)
	{
		TestTrue(FString::Printf(TEXT("block (%d, %d) outside kept"), Cell.X, Cell.Y), Grid->IsBlockCell(Cell));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExplosionDamagesOnceTest, "RogueRigs.Explosions.DamagesCharactersOnce",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExplosionDamagesOnceTest::RunTest(const FString& Parameters)
{
	using namespace RRExplosionTests;

	FTestWorld TestWorld(TEXT("RRExplosionDamagesOnce"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnTestMiner(*this, TestWorld, Grid, RockWestFloorCell) : nullptr;
	URRExplosionSubsystem* Subsystem = Miner ? GetSubsystem(*this, TestWorld, nullptr) : nullptr;
	if (!Subsystem)
	{
		return false;
	}
	const FVector Center = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y);
	ARRChaser* Near = SpawnStillChaser(*this, TestWorld, Center + FVector(0.f, 200.f, 0.f));
	ARRChaser* Far = SpawnStillChaser(*this, TestWorld, Center + FVector(0.f, 600.f, 0.f));
	if (!TestNotNull(TEXT("still chasers spawned"), Near) || !TestNotNull(TEXT("far still chaser"), Far))
	{
		return false;
	}
	UAbilitySystemComponent* MinerASC = Miner->GetAbilitySystemComponent();
	UAbilitySystemComponent* NearASC = Near->GetAbilitySystemComponent();
	UAbilitySystemComponent* FarASC = Far->GetAbilitySystemComponent();
	const float NearStart = Attribute(NearASC, URRHealthAttributeSet::GetHealthAttribute());
	const float FarStart = Attribute(FarASC, URRHealthAttributeSet::GetHealthAttribute());

	TestTrue(TEXT("explosion queued"), Subsystem->QueueExplosion(Center, TEXT("Barrel"), nullptr, nullptr));
	TestEqual(TEXT("miner untouched before the tick"), Attribute(MinerASC, URRHealthAttributeSet::GetHealthAttribute()), TestMaxHealth);
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	const float MinerHealth = Attribute(MinerASC, URRHealthAttributeSet::GetHealthAttribute());
	const float NearHealth = Attribute(NearASC, URRHealthAttributeSet::GetHealthAttribute());
	AddInfo(FString::Printf(TEXT("miner %.1f -> %.1f, near still chaser %.1f -> %.1f"), TestMaxHealth, MinerHealth, NearStart, NearHealth));
	TestEqual(TEXT("miner hit once by 40"), MinerHealth, TestMaxHealth - 40.f);
	TestEqual(TEXT("near still chaser hit once by 40"), NearHealth, NearStart - 40.f);
	TestEqual(TEXT("far still chaser untouched"), Attribute(FarASC, URRHealthAttributeSet::GetHealthAttribute()), FarStart);
	TestFalse(TEXT("miner alive"), Miner->IsDead());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExplosionKamikazeReachTest, "RogueRigs.Explosions.KamikazeReach",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExplosionKamikazeReachTest::RunTest(const FString& Parameters)
{
	using namespace RRExplosionTests;

	FTestWorld TestWorld(TEXT("RRExplosionKamikazeReach"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, nullptr) : nullptr;
	FRRExplosionStatsRow Stats;
	if (!Subsystem || !TestTrue(TEXT("Kamikaze row loads"), Subsystem->GetExplosionStats(TEXT("Kamikaze"), Stats)))
	{
		return false;
	}
	// 300 uu was outside the old 250 uu radius and is inside 312 uu; 400 uu stays outside.
	const FVector Center = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y);
	ARRChaser* Reached = SpawnStillChaser(*this, TestWorld, Center + FVector(0.f, 300.f, 0.f));
	ARRChaser* Beyond = SpawnStillChaser(*this, TestWorld, Center + FVector(0.f, 400.f, 0.f));
	if (!TestNotNull(TEXT("still chaser at 300 uu"), Reached) || !TestNotNull(TEXT("still chaser at 400 uu"), Beyond))
	{
		return false;
	}
	UAbilitySystemComponent* ReachedASC = Reached->GetAbilitySystemComponent();
	UAbilitySystemComponent* BeyondASC = Beyond->GetAbilitySystemComponent();
	const float ReachedStart = Attribute(ReachedASC, URRHealthAttributeSet::GetHealthAttribute());
	const float BeyondStart = Attribute(BeyondASC, URRHealthAttributeSet::GetHealthAttribute());

	TestTrue(TEXT("explosion queued"), Subsystem->QueueExplosion(Center, TEXT("Kamikaze"), nullptr, nullptr));
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	const float ReachedHealth = Attribute(ReachedASC, URRHealthAttributeSet::GetHealthAttribute());
	AddInfo(FString::Printf(TEXT("radius %.0f, still chaser at 300 uu %.1f -> %.1f"), Stats.Radius, ReachedStart, ReachedHealth));
	TestEqual(TEXT("still chaser at 300 uu hit once by the Kamikaze damage"), ReachedHealth, ReachedStart - Stats.Damage);
	TestEqual(TEXT("still chaser at 400 uu untouched"), Attribute(BeyondASC, URRHealthAttributeSet::GetHealthAttribute()), BeyondStart);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExplosionChainDelayTest, "RogueRigs.Explosions.ChainDelay",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExplosionChainDelayTest::RunTest(const FString& Parameters)
{
	using namespace RRExplosionTests;

	FTestWorld TestWorld(TEXT("RRExplosionChainDelay"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, Recorder.Get()) : nullptr;
	if (!Subsystem)
	{
		return false;
	}
	TestEqual(TEXT("ChainDelay 0.08 s"), Subsystem->GetChainDelay(), 0.08f);

	const FVector First = Grid->GetCellCenter(30, 43);
	const FVector Second = Grid->GetCellCenter(36, 43);
	TestTrue(TEXT("first queued"), Subsystem->QueueExplosion(First, TEXT("Kamikaze"), nullptr, nullptr));
	TestTrue(TEXT("second queued"), Subsystem->QueueExplosion(Second, TEXT("Kamikaze"), nullptr, nullptr));

	TArray<double> Times;
	for (int32 Frame = 0; Frame < 30 && Times.Num() < 2; ++Frame)
	{
		const int32 Before = Recorder->GetExplosionRows().Num();
		TestWorld.Tick();
		const int32 After = Recorder->GetExplosionRows().Num();
		TestTrue(TEXT("at most one link per tick"), After - Before <= 1);
		if (After > Before)
		{
			Times.Add(TestWorld.Get()->GetTimeSeconds());
		}
	}
	if (!TestEqual(TEXT("both explosions resolved"), Times.Num(), 2))
	{
		return false;
	}
	const double Gap = Times[1] - Times[0];
	AddInfo(FString::Printf(TEXT("explosions at %.4f and %.4f s, gap %.4f s"), Times[0], Times[1], Gap));
	TestTrue(TEXT("first resolved on the first tick"), FMath::IsNearlyEqual(Times[0], (double)FrameSeconds, 1.e-3));
	TestTrue(TEXT("gap >= 0.08 s"), Gap >= 0.08 - 1.e-3);
	TestTrue(TEXT("gap within one frame of 0.08 s"), Gap <= 0.08 + FrameSeconds + 1.e-3);
	TestTrue(TEXT("queue order kept"), Recorder->GetExplosionCenters()[0].Equals(First) && Recorder->GetExplosionCenters()[1].Equals(Second));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExplosionSourceOnceTest, "RogueRigs.Explosions.SourceExplodesOnce",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExplosionSourceOnceTest::RunTest(const FString& Parameters)
{
	using namespace RRExplosionTests;

	FTestWorld TestWorld(TEXT("RRExplosionSourceOnce"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, Recorder.Get()) : nullptr;
	if (!Subsystem)
	{
		return false;
	}
	AActor* Source = TestWorld.Get()->SpawnActor<AActor>();
	const FVector Center = Grid->GetCellCenter(30, 43);
	TestFalse(TEXT("source not spent before"), Subsystem->IsSourceSpent(Source));
	TestTrue(TEXT("first queue accepted"), Subsystem->QueueExplosion(Center, TEXT("Barrel"), nullptr, Source));
	TestFalse(TEXT("second queue refused"), Subsystem->QueueExplosion(Center, TEXT("Barrel"), nullptr, Source));
	TestTrue(TEXT("source spent"), Subsystem->IsSourceSpent(Source));
	TestEqual(TEXT("one pending"), Subsystem->GetPendingCount(), 1);
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestFalse(TEXT("queue after the explosion refused"), Subsystem->QueueExplosion(Center, TEXT("Barrel"), nullptr, Source));
	TickUntilIdle(TestWorld, Subsystem);
	TestEqual(TEXT("exploded once"), Recorder->GetExplosionRows().Num(), 1);
	AddExpectedError(TEXT("row 'NoSuchRow' not found"), EAutomationExpectedErrorFlags::Contains, 1);
	TestFalse(TEXT("unknown row refused"), Subsystem->QueueExplosion(Center, TEXT("NoSuchRow"), nullptr, nullptr));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExplosionNoOneShotTest, "RogueRigs.Explosions.NoOneShotMiner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExplosionNoOneShotTest::RunTest(const FString& Parameters)
{
	using namespace RRExplosionTests;

	FTestWorld TestWorld(TEXT("RRExplosionNoOneShot"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnTestMiner(*this, TestWorld, Grid, RockWestFloorCell) : nullptr;
	URRExplosionSubsystem* Subsystem = Miner ? GetSubsystem(*this, TestWorld, nullptr) : nullptr;
	if (!Subsystem)
	{
		return false;
	}
	UAbilitySystemComponent* MinerASC = Miner->GetAbilitySystemComponent();
	FRRExplosionStatsRow Huge;
	Huge.Radius = 300.f;
	Huge.Damage = 1000.f;
	Huge.TerrainSource = ERRTerrainDamageSource::Barrel;
	const FVector Center = Miner->GetActorLocation();

	// Full health: capped, the miner keeps 1 Health.
	TestTrue(TEXT("huge explosion queued"), Subsystem->QueueExplosionWithStats(Center, TEXT("Huge"), Huge, nullptr, nullptr));
	TickUntilIdle(TestWorld, Subsystem);
	const float AfterFirst = Attribute(MinerASC, URRHealthAttributeSet::GetHealthAttribute());
	AddInfo(FString::Printf(TEXT("full-health miner after a 1000 explosion: %.1f"), AfterFirst));
	TestEqual(TEXT("full-health miner left at 1"), AfterFirst, 1.f);
	TestFalse(TEXT("miner alive"), Miner->IsDead());
	TestFalse(TEXT("no State.Dead"), MinerASC->HasMatchingGameplayTag(RRGameplayTags::State_Dead));
	TestTrue(TEXT("i-frames granted"), MinerASC->HasMatchingGameplayTag(RRGameplayTags::State_Invulnerable));

	// During the i-frames a second explosion is refused.
	TestTrue(TEXT("second queued"), Subsystem->QueueExplosionWithStats(Center, TEXT("Huge"), Huge, nullptr, nullptr));
	TickUntilIdle(TestWorld, Subsystem);
	TestEqual(TEXT("refused during i-frames"), Attribute(MinerASC, URRHealthAttributeSet::GetHealthAttribute()), 1.f);

	// Below full health the rule does not apply: an explosion can kill.
	const int32 IFrameFrames = FMath::CeilToInt32(Miner->GetIFramesDuration() / FrameSeconds) + 2;
	for (int32 Frame = 0; Frame < IFrameFrames; ++Frame)
	{
		TestWorld.Tick();
	}
	TestTrue(TEXT("third queued"), Subsystem->QueueExplosionWithStats(Center, TEXT("Huge"), Huge, nullptr, nullptr));
	TickUntilIdle(TestWorld, Subsystem);
	TestEqual(TEXT("wounded miner killed"), Attribute(MinerASC, URRHealthAttributeSet::GetHealthAttribute()), 0.f);
	TestTrue(TEXT("miner dead"), Miner->IsDead());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
