// Rogue Rigs - automation test of the ore number shown when the miner collects a Duskite chunk (RogueRigs.Pickups.OreNumber): task-430-02.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "Damage/RRDamageNumberSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Misc/ScopeExit.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Settings/RRGameUserSettings.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestStatsRows.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRPickupOreNumberTests
{
	using RRTestHarness::FTestWorld;

	/** The Duskite chunk Blueprint dropped by ore blocks. */
	const TCHAR* const OrePickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupOre.BP_PickupOre_C");

	/** Open floor cell of the legacy arena_m1 layout (as the heal pickup tests). */
	const FIntPoint OreMinerCell(40, 40);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPickupOreNumberTest, "RogueRigs.Pickups.OreNumber",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPickupOreNumberTest::RunTest(const FString& Parameters)
{
	using namespace RRPickupOreNumberTests;
	// OreTypes.csv row Duskite ChunkValue (1 today): the value shown after the "+".
	const int32 ChunkValue = RRTestStats::OreRow(*this, RRTestStats::DuskiteRow).ChunkValue;

	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!TestNotNull(TEXT("game user settings are URRGameUserSettings"), Settings))
	{
		return false;
	}
	const bool bWasShown = Settings->GetShowDamageNumbers();
	Settings->SetShowDamageNumbers(true);
	ON_SCOPE_EXIT { Settings->SetShowDamageNumbers(bWasShown); };

	FTestWorld TestWorld(TEXT("RRPickupOreNumber"));
	UClass* OreClass = RRTestHarness::LoadTestClass<ARRPickup>(*this, OrePickupClassPath);
	ARRArenaGrid* Grid = OreClass ? RRTestHarness::SpawnGrid(*this, TestWorld) : nullptr;
	ARRMiner* Miner = Grid ? RRTestHarness::SpawnMiner(*this, TestWorld, Grid, OreMinerCell) : nullptr;
	URRDamageNumberSubsystem* Numbers = TestWorld.Get()->GetSubsystem<URRDamageNumberSubsystem>();
	if (!Miner || !TestNotNull(TEXT("damage number subsystem"), Numbers))
	{
		return false;
	}
	for (ARRWeapon* Weapon : Miner->GetWeapons())
	{
		if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
		{
			Weapon->GetAutoFireComponent()->StopFiring();
		}
	}
	APlayerController* Controller = TestWorld.Spawn<APlayerController>(APlayerController::StaticClass(), Miner->GetActorLocation());
	if (!TestNotNull(TEXT("player controller spawned"), Controller))
	{
		return false;
	}
	Controller->Possess(Miner);

	// Every pushed number and the digits of the batch holding an ore number (laid out before the broadcast).
	TArray<FRRDamageNumber> Pushed;
	TArray<FRRDamageDigit> OreDigits;
	const FDelegateHandle Handle = Numbers->OnNumbersPushed.AddLambda([&Pushed, &OreDigits, Numbers](const TArray<FRRDamageNumber>& Batch)
	{
		Pushed.Append(Batch);
		if (OreDigits.Num() == 0 && Batch.ContainsByPredicate([](const FRRDamageNumber& Number) { return Number.bOre; }))
		{
			OreDigits = Numbers->GetLastDigits();
		}
	});
	ON_SCOPE_EXIT { Numbers->OnNumbersPushed.Remove(Handle); };

	// One Duskite chunk pops at the miner feet, inside the pickup radius: collected after the pop and the attract delay.
	const float CapsuleTop = Miner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FTransform Transform(Miner->GetActorLocation() - FVector(0.f, 0.f, CapsuleTop));
	ARRPickup* Ore = TestWorld.Get()->SpawnActorDeferred<ARRPickup>(OreClass, Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!TestNotNull(TEXT("ore pickup"), Ore))
	{
		return false;
	}
	Ore->SetPopYaw(0.f);
	Ore->SetOreValue(ChunkValue);
	Ore->FinishSpawning(Transform);
	for (int32 Frame = 0; Frame < 240 && OreDigits.Num() == 0; ++Frame)
	{
		TestWorld.Tick();
	}

	FString DigitList;
	for (const FRRDamageDigit& Digit : OreDigits)
	{
		DigitList += FString::Printf(TEXT("%s%d"), DigitList.IsEmpty() ? TEXT("") : TEXT(", "), Digit.Digit);
	}
	AddInfo(FString::Printf(TEXT("OREnumber ChunkValue %d -> digits [%s], colors %s"), ChunkValue, *DigitList,
		*FString::JoinBy(OreDigits, TEXT(" "), [](const FRRDamageDigit& Digit) { return Digit.Color.ToString(); })));

	const int32 OreNumbers = Pushed.FilterByPredicate([](const FRRDamageNumber& Number) { return Number.bOre; }).Num();
	if (!TestEqual(TEXT("one ore number for one chunk"), OreNumbers, 1) || !TestEqual(TEXT("icon, plus and one digit (ChunkValue < 10)"), OreDigits.Num(), 3))
	{
		return false;
	}
	TestEqual(TEXT("icon glyph first"), OreDigits[0].Digit, URRDamageNumberSubsystem::OreGlyph);
	TestEqual(TEXT("plus glyph second"), OreDigits[1].Digit, URRDamageNumberSubsystem::PlusGlyph);
	TestEqual(TEXT("ChunkValue digit last"), OreDigits[2].Digit, ChunkValue);
	TestTrue(TEXT("icon drawn white"), OreDigits[0].Color.Equals(FLinearColor::White));
	const FRRDamageNumber* OreNumber = Pushed.FindByPredicate([](const FRRDamageNumber& Number) { return Number.bOre; });
	TestTrue(TEXT("\"+N\" in the number colour (OreColor)"), OreDigits[1].Color.Equals(OreNumber->Color) && OreDigits[2].Color.Equals(OreNumber->Color));
	TestFalse(TEXT("ore number never critical"), OreNumber->bCritical || OreNumber->bHeal);
	TestFalse(TEXT("ore digits never critical"), OreDigits[0].bCritical || OreDigits[1].bCritical || OreDigits[2].bCritical);
	const float IconGap = static_cast<float>(OreDigits[1].Location.Y - OreDigits[0].Location.Y);
	const float DigitGap = static_cast<float>(OreDigits[2].Location.Y - OreDigits[1].Location.Y);
	TestTrue(TEXT("icon farther from the \"+\" than a digit advance (no overlap)"), IconGap > DigitGap && DigitGap > 0.f);
	TestNearlyEqual(TEXT("glyphs centred on the number"), static_cast<float>(0.5 * (OreDigits[0].Location.Y + OreDigits[2].Location.Y)), static_cast<float>(OreNumber->Location.Y), 0.01f);
	// Spawn point: miner capsule top + HealNumberHeight (40), the 30 uu disc is horizontal; the miner may have moved a little.
	TestNearlyEqual(TEXT("number at capsule top + 40 uu"), static_cast<float>(OreNumber->Location.Z), static_cast<float>(Miner->GetActorLocation().Z) + CapsuleTop + 40.f, 5.f);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
