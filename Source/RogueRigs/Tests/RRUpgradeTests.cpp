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
#include "Tests/RRTestStatsRows.h"
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

	/** Card value of Stat at Tier from UpgradeValues.csv (GlobalValue, x TargetedMultiplier when bTargeted). */
	float CsvValue(FAutomationTestBase& Test, ERRUpgradeStat Stat, ERRUpgradeRarity Tier, bool bTargeted = false)
	{
		return RRTestStats::UpgradeCardValue(Test, Stat, Tier, bTargeted);
	}

	/** Expected card text of Value: percent for rate stats, flat for MaxHealth / Luck; one decimal at most, no trailing ".0". */
	FString ExpectedValueText(ERRUpgradeStat Stat, float Value)
	{
		const bool bFlat = Stat == ERRUpgradeStat::MaxHealth || Stat == ERRUpgradeStat::Luck;
		const double Rounded = FMath::RoundToDouble((bFlat ? Value : Value * 100.0) * 10.0) / 10.0;
		const FString Number = FMath::IsNearlyEqual(Rounded, FMath::RoundToDouble(Rounded))
			? FString::Printf(TEXT("%lld"), static_cast<long long>(FMath::RoundToDouble(Rounded)))
			: FString::Printf(TEXT("%.1f"), Rounded);
		return TEXT("+") + Number + (bFlat ? TEXT("") : TEXT("%"));
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

	// Every stat with a row per tier in UpgradeValues.csv ("<Stat>_<Tier>"): global = GlobalValue, targeted = GlobalValue x TargetedMultiplier.
	const ERRUpgradeStat Stats[] = { ERRUpgradeStat::Damage, ERRUpgradeStat::FireRate, ERRUpgradeStat::ReloadSpeed, ERRUpgradeStat::CritChance,
		ERRUpgradeStat::CritDamage, ERRUpgradeStat::MaxHealth, ERRUpgradeStat::MoveSpeed, ERRUpgradeStat::PickupRadius, ERRUpgradeStat::XPGain,
		ERRUpgradeStat::MiningSpeed, ERRUpgradeStat::Luck };
	for (const ERRUpgradeStat Stat : Stats)
	{
		for (int32 Rank = 0; Rank < 5; ++Rank)
		{
			const ERRUpgradeRarity Tier = static_cast<ERRUpgradeRarity>(Rank);
			const FString Label = UEnum::GetValueAsString(Stat) + TEXT(" ") + UEnum::GetValueAsString(Tier);
			const FRRUpgradeValueRow Row = RRTestStats::UpgradeValueRow(*this, Stat, Tier);
			TestTrue(Label + TEXT(" CSV GlobalValue > 0"), Row.GlobalValue > 0.f);
			float Global = -1.f;
			float Targeted = -1.f;
			TestTrue(Label + TEXT(" global row"), S.Upgrades->GetUpgradeValue(Stat, Tier, false, Global));
			TestTrue(Label + TEXT(" targeted row"), S.Upgrades->GetUpgradeValue(Stat, Tier, true, Targeted));
			TestEqual(Label + TEXT(" global value = GlobalValue"), Global, Row.GlobalValue, 1e-5f);
			TestEqual(Label + TEXT(" targeted = TargetedMultiplier x global"), Targeted, Row.GlobalValue * Row.TargetedMultiplier, 1e-5f);
		}
	}
	float Unused = 0.f;
	TestFalse(TEXT("no magazine row (M1)"), S.Upgrades->GetUpgradeValue(ERRUpgradeStat::MagazineSize, ERRUpgradeRarity::Grey, false, Unused));

	// Cards carry the value and the English texts.
	const FRRUpgradeCard Global = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue, ERRUpgradeTarget::AllWeapons);
	const float DamageBlue = CsvValue(*this, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue);
	TestEqual(TEXT("global damage blue value"), Global.Value, DamageBlue, 1e-5f);
	TestEqual(TEXT("global damage name"), Global.DisplayName.ToString(), FString(TEXT("Damage")));
	TestEqual(TEXT("global damage target"), Global.TargetText.ToString(), FString(TEXT("All weapons")));
	TestEqual(TEXT("global damage value text"), Global.ValueText.ToString(), ExpectedValueText(ERRUpgradeStat::Damage, DamageBlue));
	TestTrue(TEXT("global damage operation"), Global.Operation == ERRUpgradeOperation::MultiplyAdditive);

	const FRRUpgradeCard Targeted = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Damage, ERRUpgradeRarity::Grey, ERRUpgradeTarget::Weapon, S.Rifle);
	const float DamageGreyTargeted = CsvValue(*this, ERRUpgradeStat::Damage, ERRUpgradeRarity::Grey, true);
	TestEqual(TEXT("targeted damage grey value"), Targeted.Value, DamageGreyTargeted, 1e-5f);
	TestEqual(TEXT("targeted damage value text"), Targeted.ValueText.ToString(), ExpectedValueText(ERRUpgradeStat::Damage, DamageGreyTargeted));
	TestEqual(TEXT("targeted damage target = weapon name (CSV DisplayName)"), Targeted.TargetText.ToString(),
		RRTestStats::WeaponRow(*this, RRTestStats::BurstRifleRow).DisplayName);

	const FRRUpgradeCard Health = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::MaxHealth, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Player);
	TestEqual(TEXT("max health text"), Health.ValueText.ToString(),
		ExpectedValueText(ERRUpgradeStat::MaxHealth, CsvValue(*this, ERRUpgradeStat::MaxHealth, ERRUpgradeRarity::Blue)));
	TestEqual(TEXT("max health name"), Health.DisplayName.ToString(), FString(TEXT("Max health")));
	TestTrue(TEXT("max health has no target text"), Health.TargetText.IsEmpty());
	TestTrue(TEXT("max health operation"), Health.Operation == ERRUpgradeOperation::AddBase);

	const FRRUpgradeCard Crit = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::CritChance, ERRUpgradeRarity::Blue, ERRUpgradeTarget::AllWeapons);
	TestEqual(TEXT("crit chance text"), Crit.ValueText.ToString(),
		ExpectedValueText(ERRUpgradeStat::CritChance, CsvValue(*this, ERRUpgradeStat::CritChance, ERRUpgradeRarity::Blue)));

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

	// UpgradeRarities.csv: luck 0 draws each tier by BaseWeight; luck L by BaseWeight x (1 + LuckFactor x L)^rank; both normalised.
	float Luck0[5] = {};
	float Luck10[5] = {};
	double Total0 = 0.0;
	double Total10 = 0.0;
	for (int32 Rank = 0; Rank < 5; ++Rank)
	{
		const FRRUpgradeRarityRow Rarity = RRTestStats::RarityRow(*this, static_cast<ERRUpgradeRarity>(Rank));
		Luck0[Rank] = Rarity.BaseWeight;
		Luck10[Rank] = Rarity.BaseWeight * FMath::Pow(FMath::Max(0.f, 1.f + Rarity.LuckFactor * 10.f), static_cast<float>(Rank));
		Total0 += Luck0[Rank];
		Total10 += Luck10[Rank];
	}
	if (!TestTrue(TEXT("CSV: rarity weights > 0"), Total0 > 0.0 && Total10 > 0.0))
	{
		return false;
	}
	for (int32 Rank = 0; Rank < 5; ++Rank)
	{
		Luck0[Rank] = static_cast<float>(Luck0[Rank] / Total0);
		Luck10[Rank] = static_cast<float>(Luck10[Rank] / Total10);
	}
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
	TestEqual(TEXT("unowned rifle damage untouched (CSV Damage)"), UpgradeAttr(Unowned, URRWeaponAttributeSet::GetDamageAttribute()),
		RRTestStats::WeaponRow(*this, RRTestStats::BurstRifleRow).Damage, 1e-4f);
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

	// CSV rows: BurstRifle Damage / FireRate, Miner MoveSpeed, upgrade values Damage_Blue, FireRate_Blue (targeted), MoveSpeed_Grey.
	const FRRWeaponStatsRow RifleRow = RRTestStats::WeaponRow(*this, RRTestStats::BurstRifleRow);
	const FGameplayAttribute Damage = URRWeaponAttributeSet::GetDamageAttribute();
	TestEqual(TEXT("rifle damage from the CSV"), UpgradeAttr(S.Rifle, Damage), RifleRow.Damage, 1e-4f);

	S.Upgrades->BuildOffer(3, 0.f);
	TestTrue(TEXT("offer open"), S.Upgrades->HasOffer());
	const FRRUpgradeCard Global = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue, ERRUpgradeTarget::AllWeapons);
	const float DamageBlue = CsvValue(*this, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue);
	TestTrue(TEXT("global damage blue applied"), S.Upgrades->ApplyUpgrade(Global));
	TestEqual(TEXT("rifle damage x (1 + Damage_Blue)"), UpgradeAttr(S.Rifle, Damage), RifleRow.Damage * (1.f + DamageBlue), 1e-4f);
	TestEqual(TEXT("rifle damage base unchanged"), S.Rifle->GetAbilitySystemComponent()->GetNumericAttributeBase(Damage), RifleRow.Damage, 1e-4f);
	TestFalse(TEXT("applying ends the offer"), S.Upgrades->HasOffer());
	TestEqual(TEXT("one picked upgrade"), S.Upgrades->GetPickedUpgrades().Num(), 1);

	const FRRUpgradeCard Targeted = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::FireRate, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Weapon, S.Rifle);
	TestTrue(TEXT("targeted fire rate blue applied"), S.Upgrades->ApplyUpgrade(Targeted));
	TestEqual(TEXT("rifle fire rate x (1 + targeted FireRate_Blue)"), UpgradeAttr(S.Rifle, URRWeaponAttributeSet::GetFireRateAttribute()),
		RifleRow.FireRate * (1.f + CsvValue(*this, ERRUpgradeStat::FireRate, ERRUpgradeRarity::Blue, true)), 1e-4f);

	const FRRUpgradeCard Speed = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::MoveSpeed, ERRUpgradeRarity::Grey, ERRUpgradeTarget::Player);
	TestTrue(TEXT("move speed grey applied"), S.Upgrades->ApplyUpgrade(Speed));
	TestEqual(TEXT("miner move speed x (1 + MoveSpeed_Grey)"), UpgradeAttr(S.Miner, URRMovementAttributeSet::GetMoveSpeedAttribute()),
		RRTestStats::PlayerRow(*this).MoveSpeed * (1.f + CsvValue(*this, ERRUpgradeStat::MoveSpeed, ERRUpgradeRarity::Grey)), 1e-3f);
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
	// CSV rows: Miner MaxHealth (PlayerBaseStats), MaxHealth_Blue (UpgradeValues).
	const float Base = RRTestStats::PlayerRow(*this).MaxHealth;
	const float Bonus = CsvValue(*this, ERRUpgradeStat::MaxHealth, ERRUpgradeRarity::Blue);
	TestEqual(TEXT("max health = CSV MaxHealth"), UpgradeAttr(S.Miner, MaxHealth), Base, 1e-4f);
	TestEqual(TEXT("health = CSV MaxHealth"), UpgradeAttr(S.Miner, Health), Base, 1e-4f);

	const FRRUpgradeCard Plus20 = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::MaxHealth, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Player);
	TestTrue(TEXT("max health blue applied"), S.Upgrades->ApplyUpgrade(Plus20));
	TestEqual(TEXT("max health + bonus"), UpgradeAttr(S.Miner, MaxHealth), Base + Bonus, 1e-4f);
	TestEqual(TEXT("health + bonus"), UpgradeAttr(S.Miner, Health), Base + Bonus, 1e-4f);

	// Wounded miner (health 50, scenario value): the bonus raises current health by the same amount.
	S.Miner->GetAbilitySystemComponent()->SetNumericAttributeBase(Health, 50.f);
	TestTrue(TEXT("second max health blue applied"), S.Upgrades->ApplyUpgrade(Plus20));
	TestEqual(TEXT("max health + 2 x bonus"), UpgradeAttr(S.Miner, MaxHealth), Base + 2.f * Bonus, 1e-4f);
	TestEqual(TEXT("health 50 + bonus"), UpgradeAttr(S.Miner, Health), 50.f + Bonus, 1e-4f);
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

	// CSV rows: BurstRifle Damage / CritDamage, Miner Luck, upgrade values Damage_Blue (global, targeted), CritDamage_Blue, Luck_Blue, Luck_Grey.
	const FRRWeaponStatsRow RifleRow = RRTestStats::WeaponRow(*this, RRTestStats::BurstRifleRow);
	const float DamageBlue = CsvValue(*this, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue);
	const float DamageBlueTargeted = CsvValue(*this, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue, true);
	const FGameplayAttribute Damage = URRWeaponAttributeSet::GetDamageAttribute();
	const FRRUpgradeCard Global = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue, ERRUpgradeTarget::AllWeapons);
	S.Upgrades->ApplyUpgrade(Global);
	S.Upgrades->ApplyUpgrade(Global);
	TestEqual(TEXT("2 x global = x (1 + 2 x value) (added, not compounded)"), UpgradeAttr(S.Rifle, Damage), RifleRow.Damage * (1.f + 2.f * DamageBlue), 1e-4f);

	const FRRUpgradeCard Targeted = MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Weapon, S.Rifle);
	S.Upgrades->ApplyUpgrade(Targeted);
	TestEqual(TEXT("+ targeted value, still added"), UpgradeAttr(S.Rifle, Damage), RifleRow.Damage * (1.f + 2.f * DamageBlue + DamageBlueTargeted), 1e-4f);

	const FGameplayAttribute CritDamage = URRWeaponAttributeSet::GetCritDamageAttribute();
	TestEqual(TEXT("crit damage base from the CSV"), UpgradeAttr(S.Rifle, CritDamage), RifleRow.CritDamage, 1e-4f);
	S.Upgrades->ApplyUpgrade(MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::CritDamage, ERRUpgradeRarity::Blue, ERRUpgradeTarget::AllWeapons));
	TestEqual(TEXT("crit damage base + CritDamage_Blue"), UpgradeAttr(S.Rifle, CritDamage),
		RifleRow.CritDamage + CsvValue(*this, ERRUpgradeStat::CritDamage, ERRUpgradeRarity::Blue), 1e-4f);

	const FGameplayAttribute Luck = URRPlayerAttributeSet::GetLuckAttribute();
	S.Upgrades->ApplyUpgrade(MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Luck, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Player));
	S.Upgrades->ApplyUpgrade(MakeUpgradeCard(*this, S.Upgrades, ERRUpgradeStat::Luck, ERRUpgradeRarity::Grey, ERRUpgradeTarget::Player));
	TestEqual(TEXT("luck base + Luck_Blue + Luck_Grey"), UpgradeAttr(S.Miner, Luck), RRTestStats::PlayerRow(*this).Luck
		+ CsvValue(*this, ERRUpgradeStat::Luck, ERRUpgradeRarity::Blue) + CsvValue(*this, ERRUpgradeStat::Luck, ERRUpgradeRarity::Grey), 1e-4f);
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
	const FRRWeaponStatsRow RifleRow = RRTestStats::WeaponRow(*this, RRTestStats::BurstRifleRow);
	TestEqual(TEXT("global damage replayed: Damage x (1 + Damage_Blue)"), UpgradeAttr(Later, Damage),
		RifleRow.Damage * (1.f + CsvValue(*this, ERRUpgradeStat::Damage, ERRUpgradeRarity::Blue)), 1e-4f);
	TestEqual(TEXT("targeted fire rate not replayed: CSV FireRate"), UpgradeAttr(Later, FireRate), RifleRow.FireRate, 1e-4f);
	TestEqual(TEXT("first rifle keeps both"), UpgradeAttr(S.Rifle, FireRate),
		RifleRow.FireRate * (1.f + CsvValue(*this, ERRUpgradeStat::FireRate, ERRUpgradeRarity::Blue, true)), 1e-4f);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
