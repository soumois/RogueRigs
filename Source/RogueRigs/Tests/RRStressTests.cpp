// Rogue Rigs - automation tests of the stress tooling (RogueRigs.Stress.*): run mode parsing, burst kill through the health set, barrel burst.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "Debug/RRStressSubsystem.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RRKamikaze.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "Grid/RRArenaGrid.h"
#include "Tests/RRTestHarness.h"

namespace RRStressTests
{
	using namespace RRTestHarness;

	/** Open floor cells of arena_m1 (same rows as the kamikaze tests). */
	const FIntPoint ChaserCell(44, 41);
	const FIntPoint KamikazeCell(40, 40);

	/** Spawns a native robot of class T with its feet on the floor of Cell. */
	template <typename T>
	T* SpawnRobot(FAutomationTestBase& Test, const FTestWorld& TestWorld, const ARRArenaGrid* Grid, FIntPoint Cell)
	{
		const float HalfHeight = GetDefault<T>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		T* Robot = TestWorld.Spawn<T>(T::StaticClass(), Grid->GetCellCenter(Cell.X, Cell.Y) + FVector(0.f, 0.f, HalfHeight + 2.f));
		Test.TestNotNull(TEXT("robot spawned"), Robot);
		return Robot;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRStressParseModeTest, "RogueRigs.Stress.ParseMode",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRStressParseModeTest::RunTest(const FString& Parameters)
{
	ERRStressMode Mode = ERRStressMode::Burst;
	TestTrue(TEXT("peace parses"), URRStressSubsystem::ParseMode(TEXT("Peace"), Mode) && Mode == ERRStressMode::Peace);
	TestTrue(TEXT("fire parses"), URRStressSubsystem::ParseMode(TEXT(" fire "), Mode) && Mode == ERRStressMode::Fire);
	TestTrue(TEXT("2 parses as burst"), URRStressSubsystem::ParseMode(TEXT("2"), Mode) && Mode == ERRStressMode::Burst);
	TestFalse(TEXT("unknown mode refused"), URRStressSubsystem::ParseMode(TEXT("storm"), Mode));
	TestEqual(TEXT("refused mode leaves the output"), Mode, ERRStressMode::Burst);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRStressBurstTest, "RogueRigs.Stress.BurstKillsAndBarrels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRStressBurstTest::RunTest(const FString& Parameters)
{
	using namespace RRStressTests;

	FTestWorld TestWorld(TEXT("RRStressBurst"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	URRExplosionSubsystem* Explosions = Grid ? TestWorld.Get()->GetSubsystem<URRExplosionSubsystem>() : nullptr;
	ARRChaser* Chaser = Explosions ? SpawnRobot<ARRChaser>(*this, TestWorld, Grid, ChaserCell) : nullptr;
	ARRKamikaze* Kamikaze = Chaser ? SpawnRobot<ARRKamikaze>(*this, TestWorld, Grid, KamikazeCell) : nullptr;
	if (!Kamikaze)
	{
		return false;
	}
	TestWorld.Tick();

	TestTrue(TEXT("chaser killed"), URRStressSubsystem::KillEnemy(Chaser));
	TestTrue(TEXT("chaser dead"), Chaser->IsDead());
	TestFalse(TEXT("a dead robot is not killed twice"), URRStressSubsystem::KillEnemy(Chaser));
	TestFalse(TEXT("null robot refused"), URRStressSubsystem::KillEnemy(nullptr));
	TestTrue(TEXT("kamikaze killed"), URRStressSubsystem::KillEnemy(Kamikaze));
	TestTrue(TEXT("killed kamikaze queued its explosion"), Explosions->IsSourceSpent(Kamikaze));

	int32 BarrelCells = 0;
	const FIntPoint Size = Grid->GetGridSize();
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			BarrelCells += Grid->GetCellType(FIntPoint(X, Y)) == ERRCellType::Barrel ? 1 : 0;
		}
	}
	TestTrue(TEXT("arena_m1 has barrels"), BarrelCells > 0);
	const int32 PendingBefore = Explosions->GetPendingCount();
	// Barrel fuse (task-114-01): every barrel is lit, none queued yet; lit barrels ignore a second call.
	TestEqual(TEXT("every barrel lit"), URRStressSubsystem::DestroyAllBarrels(Grid), BarrelCells);
	TestEqual(TEXT("every barrel fusing"), Grid->GetFusingBarrelCount(), BarrelCells);
	TestEqual(TEXT("nothing queued during the fuse"), Explosions->GetPendingCount(), PendingBefore);
	TestEqual(TEXT("fusing barrels ignored"), URRStressSubsystem::DestroyAllBarrels(Grid), 0);
	TestEqual(TEXT("null grid"), URRStressSubsystem::DestroyAllBarrels(nullptr), 0);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
