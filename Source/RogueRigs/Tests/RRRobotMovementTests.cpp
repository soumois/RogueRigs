// Rogue Rigs - automation tests of the robot light movement (RogueRigs.Enemies.LightMovement*, task-071-01): the light step reaches the miner around rock, shotgun knockback decays to the walk speed, the heavy walks slower than the chaser.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RRHeavy.h"
#include "Enemies/RRRobotMovementComponent.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRRobotMovementTests
{
	using namespace RRTestHarness;
	using namespace RRTestAccess;

	/** Spawn cell of arena_m1 (legacy layout): the miner of the path test. */
	const FIntPoint LightPathMinerCell(40, 40);

	/** Floor cell with rocks (38-39, 45-47) between it and LightPathMinerCell (as RogueRigs.FlowField.MovementComponentReachesMiner). */
	const FIntPoint LightPathRobotCell(38, 48);

	/** Open row 43 of arena_m1 (x 28 to 51 floor): miner on the west, the robot walks west along the row. */
	const FIntPoint LightOpenMinerCell(30, 43);
	const FIntPoint LightOpenRobotCell(48, 43);

	/** Spawns a native robot of class T with its feet on the floor at FloorLocation (XP drop: ARRTestPickupXP). */
	template <typename T>
	T* SpawnLightRobot(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FVector& FloorLocation)
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

	/** Grid and an unarmed miner on Cell (its weapons would shoot the robots); null when something failed. */
	ARRMiner* MakeLightArena(FAutomationTestBase& Test, const FTestWorld& TestWorld, FIntPoint Cell, ARRArenaGrid*& OutGrid)
	{
		OutGrid = SpawnGrid(Test, TestWorld);
		ARRMiner* Miner = OutGrid ? SpawnMiner(Test, TestWorld, OutGrid, Cell) : nullptr;
		if (Miner)
		{
			for (ARRWeapon* Weapon : Miner->GetWeapons())
			{
				if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
				{
					Weapon->GetAutoFireComponent()->StopFiring();
				}
			}
		}
		return Miner;
	}

	/** Light movement component of Robot, null (error) when the robot has another movement class. */
	URRRobotMovementComponent* GetRobotMovement(FAutomationTestBase& Test, const ARREnemy* Robot)
	{
		URRRobotMovementComponent* Movement = Robot ? Cast<URRRobotMovementComponent>(Robot->GetCharacterMovement()) : nullptr;
		Test.TestNotNull(TEXT("robot movement is URRRobotMovementComponent"), Movement);
		return Movement;
	}

	/** Walk speed (uu/s, 2D) of a robot of class T on the open row: 30 frames to start, then the distance over 30 frames. */
	template <typename T>
	float MeasureWalkSpeed(FAutomationTestBase& Test, float& OutMaxWalkSpeed, int32& OutLightMoves)
	{
		FTestWorld TestWorld(TEXT("RRRobotWalkSpeed"));
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = MakeLightArena(Test, TestWorld, LightOpenMinerCell, Grid);
		T* Robot = Miner ? SpawnLightRobot<T>(Test, TestWorld, Grid->GetCellCenter(LightOpenRobotCell.X, LightOpenRobotCell.Y)) : nullptr;
		const URRRobotMovementComponent* Movement = GetRobotMovement(Test, Robot);
		if (!Movement)
		{
			return 0.f;
		}
		for (int32 Frame = 0; Frame < 30; ++Frame)
		{
			TestWorld.Tick();
		}
		const FVector Start = Robot->GetActorLocation();
		constexpr int32 MeasureFrames = 30;
		for (int32 Frame = 0; Frame < MeasureFrames; ++Frame)
		{
			TestWorld.Tick();
		}
		OutMaxWalkSpeed = Movement->MaxWalkSpeed;
		OutLightMoves = Movement->GetLightMoveCount();
		return FVector::Dist2D(Start, Robot->GetActorLocation()) / (MeasureFrames * FrameSeconds);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLightMovementAroundRockTest, "RogueRigs.Enemies.LightMovementAroundRock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRLightMovementAroundRockTest::RunTest(const FString& Parameters)
{
	using namespace RRRobotMovementTests;

	FTestWorld TestWorld(TEXT("RRLightMovementAroundRock"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = MakeLightArena(*this, TestWorld, LightPathMinerCell, Grid);
	ARRChaser* Chaser = Miner ? SpawnLightRobot<ARRChaser>(*this, TestWorld, Grid->GetCellCenter(LightPathRobotCell.X, LightPathRobotCell.Y)) : nullptr;
	URRRobotMovementComponent* Movement = GetRobotMovement(*this, Chaser);
	URRFlowFieldSubsystem* FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
	if (!Movement || !TestNotNull(TEXT("flow field subsystem"), FlowField))
	{
		return false;
	}
	TestTrue(TEXT("rr.Enemy.LightMovement is on by default"), URRRobotMovementComponent::IsLightMovementEnabled());
	TestFalse(TEXT("no direct path from the start"), FlowField->HasDirectPath(Chaser->GetActorLocation(), Miner->GetActorLocation()));
	TestFalse(TEXT("root capsule generates no overlap events"), Chaser->GetCapsuleComponent()->GetGenerateOverlapEvents());

	const float StartZ = Chaser->GetActorLocation().Z;
	float MaxHeightChange = 0.f;
	int32 ArrivalFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= 900 && ArrivalFrame == INDEX_NONE; ++Frame)
	{
		TestWorld.Tick();
		if (Frame > 30)
		{
			MaxHeightChange = FMath::Max(MaxHeightChange, FMath::Abs(Chaser->GetActorLocation().Z - StartZ));
		}
		// The chaser stops at its attack range (its movement is then off, so HasArrived is not refreshed).
		if (FVector::Dist2D(Chaser->GetActorLocation(), Miner->GetActorLocation()) <= Chaser->GetStats().AttackRange + 1.f)
		{
			ArrivalFrame = Frame;
		}
	}
	const float Distance = FVector::Dist2D(Chaser->GetActorLocation(), Miner->GetActorLocation());
	AddInfo(FString::Printf(TEXT("arrived at frame %d (%.2f s), %.1f uu from the miner; light ticks %d, engine ticks %d, max height change %.2f uu"),
		ArrivalFrame, ArrivalFrame * FrameSeconds, Distance, Movement->GetLightMoveCount(), Movement->GetEngineMoveCount(), MaxHeightChange));
	TestTrue(TEXT("chaser reaches the miner around the rock within 15 s"), ArrivalFrame != INDEX_NONE);
	TestTrue(TEXT("the light step moved the robot (most ticks)"), Movement->GetLightMoveCount() > Movement->GetEngineMoveCount());
	TestTrue(TEXT("robot stayed on the floor"), Movement->IsMovingOnGround());
	TestTrue(TEXT("robot kept its floor height"), MaxHeightChange < 3.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLightMovementKnockbackTest, "RogueRigs.Enemies.LightMovementKnockbackDecays",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRLightMovementKnockbackTest::RunTest(const FString& Parameters)
{
	using namespace RRRobotMovementTests;

	FTestWorld TestWorld(TEXT("RRLightMovementKnockback"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = MakeLightArena(*this, TestWorld, LightOpenMinerCell, Grid);
	ARRChaser* Chaser = Miner ? SpawnLightRobot<ARRChaser>(*this, TestWorld, Grid->GetCellCenter(LightOpenRobotCell.X, LightOpenRobotCell.Y)) : nullptr;
	URRRobotMovementComponent* Movement = GetRobotMovement(*this, Chaser);
	if (!Movement)
	{
		return false;
	}
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	const int32 LightBefore = Movement->GetLightMoveCount();

	// One shotgun pellet (shotgun: 250 uu/s, chaser resistance from the CSV) pushes the walking chaser away from the miner.
	constexpr float PelletKnockback = 250.f;
	const FVector Away = (Chaser->GetActorLocation() - Miner->GetActorLocation()).GetSafeNormal2D();
	const float Expected = PelletKnockback * (1.f - Chaser->GetKnockbackResistance());
	const float StartDistance = FVector::Dist2D(Chaser->GetActorLocation(), Miner->GetActorLocation());
	Chaser->ApplyKnockback(Away * PelletKnockback);
	TestWorld.Tick();
	const float LaunchSpeed = Movement->Velocity.Size2D();
	const float LaunchAway = Movement->Velocity | Away;
	int32 DecayFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= 30 && DecayFrame == INDEX_NONE; ++Frame)
	{
		TestWorld.Tick();
		if (Movement->Velocity.Size2D() <= Movement->MaxWalkSpeed + 1.f && (Movement->Velocity | Away) < 0.f)
		{
			DecayFrame = Frame;
		}
	}
	AddInfo(FString::Printf(TEXT("launch %.1f uu/s (expected %.1f), away component %.1f; back to walking toward the miner after %d frames; moved %.1f uu"),
		LaunchSpeed, Expected, LaunchAway, DecayFrame, FVector::Dist2D(Chaser->GetActorLocation(), Miner->GetActorLocation()) - StartDistance));
	TestTrue(TEXT("launch frame used the light step"), Movement->GetLightMoveCount() > LightBefore);
	TestTrue(TEXT("launch velocity = pellet knockback x (1 - resistance)"), FMath::IsNearlyEqual(LaunchSpeed, Expected, 1.f) && LaunchAway > 0.f);
	TestTrue(TEXT("knockback decays: walking toward the miner again within 30 frames"), DecayFrame != INDEX_NONE);
	TestTrue(TEXT("robot stayed on the floor"), Movement->IsMovingOnGround());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRLightMovementHeavySlowerTest, "RogueRigs.Enemies.LightMovementHeavySlower",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRLightMovementHeavySlowerTest::RunTest(const FString& Parameters)
{
	using namespace RRRobotMovementTests;

	float ChaserMax = 0.f;
	float HeavyMax = 0.f;
	int32 ChaserLight = 0;
	int32 HeavyLight = 0;
	const float ChaserSpeed = MeasureWalkSpeed<ARRChaser>(*this, ChaserMax, ChaserLight);
	const float HeavySpeed = MeasureWalkSpeed<ARRHeavy>(*this, HeavyMax, HeavyLight);
	AddInfo(FString::Printf(TEXT("chaser %.1f uu/s (max %.1f, %d light ticks), heavy %.1f uu/s (max %.1f, %d light ticks)"),
		ChaserSpeed, ChaserMax, ChaserLight, HeavySpeed, HeavyMax, HeavyLight));
	TestTrue(TEXT("chaser walks at its MoveSpeed"), FMath::IsNearlyEqual(ChaserSpeed, ChaserMax, ChaserMax * 0.03f));
	TestTrue(TEXT("heavy walks at its MoveSpeed"), FMath::IsNearlyEqual(HeavySpeed, HeavyMax, HeavyMax * 0.03f));
	TestTrue(TEXT("heavy slower than the chaser"), HeavySpeed < ChaserSpeed - 10.f);
	TestTrue(TEXT("both moved with the light step"), ChaserLight > 0 && HeavyLight > 0);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
