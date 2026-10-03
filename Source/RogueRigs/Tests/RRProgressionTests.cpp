// Rogue Rigs - automation tests of the XP and level progression (RogueRigs.Progression.*): curve, carry-over, XPGain, XP pickup.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Pickups/RRPickup.h"
#include "Progression/RRProgressionComponent.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRProgressionTests
{
	using namespace RRTestHarness;

	/** Final XP pickup Blueprint (its XPAmount is the C++ default unless overridden). */
	const TCHAR* const XPPickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupXP.BP_PickupXP_C");

	/** Spawns the grid and BP_Miner on the floor cell, weapons stopped; returns its progression bound to Recorder, or null. */
	URRProgressionComponent* SpawnProgression(FAutomationTestBase& Test, const FTestWorld& TestWorld, URRTestEventRecorder* Recorder, ARRMiner** OutMiner = nullptr)
	{
		ARRArenaGrid* Grid = SpawnGrid(Test, TestWorld);
		ARRMiner* Miner = Grid ? SpawnMiner(Test, TestWorld, Grid, RockWestFloorCell) : nullptr;
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
		URRProgressionComponent* Progression = Miner->GetProgression();
		if (!Test.TestNotNull(TEXT("miner has a progression component"), Progression))
		{
			return nullptr;
		}
		Test.TestEqual(TEXT("starts at level 1"), Progression->GetLevel(), 1);
		Test.TestEqual(TEXT("starts with 0 XP"), Progression->GetXP(), 0.f);
		Test.TestEqual(TEXT("level 1 needs 5 XP (5 x level)"), Progression->GetXPToNextLevel(), 5.f);
		Test.TestEqual(TEXT("level 50 needs 250 XP"), Progression->GetXPToNextLevelAt(50), 250.f);
		Test.TestEqual(TEXT("XPGain base 1 from the CSV"),
			Miner->GetAbilitySystemComponent()->GetNumericAttribute(URRPlayerAttributeSet::GetXPGainAttribute()), 1.f);
		Progression->OnXPChanged.AddDynamic(Recorder, &URRTestEventRecorder::HandleXPChanged);
		Progression->OnLevelUp.AddDynamic(Recorder, &URRTestEventRecorder::HandleLevelUp);
		if (OutMiner)
		{
			*OutMiner = Miner;
		}
		return Progression;
	}
}

