// Rogue Rigs - automation tests of the upgrade system (RogueRigs.Upgrades.*): value table, rarity draw, offer targets, GAS application, reroll, replay.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRMovementAttributeSet.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Tests/RRTestHarness.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRUpgradeTests
{
	using namespace RRTestHarness;

	/** Final burst rifle Blueprint (an extra, unowned copy is spawned by some tests). */
	const TCHAR* const UpgradeRifleClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle.BP_BurstRifle_C");

	/** What the setup returns. */
	struct FUpgradeSetup
	{
		/** The spawned BP_Miner. */
		ARRMiner* Miner = nullptr;
		/** Its upgrade component. */
		URRUpgradeComponent* Upgrades = nullptr;
		/** Its starting BP_BurstRifle. */
		ARRWeapon* Rifle = nullptr;
	};

	/** Spawns the grid and BP_Miner with the rifle only (weapons stopped) and checks it holds an upgrade component and one rifle. */
	bool SpawnUpgradeSetup(FAutomationTestBase& Test, const FTestWorld& TestWorld, FUpgradeSetup& Out)
	{
		ARRArenaGrid* Grid = SpawnGrid(Test, TestWorld);
		// Rifle only (no shotgun): the seeded draws and target counts below were measured with one weapon; the shotgun cards are proven by RogueRigs.Shotgun.CardsOnlyWhenEquipped.
		Out.Miner = Grid ? SpawnMiner(Test, TestWorld, Grid, RockWestFloorCell, KeepRifleOnly) : nullptr;
		if (!Out.Miner)
		{
			return false;
		}
		for (ARRWeapon* Weapon : Out.Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		Out.Upgrades = Out.Miner->GetUpgrades();
		if (!Test.TestNotNull(TEXT("miner has an upgrade component"), Out.Upgrades)
			|| !Test.TestEqual(TEXT("miner owns one weapon"), Out.Miner->GetWeapons().Num(), 1))
		{
			return false;
		}
		Out.Rifle = Out.Miner->GetWeapons()[0];
		Out.Upgrades->SetRandomSeed(20261003);
		return Test.TestNotNull(TEXT("rifle spawned"), Out.Rifle);
	}

	/** Current value of Attribute on Actor's ASC. */
	float UpgradeAttr(const AActor* Actor, const FGameplayAttribute& Attribute)
	{
		const IAbilitySystemInterface* Owner = Cast<IAbilitySystemInterface>(Actor);
		const UAbilitySystemComponent* ASC = Owner ? Owner->GetAbilitySystemComponent() : nullptr;
		return ASC ? ASC->GetNumericAttribute(Attribute) : -1.f;
	}

	/** Builds a card or fails the test. */
	FRRUpgradeCard MakeUpgradeCard(FAutomationTestBase& Test, const URRUpgradeComponent* Upgrades, ERRUpgradeStat Stat, ERRUpgradeRarity Tier,
		ERRUpgradeTarget Target, ARRWeapon* Weapon = nullptr)
	{
		FRRUpgradeCard Result;
		Test.TestTrue(FString::Printf(TEXT("card %s %s builds"), *UEnum::GetValueAsString(Stat), *UEnum::GetValueAsString(Tier)),
			Upgrades->MakeCard(Stat, Tier, Target, Weapon, Result));
		return Result;
	}
}

using namespace RRUpgradeTests;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRUpgradeValueLookupTest, "RogueRigs.Upgrades.ValueLookup",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRUpgradeValueLookupTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FUpgradeSetup S;
	if (!SpawnUpgradeSetup(*this, TestWorld, S))
	{
		return false;
	}

	struct FExpected { ERRUpgradeStat Stat; float Values[5]; };
	const FExpected Table[] = {
		{ ERRUpgradeStat::Damage,       { 0.10f, 0.15f, 0.25f, 0.30f, 0.40f } },
		{ ERRUpgradeStat::FireRate,     { 0.10f, 0.15f, 0.25f, 0.30f, 0.40f } },
		{ ERRUpgradeStat::ReloadSpeed,  { 0.10f, 0.15f, 0.25f, 0.30f, 0.40f } },
		{ ERRUpgradeStat::CritChance,   { 0.03f, 0.05f, 0.08f, 0.11f, 0.15f } },
		{ ERRUpgradeStat::CritDamage,   { 0.10f, 0.20f, 0.30f, 0.40f, 0.50f } },
		{ ERRUpgradeStat::MaxHealth,    { 20.f, 30.f, 45.f, 60.f, 80.f } },
		{ ERRUpgradeStat::MoveSpeed,    { 0.10f, 0.15f, 0.25f, 0.30f, 0.40f } },
		{ ERRUpgradeStat::PickupRadius, { 0.10f, 0.15f, 0.25f, 0.30f, 0.40f } },
		{ ERRUpgradeStat::XPGain,       { 0.10f, 0.15f, 0.25f, 0.30f, 0.40f } },
		{ ERRUpgradeStat::MiningSpeed,  { 0.10f, 0.15f, 0.25f, 0.30f, 0.40f } },
		{ ERRUpgradeStat::Luck,         { 1.f, 2.f, 3.f, 4.f, 5.f } },
	};
	for (const FExpected& Row : Table)
	{
		for (int32 Rank = 0; Rank < 5; ++Rank)
		{
			const ERRUpgradeRarity Tier = static_cast<ERRUpgradeRarity>(Rank);
			const FString Label = UEnum::GetValueAsString(Row.Stat) + TEXT(" ") + UEnum::GetValueAsString(Tier);
			float Global = -1.f;
			float Targeted = -1.f;
			TestTrue(Label + TEXT(" global row"), S.Upgrades->GetUpgradeValue(Row.Stat, Tier, false, Global));
			TestTrue(Label + TEXT(" targeted row"), S.Upgrades->GetUpgradeValue(Row.Stat, Tier, true, Targeted));
			TestEqual(Label + TEXT(" global value"), Global, Row.Values[Rank], 1e-5f);
			TestEqual(Label + TEXT(" targeted = 1.5 x global"), Targeted, Row.Values[Rank] * 1.5f, 1e-5f);
		}
	}
	float Unused = 0.f;
	TestFalse(TEXT("no magazine row (M1)"), S.Upgrades->GetUpgradeValue(ERRUpgradeStat::MagazineSize, ERRUpgradeRarity::Grey, false, Unused));

	// Cards carry the value and the English texts.
	const FRRUpgradeCard Global = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue, ERRUpgradeTarget::AllWeapons);
	TestEqual(TEXT("global damage blue value"), Global.Value, 0.25f, 1e-5f);
	TestEqual(TEXT("global damage name"), Global.DisplayName.ToString(), FString(TEXT("Damage")));
	TestEqual(TEXT("global damage target"), Global.TargetText.ToString(), FString(TEXT("All weapons")));
	TestEqual(TEXT("global damage value text"), Global.ValueText.ToString(), FString(TEXT("+25%")));
	TestTrue(TEXT("global damage operation"), Global.Operation == ERRUpgradeOperation::MultiplyAdditive);

	const FRRUpgradeCard Targeted = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Damage, ERRUpgradeRarity::Grey, ERRUpgradeTarget::Weapon, S.Rifle);
	TestEqual(TEXT("targeted damage grey value"), Targeted.Value, 0.15f, 1e-5f);
	TestEqual(TEXT("targeted damage value text"), Targeted.ValueText.ToString(), FString(TEXT("+15%")));
	TestEqual(TEXT("targeted damage target = weapon name"), Targeted.TargetText.ToString(), FString(TEXT("Burst rifle")));

	const FRRUpgradeCard Health = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::MaxHealth, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Player);
	TestEqual(TEXT("max health text"), Health.ValueText.ToString(), FString(TEXT("+45")));
	TestEqual(TEXT("max health name"), Health.DisplayName.ToString(), FString(TEXT("Max health")));
	TestTrue(TEXT("max health has no target text"), Health.TargetText.IsEmpty());
	TestTrue(TEXT("max health operation"), Health.Operation == ERRUpgradeOperation::AddBase);

	const FRRUpgradeCard Crit = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::CritChance, ERRUpgradeRarity::Blue, ERRUpgradeTarget::AllWeapons);
	TestEqual(TEXT("crit chance text"), Crit.ValueText.ToString(), FString(TEXT("+8%")));

	FRRUpgradeCard Invalid;
	TestFalse(TEXT("a player stat cannot target a weapon"), S.Upgrades->MakeCard(ERRUpgradeStat::Luck, ERRUpgradeRarity::Grey, ERRUpgradeTarget::Weapon, S.Rifle, Invalid));
	TestFalse(TEXT("a weapon stat cannot target the player"), S.Upgrades->MakeCard(ERRUpgradeStat::Damage, ERRUpgradeRarity::Grey, ERRUpgradeTarget::Player, nullptr, Invalid));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRUpgradeRarityDistributionTest, "RogueRigs.Upgrades.RarityDistribution",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRUpgradeRarityDistributionTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FUpgradeSetup S;
	if (!SpawnUpgradeSetup(*this, TestWorld, S))
	{
		return false;
	}

	// Luck 0: the base weights 50 / 30 / 13 / 5 / 2. Luck 10, k 0.1: base x 2^rank = 50 / 60 / 52 / 40 / 32 (total 234).
	const float Luck0[5] = { 0.50f, 0.30f, 0.13f, 0.05f, 0.02f };
	const float Luck10[5] = { 50.f / 234.f, 60.f / 234.f, 52.f / 234.f, 40.f / 234.f, 32.f / 234.f };
	const TArray<float> Weights0 = S.Upgrades->GetTierWeights(0.f);
	const TArray<float> Weights10 = S.Upgrades->GetTierWeights(10.f);
	for (int32 Rank = 0; Rank < 5; ++Rank)
	{
		TestEqual(FString::Printf(TEXT("luck 0 weight rank %d"), Rank), Weights0[Rank], Luck0[Rank], 1e-5f);
		TestEqual(FString::Printf(TEXT("luck 10 weight rank %d"), Rank), Weights10[Rank], Luck10[Rank], 1e-5f);
	}

	constexpr int32 Draws = 20000;
	double MeanRank[2] = {};
	for (int32 Pass = 0; Pass < 2; ++Pass)
	{
		const float Luck = Pass == 0 ? 0.f : 10.f;
		const float* Expected = Pass == 0 ? Luck0 : Luck10;
		S.Upgrades->SetRandomSeed(20261003);
		int32 Counts[5] = {};
		for (int32 Index = 0; Index < Draws; ++Index)
		{
			const int32 Rank = static_cast<int32>(S.Upgrades->DrawTier(Luck));
			++Counts[Rank];
			MeanRank[Pass] += Rank;
		}
		MeanRank[Pass] /= Draws;
		for (int32 Rank = 0; Rank < 5; ++Rank)
		{
			const float Share = static_cast<float>(Counts[Rank]) / Draws;
			AddInfo(FString::Printf(TEXT("luck %.0f rank %d: %d draws = %.2f %% (expected %.2f %%)"), Luck, Rank, Counts[Rank], Share * 100.f, Expected[Rank] * 100.f));
			TestEqual(FString::Printf(TEXT("luck %.0f share of rank %d near its weight"), Luck, Rank), Share, Expected[Rank], 0.015f);
		}
	}
	TestTrue(FString::Printf(TEXT("luck 10 shifts the draw up (mean rank %.3f > %.3f)"), MeanRank[1], MeanRank[0]), MeanRank[1] > MeanRank[0] + 0.5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRUpgradeOfferTargetsTest, "RogueRigs.Upgrades.OfferTargets",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRUpgradeOfferTargetsTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FUpgradeSetup S;
	if (!SpawnUpgradeSetup(*this, TestWorld, S))
	{
		return false;
	}

	// A second rifle in the world, acquired by no miner: never a target.
	UClass* RifleClass = LoadTestClass<ARRWeapon>(*this, UpgradeRifleClassPath);
	ARRWeapon* Unowned = RifleClass ? TestWorld.Spawn<ARRWeapon>(RifleClass, FVector(0.f, 0.f, 500.f)) : nullptr;
	if (!TestNotNull(TEXT("unowned rifle spawned"), Unowned))
	{
		return false;
	}
	Unowned->Acquire(nullptr);
	Unowned->GetAutoFireComponent()->StopFiring();

	constexpr int32 Offers = 300;
	int32 MagazineCards = 0;
	int32 BadTargets = 0;
	int32 Duplicates = 0;
	int32 ShortOffers = 0;
	int32 RifleCards = 0;
	int32 GlobalCards = 0;
	int32 PlayerCards = 0;
	for (int32 Offer = 0; Offer < Offers; ++Offer)
	{
		const TArray<FRRUpgradeCard> Cards = S.Upgrades->BuildOffer(3, static_cast<float>(Offer % 11));
		ShortOffers += Cards.Num() != 3 ? 1 : 0;
		for (int32 Index = 0; Index < Cards.Num(); ++Index)
		{
			const FRRUpgradeCard& C = Cards[Index];
			MagazineCards += C.Stat == ERRUpgradeStat::MagazineSize ? 1 : 0;
			const bool bWeaponStat = URRUpgradeComponent::IsWeaponStat(C.Stat);
			const bool bTargetOk = bWeaponStat
				? (C.Target == ERRUpgradeTarget::AllWeapons && C.TargetWeapon == nullptr) || (C.Target == ERRUpgradeTarget::Weapon && C.TargetWeapon == S.Rifle)
				: C.Target == ERRUpgradeTarget::Player && C.TargetWeapon == nullptr;
			BadTargets += bTargetOk ? 0 : 1;
			RifleCards += C.Target == ERRUpgradeTarget::Weapon ? 1 : 0;
			GlobalCards += C.Target == ERRUpgradeTarget::AllWeapons ? 1 : 0;
			PlayerCards += C.Target == ERRUpgradeTarget::Player ? 1 : 0;
			for (int32 Other = Index + 1; Other < Cards.Num(); ++Other)
			{
				Duplicates += C.IsSameChoice(Cards[Other]) ? 1 : 0;
			}
		}
	}
	AddInfo(FString::Printf(TEXT("%d offers: %d player, %d all-weapons, %d rifle cards"), Offers, PlayerCards, GlobalCards, RifleCards));
	TestEqual(TEXT("every offer has 3 cards"), ShortOffers, 0);
	TestEqual(TEXT("never a magazine card"), MagazineCards, 0);
	TestEqual(TEXT("never an unowned weapon or a mismatched target"), BadTargets, 0);
	TestEqual(TEXT("no two cards of an offer share stat and target"), Duplicates, 0);
	TestTrue(TEXT("the owned rifle is offered as a target"), RifleCards > 0);
	TestTrue(TEXT("all-weapons cards are offered"), GlobalCards > 0);
	TestTrue(TEXT("player cards are offered"), PlayerCards > 0);

	FRRUpgradeCard ForUnowned;
	TestTrue(TEXT("a card for the unowned rifle can be built"),
		S.Upgrades->MakeCard(ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Weapon, Unowned, ForUnowned));
	TestFalse(TEXT("but applying it is refused"), S.Upgrades->ApplyUpgrade(ForUnowned));
	TestEqual(TEXT("unowned rifle damage untouched"), UpgradeAttr(Unowned, URRWeaponAttributeSet::GetDamageAttribute()), 10.f, 1e-4f);
	TestEqual(TEXT("nothing picked"), S.Upgrades->GetPickedUpgrades().Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRUpgradeDamageAppliesTest, "RogueRigs.Upgrades.DamageAppliesToRifle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRUpgradeDamageAppliesTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FUpgradeSetup S;
	if (!SpawnUpgradeSetup(*this, TestWorld, S))
	{
		return false;
	}

	const FGameplayAttribute Damage = URRWeaponAttributeSet::GetDamageAttribute();
	TestEqual(TEXT("rifle damage 10 from the CSV"), UpgradeAttr(S.Rifle, Damage), 10.f, 1e-4f);

	S.Upgrades->BuildOffer(3, 0.f);
	TestTrue(TEXT("offer open"), S.Upgrades->HasOffer());
	const FRRUpgradeCard Global = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue, ERRUpgradeTarget::AllWeapons);
	TestTrue(TEXT("global damage +25 % applied"), S.Upgrades->ApplyUpgrade(Global));
	TestEqual(TEXT("rifle damage 10 x 1.25"), UpgradeAttr(S.Rifle, Damage), 12.5f, 1e-4f);
	TestEqual(TEXT("rifle damage base unchanged"), S.Rifle->GetAbilitySystemComponent()->GetNumericAttributeBase(Damage), 10.f, 1e-4f);
	TestFalse(TEXT("applying ends the offer"), S.Upgrades->HasOffer());
	TestEqual(TEXT("one picked upgrade"), S.Upgrades->GetPickedUpgrades().Num(), 1);

	const FRRUpgradeCard Targeted = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::FireRate, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Weapon, S.Rifle);
	TestTrue(TEXT("targeted fire rate +37.5 % applied"), S.Upgrades->ApplyUpgrade(Targeted));
	TestEqual(TEXT("rifle fire rate 1.25 x 1.375"), UpgradeAttr(S.Rifle, URRWeaponAttributeSet::GetFireRateAttribute()), 1.25f * 1.375f, 1e-4f);

	const FRRUpgradeCard Speed = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::MoveSpeed, ERRUpgradeRarity::Grey, ERRUpgradeTarget::Player);
	TestTrue(TEXT("move speed +10 % applied"), S.Upgrades->ApplyUpgrade(Speed));
	TestEqual(TEXT("miner move speed 600 x 1.1"), UpgradeAttr(S.Miner, URRMovementAttributeSet::GetMoveSpeedAttribute()), 660.f, 1e-3f);
	TestEqual(TEXT("three picked upgrades"), S.Upgrades->GetPickedUpgrades().Num(), 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRUpgradeMaxHealthTest, "RogueRigs.Upgrades.MaxHealthRaisesHealth",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRUpgradeMaxHealthTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FUpgradeSetup S;
	if (!SpawnUpgradeSetup(*this, TestWorld, S))
	{
		return false;
	}

	const FGameplayAttribute Health = URRHealthAttributeSet::GetHealthAttribute();
	const FGameplayAttribute MaxHealth = URRHealthAttributeSet::GetMaxHealthAttribute();
	TestEqual(TEXT("max health 100"), UpgradeAttr(S.Miner, MaxHealth), 100.f, 1e-4f);
	TestEqual(TEXT("health 100"), UpgradeAttr(S.Miner, Health), 100.f, 1e-4f);

	const FRRUpgradeCard Plus20 = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::MaxHealth, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Player);
	TestTrue(TEXT("max health +45 applied"), S.Upgrades->ApplyUpgrade(Plus20));
	TestEqual(TEXT("max health 145"), UpgradeAttr(S.Miner, MaxHealth), 145.f, 1e-4f);
	TestEqual(TEXT("health 145"), UpgradeAttr(S.Miner, Health), 145.f, 1e-4f);

	// Wounded miner: the bonus raises current health by the same amount.
	S.Miner->GetAbilitySystemComponent()->SetNumericAttributeBase(Health, 50.f);
	TestTrue(TEXT("second max health +45 applied"), S.Upgrades->ApplyUpgrade(Plus20));
	TestEqual(TEXT("max health 190"), UpgradeAttr(S.Miner, MaxHealth), 190.f, 1e-4f);
	TestEqual(TEXT("health 50 + 45"), UpgradeAttr(S.Miner, Health), 95.f, 1e-4f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRUpgradeStackingTest, "RogueRigs.Upgrades.PercentStacking",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRUpgradeStackingTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FUpgradeSetup S;
	if (!SpawnUpgradeSetup(*this, TestWorld, S))
	{
		return false;
	}

	const FGameplayAttribute Damage = URRWeaponAttributeSet::GetDamageAttribute();
	const FRRUpgradeCard Global = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue, ERRUpgradeTarget::AllWeapons);
	S.Upgrades->ApplyUpgrade(Global);
	S.Upgrades->ApplyUpgrade(Global);
	TestEqual(TEXT("2 x +25 % = x1.5 (added, not compounded)"), UpgradeAttr(S.Rifle, Damage), 15.f, 1e-4f);

	const FRRUpgradeCard Targeted = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Weapon, S.Rifle);
	S.Upgrades->ApplyUpgrade(Targeted);
	TestEqual(TEXT("+ targeted 37.5 % = x1.875"), UpgradeAttr(S.Rifle, Damage), 18.75f, 1e-4f);

	const FGameplayAttribute CritDamage = URRWeaponAttributeSet::GetCritDamageAttribute();
	TestEqual(TEXT("crit damage x1.5 base"), UpgradeAttr(S.Rifle, CritDamage), 1.5f, 1e-4f);
	S.Upgrades->ApplyUpgrade(MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::CritDamage, ERRUpgradeRarity::Blue, ERRUpgradeTarget::AllWeapons));
	TestEqual(TEXT("crit damage 1.5 + 0.3 = 1.8"), UpgradeAttr(S.Rifle, CritDamage), 1.8f, 1e-4f);

	const FGameplayAttribute Luck = URRPlayerAttributeSet::GetLuckAttribute();
	S.Upgrades->ApplyUpgrade(MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Luck, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Player));
	S.Upgrades->ApplyUpgrade(MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Luck, ERRUpgradeRarity::Grey, ERRUpgradeTarget::Player));
	TestEqual(TEXT("luck 0 + 3 + 1"), UpgradeAttr(S.Miner, Luck), 4.f, 1e-4f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRUpgradeRerollTest, "RogueRigs.Upgrades.RerollRepeat",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRUpgradeRerollTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FUpgradeSetup S;
	if (!SpawnUpgradeSetup(*this, TestWorld, S))
	{
		return false;
	}

	TArray<FRRUpgradeCard> Cards;
	TestFalse(TEXT("no reroll without an offer"), S.Upgrades->Reroll(Cards));

	for (int32 Round = 0; Round < 50; ++Round)
	{
		const TArray<FRRUpgradeCard> First = S.Upgrades->BuildOffer(3, 0.f);
		TestTrue(TEXT("reroll available on a new offer"), S.Upgrades->CanReroll());
		TArray<FRRUpgradeCard> Second;
		if (!TestTrue(TEXT("first reroll accepted"), S.Upgrades->Reroll(Second)))
		{
			return false;
		}
		TestEqual(TEXT("reroll gives 3 cards"), Second.Num(), 3);
		int32 Shared = 0;
		for (const FRRUpgradeCard& New : Second)
		{
			for (const FRRUpgradeCard& Old : First)
			{
				Shared += New.IsSameChoice(Old) ? 1 : 0;
			}
		}
		TestEqual(TEXT("rerolled cards share no stat and target with the replaced ones"), Shared, 0);
		TestTrue(TEXT("no limit per offer (task-315: the caller pays)"), S.Upgrades->CanReroll());

		TArray<FRRUpgradeCard> Third;
		TestTrue(TEXT("second reroll accepted"), S.Upgrades->Reroll(Third));
		TestEqual(TEXT("second reroll gives 3 cards"), Third.Num(), 3);
		int32 SharedAgain = 0;
		for (const FRRUpgradeCard& New : Third)
		{
			for (const FRRUpgradeCard& Old : Second)
			{
				SharedAgain += New.IsSameChoice(Old) ? 1 : 0;
			}
		}
		TestEqual(TEXT("second reroll shares no stat and target with the first reroll"), SharedAgain, 0);
	}

	S.Upgrades->ApplyUpgrade(S.Upgrades->GetCurrentOffer()[0]);
	TestFalse(TEXT("no reroll after picking"), S.Upgrades->Reroll(Cards));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRUpgradeReplayTest, "RogueRigs.Upgrades.ReplayOnAcquire",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRUpgradeReplayTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FUpgradeSetup S;
	if (!SpawnUpgradeSetup(*this, TestWorld, S))
	{
		return false;
	}

	const FGameplayAttribute Damage = URRWeaponAttributeSet::GetDamageAttribute();
	const FGameplayAttribute FireRate = URRWeaponAttributeSet::GetFireRateAttribute();
	S.Upgrades->ApplyUpgrade(MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue, ERRUpgradeTarget::AllWeapons));
	S.Upgrades->ApplyUpgrade(MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::FireRate, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Weapon, S.Rifle));

	// A weapon acquired later by the miner gets the global upgrades, not the other weapon's targeted one.
	UClass* RifleClass = LoadTestClass<ARRWeapon>(*this, UpgradeRifleClassPath);
	ARRWeapon* Later = RifleClass ? TestWorld.Spawn<ARRWeapon>(RifleClass, S.Miner->GetActorLocation()) : nullptr;
	if (!TestNotNull(TEXT("second rifle spawned"), Later))
	{
		return false;
	}
	TestTrue(TEXT("second rifle acquired by the miner"), Later->Acquire(S.Miner));
	Later->GetAutoFireComponent()->StopFiring();
	TestEqual(TEXT("global damage replayed: 12.5"), UpgradeAttr(Later, Damage), 12.5f, 1e-4f);
	TestEqual(TEXT("targeted fire rate not replayed: 1.25"), UpgradeAttr(Later, FireRate), 1.25f, 1e-4f);
	TestEqual(TEXT("first rifle keeps both"), UpgradeAttr(S.Rifle, FireRate), 1.25f * 1.375f, 1e-4f);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
