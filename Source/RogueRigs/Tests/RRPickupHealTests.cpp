// Rogue Rigs - automation tests of the heal pickup refusal at full health (RogueRigs.Pickups.Heal*): task-089-01, heal-ore owner 2026-10-10.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRPickupHealTests
{
	using RRTestHarness::FTestWorld;

	/** The heal pickup Blueprint dropped by heal veins. */
	const TCHAR* const HealPickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal.BP_PickupHeal_C");

	/** Open floor cell of the legacy arena_m1 layout (as the pickup motion tests). */
	const FIntPoint HealMinerCell(40, 40);

	/** Heal per pickup in the tests (OreTypes.csv row Heal, task-088-02). */
	constexpr float HealPerPickup = 15.f;

	/** Spawns BP_Miner on HealMinerCell, possessed by a player controller (pickups look for the player 0 pawn), weapons stopped, MaxHealth 100 and Health set. */
	ARRMiner* SpawnHealMiner(FAutomationTestBase& Test, const FTestWorld& TestWorld, float Health)
	{
		ARRArenaGrid* Grid = RRTestHarness::SpawnGrid(Test, TestWorld);
		ARRMiner* Miner = Grid ? RRTestHarness::SpawnMiner(Test, TestWorld, Grid, HealMinerCell) : nullptr;
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

	/** Miner feet (pickups spawned here rest within the 300 uu pickup radius). */
	FVector MinerFeet(const ARRMiner* Miner)
	{
		return Miner->GetActorLocation() - FVector(0.f, 0.f, Miner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	}

	/** Current Health of the miner. */
	float HealthOf(const ARRMiner* Miner)
	{
		return Miner->GetRRAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Spawns a pickup of Class at Location (deferred: pop yaw and heal amount set before BeginPlay). */
	ARRPickup* SpawnPickup(const FTestWorld& TestWorld, UClass* Class, const FVector& Location, float YawDegrees, float HealAmount)
	{
		const FTransform Transform(Location);
		ARRPickup* Pickup = TestWorld.Get()->SpawnActorDeferred<ARRPickup>(Class, Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Pickup)
		{
			Pickup->SetPopYaw(YawDegrees);
			Pickup->SetHealAmount(HealAmount);
			Pickup->FinishSpawning(Transform);
		}
		return Pickup;
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
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPickupHealFullHealthTest, "RogueRigs.Pickups.HealFullHealthWaits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPickupHealFullHealthTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupHealTests;

	FTestWorld TestWorld(TEXT("RRPickupHealFull"));
	UClass* HealClass = RRTestHarness::LoadTestClass<ARRPickup>(*this, HealPickupClassPath);
	ARRMiner* Miner = HealClass ? SpawnHealMiner(*this, TestWorld, 100.f) : nullptr;
	if (!Miner)
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);

	// Full health: a heal pickup and an XP cube pop inside the pickup radius.
	const FVector Feet = MinerFeet(Miner);
	ARRPickup* Heal = SpawnPickup(TestWorld, HealClass, Feet, 0.f, HealPerPickup);
	ARRPickup* XP = SpawnPickup(TestWorld, ARRTestPickupXP::StaticClass(), Feet, 180.f, 0.f);
	if (!TestNotNull(TEXT("heal pickup"), Heal) || !TestNotNull(TEXT("XP pickup"), XP))
	{
		return false;
	}
	TWeakObjectPtr<ARRPickup> HealWeak(Heal);
	TWeakObjectPtr<ARRPickup> XPWeak(XP);
	TestTrue(TEXT("heal pickup rests inside the pickup radius"), FVector::Dist2D(Heal->GetPopEnd(), Miner->GetActorLocation()) < Miner->GetPickupRadius());

	// 4 s: well past pop (0.35 s) + AttractDelay (1 s).
	bool bHealEverMagnetized = false;
	for (int32 Frame = 0; Frame < 240; ++Frame)
	{
		TestWorld.Tick();
		bHealEverMagnetized |= HealWeak.IsValid() && HealWeak->IsMagnetized();
	}
	AddInfo(FString::Printf(TEXT("HEALFULL after 240 frames at full health: heal alive %d, XP collected %d, health %.1f"), HealWeak.IsValid() ? 1 : 0,
		CountCollected(Recorder.Get(), ERRPickupType::XP), HealthOf(Miner)));
	TestEqual(TEXT("XP pickup still collected at full health"), CountCollected(Recorder.Get(), ERRPickupType::XP), 1);
	TestFalse(TEXT("XP pickup destroyed"), XPWeak.IsValid() && !XPWeak->IsActorBeingDestroyed());
	TestEqual(TEXT("heal pickup not collected at full health"), CountCollected(Recorder.Get(), ERRPickupType::Heal), 0);
	if (!TestTrue(TEXT("heal pickup not consumed"), HealWeak.IsValid() && !HealWeak->IsActorBeingDestroyed()))
	{
		return false;
	}
	TestFalse(TEXT("heal pickup never attracted at full health"), bHealEverMagnetized);
	TestEqual(TEXT("heal pickup resting"), HealWeak->GetPhase(), ERRPickupPhase::Rest);
	// task-229-02: the landed heal settles under physics near its rest point, on the floor (no longer exactly at the kinematic rest point).
	TestTrue(TEXT("heal pickup settled near its rest point"), FVector::Dist2D(HealWeak->GetActorLocation(), HealWeak->GetPopEnd()) < 100.f);
	TestTrue(TEXT("heal pickup on the floor (not above the rest height, not fallen through)"), HealWeak->GetActorLocation().Z <= HealWeak->GetPopEnd().Z + 0.5 && HealWeak->GetActorLocation().Z > HealWeak->GetPopEnd().Z - 50.0);
	TestNearlyEqual(TEXT("health still 100"), HealthOf(Miner), 100.f, 0.01f);

	// The miner takes damage: the waiting pickup is attracted and collected, healing 15.
	Miner->GetRRAbilitySystemComponent()->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 70.f);
	int32 Frames = 0;
	for (; Frames < 120 && CountCollected(Recorder.Get(), ERRPickupType::Heal) == 0; ++Frames)
	{
		TestWorld.Tick();
	}
	AddInfo(FString::Printf(TEXT("HEALFULL left-behind pickup collected %d frames after the damage, health %.1f"), Frames, HealthOf(Miner)));
	TestEqual(TEXT("left-behind heal pickup collected once damaged"), CountCollected(Recorder.Get(), ERRPickupType::Heal), 1);
	TestFalse(TEXT("left-behind heal pickup destroyed"), HealWeak.IsValid() && !HealWeak->IsActorBeingDestroyed());
	TestNearlyEqual(TEXT("damaged 70 + 15 = 85"), HealthOf(Miner), 85.f, 0.01f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPickupHealPartialTest, "RogueRigs.Pickups.HealPartialClamp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPickupHealPartialTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupHealTests;

	FTestWorld TestWorld(TEXT("RRPickupHealPartial"));
	UClass* HealClass = RRTestHarness::LoadTestClass<ARRPickup>(*this, HealPickupClassPath);
	ARRMiner* Miner = HealClass ? SpawnHealMiner(*this, TestWorld, 95.f) : nullptr;
	if (!Miner)
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);

	// 95 / 100 and two heal pickups of 15 popping together (as a vein): they latch on the same frame.
	const FVector Feet = MinerFeet(Miner);
	ARRPickup* First = SpawnPickup(TestWorld, HealClass, Feet, 90.f, HealPerPickup);
	ARRPickup* Second = SpawnPickup(TestWorld, HealClass, Feet, 270.f, HealPerPickup);
	if (!TestNotNull(TEXT("first heal pickup"), First) || !TestNotNull(TEXT("second heal pickup"), Second))
	{
		return false;
	}
	TArray<TWeakObjectPtr<ARRPickup>> Heals = { First, Second };
	for (int32 Frame = 0; Frame < 240; ++Frame)
	{
		TestWorld.Tick();
	}
	int32 Alive = 0;
	ARRPickup* Left = nullptr;
	for (const TWeakObjectPtr<ARRPickup>& Heal : Heals)
	{
		if (Heal.IsValid() && !Heal->IsActorBeingDestroyed())
		{
			++Alive;
			Left = Heal.Get();
		}
	}
	AddInfo(FString::Printf(TEXT("HEALPARTIAL health %.2f, heal collected %d, alive %d"), HealthOf(Miner), CountCollected(Recorder.Get(), ERRPickupType::Heal), Alive));
	TestNearlyEqual(TEXT("95 + 15 clamped at 100 (5 healed, surplus lost)"), HealthOf(Miner), 100.f, 0.01f);
	TestEqual(TEXT("one heal pickup collected"), CountCollected(Recorder.Get(), ERRPickupType::Heal), 1);
	if (!TestEqual(TEXT("the other one is not consumed once health is full"), Alive, 1) || !Left)
	{
		return false;
	}
	TestEqual(TEXT("the refused one rests"), Left->GetPhase(), ERRPickupPhase::Rest);
	TestFalse(TEXT("the refused one is not magnetized"), Left->IsMagnetized());
	// task-229-02: a heal refused in flight falls under physics and lies on the floor (below the kinematic rest height, not fallen through).
	TestTrue(TEXT("the refused one lies on the floor"), Left->GetActorLocation().Z <= Left->GetPopEnd().Z + 0.5 && Left->GetActorLocation().Z > Left->GetPopEnd().Z - 50.0);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
