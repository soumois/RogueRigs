// Rogue Rigs - automation tests of the new-weapon cards (RogueRigs.Upgrades.WeaponCards*): native loadout and catalogue defaults, offer rule (30 % per slot, one per offer, only while a weapon can be acquired), picking equips with global upgrade replay, reroll.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Tests/RRTestHarness.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRShotgun.h"
#include "Weapons/RRTurretLauncher.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRWeaponCardTests
{
	using namespace RRTestHarness;

	/** Seed of every offer series below. */
	constexpr int32 CardSeed = 20261005;

	/** What the setup returns. */
	struct FCardSetup
	{
		/** The spawned BP_Miner (rifle only). */
		ARRMiner* Miner = nullptr;
		/** Its upgrade component. */
		URRUpgradeComponent* Upgrades = nullptr;
		/** Its starting burst rifle. */
		ARRWeapon* Rifle = nullptr;
	};

	/** Stops the auto-fire of every weapon of Miner (weapons added later included). */
	void StopAllWeapons(const ARRMiner* Miner)
	{
		for (ARRWeapon* Weapon : Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
	}

	/**
	 * Spawns the grid and BP_Miner with the rifle only (no shotgun in StartingWeapons, no guaranteed class), the default card
	 * tunables, and a known catalogue: the rifle's class, native ARRShotgun, native ARRTurretLauncher (whatever BP_Miner lists).
	 */
	bool SpawnCardSetup(FAutomationTestBase& Test, const FTestWorld& TestWorld, FCardSetup& Out)
	{
		ARRArenaGrid* Grid = SpawnGrid(Test, TestWorld);
		Out.Miner = Grid ? SpawnMiner(Test, TestWorld, Grid, RockWestFloorCell, [](ARRMiner* Miner)
		{
			FMinerAccess::StartingWeaponsOf(Miner).RemoveAll([](const TSubclassOf<ARRWeapon>& Entry)
			{
				return !Entry || Entry->IsChildOf(ARRShotgun::StaticClass()) || Entry->IsChildOf(ARRTurretLauncher::StaticClass());
			});
			FMinerAccess::GuaranteedWeaponClassesOf(Miner).Reset();
		}) : nullptr;
		if (!Out.Miner)
		{
			return false;
		}
		StopAllWeapons(Out.Miner);
		Out.Upgrades = Out.Miner->GetUpgrades();
		if (!Test.TestNotNull(TEXT("miner has an upgrade component"), Out.Upgrades)
			|| !Test.TestEqual(TEXT("miner starts with one weapon"), Out.Miner->GetWeapons().Num(), 1))
		{
			return false;
		}
		Out.Rifle = Out.Miner->GetWeapons()[0];
		if (!Test.TestNotNull(TEXT("rifle spawned"), Out.Rifle))
		{
			return false;
		}
		TArray<FRRWeaponCatalogueEntry>& Catalogue = FMinerAccess::WeaponCatalogueOf(Out.Miner);
		Catalogue.Reset();
		Catalogue.Add({ Out.Rifle->GetClass(), FText::FromString(TEXT("Rifle")) });
		Catalogue.Add({ ARRShotgun::StaticClass(), FText::FromString(TEXT("Shotgun")) });
		Catalogue.Add({ ARRTurretLauncher::StaticClass(), FText::FromString(TEXT("Launcher")) });
		FMinerAccess::MaxWeaponsOf(Out.Miner) = 4;
		FUpgradeAccess::NewWeaponCardChanceOf(Out.Upgrades) = 0.3f;
		FUpgradeAccess::MaxNewWeaponCardsPerOfferOf(Out.Upgrades) = 1;
		Out.Upgrades->SetRandomSeed(CardSeed);
		return true;
	}

	/** Number of new-weapon cards in Cards. */
	int32 CountWeaponCards(const TArray<FRRUpgradeCard>& Cards)
	{
		return Cards.FilterByPredicate([](const FRRUpgradeCard& Card) { return Card.IsNewWeapon(); }).Num();
	}

	/** Builds offers until one holds a new-weapon card for WeaponClass (at most 500); returns that card through OutCard. */
	bool FindWeaponCard(URRUpgradeComponent* Upgrades, UClass* WeaponClass, FRRUpgradeCard& OutCard)
	{
		for (int32 Offer = 0; Offer < 500; ++Offer)
		{
			for (const FRRUpgradeCard& Card : Upgrades->BuildOffer(3, 0.f))
			{
				if (Card.IsNewWeapon() && Card.WeaponClass.Get() == WeaponClass)
				{
					OutCard = Card;
					return true;
				}
			}
		}
		return false;
	}

	/** Current value of Attribute on Weapon's ASC (-1 without one). */
	float WeaponAttr(const ARRWeapon* Weapon, const FGameplayAttribute& Attribute)
	{
		const UAbilitySystemComponent* ASC = Weapon ? Weapon->GetAbilitySystemComponent() : nullptr;
		return ASC ? ASC->GetNumericAttribute(Attribute) : -1.f;
	}
}

using namespace RRWeaponCardTests;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponCardsDefaultsTest, "RogueRigs.Upgrades.WeaponCardsDefaults",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponCardsDefaultsTest::RunTest(const FString& Parameters)
{
	// Native miner: no guaranteed weapon (the rifle comes from StartingWeapons), catalogue shotgun + turret launcher, limit 4.
	ARRMiner* NativeMiner = GetMutableDefault<ARRMiner>();
	TestEqual(TEXT("native GuaranteedWeaponClasses empty"), FMinerAccess::GuaranteedWeaponClassesOf(NativeMiner).Num(), 0);
	TestEqual(TEXT("native StartingWeapons empty"), FMinerAccess::StartingWeaponsOf(NativeMiner).Num(), 0);
	TestEqual(TEXT("MaxWeapons 4"), NativeMiner->GetMaxWeapons(), 4);
	const TArray<FRRWeaponCatalogueEntry>& Catalogue = NativeMiner->GetWeaponCatalogue();
	if (!TestEqual(TEXT("native catalogue holds 2 weapons"), Catalogue.Num(), 2))
	{
		return false;
	}
	TestTrue(TEXT("catalogue 0 is the native shotgun"), Catalogue[0].WeaponClass.Get() == ARRShotgun::StaticClass());
	TestTrue(TEXT("catalogue 1 is the native turret launcher"), Catalogue[1].WeaponClass.Get() == ARRTurretLauncher::StaticClass());
	TestFalse(TEXT("shotgun description set"), Catalogue[0].Description.IsEmpty());
	TestFalse(TEXT("turret launcher description set"), Catalogue[1].Description.IsEmpty());

	const URRUpgradeComponent* NativeUpgrades = NativeMiner->GetUpgrades();
	if (!TestNotNull(TEXT("native miner upgrade component"), NativeUpgrades))
	{
		return false;
	}
	TestEqual(TEXT("new-weapon card chance 0.3"), NativeUpgrades->GetNewWeaponCardChance(), 0.3f);
	TestEqual(TEXT("one new-weapon card per offer"), NativeUpgrades->GetMaxNewWeaponCardsPerOffer(), 1);

	// Card texts: "Nouvelle arme", the CSV weapon name, the catalogue description; no value text.
	FRRUpgradeCard Shotgun;
	FRRUpgradeCard Launcher;
	if (!TestTrue(TEXT("shotgun card builds"), URRUpgradeComponent::MakeWeaponCard(Catalogue[0], Shotgun))
		|| !TestTrue(TEXT("launcher card builds"), URRUpgradeComponent::MakeWeaponCard(Catalogue[1], Launcher)))
	{
		return false;
	}
	TestTrue(TEXT("card type NewWeapon"), Shotgun.IsNewWeapon());
	TestEqual(TEXT("title"), Shotgun.DisplayName.ToString(), FString(TEXT("Nouvelle arme")));
	TestEqual(TEXT("shotgun name"), Shotgun.TargetText.ToString(), FString(TEXT("Fusil à pompe")));
	TestEqual(TEXT("launcher name"), Launcher.TargetText.ToString(), FString(TEXT("Lance-tourelles")));
	TestEqual(TEXT("shotgun description"), Shotgun.DescriptionText.ToString(), Catalogue[0].Description.ToString());
	TestTrue(TEXT("no value text"), Shotgun.ValueText.IsEmpty());
	TestTrue(TEXT("weapon class kept"), Shotgun.WeaponClass.Get() == ARRShotgun::StaticClass());
	TestFalse(TEXT("two weapons are different choices"), Shotgun.IsSameChoice(Launcher));
	TestTrue(TEXT("same weapon is the same choice"), Shotgun.IsSameChoice(Shotgun));
	FRRWeaponCatalogueEntry Empty;
	FRRUpgradeCard Unused;
	TestFalse(TEXT("entry without class gives no card"), URRUpgradeComponent::MakeWeaponCard(Empty, Unused));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponCardsOfferRuleTest, "RogueRigs.Upgrades.WeaponCardsOfferRule",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponCardsOfferRuleTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FCardSetup S;
	if (!SpawnCardSetup(*this, TestWorld, S))
	{
		return false;
	}
	TestEqual(TEXT("acquirable: shotgun and launcher (the rifle is owned)"), S.Miner->GetAcquirableWeapons().Num(), 2);

	// 3000 offers: per slot 30 %, at most one weapon card, so P(one) = 0.3 + 0.7 x 0.3 + 0.49 x 0.3 = 0.657.
	constexpr int32 Offers = 3000;
	int32 WithWeapon = 0;
	int32 SlotCounts[3] = {};
	int32 ShotgunCards = 0;
	int32 LauncherCards = 0;
	int32 Bad = 0;
	for (int32 Offer = 0; Offer < Offers; ++Offer)
	{
		const TArray<FRRUpgradeCard> Cards = S.Upgrades->BuildOffer(3, static_cast<float>(Offer % 11));
		const int32 WeaponCount = CountWeaponCards(Cards);
		Bad += (Cards.Num() != 3 || WeaponCount > 1) ? 1 : 0;
		for (int32 Index = 0; Index < Cards.Num(); ++Index)
		{
			const FRRUpgradeCard& Card = Cards[Index];
			for (int32 Other = Index + 1; Other < Cards.Num(); ++Other)
			{
				Bad += Card.IsSameChoice(Cards[Other]) ? 1 : 0;
			}
			if (Card.IsNewWeapon())
			{
				++SlotCounts[Index];
				ShotgunCards += Card.WeaponClass.Get() == ARRShotgun::StaticClass() ? 1 : 0;
				LauncherCards += Card.WeaponClass.Get() == ARRTurretLauncher::StaticClass() ? 1 : 0;
				Bad += Card.WeaponClass.Get() == S.Rifle->GetClass() ? 1 : 0;
			}
		}
		WithWeapon += WeaponCount;
	}
	const float Share = static_cast<float>(WithWeapon) / Offers;
	AddInfo(FString::Printf(TEXT("WEAPONCARDS offers with a weapon card %.3f (expected 0.657), slots %d / %d / %d, shotgun %d, launcher %d"),
		Share, SlotCounts[0], SlotCounts[1], SlotCounts[2], ShotgunCards, LauncherCards));
	TestEqual(TEXT("always 3 cards, at most one weapon card, no duplicate, never the owned rifle"), Bad, 0);
	TestTrue(TEXT("share of offers with a weapon card near 65.7 %"), FMath::Abs(Share - 0.657f) < 0.03f);
	TestTrue(TEXT("slot 1 near 30 %"), FMath::Abs(SlotCounts[0] / static_cast<float>(Offers) - 0.3f) < 0.03f);
	TestTrue(TEXT("slot 2 near 21 %"), FMath::Abs(SlotCounts[1] / static_cast<float>(Offers) - 0.21f) < 0.03f);
	TestTrue(TEXT("slot 3 near 14.7 %"), FMath::Abs(SlotCounts[2] / static_cast<float>(Offers) - 0.147f) < 0.03f);
	TestTrue(TEXT("shotgun and launcher equally likely"), FMath::Abs(ShotgunCards - LauncherCards) < WithWeapon / 10);

	// Weapon limit: with MaxWeapons 1 the rifle fills the rack, no weapon card.
	FMinerAccess::MaxWeaponsOf(S.Miner) = 1;
	int32 WhenFull = 0;
	for (int32 Offer = 0; Offer < 300; ++Offer)
	{
		WhenFull += CountWeaponCards(S.Upgrades->BuildOffer(3, 0.f));
	}
	TestEqual(TEXT("no weapon card at the weapon limit"), WhenFull, 0);
	FRRUpgradeCard ShotgunCard;
	URRUpgradeComponent::MakeWeaponCard(S.Miner->GetWeaponCatalogue()[1], ShotgunCard);
	TestFalse(TEXT("a weapon card is refused at the limit"), S.Upgrades->ApplyUpgrade(ShotgunCard));
	TestEqual(TEXT("still one weapon"), S.Miner->GetWeapons().Num(), 1);
	FMinerAccess::MaxWeaponsOf(S.Miner) = 4;

	// Every catalogue weapon owned: no weapon card although fewer than 4 weapons are owned.
	S.Miner->AddWeapon(ARRShotgun::StaticClass());
	S.Miner->AddWeapon(ARRTurretLauncher::StaticClass());
	StopAllWeapons(S.Miner);
	TestEqual(TEXT("three weapons owned"), S.Miner->GetOwnedWeaponCount(), 3);
	TestEqual(TEXT("nothing left to acquire"), S.Miner->GetAcquirableWeapons().Num(), 0);
	int32 WhenAllOwned = 0;
	for (int32 Offer = 0; Offer < 300; ++Offer)
	{
		WhenAllOwned += CountWeaponCards(S.Upgrades->BuildOffer(3, 0.f));
	}
	TestEqual(TEXT("no weapon card once every weapon is owned"), WhenAllOwned, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponCardsPickEquipsTest, "RogueRigs.Upgrades.WeaponCardsPickEquips",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponCardsPickEquipsTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FCardSetup S;
	if (!SpawnCardSetup(*this, TestWorld, S))
	{
		return false;
	}
	const FGameplayAttribute Damage = URRWeaponAttributeSet::GetDamageAttribute();
	const FGameplayAttribute FireRate = URRWeaponAttributeSet::GetFireRateAttribute();

	// A global damage card and a rifle-only fire rate card picked before the shotgun arrives.
	FRRUpgradeCard GlobalDamage;
	FRRUpgradeCard RifleFireRate;
	if (!TestTrue(TEXT("global damage card"), S.Upgrades->MakeCard(ERRUpgradeStat::Damage, ERRUpgradeRarity::Grey, ERRUpgradeTarget::AllWeapons, nullptr, GlobalDamage))
		|| !TestTrue(TEXT("rifle fire rate card"), S.Upgrades->MakeCard(ERRUpgradeStat::FireRate, ERRUpgradeRarity::Grey, ERRUpgradeTarget::Weapon, S.Rifle, RifleFireRate)))
	{
		return false;
	}
	TestTrue(TEXT("global damage applied"), S.Upgrades->ApplyUpgrade(GlobalDamage));
	TestTrue(TEXT("rifle fire rate applied"), S.Upgrades->ApplyUpgrade(RifleFireRate));

	// Before: no stat card targets a shotgun.
	FRRUpgradeCard ShotgunCard;
	if (!TestTrue(TEXT("an offer proposes the shotgun"), FindWeaponCard(S.Upgrades, ARRShotgun::StaticClass(), ShotgunCard)))
	{
		return false;
	}
	TestTrue(TEXT("offer open"), S.Upgrades->HasOffer());
	const int32 PickedBefore = S.Upgrades->GetPickedUpgrades().Num();
	if (!TestTrue(TEXT("shotgun card applied"), S.Upgrades->ApplyUpgrade(ShotgunCard)))
	{
		return false;
	}
	StopAllWeapons(S.Miner);
	ARRShotgun* Shotgun = FindShotgun(S.Miner);
	if (!TestNotNull(TEXT("miner holds a shotgun"), Shotgun))
	{
		return false;
	}
	TestEqual(TEXT("two weapons"), S.Miner->GetWeapons().Num(), 2);
	TestFalse(TEXT("offer ended"), S.Upgrades->HasOffer());
	TestEqual(TEXT("card recorded"), S.Upgrades->GetPickedUpgrades().Num(), PickedBefore + 1);
	TestTrue(TEXT("recorded card points at the acquired shotgun"), S.Upgrades->GetPickedUpgrades().Last().TargetWeapon.Get() == Shotgun);
	TestEqual(TEXT("shotgun damage = base 6 with the global card replayed"), WeaponAttr(Shotgun, Damage), 6.f * (1.f + GlobalDamage.Value), 1e-4f);
	TestEqual(TEXT("shotgun fire rate = base 2 (rifle card not replayed)"), WeaponAttr(Shotgun, FireRate), 2.f, 1e-4f);
	TestEqual(TEXT("rifle damage unchanged by the pick"), WeaponAttr(S.Rifle, Damage), 10.f * (1.f + GlobalDamage.Value), 1e-4f);
	TestFalse(TEXT("the same weapon card is refused once owned"), S.Upgrades->ApplyUpgrade(ShotgunCard));
	TestEqual(TEXT("still two weapons"), S.Miner->GetWeapons().Num(), 2);

	// After: the shotgun is a target of stat cards and is no longer offered as a new weapon.
	int32 ShotgunTargeted = 0;
	int32 ShotgunOffered = 0;
	int32 LauncherOffered = 0;
	for (int32 Offer = 0; Offer < 300; ++Offer)
	{
		for (const FRRUpgradeCard& Card : S.Upgrades->BuildOffer(3, 0.f))
		{
			ShotgunTargeted += (!Card.IsNewWeapon() && Card.TargetWeapon.Get() == Shotgun) ? 1 : 0;
			ShotgunOffered += (Card.IsNewWeapon() && Card.WeaponClass.Get() == ARRShotgun::StaticClass()) ? 1 : 0;
			LauncherOffered += (Card.IsNewWeapon() && Card.WeaponClass.Get() == ARRTurretLauncher::StaticClass()) ? 1 : 0;
		}
	}
	AddInfo(FString::Printf(TEXT("WEAPONCARDS after the shotgun: %d shotgun-targeted cards, %d launcher cards in 300 offers"), ShotgunTargeted, LauncherOffered));
	TestTrue(TEXT("shotgun-targeted stat cards appear"), ShotgunTargeted > 0);
	TestEqual(TEXT("shotgun never offered again"), ShotgunOffered, 0);
	TestTrue(TEXT("launcher still offered"), LauncherOffered > 0);

	// Turret launcher picked: third weapon, found by FindTurretLauncher, global card replayed.
	FRRUpgradeCard LauncherCard;
	if (!TestTrue(TEXT("an offer proposes the launcher"), FindWeaponCard(S.Upgrades, ARRTurretLauncher::StaticClass(), LauncherCard))
		|| !TestTrue(TEXT("launcher card applied"), S.Upgrades->ApplyUpgrade(LauncherCard)))
	{
		return false;
	}
	StopAllWeapons(S.Miner);
	ARRTurretLauncher* Launcher = S.Miner->FindTurretLauncher();
	if (!TestNotNull(TEXT("miner holds a turret launcher"), Launcher))
	{
		return false;
	}
	TestEqual(TEXT("three weapons"), S.Miner->GetWeapons().Num(), 3);
	TestEqual(TEXT("launcher damage = base 6 with the global card replayed"), WeaponAttr(Launcher, Damage), 6.f * (1.f + GlobalDamage.Value), 1e-4f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponCardsRerollTest, "RogueRigs.Upgrades.WeaponCardsReroll",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponCardsRerollTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FCardSetup S;
	if (!SpawnCardSetup(*this, TestWorld, S))
	{
		return false;
	}
	// Offers holding a weapon card, rerolled: the rerolled offer never proposes the same weapon and keeps the rule.
	int32 Rerolled = 0;
	int32 Bad = 0;
	int32 OtherWeaponAfter = 0;
	for (int32 Offer = 0; Offer < 400; ++Offer)
	{
		const TArray<FRRUpgradeCard> First = S.Upgrades->BuildOffer(3, 0.f);
		const FRRUpgradeCard* WeaponCard = First.FindByPredicate([](const FRRUpgradeCard& Card) { return Card.IsNewWeapon(); });
		if (!WeaponCard)
		{
			continue;
		}
		TArray<FRRUpgradeCard> Second;
		if (!S.Upgrades->Reroll(Second))
		{
			++Bad;
			continue;
		}
		++Rerolled;
		Bad += (Second.Num() != 3 || CountWeaponCards(Second) > 1) ? 1 : 0;
		for (const FRRUpgradeCard& New : Second)
		{
			for (const FRRUpgradeCard& Old : First)
			{
				Bad += New.IsSameChoice(Old) ? 1 : 0;
			}
			OtherWeaponAfter += New.IsNewWeapon() ? 1 : 0;
		}
	}
	AddInfo(FString::Printf(TEXT("WEAPONCARDS %d rerolled offers with a weapon card, %d rerolls proposing the other weapon"), Rerolled, OtherWeaponAfter));
	TestTrue(TEXT("enough rerolled offers"), Rerolled > 150);
	TestEqual(TEXT("rerolled offers share no choice and keep at most one weapon card"), Bad, 0);
	TestTrue(TEXT("the other weapon can be offered after a reroll"), OtherWeaponAfter > 0);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
