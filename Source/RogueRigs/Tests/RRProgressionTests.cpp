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
#include "Tests/RRTestStatsRows.h"
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
		// XPCurve.csv rows 1, 2, 10, 50 and PlayerBaseStats.csv Miner XPGain (task-420: read, never copied).
		Test.TestEqual(TEXT("level 1 need = XPCurve row 1"), Progression->GetXPToNextLevel(), RRTestStats::XPToNextLevel(Test, 1));
		for (const int32 Level : { 2, 10, 50 })
		{
			Test.TestEqual(FString::Printf(TEXT("level %d need = XPCurve row %d"), Level, Level), Progression->GetXPToNextLevelAt(Level),
				RRTestStats::XPToNextLevel(Test, Level));
		}
		Test.TestEqual(TEXT("XPGain base from the CSV"),
			Miner->GetAbilitySystemComponent()->GetNumericAttribute(URRPlayerAttributeSet::GetXPGainAttribute()), RRTestStats::PlayerRow(Test).XPGain);
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

	// XPCurve.csv: exactly the level 1 need (row 1), then the level 2 need (row 2). XPGain set to 1 so raw XP = XP.
	const float Need1 = RRTestStats::XPToNextLevel(*this, 1);
	const float Need2 = RRTestStats::XPToNextLevel(*this, 2);
	Progression->GetOwner<ARRMiner>()->GetAbilitySystemComponent()->SetNumericAttributeBase(URRPlayerAttributeSet::GetXPGainAttribute(), 1.f);
	Progression->AddXP(Need1);
	TestEqual(TEXT("level 1 need: level 2"), Progression->GetLevel(), 2);
	TestEqual(TEXT("level 1 need: 0 carried"), Progression->GetXP(), 0.f);
	TestEqual(TEXT("level 2 needs row 2"), Progression->GetXPToNextLevel(), Need2);
	TestEqual(TEXT("one level-up event"), Recorder->GetLevelUps().Num(), 1);
	if (Recorder->GetLevelUps().Num() == 1)
	{
		TestEqual(TEXT("level-up to 2"), Recorder->GetLevelUps()[0], 2);
	}
	TestEqual(TEXT("one XP event"), Recorder->GetXPChanges().Num(), 1);
	if (Recorder->GetXPChanges().Num() == 1)
	{
		TestEqual(TEXT("XP event (0, need 2, 2)"), Recorder->GetXPChanges()[0], FVector(0.f, Need2, 2.f));
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

	// Need 1 + need 2 + 2 carried (XPCurve.csv rows 1 to 3; 2 is a scenario value below the level 3 need). XPGain set to 1.
	const float Need1 = RRTestStats::XPToNextLevel(*this, 1);
	const float Need2 = RRTestStats::XPToNextLevel(*this, 2);
	const float Need3 = RRTestStats::XPToNextLevel(*this, 3);
	if (!TestTrue(TEXT("CSV: level 3 needs more than 2 XP"), Need3 > 2.f))
	{
		return false;
	}
	Progression->GetOwner<ARRMiner>()->GetAbilitySystemComponent()->SetNumericAttributeBase(URRPlayerAttributeSet::GetXPGainAttribute(), 1.f);
	Progression->AddXP(Need1 + Need2 + 2.f);
	TestEqual(TEXT("need 1 + need 2 + 2: level 3"), Progression->GetLevel(), 3);
	TestEqual(TEXT("2 carried"), Progression->GetXP(), 2.f);
	TestEqual(TEXT("level 3 needs row 3"), Progression->GetXPToNextLevel(), Need3);
	TestEqual(TEXT("two level-up events"), Recorder->GetLevelUps().Num(), 2);
	if (Recorder->GetLevelUps().Num() == 2)
	{
		TestEqual(TEXT("first level-up to 2"), Recorder->GetLevelUps()[0], 2);
		TestEqual(TEXT("second level-up to 3"), Recorder->GetLevelUps()[1], 3);
	}
	TestEqual(TEXT("one XP event"), Recorder->GetXPChanges().Num(), 1);
	if (Recorder->GetXPChanges().Num() == 1)
	{
		TestEqual(TEXT("XP event (2, need 3, 3)"), Recorder->GetXPChanges()[0], FVector(2.f, Need3, 3.f));
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
	Progression->AddXP(0.25f);
	TestEqual(TEXT("XPGain 2: 0.25 raw gives 0.5 XP"), Progression->GetXP(), 0.5f);
	TestEqual(TEXT("still level 1"), Progression->GetLevel(), 1);
	// Half the level 1 need (XPCurve.csv row 1) more raw: need + 0.5 total, level 2 with 0.5 carried.
	Progression->AddXP(0.5f * RRTestStats::XPToNextLevel(*this, 1));
	TestEqual(TEXT("XPGain 2: half the level 1 need more raw: level 2"), Progression->GetLevel(), 2);
	TestEqual(TEXT("0.5 carried"), Progression->GetXP(), 0.5f, 1e-4f);
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

	// The pop has a random yaw and distance (80-220 uu, global RNG): from 100 uu out, a draw away from the miner could land
	// up to 320 uu off, beyond its 300 uu PickupRadius, and rest there uncollected (task-057-01). Yaw 270 pops it back
	// along -Y, through the miner: it lands within 120 uu whatever the distance draw.
	const FTransform PickupTransform(Miner->GetActorLocation() + FVector(0.f, 100.f, 0.f));
	ARRPickup* Pickup = TestWorld.Get()->SpawnActorDeferred<ARRPickup>(XPClass, PickupTransform, nullptr, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!TestNotNull(TEXT("XP pickup spawned"), Pickup))
	{
		return false;
	}
	Pickup->SetPopYaw(270.f);
	Pickup->FinishSpawning(PickupTransform);
	TestEqual(TEXT("pickup type XP"), Pickup->GetPickupType(), ERRPickupType::XP);
	const float PopEndDistance = FVector::Dist2D(Pickup->GetPopEnd(), Miner->GetActorLocation());
	AddInfo(FString::Printf(TEXT("pop end %.1f uu from the miner (PickupRadius %.0f)"), PopEndDistance, Miner->GetPickupRadius()));
	TestTrue(TEXT("pop lands inside the pickup radius"), PopEndDistance <= Miner->GetPickupRadius());
	// XPGain 0.5: the 1 raw XP of the cube stays under the level 1 need (XPCurve.csv row 1, above 0.5) and no level-up flow starts.
	Miner->GetAbilitySystemComponent()->SetNumericAttributeBase(URRPlayerAttributeSet::GetXPGainAttribute(), 0.5f);

	// Pop 0.35 s + AttractDelay 1 s + flight: 240 frames (4 s) is ample.
	for (int32 Frame = 1; Frame <= 240 && Recorder->GetCollectedPickups().Num() == 0; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("XP pickup collected once"), Recorder->GetCollectedPickups().Num(), 1);
	TestEqual(TEXT("collecting adds 1 raw XP (x XPGain 0.5)"), Progression->GetXP(), 0.5f);
	TestEqual(TEXT("still level 1"), Progression->GetLevel(), 1);
	TestEqual(TEXT("one XP event"), Recorder->GetXPChanges().Num(), 1);
	if (Recorder->GetXPChanges().Num() == 1)
	{
		TestEqual(TEXT("XP event (0.5, 4, 1)"), Recorder->GetXPChanges()[0], FVector(0.5f, 4.f, 1.f));
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
