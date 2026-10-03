// Rogue Rigs - automation tests of barrels (RogueRigs.Barrel.*): two rifle shots explode a barrel, barrels chain, a dug barrel explodes.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "EngineUtils.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "Grid/RRBarrelSource.h"
#include "Projectiles/RRProjectile.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRBarrelTests
{
	using namespace RRTestHarness;

	/** Blueprint of the rifle. */
	const TCHAR* const BurstRifleClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle.BP_BurstRifle_C");

	/** Single barrel of arena_m1 with open floor to its west (shooting cell), no other barrel within 350 uu. */
	const FIntPoint ShotBarrelCell(43, 6);
	const FIntPoint ShotFromCell(42, 6);

	/** Barrel pair of arena_m1, 200 uu apart (inside the 350 uu barrel radius). */
	const FIntPoint ChainFirstCell(73, 31);
	const FIntPoint ChainSecondCell(73, 32);

	/** Single barrel of arena_m1 dug by the drone path (ApplyTerrainDamage, source Drone). */
	const FIntPoint DugBarrelCell(30, 27);

	/** Explosion subsystem of the test world, with a recorder bound to OnExplosion. */
	URRExplosionSubsystem* GetSubsystem(FAutomationTestBase& Test, const FTestWorld& TestWorld, URRTestEventRecorder* Recorder)
	{
		URRExplosionSubsystem* Subsystem = TestWorld.Get()->GetSubsystem<URRExplosionSubsystem>();
		if (Test.TestNotNull(TEXT("explosion subsystem exists"), Subsystem))
		{
			Subsystem->OnExplosion.AddDynamic(Recorder, &URRTestEventRecorder::HandleExplosion);
		}
		return Subsystem;
	}

	/** Live projectiles in the world. */
	int32 CountProjectiles(UWorld* World)
	{
		int32 Count = 0;
		for (TActorIterator<ARRProjectile> It(World); It; ++It)
		{
			Count += IsValid(*It) && !It->IsActorBeingDestroyed() ? 1 : 0;
		}
		return Count;
	}

	/** Fires exactly one rifle shot now (auto-fire re-configured, advanced 1 ms and stopped), then ticks until the projectile is gone. Returns the frames ticked. */
	int32 FireOneShotAndWait(FAutomationTestBase& Test, const FTestWorld& TestWorld, ARRWeapon* Rifle)
	{
		URRAutoFireComponent* AutoFire = Rifle->GetAutoFireComponent();
		AutoFire->Configure(Rifle->GetAbilitySystemComponent(), Rifle->GetBurstCount(), Rifle->GetBurstDuty(), Rifle->GetReloadTime());
		AutoFire->StartFiring();
		AutoFire->AdvanceTime(0.001f);
		AutoFire->StopFiring();
		Test.TestEqual(TEXT("one shot fired"), AutoFire->GetTotalShotsFired(), 1);
		Test.TestEqual(TEXT("one projectile in flight"), CountProjectiles(TestWorld.Get()), 1);
		int32 Frames = 0;
		while (CountProjectiles(TestWorld.Get()) > 0 && Frames < 30)
		{
			TestWorld.Tick();
			++Frames;
		}
		return Frames;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRBarrelShotTest, "RogueRigs.Barrel.TwoRifleShotsExplode",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRBarrelShotTest::RunTest(const FString& Parameters)
{
	using namespace RRBarrelTests;

	FTestWorld TestWorld(TEXT("RRBarrelShot"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, Recorder.Get()) : nullptr;
	UClass* RifleClass = LoadTestClass<ARRWeapon>(*this, BurstRifleClassPath);
	if (!Subsystem || !RifleClass)
	{
		return false;
	}
	TestEqual(TEXT("(43, 6) is a barrel"), Grid->GetCellType(ShotBarrelCell), ERRCellType::Barrel);
	TestEqual(TEXT("(42, 6) is floor"), Grid->GetCellType(ShotFromCell), ERRCellType::Floor);
	TestNearlyEqual(TEXT("barrel shot health 20"), Grid->GetCellShotHealth(ShotBarrelCell), 20.f);
	const float DigHealth = Grid->GetCellHealth(ShotBarrelCell);

	ARRWeapon* Rifle = TestWorld.Spawn<ARRWeapon>(RifleClass, FVector::ZeroVector);
	if (!TestNotNull(TEXT("rifle spawned"), Rifle) || !TestTrue(TEXT("rifle acquired"), Rifle->Acquire(nullptr)))
	{
		return false;
	}
	Rifle->GetAutoFireComponent()->StopFiring();
	// Shot origin on the floor cell west of the barrel at half the block height; aim +X (no miner: the weapon's forward).
	const FVector Target = Grid->GetCellCenter(ShotFromCell.X, ShotFromCell.Y) + FVector(0.f, 0.f, 0.5f * Grid->GetBlockHeight());
	Rifle->SetActorLocation(Target - (Rifle->GetShotOrigin() - Rifle->GetActorLocation()), false, nullptr, ETeleportType::TeleportPhysics);

	const int32 FirstFrames = FireOneShotAndWait(*this, TestWorld, Rifle);
	const float ShotHealthAfterFirst = Grid->GetCellShotHealth(ShotBarrelCell);
	AddInfo(FString::Printf(TEXT("shot 1: projectile gone after %d frame(s), barrel shot health %.1f"), FirstFrames, ShotHealthAfterFirst));
	TestNearlyEqual(TEXT("first shot: shot health 20 -> 10"), ShotHealthAfterFirst, 10.f);
	TestEqual(TEXT("first shot: barrel still there"), Grid->GetCellType(ShotBarrelCell), ERRCellType::Barrel);
	TestNearlyEqual(TEXT("first shot: dig health untouched"), Grid->GetCellHealth(ShotBarrelCell), DigHealth);
	TestEqual(TEXT("first shot: nothing queued"), Subsystem->GetPendingCount(), 0);

	const int32 SecondFrames = FireOneShotAndWait(*this, TestWorld, Rifle);
	AddInfo(FString::Printf(TEXT("shot 2: projectile gone after %d frame(s), pending %d, explosions %d"), SecondFrames,
		Subsystem->GetPendingCount(), Recorder->GetExplosionRows().Num()));
	TestEqual(TEXT("second shot: barrel destroyed"), Grid->GetCellType(ShotBarrelCell), ERRCellType::Floor);
	TestTrue(TEXT("barrel source spent"), Subsystem->IsSourceSpent(Grid->GetBarrelSource(ShotBarrelCell)));

	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("one explosion"), Recorder->GetExplosionRows().Num(), 1);
	if (Recorder->GetExplosionRows().Num() == 1)
	{
		const FVector Center = Grid->GetCellCenter(ShotBarrelCell.X, ShotBarrelCell.Y);
		TestEqual(TEXT("row Barrel"), Recorder->GetExplosionRows()[0], FName(TEXT("Barrel")));
		TestTrue(TEXT("at the barrel cell center"), Recorder->GetExplosionCenters()[0].Equals(Center));
	}
	TestEqual(TEXT("queue empty"), Subsystem->GetPendingCount(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRBarrelChainTest, "RogueRigs.Barrel.ChainReaction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRBarrelChainTest::RunTest(const FString& Parameters)
{
	using namespace RRBarrelTests;

	FTestWorld TestWorld(TEXT("RRBarrelChain"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, Recorder.Get()) : nullptr;
	if (!Subsystem)
	{
		return false;
	}
	TestEqual(TEXT("(73, 31) is a barrel"), Grid->GetCellType(ChainFirstCell), ERRCellType::Barrel);
	TestEqual(TEXT("(73, 32) is a barrel"), Grid->GetCellType(ChainSecondCell), ERRCellType::Barrel);
	FRRExplosionStatsRow Stats;
	TestTrue(TEXT("Barrel row loads"), Subsystem->GetExplosionStats(TEXT("Barrel"), Stats));
	const FVector FirstCenter = Grid->GetCellCenter(ChainFirstCell.X, ChainFirstCell.Y);
	const FVector SecondCenter = Grid->GetCellCenter(ChainSecondCell.X, ChainSecondCell.Y);
	TestTrue(TEXT("second barrel inside the first's radius"), FVector::Dist2D(FirstCenter, SecondCenter) <= Stats.Radius);

	// Shoot the first barrel to 0 through the grid (the projectile path is covered by TwoRifleShotsExplode).
	TestTrue(TEXT("shot damage destroys the first barrel"), Grid->ApplyShotDamage(ChainFirstCell, Grid->GetCellShotHealth(ChainFirstCell)));
	TestEqual(TEXT("first explosion queued"), Subsystem->GetPendingCount(), 1);
	TestEqual(TEXT("second barrel still there"), Grid->GetCellType(ChainSecondCell), ERRCellType::Barrel);

	TArray<double> Times;
	const double Start = TestWorld.Get()->GetTimeSeconds();
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
		while (Times.Num() < Recorder->GetExplosionRows().Num())
		{
			Times.Add(TestWorld.Get()->GetTimeSeconds() - Start);
		}
		if (Times.Num() == 1)
		{
			TestEqual(TEXT("first explosion destroyed the second barrel"), Grid->GetCellType(ChainSecondCell), ERRCellType::Floor);
		}
	}
	TestEqual(TEXT("two explosions, each barrel once"), Recorder->GetExplosionRows().Num(), 2);
	if (Times.Num() == 2)
	{
		const double Gap = Times[1] - Times[0];
		AddInfo(FString::Printf(TEXT("explosions at %.4f and %.4f s, gap %.4f s (chain delay %.2f)"), Times[0], Times[1], Gap, Subsystem->GetChainDelay()));
		TestTrue(TEXT("first at the barrel center"), Recorder->GetExplosionCenters()[0].Equals(FirstCenter));
		TestTrue(TEXT("second at the chained barrel center"), Recorder->GetExplosionCenters()[1].Equals(SecondCenter));
		TestTrue(TEXT("chain link after the chain delay, within one frame"), Gap >= Subsystem->GetChainDelay() - 1e-4 && Gap < Subsystem->GetChainDelay() + FrameSeconds);
	}
	TestTrue(TEXT("both sources spent"), Subsystem->IsSourceSpent(Grid->GetBarrelSource(ChainFirstCell)) && Subsystem->IsSourceSpent(Grid->GetBarrelSource(ChainSecondCell)));
	TestTrue(TEXT("sources distinct per cell"), Grid->GetBarrelSource(ChainFirstCell) != Grid->GetBarrelSource(ChainSecondCell));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRBarrelDigTest, "RogueRigs.Barrel.DigExplodes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRBarrelDigTest::RunTest(const FString& Parameters)
{
	using namespace RRBarrelTests;

	FTestWorld TestWorld(TEXT("RRBarrelDig"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, Recorder.Get()) : nullptr;
	if (!Subsystem)
	{
		return false;
	}
	TestEqual(TEXT("(30, 27) is a barrel"), Grid->GetCellType(DugBarrelCell), ERRCellType::Barrel);
	const float DigHealth = Grid->GetCellHealth(DugBarrelCell);
	AddInfo(FString::Printf(TEXT("barrel dig health %.2f dig-seconds"), DigHealth));

	// The drone's dig path: per-frame terrain damage at mining speed 1 until the block breaks.
	int32 DigFrames = 0;
	while (Grid->GetCellType(DugBarrelCell) == ERRCellType::Barrel && DigFrames < 600)
	{
		Grid->ApplyTerrainDamage(DugBarrelCell, FrameSeconds, ERRTerrainDamageSource::Drone);
		++DigFrames;
		TestEqual(TEXT("no explosion while it stands"), Recorder->GetExplosionRows().Num(), 0);
	}
	AddInfo(FString::Printf(TEXT("dug in %d frames"), DigFrames));
	TestEqual(TEXT("barrel destroyed by the dig"), Grid->GetCellType(DugBarrelCell), ERRCellType::Floor);
	TestEqual(TEXT("explosion queued"), Subsystem->GetPendingCount(), 1);

	TestWorld.Tick();
	TestEqual(TEXT("one explosion"), Recorder->GetExplosionRows().Num(), 1);
	if (Recorder->GetExplosionRows().Num() == 1)
	{
		TestEqual(TEXT("row Barrel"), Recorder->GetExplosionRows()[0], FName(TEXT("Barrel")));
		TestTrue(TEXT("at the barrel cell center"), Recorder->GetExplosionCenters()[0].Equals(Grid->GetCellCenter(DugBarrelCell.X, DugBarrelCell.Y)));
	}
	TestTrue(TEXT("source spent"), Subsystem->IsSourceSpent(Grid->GetBarrelSource(DugBarrelCell)));
	TestFalse(TEXT("a destroyed barrel cannot be queued again"), Subsystem->QueueExplosion(FVector::ZeroVector, TEXT("Barrel"), nullptr, Grid->GetBarrelSource(DugBarrelCell)));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
