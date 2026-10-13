// Rogue Rigs - automation tests of pickups from the actor pool (RogueRigs.PickupPool.*): task-092-01, XP cubes, ore and heal drops reused through URRActorPoolSubsystem.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Pickups/RRPickup.h"
#include "Pickups/RRPickupSubsystem.h"
#include "Player/RRMiner.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Progression/RRProgressionComponent.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRPickupPoolTests
{
	using RRTestHarness::FTestWorld;

	/** Pickup Blueprints dropped by heal and ore veins. */
	const TCHAR* const HealPickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal.BP_PickupHeal_C");
	const TCHAR* const OrePickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupOre.BP_PickupOre_C");

	/** Open floor cell of the legacy arena_m1 layout (as the pickup heal tests). */
	const FIntPoint MinerCell(40, 40);

	/** Spawns BP_Miner on MinerCell, possessed (pickups look for the player 0 pawn), weapons stopped, MaxHealth 100 and Health set. */
	ARRMiner* SpawnMiner(FAutomationTestBase& Test, const FTestWorld& TestWorld, float Health)
	{
		ARRArenaGrid* Grid = RRTestHarness::SpawnGrid(Test, TestWorld);
		ARRMiner* Miner = Grid ? RRTestHarness::SpawnMiner(Test, TestWorld, Grid, MinerCell) : nullptr;
		URRAbilitySystemComponent* ASC = Miner ? Miner->GetRRAbilitySystemComponent() : nullptr;
		if (!Test.TestNotNull(TEXT("miner ASC"), ASC))
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
		APlayerController* Controller = TestWorld.Spawn<APlayerController>(APlayerController::StaticClass(), Miner->GetActorLocation());
		if (!Test.TestNotNull(TEXT("player controller spawned"), Controller))
		{
			return nullptr;
		}
		Controller->Possess(Miner);
		ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), 100.f);
		ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), Health);
		return Miner;
	}

	/** Miner feet (pickups requested here rest within the pickup radius). */
	FVector MinerFeet(const ARRMiner* Miner)
	{
		return Miner->GetActorLocation() - FVector(0.f, 0.f, Miner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	}

	/** Current Health of the miner. */
	float HealthOf(const ARRMiner* Miner)
	{
		return Miner->GetRRAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Number of entries of Type in the recorded collections. */
	int32 CountCollected(const URRTestEventRecorder* Recorder, ERRPickupType Type)
	{
		int32 Count = 0;
		for (const ERRPickupType Collected : Recorder->GetCollectedPickups())
		{
			Count += Collected == Type ? 1 : 0;
		}
		return Count;
	}

	/** Live (not pooled idle) pickups of the world, and their summed XP. */
	int32 CountLivePickups(UWorld* World, float& OutXP)
	{
		int32 Count = 0;
		OutXP = 0.f;
		for (TActorIterator<ARRPickup> It(World); It; ++It)
		{
			if (!URRActorPoolSubsystem::IsPooledIdle(*It))
			{
				++Count;
				OutXP += It->GetXPAmount();
			}
		}
		return Count;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPickupPoolReuseTest, "RogueRigs.PickupPool.ReuseAfterCollect",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPickupPoolReuseTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupPoolTests;

	FTestWorld TestWorld(TEXT("RRPickupPoolReuse"));
	ARRMiner* Miner = SpawnMiner(*this, TestWorld, 100.f);
	URRPickupSubsystem* Pickups = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>();
	URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
	if (!Miner || !TestNotNull(TEXT("pickup subsystem"), Pickups) || !TestNotNull(TEXT("actor pool"), Pool))
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);
	const FVector Feet = MinerFeet(Miner);

	// First cube: the pool is empty, so its creation is queued and delivered within 3 frames.
	TestNull(TEXT("first cube queued"), Pickups->RequestXPCube(ARRTestPickupXP::StaticClass(), Feet, 2.f, 0.f));
	TestEqual(TEXT("one creation pending"), Pickups->GetQueuedSpawnCount(), 1);
	for (int32 Frame = 0; Frame < 3 && Pickups->GetQueuedSpawnCount() > 0; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("delivered within 3 frames"), Pickups->GetQueuedSpawnCount(), 0);
	TestEqual(TEXT("one live pickup"), Pickups->GetPickupCount(), 1);
	ARRPickup* First = nullptr;
	for (TActorIterator<ARRPickup> It(TestWorld.Get()); It; ++It)
	{
		First = *It;
	}
	if (!TestNotNull(TEXT("first cube"), First))
	{
		return false;
	}
	TestTrue(TEXT("cube is a pool member"), Pool->IsPoolMember(First));
	TestNearlyEqual(TEXT("first cube worth 2 XP"), First->GetXPAmount(), 2.f, 0.001f);

	// Collected: released, not destroyed, and no longer counted.
	for (int32 Frame = 0; Frame < 240 && CountCollected(Recorder.Get(), ERRPickupType::XP) == 0; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("first cube collected"), CountCollected(Recorder.Get(), ERRPickupType::XP), 1);
	TestTrue(TEXT("collected cube kept alive"), IsValid(First) && !First->IsActorBeingDestroyed());
	TestTrue(TEXT("collected cube pooled idle"), URRActorPoolSubsystem::IsPooledIdle(First));
	TestEqual(TEXT("pooled cube not registered"), Pickups->GetPickupCount(), 0);
	float LiveXP = 0.f;
	TestEqual(TEXT("pooled cube not counted by iterators"), CountLivePickups(TestWorld.Get(), LiveXP), 0);
	TestTrue(TEXT("pooled cube hidden"), First->IsHidden());

	// Second request: the same actor comes back at once, reset for a new life at the new point with the new value.
	const FVector Second = Feet + FVector(50.f, 0.f, 0.f);
	ARRPickup* Reused = Pickups->RequestXPCube(ARRTestPickupXP::StaticClass(), Second, 3.f, 90.f);
	TestTrue(TEXT("same cube reused at once"), Reused == First);
	if (!Reused)
	{
		return false;
	}
	TestFalse(TEXT("reused cube visible"), Reused->IsHidden());
	TestEqual(TEXT("reused cube pops again"), Reused->GetPhase(), ERRPickupPhase::Pop);
	TestFalse(TEXT("magnet latch cleared"), Reused->IsMagnetized());
	TestEqual(TEXT("magnet speed cleared"), Reused->GetMagnetSpeed(), 0.f);
	TestNearlyEqual(TEXT("reused cube worth 3 XP"), Reused->GetXPAmount(), 3.f, 0.001f);
	TestTrue(TEXT("pop starts at the new point"), Reused->GetPopStart().Equals(Second, 0.5f));
	const FVector PopDirection = (Reused->GetPopEnd() - Reused->GetPopStart()).GetSafeNormal2D();
	TestTrue(TEXT("pop follows the new yaw (90 degrees)"), PopDirection.Equals(FVector(0.f, 1.f, 0.f), 0.01f));
	TestEqual(TEXT("reused cube registered once"), Pickups->GetPickupCount(), 1);
	TestTrue(TEXT("rotation reset"), Reused->GetActorRotation().IsNearlyZero(0.01f));
	TestEqual(TEXT("still one cube created"), Pickups->GetPoolStats().TotalCreated, 1);

	for (int32 Frame = 0; Frame < 240 && CountCollected(Recorder.Get(), ERRPickupType::XP) < 2; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("reused cube collected"), CountCollected(Recorder.Get(), ERRPickupType::XP), 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPickupPoolSpreadTest, "RogueRigs.PickupPool.MassDropWithin3Frames",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPickupPoolSpreadTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupPoolTests;

	// 20 robots of 3 cubes worth 5 XP each die on the same frame: 60 cubes, created over 3 frames (20 per frame), no XP lost.
	FTestWorld TestWorld(TEXT("RRPickupPoolSpread"));
	URRPickupSubsystem* Pickups = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>();
	URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
	if (!TestNotNull(TEXT("pickup subsystem"), Pickups) || !TestNotNull(TEXT("actor pool"), Pool))
	{
		return false;
	}
	TestEqual(TEXT("default SpawnMaxFrames 3"), Pickups->GetSpawnMaxFrames(), 3);
	constexpr int32 Robots = 20;
	constexpr int32 CubesPerRobot = 3;
	constexpr float RobotXP = 5.f;
	for (int32 Robot = 0; Robot < Robots; ++Robot)
	{
		for (int32 Cube = 0; Cube < CubesPerRobot; ++Cube)
		{
			Pickups->RequestXPCube(ARRTestPickupXP::StaticClass(), FVector(200.f * Robot, 0.f, 100.f), RobotXP / CubesPerRobot, 120.f * Cube);
		}
	}
	TestEqual(TEXT("60 creations pending"), Pickups->GetQueuedSpawnCount(), Robots * CubesPerRobot);
	int32 Frames = 0;
	int32 MaxCreated = 0;
	while (Pickups->GetQueuedSpawnCount() > 0 && Frames < 10)
	{
		TestWorld.Tick();
		++Frames;
		MaxCreated = FMath::Max(MaxCreated, Pickups->GetPoolStats().CreatedThisFrame);
	}
	AddInfo(FString::Printf(TEXT("PICKUPPOOL 60 cubes created in %d frames, at most %d per frame, longest wait %d"), Frames, MaxCreated,
		Pickups->GetPoolStats().LongestWaitFrames));
	TestTrue(TEXT("every cube created within 3 frames"), Frames <= 3);
	TestEqual(TEXT("at most 20 created per frame"), MaxCreated, 20);
	TestTrue(TEXT("longest wait at most 3 frames"), Pickups->GetPoolStats().LongestWaitFrames <= 3);
	float LiveXP = 0.f;
	TestEqual(TEXT("60 live cubes"), CountLivePickups(TestWorld.Get(), LiveXP), Robots * CubesPerRobot);
	TestNearlyEqual(TEXT("no XP lost (20 x 5)"), LiveXP, Robots * RobotXP, 0.01f);

	// Half released (collected): pooled cubes are counted neither by the subsystem nor by iterators.
	int32 Released = 0;
	for (TActorIterator<ARRPickup> It(TestWorld.Get()); It && Released < 30; ++It)
	{
		Released += Pool->Release(*It) ? 1 : 0;
	}
	TestEqual(TEXT("30 live after 30 releases"), CountLivePickups(TestWorld.Get(), LiveXP), 30);
	TestEqual(TEXT("30 registered"), Pickups->GetPickupCount(), 30);
	TestEqual(TEXT("30 idle in the pool"), Pickups->GetPoolStats().Idle, 30);

	// A second mass drop of 60: 30 reused at once, the other 30 created within 3 frames.
	for (int32 Cube = 0; Cube < 60; ++Cube)
	{
		Pickups->RequestXPCube(ARRTestPickupXP::StaticClass(), FVector(0.f, 300.f, 100.f), 1.f, 6.f * Cube);
	}
	TestEqual(TEXT("30 reused at once, 30 pending"), Pickups->GetQueuedSpawnCount(), 30);
	for (int32 Frame = 0; Frame < 3; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("all delivered"), Pickups->GetQueuedSpawnCount(), 0);
	TestEqual(TEXT("90 live cubes"), CountLivePickups(TestWorld.Get(), LiveXP), 90);
	TestNearlyEqual(TEXT("value kept: 30 old cubes worth 5/3 + 60 new worth 1"), LiveXP, 30.f * RobotXP / CubesPerRobot + 60.f, 0.01f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPickupPoolValuesTest, "RogueRigs.PickupPool.OreAndHealValuesReset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPickupPoolValuesTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupPoolTests;

	// Ore and heal pickups keep their request values on reuse; a heal request with 0 gets the class default, never the last value.
	FTestWorld TestWorld(TEXT("RRPickupPoolValues"));
	UClass* OreClass = RRTestHarness::LoadTestClass<ARRPickup>(*this, OrePickupClassPath);
	UClass* HealClass = RRTestHarness::LoadTestClass<ARRPickup>(*this, HealPickupClassPath);
	URRPickupSubsystem* Pickups = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>();
	URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
	if (!OreClass || !HealClass || !Pickups || !Pool)
	{
		return false;
	}
	Pickups->SetSpawnMaxFrames(0);
	ARRPickup* Ore = Pickups->RequestSpawn(OreClass, FVector(0.f, 0.f, 100.f), nullptr, 5);
	ARRPickup* Heal = Pickups->RequestSpawn(HealClass, FVector(100.f, 0.f, 100.f), nullptr, 1, 15.f);
	if (!TestNotNull(TEXT("ore delivered at once (SpawnMaxFrames 0)"), Ore) || !TestNotNull(TEXT("heal delivered"), Heal))
	{
		return false;
	}
	TestEqual(TEXT("ore worth 5"), Ore->GetOreValue(), 5);
	TestNearlyEqual(TEXT("heal 15"), Heal->GetHealAmount(), 15.f, 0.001f);
	TestTrue(TEXT("ore released"), Pool->Release(Ore));
	TestTrue(TEXT("heal released"), Pool->Release(Heal));

	Pickups->SetSpawnMaxFrames(3);
	ARRPickup* Ore2 = Pickups->RequestSpawn(OreClass, FVector(0.f, 0.f, 100.f), nullptr, 1);
	ARRPickup* Heal2 = Pickups->RequestSpawn(HealClass, FVector(100.f, 0.f, 100.f), nullptr, 1, 0.f);
	TestTrue(TEXT("ore reused at once"), Ore2 == Ore);
	TestTrue(TEXT("heal reused at once"), Heal2 == Heal);
	if (!Ore2 || !Heal2)
	{
		return false;
	}
	TestEqual(TEXT("reused ore worth the new value 1"), Ore2->GetOreValue(), 1);
	TestNearlyEqual(TEXT("reused heal with 0 gets the class default"), Heal2->GetHealAmount(), GetDefault<ARRPickup>(HealClass)->GetHealAmount(), 0.001f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPickupPoolHealFullHealthTest, "RogueRigs.PickupPool.ReusedHealRefusedAtFullHealth",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPickupPoolHealFullHealthTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupPoolTests;

	// A heal pickup reused from the pool still waits on the floor while the miner is at full health, then is collected (even
	// though it pops inside the pickup radius) once the miner is damaged.
	FTestWorld TestWorld(TEXT("RRPickupPoolHeal"));
	UClass* HealClass = RRTestHarness::LoadTestClass<ARRPickup>(*this, HealPickupClassPath);
	ARRMiner* Miner = HealClass ? SpawnMiner(*this, TestWorld, 70.f) : nullptr;
	URRPickupSubsystem* Pickups = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>();
	if (!Miner || !TestNotNull(TEXT("pickup subsystem"), Pickups))
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);
	const FVector Feet = MinerFeet(Miner);

	// First life: damaged miner collects it (70 + 15), then it goes back to the pool.
	Pickups->SetSpawnMaxFrames(0);
	ARRPickup* Heal = Pickups->RequestSpawn(HealClass, Feet, nullptr, 1, 15.f);
	if (!TestNotNull(TEXT("heal pickup"), Heal))
	{
		return false;
	}
	for (int32 Frame = 0; Frame < 240 && CountCollected(Recorder.Get(), ERRPickupType::Heal) == 0; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("first life collected"), CountCollected(Recorder.Get(), ERRPickupType::Heal), 1);
	TestNearlyEqual(TEXT("70 + 15 = 85"), HealthOf(Miner), 85.f, 0.01f);
	TestTrue(TEXT("heal pickup pooled"), URRActorPoolSubsystem::IsPooledIdle(Heal));

	// Second life at full health, popping inside the radius: refused, rests.
	Miner->GetRRAbilitySystemComponent()->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 100.f);
	ARRPickup* Reused = Pickups->RequestSpawn(HealClass, Feet, nullptr, 1, 15.f);
	TestTrue(TEXT("same heal pickup reused"), Reused == Heal);
	if (!Reused)
	{
		return false;
	}
	bool bEverMagnetized = false;
	for (int32 Frame = 0; Frame < 240; ++Frame)
	{
		TestWorld.Tick();
		bEverMagnetized |= Reused->IsMagnetized();
	}
	TestEqual(TEXT("reused heal not collected at full health"), CountCollected(Recorder.Get(), ERRPickupType::Heal), 1);
	TestFalse(TEXT("reused heal never attracted at full health"), bEverMagnetized);
	TestFalse(TEXT("reused heal still live"), URRActorPoolSubsystem::IsPooledIdle(Reused));
	TestEqual(TEXT("reused heal resting"), Reused->GetPhase(), ERRPickupPhase::Rest);
	TestNearlyEqual(TEXT("health still 100"), HealthOf(Miner), 100.f, 0.01f);

	// Damaged: collected, heals 15.
	Miner->GetRRAbilitySystemComponent()->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 70.f);
	for (int32 Frame = 0; Frame < 120 && CountCollected(Recorder.Get(), ERRPickupType::Heal) < 2; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("reused heal collected once damaged"), CountCollected(Recorder.Get(), ERRPickupType::Heal), 2);
	TestNearlyEqual(TEXT("70 + 15 = 85 again"), HealthOf(Miner), 85.f, 0.01f);
	return true;
}

namespace RRPickupPoolTests
{
	/** Live (not pooled idle) pickups of exactly Class, and their summed XP. */
	int32 CountLiveOfClass(UWorld* World, const UClass* Class, float& OutXP)
	{
		int32 Count = 0;
		OutXP = 0.f;
		for (TActorIterator<ARRPickup> It(World); It; ++It)
		{
			if (It->GetClass() == Class && !URRActorPoolSubsystem::IsPooledIdle(*It))
			{
				++Count;
				OutXP += It->GetXPAmount();
			}
		}
		return Count;
	}

	/** Requests Count merging test cubes worth XP each at Location (pop yaws spread). */
	void RequestMergingCubes(URRPickupSubsystem* Pickups, const FVector& Location, int32 Count, float XP)
	{
		for (int32 Cube = 0; Cube < Count; ++Cube)
		{
			Pickups->RequestXPCube(ARRTestPickupXPMerging::StaticClass(), Location, XP, 360.f * Cube / Count);
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRXPBigBlockMergeTest, "RogueRigs.PickupPool.XPBigBlock.MergeRule",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRXPBigBlockMergeTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupPoolTests;

	// task-110-01: per frame and per 200 uu bucket, more than 8 cubes = groups of 8 become one big block, total XP kept.
	FTestWorld TestWorld(TEXT("RRXPBigBlockMerge"));
	URRPickupSubsystem* Pickups = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>();
	if (!TestNotNull(TEXT("pickup subsystem"), Pickups))
	{
		return false;
	}
	Pickups->SetSpawnMaxFrames(0);
	UClass* Small = ARRTestPickupXPMerging::StaticClass();
	UClass* Big = ARRTestPickupXPBig::StaticClass();
	float SmallXP = 0.f;
	float BigXP = 0.f;

	// 8 cubes: collected for the frame, then delivered unchanged.
	TestNull(TEXT("merging cube request collected"), Pickups->RequestXPCube(Small, FVector(50.f, 50.f, 100.f), 1.f, 0.f));
	RequestMergingCubes(Pickups, FVector(50.f, 50.f, 100.f), 7, 1.f);
	TestEqual(TEXT("8 requests pending"), Pickups->GetPendingXPCubeCount(), 8);
	Pickups->FlushXPCubes();
	TestEqual(TEXT("none pending after flush"), Pickups->GetPendingXPCubeCount(), 0);
	TestEqual(TEXT("8 cubes stay small"), CountLiveOfClass(TestWorld.Get(), Small, SmallXP), 8);
	TestEqual(TEXT("no big block for 8"), CountLiveOfClass(TestWorld.Get(), Big, BigXP), 0);
	for (TActorIterator<ARRPickup> It(TestWorld.Get()); It; ++It)
	{
		if (!URRActorPoolSubsystem::IsPooledIdle(*It)) { TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>()->Release(*It); }
	}

	// 20 cubes worth 1.5 in one bucket, resolved by the subsystem tick: 2 big (12 XP each) + 4 small, 30 XP in total.
	RequestMergingCubes(Pickups, FVector(50.f, 50.f, 100.f), 20, 1.5f);
	TestWorld.Tick();
	TestEqual(TEXT("tick flushed the requests"), Pickups->GetPendingXPCubeCount(), 0);
	TestEqual(TEXT("2 big blocks"), CountLiveOfClass(TestWorld.Get(), Big, BigXP), 2);
	TestEqual(TEXT("4 small cubes left"), CountLiveOfClass(TestWorld.Get(), Small, SmallXP), 4);
	TestNearlyEqual(TEXT("big blocks worth 2 x 12"), BigXP, 24.f, 0.001f);
	TestNearlyEqual(TEXT("total XP kept (20 x 1.5)"), BigXP + SmallXP, 30.f, 0.001f);
	for (TActorIterator<ARRPickup> It(TestWorld.Get()); It; ++It)
	{
		if (!URRActorPoolSubsystem::IsPooledIdle(*It)) { TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>()->Release(*It); }
	}

	// Two buckets far apart, 6 cubes each: 12 in the frame but no bucket above 8, nothing merged.
	RequestMergingCubes(Pickups, FVector(50.f, 50.f, 100.f), 6, 1.f);
	RequestMergingCubes(Pickups, FVector(1050.f, 50.f, 100.f), 6, 1.f);
	Pickups->FlushXPCubes();
	TestEqual(TEXT("far buckets: 12 small cubes"), CountLiveOfClass(TestWorld.Get(), Small, SmallXP), 12);
	TestEqual(TEXT("far buckets: no big block"), CountLiveOfClass(TestWorld.Get(), Big, BigXP), 0);
	TestEqual(TEXT("2 big blocks requested in total"), Pickups->GetTotalXPBigBlocksRequested(), 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRXPBigBlockOffTest, "RogueRigs.PickupPool.XPBigBlock.OffWithoutClass",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRXPBigBlockOffTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupPoolTests;

	// task-110-01: a cube class without XPBigPickupClass is never collected nor merged: delivered at once as before.
	FTestWorld TestWorld(TEXT("RRXPBigBlockOff"));
	URRPickupSubsystem* Pickups = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>();
	if (!TestNotNull(TEXT("pickup subsystem"), Pickups))
	{
		return false;
	}
	TestNull(TEXT("ARRTestPickupXP has no big block class"), GetDefault<ARRPickup>(ARRTestPickupXP::StaticClass())->GetXPBigPickupClass().Get());
	Pickups->SetSpawnMaxFrames(0);
	int32 Delivered = 0;
	for (int32 Cube = 0; Cube < 20; ++Cube)
	{
		Delivered += Pickups->RequestXPCube(ARRTestPickupXP::StaticClass(), FVector(50.f, 50.f, 100.f), 1.5f, 18.f * Cube) ? 1 : 0;
	}
	TestEqual(TEXT("20 cubes delivered at once"), Delivered, 20);
	TestEqual(TEXT("nothing collected for a merge"), Pickups->GetPendingXPCubeCount(), 0);
	TestWorld.Tick();
	float XP = 0.f;
	TestEqual(TEXT("still 20 small cubes after the tick"), CountLiveOfClass(TestWorld.Get(), ARRTestPickupXP::StaticClass(), XP), 20);
	TestNearlyEqual(TEXT("30 XP"), XP, 30.f, 0.001f);
	TestEqual(TEXT("no big block requested"), Pickups->GetTotalXPBigBlocksRequested(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRXPBigBlockCollectTest, "RogueRigs.PickupPool.XPBigBlock.CollectedXPEqualsDropped",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRXPBigBlockCollectTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupPoolTests;

	// task-110-01: 20 cubes of 0.15 XP at the miner's feet (2 big + 4 small, under the 4 XP of level 1): all collected, 3 XP gained.
	FTestWorld TestWorld(TEXT("RRXPBigBlockCollect"));
	ARRMiner* Miner = SpawnMiner(*this, TestWorld, 100.f);
	URRPickupSubsystem* Pickups = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>();
	if (!Miner || !TestNotNull(TEXT("pickup subsystem"), Pickups) || !TestNotNull(TEXT("miner progression"), Miner->GetProgression()))
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);
	const float StartXP = Miner->GetProgression()->GetXP();
	RequestMergingCubes(Pickups, MinerFeet(Miner), 20, 0.15f);
	for (int32 Frame = 0; Frame < 300 && CountCollected(Recorder.Get(), ERRPickupType::XP) < 6; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("2 big blocks + 4 cubes collected"), CountCollected(Recorder.Get(), ERRPickupType::XP), 6);
	TestEqual(TEXT("2 big blocks requested"), Pickups->GetTotalXPBigBlocksRequested(), 2);
	TestEqual(TEXT("still level 1"), Miner->GetProgression()->GetLevel(), 1);
	TestNearlyEqual(TEXT("XP collected = XP dropped (3 x XPGain)"), Miner->GetProgression()->GetXP() - StartXP,
		3.f * Miner->GetRRAbilitySystemComponent()->GetNumericAttribute(URRPlayerAttributeSet::GetXPGainAttribute()), 0.001f);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
