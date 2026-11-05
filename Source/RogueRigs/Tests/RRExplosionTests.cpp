// Rogue Rigs - automation tests of the explosion system (RogueRigs.Explosions.*): blocks in the radius, characters hit once, kamikaze reach (CSV radius), chain delay, single explosion per source, miner no-one-shot, visual radius (x 1.05) and hold (0.1 s) per row, blocks destroyed over frames nearest first (task-121-01), wall debris request (task-439-01).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemies/RRChaser.h"
#include "EngineUtils.h"
#include "Explosions/RRExplosionFX.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Grid/RRBlockColors.h"
#include "Misc/App.h"
#include "NiagaraComponent.h"
#include "NiagaraDataSetAccessor.h"
#include "NiagaraEmitterInstance.h"
#include "NiagaraSystem.h"
#include "NiagaraSystemInstance.h"
#include "NiagaraSystemInstanceController.h"
#include "Pool/RRNiagaraPoolSubsystem.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestStatsRows.h"
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

	/** Ticks until the grid has no queued explosion block left (at most MaxFrames); returns the frames ticked. */
	int32 TickUntilGridIdle(const FTestWorld& TestWorld, const ARRArenaGrid* Grid, int32 MaxFrames = 600)
	{
		int32 Frames = 0;
		while (Grid->GetPendingCellDestructionCount() > 0 && Frames < MaxFrames)
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
	// The values live in ExplosionStats.csv (task-420): the rows must be usable; the checks below derive from them.
	TestTrue(TEXT("Barrel radius > 0"), Stats.Radius > 0.f);
	TestTrue(TEXT("Barrel damage > 0"), Stats.Damage > 0.f);
	TestTrue(TEXT("Barrel miner damage > 0"), Stats.MinerDamage > 0.f);
	TestTrue(TEXT("Barrel terrain source"), Stats.TerrainSource == ERRTerrainDamageSource::Barrel);
	FRRExplosionStatsRow KamikazeStats;
	TestTrue(TEXT("Kamikaze row loads"), Subsystem->GetExplosionStats(TEXT("Kamikaze"), KamikazeStats));
	TestTrue(TEXT("Kamikaze radius > 0"), KamikazeStats.Radius > 0.f);
	TestTrue(TEXT("Kamikaze damage > 0"), KamikazeStats.Damage > 0.f);
	TestTrue(TEXT("Kamikaze miner damage > 0"), KamikazeStats.MinerDamage > 0.f);

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
	// Task-121-01: the blocks are destroyed over frames under the grid budget (time based, so the frame count varies).
	const int32 SpreadFrames = TickUntilGridIdle(TestWorld, Grid);
	AddInfo(FString::Printf(TEXT("blocks destroyed over %d more frame(s)"), SpreadFrames));
	TestEqual(TEXT("no block left queued"), Grid->GetPendingCellDestructionCount(), 0);
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
	// Outside the 700 uu Barrel radius (was 600 with the 350 uu radius, task-114-01).
	ARRChaser* Far = SpawnStillChaser(*this, TestWorld, Center + FVector(0.f, 800.f, 0.f));
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
	const FRRExplosionStatsRow BarrelRow = RRTestStats::ExplosionRow(*this, RRTestStats::BarrelRow);
	TestEqual(TEXT("miner hit once by the Barrel MinerDamage"), MinerHealth, TestMaxHealth - BarrelRow.MinerDamage);
	TestEqual(TEXT("near still chaser hit once by the Barrel Damage"), NearHealth, NearStart - BarrelRow.Damage);
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
	Huge.MinerDamage = 1000.f;
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExplosionVisualTest, "RogueRigs.Explosions.VisualRadiusAndHold",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExplosionVisualTest::RunTest(const FString& Parameters)
{
	using namespace RRExplosionTests;

	FTestWorld TestWorld(TEXT("RRExplosionVisual"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, nullptr) : nullptr;
	if (!Subsystem)
	{
		return false;
	}
	// explosions gdd (owner 2026-10-10): visible part about 5 % larger than the collision radius, final state held 0.1 s.
	TestNearlyEqual(TEXT("VisualRadiusScale 1.05"), Subsystem->GetVisualRadiusScale(), 1.05f, 1e-5f);
	TestNearlyEqual(TEXT("VisualHoldSeconds 0.1"), Subsystem->GetVisualHoldSeconds(), 0.1f, 1e-5f);

	struct FRowCase
	{
		const TCHAR* Row;
		float CollisionRadius;
		float Damage;
	};
	// Expected collision radius and damage read once from the CSV rows (task-420); the subsystem must return them unchanged.
	const FRRExplosionStatsRow BarrelCsv = RRTestStats::ExplosionRow(*this, RRTestStats::BarrelRow);
	const FRRExplosionStatsRow KamikazeCsv = RRTestStats::ExplosionRow(*this, RRTestStats::KamikazeRow);
	const FRowCase Cases[] = { { TEXT("Barrel"), BarrelCsv.Radius, BarrelCsv.Damage }, { TEXT("Kamikaze"), KamikazeCsv.Radius, KamikazeCsv.Damage } };
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Cases); ++Index)
	{
		const FRowCase& Case = Cases[Index];
		FRRExplosionStatsRow Stats;
		if (!TestTrue(FString::Printf(TEXT("%s row loads"), Case.Row), Subsystem->GetExplosionStats(Case.Row, Stats)))
		{
			continue;
		}
		TestEqual(FString::Printf(TEXT("%s collision radius unchanged (%.0f)"), Case.Row, Case.CollisionRadius), Stats.Radius, Case.CollisionRadius);
		TestEqual(FString::Printf(TEXT("%s damage unchanged (%.0f)"), Case.Row, Case.Damage), Stats.Damage, Case.Damage);
		const float VisualRadius = Subsystem->GetVisualRadius(Stats.Radius);
		TestNearlyEqual(FString::Printf(TEXT("%s visual radius = 1.05 x radius"), Case.Row), VisualRadius, 1.05f * Case.CollisionRadius, 0.01f);

		// The spawned FX gets the visual radius and the hold; it holds its final state, then fades and goes.
		const FVector Center = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y);
		if (!TestTrue(FString::Printf(TEXT("%s explosion queued"), Case.Row), Subsystem->QueueExplosion(Center, Case.Row, nullptr, nullptr)))
		{
			continue;
		}
		TickUntilIdle(TestWorld, Subsystem);
		ARRExplosionFX* FX = nullptr;
		for (TActorIterator<ARRExplosionFX> It(TestWorld.Get()); It; ++It)
		{
			if (IsValid(*It) && !It->IsActorBeingDestroyed())
			{
				FX = *It;
			}
		}
		if (!TestNotNull(FString::Printf(TEXT("%s FX spawned"), Case.Row), FX))
		{
			continue;
		}
		TestNearlyEqual(FString::Printf(TEXT("%s FX radius = visual radius"), Case.Row), FX->GetRadius(), VisualRadius, 0.01f);
		TestNearlyEqual(FString::Printf(TEXT("%s FX hold = VisualHoldSeconds"), Case.Row), FX->GetHoldSeconds(), Subsystem->GetVisualHoldSeconds(), 1e-5f);
		const float GrowSeconds = RRTestAccess::FExplosionFXAccess::DurationOf(FX);
		TestTrue(FString::Printf(TEXT("%s FX lifetime longer than grow + hold"), Case.Row), FX->GetLifetime() > GrowSeconds + FX->GetHoldSeconds());

		// Middle of the hold: full size (Radius / 50 for the engine sphere).
		const UStaticMeshComponent* Mesh = FX->FindComponentByClass<UStaticMeshComponent>();
		const float FullScale = FX->GetRadius() / 50.f;
		const TWeakObjectPtr<ARRExplosionFX> WeakFX(FX);
		const int32 HoldMiddleFrames = FMath::CeilToInt32((GrowSeconds + 0.5f * FX->GetHoldSeconds()) / FrameSeconds) - 1;
		for (int32 Frame = 0; Frame < HoldMiddleFrames; ++Frame)
		{
			TestWorld.Tick();
		}
		if (TestTrue(FString::Printf(TEXT("%s FX alive during the hold"), Case.Row), WeakFX.IsValid() && !WeakFX->IsActorBeingDestroyed()) && Mesh)
		{
			TestNearlyEqual(FString::Printf(TEXT("%s FX at full size during the hold"), Case.Row), static_cast<float>(Mesh->GetRelativeScale3D().X), FullScale, 0.01f);
		}
		for (int32 Frame = 0; Frame < 120 && WeakFX.IsValid() && !WeakFX->IsActorBeingDestroyed(); ++Frame)
		{
			TestWorld.Tick();
		}
		TestTrue(FString::Printf(TEXT("%s FX destroyed after its lifetime"), Case.Row), !WeakFX.IsValid() || WeakFX->IsActorBeingDestroyed());
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExplosionBlocksSpreadTest, "RogueRigs.Explosions.BlocksSpreadOverFrames",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExplosionBlocksSpreadTest::RunTest(const FString& Parameters)
{
	using namespace RRExplosionTests;

	FTestWorld TestWorld(TEXT("RRExplosionBlocksSpread"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnTestMiner(*this, TestWorld, Grid, RockWestFloorCell) : nullptr;
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	URRExplosionSubsystem* Subsystem = Miner ? GetSubsystem(*this, TestWorld, Recorder.Get()) : nullptr;
	FRRExplosionStatsRow Stats;
	if (!Subsystem || !TestTrue(TEXT("Barrel row loads"), Subsystem->GetExplosionStats(TEXT("Barrel"), Stats)))
	{
		return false;
	}
	TestNearlyEqual(TEXT("default budget 2 ms"), Grid->GetCellDestructionBudgetMs(), 2.f);
	// A budget far below one block's cost: exactly one block per frame, so the split is deterministic.
	Grid->SetCellDestructionBudgetMs(1.e-6f);

	// Every non-barrel block in the radius (barrels are handled at once), and the ring of one cell beyond it.
	const FVector Center = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y);
	const int32 Reach = FMath::CeilToInt32(Stats.Radius / Grid->GetCellSize()) + 2;
	TArray<FIntPoint> Inside;
	TArray<FIntPoint> Outside;
	int32 BarrelsInside = 0;
	for (int32 Y = RockWestFloorCell.Y - Reach; Y <= RockWestFloorCell.Y + Reach; ++Y)
	{
		for (int32 X = RockWestFloorCell.X - Reach; X <= RockWestFloorCell.X + Reach; ++X)
		{
			const FIntPoint Cell(X, Y);
			const float Distance = FVector::Dist2D(Grid->GetCellCenter(X, Y), Center);
			if (Grid->GetCellType(Cell) == ERRCellType::Barrel)
			{
				BarrelsInside += Distance <= Stats.Radius ? 1 : 0;
				continue;
			}
			if (!Grid->IsBlockCell(Cell))
			{
				continue;
			}
			if (Distance <= Stats.Radius)
			{
				Inside.Add(Cell);
			}
			else if (Distance <= Stats.Radius + Grid->GetCellSize())
			{
				Outside.Add(Cell);
			}
		}
	}
	TestEqual(TEXT("no barrel in the radius (no chain during the test)"), BarrelsInside, 0);
	if (!TestTrue(TEXT("several blocks inside the radius"), Inside.Num() > 2))
	{
		return false;
	}
	auto CountStanding = [Grid](const TArray<FIntPoint>& Cells)
	{
		int32 Count = 0;
		for (const FIntPoint& Cell : Cells)
		{
			Count += Grid->IsBlockCell(Cell) ? 1 : 0;
		}
		return Count;
	};
	auto DistanceOf = [Grid, &Center](const FIntPoint& Cell) { return FVector::Dist2D(Grid->GetCellCenter(Cell.X, Cell.Y), Center); };
	// Largest distance among the destroyed blocks and smallest among the standing ones: nearest first means Destroyed <= Standing.
	auto CheckNearestFirst = [&]()
	{
		float FarthestDestroyed = 0.f;
		float NearestStanding = TNumericLimits<float>::Max();
		for (const FIntPoint& Cell : Inside)
		{
			if (Grid->IsBlockCell(Cell))
			{
				NearestStanding = FMath::Min(NearestStanding, DistanceOf(Cell));
			}
			else
			{
				FarthestDestroyed = FMath::Max(FarthestDestroyed, DistanceOf(Cell));
			}
		}
		return FarthestDestroyed <= NearestStanding + 1e-3f;
	};

	UAbilitySystemComponent* MinerASC = Miner->GetAbilitySystemComponent();
	const float BarrelMinerDamage = RRTestStats::ExplosionRow(*this, RRTestStats::BarrelRow).MinerDamage;
	TestTrue(TEXT("explosion queued"), Subsystem->QueueExplosion(Center, TEXT("Barrel"), nullptr, nullptr));
	TestWorld.Tick();

	// Explosion frame: event, damage and visual at once; one block destroyed (the nearest), the others still blocks, queued.
	TestEqual(TEXT("OnExplosion once"), Recorder->GetExplosionRows().Num(), 1);
	TestEqual(TEXT("miner hit at the explosion"), Attribute(MinerASC, URRHealthAttributeSet::GetHealthAttribute()), TestMaxHealth - BarrelMinerDamage);
	int32 FXCount = 0;
	for (TActorIterator<ARRExplosionFX> It(TestWorld.Get()); It; ++It)
	{
		FXCount += IsValid(*It) ? 1 : 0;
	}
	TestEqual(TEXT("visual spawned at the explosion"), FXCount, 1);
	TestEqual(TEXT("one block destroyed in the explosion frame"), Inside.Num() - CountStanding(Inside), 1);
	TestEqual(TEXT("the others queued"), Grid->GetPendingCellDestructionCount(), Inside.Num() - 1);
	TestTrue(TEXT("nearest block first"), CheckNearestFirst());

	int32 Frames = 1;
	bool bOnePerFrame = true;
	bool bNearestFirst = true;
	while (Grid->GetPendingCellDestructionCount() > 0 && Frames < Inside.Num() + 10)
	{
		const int32 Before = CountStanding(Inside);
		TestWorld.Tick();
		++Frames;
		bOnePerFrame &= Before - CountStanding(Inside) == 1;
		bNearestFirst &= CheckNearestFirst();
	}
	AddInfo(FString::Printf(TEXT("%d blocks destroyed over %d frames"), Inside.Num(), Frames));
	TestTrue(TEXT("split over more than one frame"), Frames > 1);
	TestEqual(TEXT("one block per frame at a tiny budget"), Frames, Inside.Num());
	TestTrue(TEXT("one block destroyed each frame"), bOnePerFrame);
	TestTrue(TEXT("nearest blocks first every frame"), bNearestFirst);
	TestEqual(TEXT("every block inside destroyed"), CountStanding(Inside), 0);
	TestEqual(TEXT("ring beyond the radius kept"), CountStanding(Outside), Outside.Num());
	TestEqual(TEXT("miner hit once"), Attribute(MinerASC, URRHealthAttributeSet::GetHealthAttribute()), TestMaxHealth - BarrelMinerDamage);

	// Cleanup: blocks still queued when the grid leaves play are dropped, never destroyed later.
	if (Outside.Num() > 0)
	{
		const FIntPoint Other = Outside[0];
		TestTrue(TEXT("second explosion queued"), Subsystem->QueueExplosion(Grid->GetCellCenter(Other.X, Other.Y), TEXT("Barrel"), nullptr, nullptr));
		TickUntilIdle(TestWorld, Subsystem);
		TestTrue(TEXT("second explosion leaves blocks queued"), Grid->GetPendingCellDestructionCount() > 0);
		Grid->Destroy();
		TestEqual(TEXT("queue dropped when the grid is destroyed"), Grid->GetPendingCellDestructionCount(), 0);
		for (int32 Frame = 0; Frame < 5; ++Frame)
		{
			TestWorld.Tick();
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExplosionWallDebrisRequestTest, "RogueRigs.Explosions.WallDebrisRequest",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExplosionWallDebrisRequestTest::RunTest(const FString& Parameters)
{
	using namespace RRExplosionTests;

	// The private empty system below cannot run under a real RHI (engine crash on tick).
	if (FApp::CanEverRender())
	{
		AddInfo(TEXT("SKIPPED under a real RHI: run with -nullrhi."));
		return true;
	}

	FTestWorld TestWorld(TEXT("RRExplosionWallDebrisRequest"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, nullptr) : nullptr;
	URRNiagaraPoolSubsystem* FXPool = Subsystem ? TestWorld.Get()->GetSubsystem<URRNiagaraPoolSubsystem>() : nullptr;
	FRRExplosionStatsRow Stats;
	if (!FXPool || !TestTrue(TEXT("Barrel row loads"), Subsystem->GetExplosionStats(TEXT("Barrel"), Stats)))
	{
		return false;
	}
	TStrongObjectPtr<UNiagaraSystem> Debris(NewObject<UNiagaraSystem>());
	Subsystem->SetWallDebrisSystem(Debris.Get());
	Subsystem->SetChainDelay(0.f);
	Stats.Damage = 0.f;
	Stats.MinerDamage = 0.f;
	Stats.FXClass.Reset();

	const FVector Center = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y);
	const auto Explode = [&](const FVector& At, float Radius)
	{
		Stats.Radius = Radius;
		TestTrue(TEXT("explosion queued"), Subsystem->QueueExplosionWithStats(At, TEXT("Test"), Stats, nullptr, nullptr));
		TickUntilIdle(TestWorld, Subsystem);
		TickUntilGridIdle(TestWorld, Grid);
	};
	// Standing blocks in the radius by type; barrels only light a fuse, so they give no debris.
	const auto StandingBlocks = [&](const FVector& At, float Radius)
	{
		TMap<FIntPoint, ERRCellType> Blocks;
		const int32 Reach = FMath::CeilToInt32(Radius / Grid->GetCellSize()) + 1;
		const FIntPoint AtCell = Grid->GetCellAtWorld(At);
		for (int32 Y = AtCell.Y - Reach; Y <= AtCell.Y + Reach; ++Y)
		{
			for (int32 X = AtCell.X - Reach; X <= AtCell.X + Reach; ++X)
			{
				const FIntPoint Cell(X, Y);
				if (Grid->IsBlockCell(Cell) && Grid->GetCellType(Cell) != ERRCellType::Barrel && FVector::Dist2D(Grid->GetCellCenter(X, Y), At) <= Radius)
				{
					Blocks.Add(Cell, Grid->GetCellType(Cell));
				}
			}
		}
		return Blocks;
	};
	const FRRWallDebrisRequest& Request = Subsystem->GetLastWallDebrisRequest();
	const auto CheckRequest = [&](const TMap<FIntPoint, ERRCellType>& Blocks, const FVector& At, TSet<FIntPoint>& OutCells)
	{
		TestEqual(TEXT("request carries the centre"), Request.Center, At);
		TestEqual(TEXT("one colour per position"), Request.Colors.Num(), Request.Positions.Num());
		for (int32 Index = 0; Index < Request.Positions.Num() && Index < Request.Colors.Num(); ++Index)
		{
			const FIntPoint Cell = Grid->GetCellAtWorld(Request.Positions[Index]);
			const ERRCellType* Type = Blocks.Find(Cell);
			if (TestNotNull(*FString::Printf(TEXT("particle %d starts on a broken block"), Index), Type))
			{
				TestTrue(TEXT("colour of its block"), Request.Colors[Index].Equals(RRBlockColors::ForType(*Type)));
				OutCells.Add(Cell);
			}
		}
	};

	// No block in the radius: no request.
	TestEqual(TEXT("the centre cell is floor"), Grid->IsBlockCell(RockWestFloorCell), false);
	Explode(Center, 1.f);
	TestEqual(TEXT("no block broken: no request"), FXPool->GetRequestedCount(Debris.Get()), 0);

	// A few blocks: one request, every block shows, ParticlesPerBlock each.
	const float SmallRadius = Grid->GetCellSize() * 1.5f;
	const TMap<FIntPoint, ERRCellType> FewBlocks = StandingBlocks(Center, SmallRadius);
	const int32 PerBlock = Subsystem->GetWallDebrisParticlesPerBlock();
	if (TestTrue(TEXT("a few blocks in the small radius"), FewBlocks.Num() > 0 && FewBlocks.Num() * PerBlock <= 60))
	{
		Explode(Center, SmallRadius);
		TestEqual(TEXT("one request"), FXPool->GetRequestedCount(Debris.Get()), 1);
		TestEqual(TEXT("particles per block"), Request.Positions.Num(), FewBlocks.Num() * PerBlock);
		TSet<FIntPoint> Cells;
		CheckRequest(FewBlocks, Center, Cells);
		TestEqual(TEXT("every block throws debris"), Cells.Num(), FewBlocks.Num());
		for (const TPair<FIntPoint, ERRCellType>& Block : FewBlocks)
		{
			TestFalse(TEXT("block broken"), Grid->IsBlockCell(Block.Key));
		}
	}

	// Very many blocks: one request, at most 60 particles, spread over the blocks.
	const float LargeRadius = 3000.f;
	const FVector FarCenter = Grid->GetCellCenter(RockCell.X + 3, RockCell.Y);
	const TMap<FIntPoint, ERRCellType> ManyBlocks = StandingBlocks(FarCenter, LargeRadius);
	if (TestTrue(TEXT("more blocks than the bound"), ManyBlocks.Num() > 60))
	{
		const int32 RequestedBefore = FXPool->GetRequestedCount(Debris.Get());
		Explode(FarCenter, LargeRadius);
		TestEqual(TEXT("one request however many blocks break"), FXPool->GetRequestedCount(Debris.Get()), RequestedBefore + 1);
		TestEqual(TEXT("bounded at 60 particles"), Request.Positions.Num(), 60);
		TSet<FIntPoint> Cells;
		CheckRequest(ManyBlocks, FarCenter, Cells);
		TestTrue(TEXT("spread over many blocks"), Cells.Num() >= 30);
	}

	// A chain: the pool never grows past its cap, whatever the number of links.
	for (int32 Link = 0; Link < 24; ++Link)
	{
		Stats.Radius = 400.f;
		const FVector At = FarCenter + FVector(4000.0 + Link * 60.0, 3000.0, 0.0);
		TestTrue(TEXT("link queued"), Subsystem->QueueExplosionWithStats(At, TEXT("Test"), Stats, nullptr, nullptr));
	}
	TickUntilIdle(TestWorld, Subsystem);
	TestTrue(TEXT("chain bounded by the pool"), FXPool->GetComponentCount(Debris.Get()) <= Subsystem->GetWallDebrisPoolSize());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExplosionWallDebrisAssetTest, "RogueRigs.Explosions.WallDebrisAsset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExplosionWallDebrisAssetTest::RunTest(const FString& Parameters)
{
	using namespace RRExplosionTests;

	UNiagaraSystem* Shipped = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/RogueRigs/FX/NS_ExplosionWallDebris.NS_ExplosionWallDebris"));
	if (!TestNotNull(TEXT("NS_ExplosionWallDebris exists"), Shipped))
	{
		return false;
	}
	// Niagara only runs under a real RHI (-RenderOffScreen).
	if (!FApp::CanEverRender())
	{
		AddInfo(TEXT("SKIPPED: FApp::CanEverRender() is false under -nullrhi; run with -RenderOffScreen."));
		return true;
	}

	FTestWorld TestWorld(TEXT("RRExplosionWallDebrisAsset"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, nullptr) : nullptr;
	URRNiagaraPoolSubsystem* FXPool = Subsystem ? TestWorld.Get()->GetSubsystem<URRNiagaraPoolSubsystem>() : nullptr;
	FRRExplosionStatsRow Stats;
	if (!FXPool || !TestTrue(TEXT("Barrel row loads"), Subsystem->GetExplosionStats(TEXT("Barrel"), Stats)))
	{
		return false;
	}
	Stats.Damage = 0.f;
	Stats.MinerDamage = 0.f;
	Stats.FXClass.Reset();
	Stats.Radius = 1500.f;

	const FVector Center = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y);
	TestTrue(TEXT("explosion queued"), Subsystem->QueueExplosionWithStats(Center, TEXT("Test"), Stats, nullptr, nullptr));
	TickUntilIdle(TestWorld, Subsystem);
	TestEqual(TEXT("the game plays the shipped system once"), FXPool->GetRequestedCount(Shipped), 1);
	const FRRWallDebrisRequest& Request = Subsystem->GetLastWallDebrisRequest();
	TestTrue(TEXT("at most 60 particles requested"), Request.Positions.Num() > 0 && Request.Positions.Num() <= 60);

	// The burst, read from the simulation itself on the first frame it has particles.
	FRRNiagaraSystemPool* Pool = FNiagaraPoolAccess::PoolOf(FXPool, Shipped);
	if (!TestNotNull(TEXT("debris pool"), Pool) || !TestEqual(TEXT("one effect, not one per block"), Pool->InUse.Num(), 1))
	{
		return false;
	}
	UNiagaraComponent* Component = Pool->InUse[0];
	const auto EmitterOf = [Component]() -> FNiagaraEmitterInstance*
	{
		FNiagaraSystemInstanceControllerPtr Controller = Component->GetSystemInstanceController();
		FNiagaraSystemInstance* Instance = Controller.IsValid() ? Controller->GetSystemInstance_Unsafe() : nullptr;
		if (!Instance)
		{
			return nullptr;
		}
		Controller->WaitForConcurrentTickAndFinalize();
		return Instance->GetEmitters().Num() > 0 ? &Instance->GetEmitters()[0].Get() : nullptr;
	};
	int32 Frame = 0;
	FNiagaraEmitterInstance* Emitter = EmitterOf();
	while ((!Emitter || Emitter->GetNumParticles() == 0) && Frame < 600)
	{
		TestWorld.Tick();
		++Frame;
		Emitter = EmitterOf();
	}
	if (!TestTrue(TEXT("the burst spawned"), Emitter && Emitter->GetNumParticles() > 0))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("first particles after %d frame(s), %d particle(s)"), Frame, Emitter->GetNumParticles()));
	TestEqual(TEXT("as many particles as requested"), Emitter->GetNumParticles(), Request.Positions.Num());

	const FNiagaraDataSet& Data = Emitter->GetParticleData();
	const auto Positions = FNiagaraDataSetAccessor<FNiagaraPosition>::CreateReader(Data, TEXT("Position"));
	const auto Velocities = FNiagaraDataSetAccessor<FVector3f>::CreateReader(Data, TEXT("Velocity"));
	const auto Colors = FNiagaraDataSetAccessor<FLinearColor>::CreateReader(Data, TEXT("Color"));
	if (!TestTrue(TEXT("particle attributes readable"), Positions.IsValid() && Velocities.IsValid() && Colors.IsValid()))
	{
		return false;
	}
	const float Cos15 = FMath::Cos(FMath::DegreesToRadians(15.f));
	double MinOutward = 1.0;
	for (int32 Index = 0; Index < Emitter->GetNumParticles(); ++Index)
	{
		const FVector Position(Positions[Index]);
		const FVector Velocity(Velocities[Index]);
		// Starts where a block stood: near one of the requested points (each read by its own index).
		double Nearest = TNumericLimits<double>::Max();
		for (const FVector& Requested : Request.Positions)
		{
			Nearest = FMath::Min(Nearest, FVector::Dist(Position, Requested));
		}
		TestTrue(*FString::Printf(TEXT("particle %d near a requested point (%.1f uu)"), Index, Nearest), Nearest < 60.0);
		TestTrue(TEXT("block colour"), Request.Colors.ContainsByPredicate([&](const FLinearColor& Color) { return Color.Equals(Colors[Index], 0.01f); }));
		TestTrue(TEXT("flies upward first"), Velocity.Z > 0.0);
		const FVector Away = (Position - Center).GetSafeNormal2D();
		if (FVector::Dist2D(Position, Center) > 100.0)
		{
			const double Outward = Velocity.GetSafeNormal2D() | Away;
			MinOutward = FMath::Min(MinOutward, Outward);
			TestTrue(*FString::Printf(TEXT("particle %d flies straight away from the centre (cos %.3f)"), Index, Outward), Outward >= Cos15);
		}
	}
	AddInfo(FString::Printf(TEXT("smallest cos between velocity and the outward direction: %.3f"), MinOutward));

	// Falls and is gone within 1.5 s; the component comes back to the pool.
	Frame = 0;
	while (FXPool->GetInUseCount(Shipped) > 0 && Frame < 600)
	{
		TestWorld.Tick();
		++Frame;
	}
	AddInfo(FString::Printf(TEXT("back in the pool after %d frame(s) of %.4f s"), Frame, FrameSeconds));
	TestEqual(TEXT("one-shot: back in the pool"), FXPool->GetInUseCount(Shipped), 0);
	TestTrue(TEXT("gone within 1.5 s (two frames of margin for the pool callback)"), Frame * FrameSeconds <= 1.5f + 2.f * FrameSeconds);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
