// Rogue Rigs - automation tests of the new-weapon cards (RogueRigs.Upgrades.WeaponCards*): native loadout and catalogue defaults, offer rule (only on levels 5, 10, 15...: 1 card, 2 when 2+ weapons remain), level-up queue, picking equips with global upgrade replay, reroll; and the stat draw weights (RogueRigs.Upgrades.StatDrawWeights).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
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

	/** A level that offers new-weapon cards (multiple of 5). */
	constexpr int32 WeaponLevel = 5;

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
		FUpgradeAccess::WeaponCardLevelIntervalOf(Out.Upgrades) = 5;
		FUpgradeAccess::MaxNewWeaponCardsPerOfferOf(Out.Upgrades) = 2;
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
			for (const FRRUpgradeCard& Card : Upgrades->BuildOffer(3, 0.f, WeaponLevel))
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

	/** Raw XP that brings Progression exactly Levels levels up (XPGain included). */
	float WeaponCardRawXPFor(const URRProgressionComponent* Progression, int32 Levels)
	{
		float Needed = -Progression->GetXP();
		for (int32 Index = 0; Index < Levels; ++Index)
		{
			Needed += Progression->GetXPToNextLevelAt(Progression->GetLevel() + Index);
		}
		const UAbilitySystemComponent* ASC = Progression->GetOwner() ? Progression->GetOwner()->FindComponentByClass<UAbilitySystemComponent>() : nullptr;
		const float Gain = ASC && ASC->HasAttributeSetForAttribute(URRPlayerAttributeSet::GetXPGainAttribute())
			? ASC->GetNumericAttribute(URRPlayerAttributeSet::GetXPGainAttribute()) : 1.f;
		return Needed / FMath::Max(Gain, KINDA_SMALL_NUMBER);
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
	TestEqual(TEXT("weapon cards every 5 levels"), NativeUpgrades->GetWeaponCardLevelInterval(), 5);
	TestEqual(TEXT("at most two new-weapon cards per offer"), NativeUpgrades->GetMaxNewWeaponCardsPerOffer(), 2);
	TestEqual(TEXT("level 5, 2 weapons left: 2 cards"), NativeUpgrades->GetNewWeaponCardCount(5, 2), 2);
	TestEqual(TEXT("level 10, 1 weapon left: 1 card"), NativeUpgrades->GetNewWeaponCardCount(10, 1), 1);
	TestEqual(TEXT("level 15, 3 weapons left: 2 cards"), NativeUpgrades->GetNewWeaponCardCount(15, 3), 2);
	TestEqual(TEXT("level 5, nothing left: none"), NativeUpgrades->GetNewWeaponCardCount(5, 0), 0);
	for (const int32 Level : { 0, 1, 2, 4, 6, 9, 11, 14 })
	{
		TestEqual(*FString::Printf(TEXT("level %d: no weapon card"), Level), NativeUpgrades->GetNewWeaponCardCount(Level, 2), 0);
	}

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

	// Levels 1-30, 20 offers each: weapon cards only on 5, 10, 15... and then exactly two (both acquirable weapons), in the
	// first slots, distinct, never the owned rifle; the other levels none.
	int32 Bad = 0;
	int32 WeaponLevelOffers = 0;
	int32 ShotgunCards = 0;
	int32 LauncherCards = 0;
	int32 OffLevelWeaponCards = 0;
	for (int32 Level = 1; Level <= 30; ++Level)
	{
		for (int32 Offer = 0; Offer < 20; ++Offer)
		{
			const TArray<FRRUpgradeCard> Cards = S.Upgrades->BuildOffer(3, static_cast<float>(Offer % 11), Level);
			const int32 WeaponCount = CountWeaponCards(Cards);
			Bad += Cards.Num() != 3 ? 1 : 0;
			for (int32 Index = 0; Index < Cards.Num(); ++Index)
			{
				for (int32 Other = Index + 1; Other < Cards.Num(); ++Other)
				{
					Bad += Cards[Index].IsSameChoice(Cards[Other]) ? 1 : 0;
				}
				if (Cards[Index].IsNewWeapon())
				{
					ShotgunCards += Cards[Index].WeaponClass.Get() == ARRShotgun::StaticClass() ? 1 : 0;
					LauncherCards += Cards[Index].WeaponClass.Get() == ARRTurretLauncher::StaticClass() ? 1 : 0;
					Bad += Cards[Index].WeaponClass.Get() == S.Rifle->GetClass() ? 1 : 0;
					Bad += Index >= 2 ? 1 : 0;
				}
			}
			if (Level % 5 == 0)
			{
				++WeaponLevelOffers;
				Bad += WeaponCount != 2 ? 1 : 0;
			}
			else
			{
				OffLevelWeaponCards += WeaponCount;
			}
		}
	}
	AddInfo(FString::Printf(TEXT("WEAPONCARDS %d weapon-level offers, shotgun %d, launcher %d, off-level weapon cards %d"),
		WeaponLevelOffers, ShotgunCards, LauncherCards, OffLevelWeaponCards));
	TestEqual(TEXT("always 3 cards; levels 5, 10... hold two distinct weapon cards in slots 1-2; no duplicate; never the owned rifle"), Bad, 0);
	TestEqual(TEXT("no weapon card on other levels"), OffLevelWeaponCards, 0);
	TestEqual(TEXT("both weapons in every weapon-level offer"), ShotgunCards, WeaponLevelOffers);
	TestEqual(TEXT("both weapons in every weapon-level offer (launcher)"), LauncherCards, WeaponLevelOffers);
	TestEqual(TEXT("no weapon card outside a level-up (chest, level 0)"), CountWeaponCards(S.Upgrades->BuildOffer(3, 0.f)), 0);

	// One weapon left: one card.
	S.Miner->AddWeapon(ARRShotgun::StaticClass());
	StopAllWeapons(S.Miner);
	int32 OneLeftBad = 0;
	for (int32 Offer = 0; Offer < 50; ++Offer)
	{
		const TArray<FRRUpgradeCard> Cards = S.Upgrades->BuildOffer(3, 0.f, 10);
		OneLeftBad += (CountWeaponCards(Cards) != 1 || !Cards[0].IsNewWeapon() || Cards[0].WeaponClass.Get() != ARRTurretLauncher::StaticClass()) ? 1 : 0;
	}
	TestEqual(TEXT("one weapon left: one weapon card (the launcher) in slot 1 on level 10"), OneLeftBad, 0);

	// Weapon limit: with MaxWeapons 2 the rifle and the shotgun fill the rack, no weapon card.
	FMinerAccess::MaxWeaponsOf(S.Miner) = 2;
	int32 WhenFull = 0;
	for (int32 Offer = 0; Offer < 300; ++Offer)
	{
		WhenFull += CountWeaponCards(S.Upgrades->BuildOffer(3, 0.f, WeaponLevel));
	}
	TestEqual(TEXT("no weapon card at the weapon limit"), WhenFull, 0);
	FRRUpgradeCard LauncherCard;
	URRUpgradeComponent::MakeWeaponCard(S.Miner->GetWeaponCatalogue()[2], LauncherCard);
	TestFalse(TEXT("a weapon card is refused at the limit"), S.Upgrades->ApplyUpgrade(LauncherCard));
	TestEqual(TEXT("still two weapons"), S.Miner->GetWeapons().Num(), 2);
	FMinerAccess::MaxWeaponsOf(S.Miner) = 4;

	// Every catalogue weapon owned: no weapon card although fewer than 4 weapons are owned.
	S.Miner->AddWeapon(ARRTurretLauncher::StaticClass());
	StopAllWeapons(S.Miner);
	TestEqual(TEXT("three weapons owned"), S.Miner->GetOwnedWeaponCount(), 3);
	TestEqual(TEXT("nothing left to acquire"), S.Miner->GetAcquirableWeapons().Num(), 0);
	int32 WhenAllOwned = 0;
	for (int32 Offer = 0; Offer < 300; ++Offer)
	{
		WhenAllOwned += CountWeaponCards(S.Upgrades->BuildOffer(3, 0.f, WeaponLevel));
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
		for (const FRRUpgradeCard& Card : S.Upgrades->BuildOffer(3, 0.f, WeaponLevel))
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
	// One weapon left (shotgun owned): level-5 offers hold the launcher card; a reroll never proposes it again and keeps 3 cards.
	S.Miner->AddWeapon(ARRShotgun::StaticClass());
	StopAllWeapons(S.Miner);
	int32 Rerolled = 0;
	int32 Bad = 0;
	int32 OtherWeaponAfter = 0;
	for (int32 Offer = 0; Offer < 200; ++Offer)
	{
		const TArray<FRRUpgradeCard> First = S.Upgrades->BuildOffer(3, 0.f, WeaponLevel);
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
	AddInfo(FString::Printf(TEXT("WEAPONCARDS %d rerolled offers with a weapon card, %d rerolls proposing a weapon"), Rerolled, OtherWeaponAfter));
	TestEqual(TEXT("every level-5 offer holds the launcher card"), Rerolled, 200);
	TestEqual(TEXT("rerolled offers share no choice and keep 3 cards"), Bad, 0);
	TestEqual(TEXT("the replaced weapon is never offered again by the reroll"), OtherWeaponAfter, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponCardsLevelQueueTest, "RogueRigs.Upgrades.WeaponCardsOnLevelFive",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponCardsLevelQueueTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FCardSetup S;
	if (!SpawnCardSetup(*this, TestWorld, S))
	{
		return false;
	}
	URRProgressionComponent* Progression = S.Miner->GetProgression();
	URRLevelUpComponent* LevelUp = S.Miner->GetLevelUp();
	if (!TestNotNull(TEXT("progression"), Progression) || !TestNotNull(TEXT("level-up flow"), LevelUp))
	{
		return false;
	}
	// Level 1 -> 6 at once: five chained screens for levels 2, 3, 4, 5, 6; only the level-5 screen holds weapon cards (two).
	Progression->AddXP(WeaponCardRawXPFor(Progression, 5));
	TestEqual(TEXT("level 6"), Progression->GetLevel(), 6);
	TestEqual(TEXT("five choices owed"), LevelUp->GetPendingChoices(), 5);
	for (int32 Frame = 0; Frame < 60 && !LevelUp->IsAwaitingChoice(); ++Frame)
	{
		TestWorld.Tick();
	}
	TArray<int32> WeaponCardsPerScreen;
	for (int32 Screen = 0; Screen < 5 && LevelUp->IsAwaitingChoice(); ++Screen)
	{
		const TArray<FRRUpgradeCard> Cards = LevelUp->GetOfferCards();
		WeaponCardsPerScreen.Add(CountWeaponCards(Cards));
		// Pick a stat card so the weapons stay acquirable.
		const int32 StatIndex = Cards.IndexOfByPredicate([](const FRRUpgradeCard& Card) { return !Card.IsNewWeapon(); });
		if (!TestTrue(TEXT("a stat card to pick"), StatIndex != INDEX_NONE) || !TestTrue(TEXT("pick accepted"), LevelUp->ChooseUpgrade(StatIndex)))
		{
			return false;
		}
	}
	AddInfo(FString::Printf(TEXT("WEAPONCARDS weapon cards on the screens of levels 2-6: %s"),
		*FString::JoinBy(WeaponCardsPerScreen, TEXT(" "), [](int32 Count) { return FString::FromInt(Count); })));
	if (TestEqual(TEXT("five screens"), WeaponCardsPerScreen.Num(), 5))
	{
		TestEqual(TEXT("level 2: none"), WeaponCardsPerScreen[0], 0);
		TestEqual(TEXT("level 3: none"), WeaponCardsPerScreen[1], 0);
		TestEqual(TEXT("level 4: none"), WeaponCardsPerScreen[2], 0);
		TestEqual(TEXT("level 5: two (shotgun and launcher left)"), WeaponCardsPerScreen[3], 2);
		TestEqual(TEXT("level 6: none"), WeaponCardsPerScreen[4], 0);
	}
	TestFalse(TEXT("closed after the last pick"), LevelUp->IsAwaitingChoice());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRUpgradeStatDrawWeightsTest, "RogueRigs.Upgrades.StatDrawWeights",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRUpgradeStatDrawWeightsTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FCardSetup S;
	if (!SpawnCardSetup(*this, TestWorld, S))
	{
		return false;
	}
	// CSV weights (upgrades gdd, owner 2026-10-06 third test): damage, fire rate, reload 3; every other stat 1.
	const ERRUpgradeStat Heavy[] = { ERRUpgradeStat::Damage, ERRUpgradeStat::FireRate, ERRUpgradeStat::ReloadSpeed };
	const ERRUpgradeStat Light[] = { ERRUpgradeStat::CritChance, ERRUpgradeStat::CritDamage, ERRUpgradeStat::MaxHealth, ERRUpgradeStat::MoveSpeed,
		ERRUpgradeStat::PickupRadius, ERRUpgradeStat::XPGain, ERRUpgradeStat::MiningSpeed, ERRUpgradeStat::Luck, ERRUpgradeStat::Pierce };
	for (const ERRUpgradeStat Stat : Heavy)
	{
		TestEqual(*FString::Printf(TEXT("stat %d weight 3"), static_cast<int32>(Stat)), S.Upgrades->GetStatDrawWeight(Stat), 3.f);
	}
	for (const ERRUpgradeStat Stat : Light)
	{
		TestEqual(*FString::Printf(TEXT("stat %d weight 1"), static_cast<int32>(Stat)), S.Upgrades->GetStatDrawWeight(Stat), 1.f);
	}

	// First card of 6000 offers at luck 0 (no weapon level): each heavy stat about 3 x each light one. Pierce is left out of the
	// light reference (its grey and green draws are dropped and redrawn).
	TMap<ERRUpgradeStat, int32> Counts;
	constexpr int32 Offers = 6000;
	for (int32 Offer = 0; Offer < Offers; ++Offer)
	{
		const TArray<FRRUpgradeCard> Cards = S.Upgrades->BuildOffer(3, 0.f);
		if (Cards.Num() > 0)
		{
			++Counts.FindOrAdd(Cards[0].Stat);
		}
	}
	float LightMean = 0.f;
	int32 LightStats = 0;
	for (const ERRUpgradeStat Stat : Light)
	{
		if (Stat != ERRUpgradeStat::Pierce)
		{
			LightMean += Counts.FindRef(Stat);
			++LightStats;
		}
	}
	LightMean /= FMath::Max(LightStats, 1);
	FString Line;
	for (const TPair<ERRUpgradeStat, int32>& Pair : Counts)
	{
		Line += FString::Printf(TEXT(" %d:%d"), static_cast<int32>(Pair.Key), Pair.Value);
	}
	AddInfo(FString::Printf(TEXT("DRAWWEIGHTS first-card counts (stat:count)%s; light mean %.1f"), *Line, LightMean));
	for (const ERRUpgradeStat Stat : Heavy)
	{
		const float Ratio = Counts.FindRef(Stat) / FMath::Max(LightMean, 1.f);
		TestTrue(*FString::Printf(TEXT("stat %d drawn about 3 x a light stat (%.2f)"), static_cast<int32>(Stat), Ratio), Ratio > 2.5f && Ratio < 3.5f);
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