using namespace RRProgressionTests;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRProgressionExactLevelTest, "RogueRigs.Progression.ExactLevel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRProgressionExactLevelTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	URRProgressionComponent* Progression = SpawnProgression(*this, TestWorld, Recorder.Get());
	if (!Progression)
	{
		return false;
	}

	Progression->AddXP(5.f);
	TestEqual(TEXT("5 XP: level 2"), Progression->GetLevel(), 2);
	TestEqual(TEXT("5 XP: 0 carried"), Progression->GetXP(), 0.f);
	TestEqual(TEXT("level 2 needs 10"), Progression->GetXPToNextLevel(), 10.f);
	TestEqual(TEXT("one level-up event"), Recorder->GetLevelUps().Num(), 1);
	if (Recorder->GetLevelUps().Num() == 1)
	{
		TestEqual(TEXT("level-up to 2"), Recorder->GetLevelUps()[0], 2);
	}
	TestEqual(TEXT("one XP event"), Recorder->GetXPChanges().Num(), 1);
	if (Recorder->GetXPChanges().Num() == 1)
	{
		TestEqual(TEXT("XP event (0, 10, 2)"), Recorder->GetXPChanges()[0], FVector(0.f, 10.f, 2.f));
	}

	Progression->AddXP(0.f);
	Progression->AddXP(-3.f);
	TestEqual(TEXT("zero and negative XP ignored (no event)"), Recorder->GetXPChanges().Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRProgressionCarryOverTest, "RogueRigs.Progression.CarryOverMultiLevel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRProgressionCarryOverTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	URRProgressionComponent* Progression = SpawnProgression(*this, TestWorld, Recorder.Get());
	if (!Progression)
	{
		return false;
	}

	// 17 = 5 (level 1) + 10 (level 2) + 2 carried.
	Progression->AddXP(17.f);
	TestEqual(TEXT("17 XP: level 3"), Progression->GetLevel(), 3);
	TestEqual(TEXT("17 XP: 2 carried"), Progression->GetXP(), 2.f);
	TestEqual(TEXT("level 3 needs 15"), Progression->GetXPToNextLevel(), 15.f);
	TestEqual(TEXT("two level-up events"), Recorder->GetLevelUps().Num(), 2);
	if (Recorder->GetLevelUps().Num() == 2)
	{
		TestEqual(TEXT("first level-up to 2"), Recorder->GetLevelUps()[0], 2);
		TestEqual(TEXT("second level-up to 3"), Recorder->GetLevelUps()[1], 3);
	}
	TestEqual(TEXT("one XP event"), Recorder->GetXPChanges().Num(), 1);
	if (Recorder->GetXPChanges().Num() == 1)
	{
		TestEqual(TEXT("XP event (2, 15, 3)"), Recorder->GetXPChanges()[0], FVector(2.f, 15.f, 3.f));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRProgressionXPGainTest, "RogueRigs.Progression.XPGainScales",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRProgressionXPGainTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	ARRMiner* Miner = nullptr;
	URRProgressionComponent* Progression = SpawnProgression(*this, TestWorld, Recorder.Get(), &Miner);
	if (!Progression)
	{
		return false;
	}

	Miner->GetAbilitySystemComponent()->SetNumericAttributeBase(URRPlayerAttributeSet::GetXPGainAttribute(), 2.f);
	Progression->AddXP(2.f);
	TestEqual(TEXT("XPGain 2: 2 raw gives 4 XP"), Progression->GetXP(), 4.f);
	TestEqual(TEXT("still level 1"), Progression->GetLevel(), 1);
	Progression->AddXP(3.f);
	TestEqual(TEXT("XPGain 2: 3 more raw gives 10 total: level 2"), Progression->GetLevel(), 2);
	TestEqual(TEXT("5 carried"), Progression->GetXP(), 5.f);
	TestEqual(TEXT("one level-up event"), Recorder->GetLevelUps().Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRProgressionXPPickupTest, "RogueRigs.Progression.XPPickupAddsOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRProgressionXPPickupTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	ARRMiner* Miner = nullptr;
	URRProgressionComponent* Progression = SpawnProgression(*this, TestWorld, Recorder.Get(), &Miner);
	UClass* XPClass = LoadTestClass<ARRPickup>(*this, XPPickupClassPath);
	if (!Progression || !XPClass)
	{
		return false;
	}

	// Pickups find the miner as the player 0 pawn: a player controller (no local player) possesses it.
	APlayerController* Controller = TestWorld.Spawn<APlayerController>(APlayerController::StaticClass(), Miner->GetActorLocation());
	if (!TestNotNull(TEXT("player controller spawned"), Controller))
	{
		return false;
	}
	Controller->Possess(Miner);
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);

	ARRPickup* Pickup = TestWorld.Spawn<ARRPickup>(XPClass, Miner->GetActorLocation() + FVector(0.f, 100.f, 0.f));
	if (!TestNotNull(TEXT("XP pickup spawned"), Pickup))
	{
		return false;
	}
	TestEqual(TEXT("pickup type XP"), Pickup->GetPickupType(), ERRPickupType::XP);

	// Pop 0.35 s + AttractDelay 1 s + flight: 240 frames (4 s) is ample.
	for (int32 Frame = 1; Frame <= 240 && Recorder->GetCollectedPickups().Num() == 0; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("XP pickup collected once"), Recorder->GetCollectedPickups().Num(), 1);
	TestEqual(TEXT("collecting adds 1 XP"), Progression->GetXP(), 1.f);
	TestEqual(TEXT("still level 1"), Progression->GetLevel(), 1);
	TestEqual(TEXT("one XP event"), Recorder->GetXPChanges().Num(), 1);
	if (Recorder->GetXPChanges().Num() == 1)
	{
		TestEqual(TEXT("XP event (1, 5, 1)"), Recorder->GetXPChanges()[0], FVector(1.f, 5.f, 1.f));
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
