// Rogue Rigs - automation tests of the barrel fuse and the 700 uu barrel explosion (RogueRigs.BarrelFuse.*, task-114-01):
// 1.5 s fuse with its Danger ground telegraph, damage ignored during the fuse, a barrel lit by an explosion fuses too,
// radius 700 on arena_m1_fine (cells destroyed and game-thread cost of one explosion).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Explosions/RRExplosionSubsystem.h"
#include "HAL/PlatformTime.h"
#include "RogueRigs.h"
#include "Telegraph/RRGroundTelegraphComponent.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"

namespace RRBarrelFuseTests
{
	using namespace RRTestHarness;

	/** Single barrel of arena_m1 (same cell as RogueRigs.Barrel.TwoRifleShotsExplode). */
	const FIntPoint BarrelCell(43, 6);

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

	/** Block cells of Grid whose centre is within Radius (2D) of Center, and those in the next ring of one cell outside it. */
	void CollectBlocks(const ARRArenaGrid* Grid, const FVector& Center, float Radius, TArray<FIntPoint>& OutInside, TArray<FIntPoint>& OutOutside)
	{
		const float CellSize = Grid->GetCellSize();
		const FIntPoint CenterCell = Grid->GetCellAtWorld(Center);
		const int32 Reach = FMath::CeilToInt32(Radius / CellSize) + 2;
		for (int32 Y = CenterCell.Y - Reach; Y <= CenterCell.Y + Reach; ++Y)
		{
			for (int32 X = CenterCell.X - Reach; X <= CenterCell.X + Reach; ++X)
			{
				const FIntPoint Cell(X, Y);
				if (!Grid->IsBlockCell(Cell))
				{
					continue;
				}
				const float Distance = FVector::Dist2D(Grid->GetCellCenter(X, Y), Center);
				if (Distance <= Radius)
				{
					OutInside.Add(Cell);
				}
				else if (Distance <= Radius + CellSize)
				{
					OutOutside.Add(Cell);
				}
			}
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRBarrelFuseTelegraphTest, "RogueRigs.BarrelFuse.FuseTelegraphAndIgnoredDamage",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRBarrelFuseTelegraphTest::RunTest(const FString& Parameters)
{
	using namespace RRBarrelFuseTests;

	FTestWorld TestWorld(TEXT("RRBarrelFuseTelegraph"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, Recorder.Get()) : nullptr;
	if (!Subsystem || !TestEqual(TEXT("(43, 6) is a barrel"), Grid->GetCellType(BarrelCell), ERRCellType::Barrel))
	{
		return false;
	}
	const float Fuse = Grid->GetBarrelFuseSeconds();
	TestNearlyEqual(TEXT("fuse 1.5 s"), Fuse, 1.5f);
	FRRExplosionStatsRow Stats;
	TestTrue(TEXT("Barrel row loads"), Subsystem->GetExplosionStats(TEXT("Barrel"), Stats));
	TestNull(TEXT("no telegraph before 0 health"), Grid->GetBarrelTelegraph(BarrelCell));

	TestTrue(TEXT("shot to 0 lights the fuse"), Grid->ApplyShotDamage(BarrelCell, Grid->GetCellShotHealth(BarrelCell)));
	const double Start = TestWorld.Get()->GetTimeSeconds();
	TestTrue(TEXT("fusing"), Grid->IsBarrelFusing(BarrelCell));
	TestEqual(TEXT("barrel stands during the fuse"), Grid->GetCellType(BarrelCell), ERRCellType::Barrel);
	TestEqual(TEXT("explosion not queued yet"), Subsystem->GetPendingCount(), 0);
	TestFalse(TEXT("source not spent yet"), Subsystem->IsSourceSpent(Grid->GetBarrelSource(BarrelCell)));

	URRGroundTelegraphComponent* Telegraph = Grid->GetBarrelTelegraph(BarrelCell);
	if (!TestNotNull(TEXT("telegraph from 0 health"), Telegraph))
	{
		return false;
	}
	TestTrue(TEXT("telegraph visible"), Telegraph->IsVisible());
	TestEqual(TEXT("telegraph mode Danger"), Telegraph->GetMode(), ERRGroundTelegraphMode::Danger);
	TestNearlyEqual(TEXT("telegraph radius = explosion radius"), Telegraph->GetRadius(), Stats.Radius);
	TestNearlyEqual(TEXT("telegraph progress 0 at 0 health"), Telegraph->GetProgress(), 0.f);
	TestTrue(TEXT("telegraph at the cell centre on the floor"), Telegraph->GetComponentLocation().Equals(Grid->GetCellCenter(BarrelCell.X, BarrelCell.Y), 0.1f));

	// Damage during the fuse is ignored: shots, dig, explosions (DestroyCell) and another explosion's blast.
	TestFalse(TEXT("shot ignored"), Grid->ApplyShotDamage(BarrelCell, 100.f));
	TestFalse(TEXT("dig ignored"), Grid->ApplyTerrainDamage(BarrelCell, 100.f, ERRTerrainDamageSource::Drone));
	TestFalse(TEXT("destroy ignored"), Grid->DestroyCell(BarrelCell, ERRTerrainDamageSource::Kamikaze));
	TestEqual(TEXT("still standing"), Grid->GetCellType(BarrelCell), ERRCellType::Barrel);

	float LastProgress = 0.f;
	float ProgressBeforeExplosion = 0.f;
	float ProgressAtExplosion = 0.f;
	bool bMonotonic = true;
	double ExplosionTime = -1.0;
	const int32 MaxFrames = FMath::CeilToInt32((Fuse + 0.5f) / FrameSeconds);
	for (int32 Frame = 0; Frame < MaxFrames && ExplosionTime < 0.0; ++Frame)
	{
		TestWorld.Tick();
		if (Recorder->GetExplosionRows().Num() > 0)
		{
			ExplosionTime = TestWorld.Get()->GetTimeSeconds() - Start;
			ProgressAtExplosion = Telegraph->GetProgress();
			break;
		}
		const float Progress = Telegraph->GetProgress();
		bMonotonic &= Progress >= LastProgress - 1e-4f;
		LastProgress = Progress;
		ProgressBeforeExplosion = Progress;
		const double Elapsed = TestWorld.Get()->GetTimeSeconds() - Start;
		if (Elapsed < Fuse - 2.f * FrameSeconds)
		{
			if (!TestNearlyEqual(TEXT("progress = elapsed / fuse"), Progress, static_cast<float>(Elapsed / Fuse), 2.f * FrameSeconds / Fuse))
			{
				break;
			}
			TestEqual(TEXT("barrel stands until the explosion"), Grid->GetCellType(BarrelCell), ERRCellType::Barrel);
		}
	}
	AddInfo(FString::Printf(TEXT("explosion %.4f s after 0 health (fuse %.2f), progress the frame before %.3f, at the explosion %.3f"), ExplosionTime, Fuse, ProgressBeforeExplosion, ProgressAtExplosion));
	TestTrue(TEXT("progress rises"), bMonotonic);
	TestTrue(TEXT("explosion after the fuse, within two frames"), ExplosionTime >= Fuse - 1e-4 && ExplosionTime < Fuse + 2.f * FrameSeconds);
	TestTrue(TEXT("progress one frame from 1 the frame before"), ProgressBeforeExplosion >= 1.f - 2.f * FrameSeconds / Fuse);
	TestNearlyEqual(TEXT("progress 1 at the explosion"), ProgressAtExplosion, 1.f);
	TestEqual(TEXT("barrel broken at the explosion"), Grid->GetCellType(BarrelCell), ERRCellType::Floor);
	TestFalse(TEXT("fuse over"), Grid->IsBarrelFusing(BarrelCell));
	TestFalse(TEXT("telegraph hidden after the explosion"), Telegraph->IsVisible());
	TestEqual(TEXT("one telegraph created, kept for reuse"), Grid->GetBarrelTelegraphPoolSize(), 1);
	TestEqual(TEXT("grid holds no fuse"), Grid->GetFusingBarrelCount(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRBarrelFuseExplosionLitTest, "RogueRigs.BarrelFuse.ExplosionLitBarrelFuses",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRBarrelFuseExplosionLitTest::RunTest(const FString& Parameters)
{
	using namespace RRBarrelFuseTests;

	FTestWorld TestWorld(TEXT("RRBarrelFuseExplosionLit"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, Recorder.Get()) : nullptr;
	if (!Subsystem || !TestEqual(TEXT("(43, 6) is a barrel"), Grid->GetCellType(BarrelCell), ERRCellType::Barrel))
	{
		return false;
	}
	// A kamikaze blast (no source) next to the barrel: the barrel fuses instead of exploding in the same chain step.
	const FVector Center = Grid->GetCellCenter(BarrelCell.X, BarrelCell.Y) + FVector(-100.f, 0.f, 0.f);
	TestTrue(TEXT("kamikaze explosion queued"), Subsystem->QueueExplosion(Center, TEXT("Kamikaze"), nullptr, nullptr));
	TestWorld.Tick();
	const double Start = TestWorld.Get()->GetTimeSeconds();
	TestEqual(TEXT("kamikaze explosion resolved"), Recorder->GetExplosionRows().Num(), 1);
	TestTrue(TEXT("barrel lit by the explosion"), Grid->IsBarrelFusing(BarrelCell));
	TestEqual(TEXT("barrel stands during its fuse"), Grid->GetCellType(BarrelCell), ERRCellType::Barrel);
	URRGroundTelegraphComponent* Telegraph = Grid->GetBarrelTelegraph(BarrelCell);
	TestTrue(TEXT("telegraph shown"), Telegraph && Telegraph->IsVisible());

	for (int32 Frame = 0; Frame < 10; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("no barrel explosion after a chain delay"), Recorder->GetExplosionRows().Num(), 1);

	int32 Frames = 0;
	while (Recorder->GetExplosionRows().Num() < 2 && Frames * FrameSeconds < Grid->GetBarrelFuseSeconds() + 0.5f)
	{
		TestWorld.Tick();
		++Frames;
	}
	const double Elapsed = TestWorld.Get()->GetTimeSeconds() - Start;
	AddInfo(FString::Printf(TEXT("barrel explosion %.4f s after the kamikaze blast"), Elapsed));
	if (TestEqual(TEXT("barrel exploded"), Recorder->GetExplosionRows().Num(), 2))
	{
		TestEqual(TEXT("second row Barrel"), Recorder->GetExplosionRows()[1], FName(TEXT("Barrel")));
		TestTrue(TEXT("after the fuse"), Elapsed >= Grid->GetBarrelFuseSeconds() - FrameSeconds && Elapsed < Grid->GetBarrelFuseSeconds() + 2.f * FrameSeconds);
	}
	TestEqual(TEXT("barrel broken"), Grid->GetCellType(BarrelCell), ERRCellType::Floor);
	TestTrue(TEXT("telegraph hidden"), Telegraph && !Telegraph->IsVisible());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRBarrelFuseRadiusFineTest, "RogueRigs.BarrelFuse.Radius700FineGridAndCost",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRBarrelFuseRadiusFineTest::RunTest(const FString& Parameters)
{
	using namespace RRBarrelFuseTests;

	FTestWorld TestWorld(TEXT("RRBarrelFuseRadiusFine"));
	ARRArenaGrid* Grid = SpawnFineGrid(*this, TestWorld);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	URRExplosionSubsystem* Subsystem = Grid ? GetSubsystem(*this, TestWorld, Recorder.Get()) : nullptr;
	FRRExplosionStatsRow Stats;
	if (!Subsystem || !TestTrue(TEXT("Barrel row loads"), Subsystem->GetExplosionStats(TEXT("Barrel"), Stats)))
	{
		return false;
	}
	TestEqual(TEXT("Barrel radius 700"), Stats.Radius, 700.f);

	// Worst case: the candidate centre (every 6 cells, away from the border) with the most blocks in the radius and no barrel in it.
	const FIntPoint Size = Grid->GetGridSize();
	const int32 Margin = FMath::CeilToInt32(Stats.Radius / Grid->GetCellSize()) + 2;
	FIntPoint BestCell(-1, -1);
	int32 BestCount = 0;
	for (int32 Y = Margin; Y < Size.Y - Margin; Y += 6)
	{
		for (int32 X = Margin; X < Size.X - Margin; X += 6)
		{
			TArray<FIntPoint> Inside;
			TArray<FIntPoint> Outside;
			CollectBlocks(Grid, Grid->GetCellCenter(X, Y), Stats.Radius, Inside, Outside);
			const bool bHasBarrel = Inside.ContainsByPredicate([Grid](const FIntPoint& Cell) { return Grid->GetCellType(Cell) == ERRCellType::Barrel; });
			if (!bHasBarrel && Inside.Num() > BestCount)
			{
				BestCount = Inside.Num();
				BestCell = FIntPoint(X, Y);
			}
		}
	}
	if (!TestTrue(TEXT("a dense centre found"), BestCount > 0))
	{
		return false;
	}
	const FVector Center = Grid->GetCellCenter(BestCell.X, BestCell.Y);
	TArray<FIntPoint> Inside;
	TArray<FIntPoint> Outside;
	CollectBlocks(Grid, Center, Stats.Radius, Inside, Outside);

	// Warm-up explosion far from the centre (first FX class load and first spawn are one-off costs), then a baseline frame,
	// then the frame that resolves the measured explosion (queue idle: resolves on the next tick).
	const FVector WarmUp = FVector::Dist2D(Grid->GetCellCenter(Margin, Margin), Center) > 3.f * Stats.Radius
		? Grid->GetCellCenter(Margin, Margin) : Grid->GetCellCenter(Size.X - Margin, Size.Y - Margin);
	TestTrue(TEXT("warm-up explosion queued"), Subsystem->QueueExplosion(WarmUp, TEXT("Kamikaze"), nullptr, nullptr));
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestWorld.Tick();
	double T0 = FPlatformTime::Seconds();
	TestWorld.Tick();
	const double BaselineMs = (FPlatformTime::Seconds() - T0) * 1000.0;
	TestTrue(TEXT("explosion queued"), Subsystem->QueueExplosion(Center, TEXT("Barrel"), nullptr, nullptr));
	T0 = FPlatformTime::Seconds();
	TestWorld.Tick();
	const double ExplosionFrameMs = (FPlatformTime::Seconds() - T0) * 1000.0;
	TestEqual(TEXT("warm-up and measured explosions resolved"), Recorder->GetExplosionRows().Num(), 2);

	int32 Kept = 0;
	for (const FIntPoint& Cell : Inside)
	{
		Kept += Grid->IsBlockCell(Cell) ? 1 : 0;
	}
	TestEqual(TEXT("every block inside 700 uu destroyed"), Kept, 0);
	int32 Lost = 0;
	for (const FIntPoint& Cell : Outside)
	{
		Lost += Grid->IsBlockCell(Cell) ? 0 : 1;
	}
	TestEqual(TEXT("blocks of the next ring kept"), Lost, 0);
	TestTrue(TEXT("blocks next to the radius exist"), Outside.Num() > 0);

	// Grid batch alone on a second dense centre (same rule, direct call), for the destruction share of the frame.
	FIntPoint SecondCell(-1, -1);
	int32 SecondCount = 0;
	for (int32 Y = Margin; Y < Size.Y - Margin; Y += 6)
	{
		for (int32 X = Margin; X < Size.X - Margin; X += 6)
		{
			if (FVector::Dist2D(Grid->GetCellCenter(X, Y), Center) < 3.f * Stats.Radius)
			{
				continue;
			}
			TArray<FIntPoint> In;
			TArray<FIntPoint> Out;
			CollectBlocks(Grid, Grid->GetCellCenter(X, Y), Stats.Radius, In, Out);
			const bool bHasBarrel = In.ContainsByPredicate([Grid](const FIntPoint& Cell) { return Grid->GetCellType(Cell) == ERRCellType::Barrel; });
			if (!bHasBarrel && In.Num() > SecondCount)
			{
				SecondCount = In.Num();
				SecondCell = FIntPoint(X, Y);
			}
		}
	}
	double BatchMs = -1.0;
	int32 BatchCount = 0;
	if (SecondCount > 0)
	{
		T0 = FPlatformTime::Seconds();
		BatchCount = Grid->DestroyCellsInRadius(Grid->GetCellCenter(SecondCell.X, SecondCell.Y), Stats.Radius, ERRTerrainDamageSource::Barrel);
		BatchMs = (FPlatformTime::Seconds() - T0) * 1000.0;
	}

	const FString Report = FString::Printf(TEXT("700 uu explosion on arena_m1_fine: centre (%d, %d), %d blocks destroyed; explosion frame %.2f ms (baseline frame %.2f ms, difference %.2f ms); DestroyCellsInRadius alone %.2f ms for %d blocks"),
		BestCell.X, BestCell.Y, Inside.Num(), ExplosionFrameMs, BaselineMs, ExplosionFrameMs - BaselineMs, BatchMs, BatchCount);
	AddInfo(Report);
	UE_LOG(LogRogueRigs, Display, TEXT("BarrelFuse cost: %s"), *Report);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
