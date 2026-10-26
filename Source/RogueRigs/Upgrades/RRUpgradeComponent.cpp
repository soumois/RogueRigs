// Rogue Rigs - upgrade offer and application on the miner: CSV values, luck-weighted rarity draw, owned-weapon targets, new-weapon cards, GAS application, replay.

#include "Upgrades/RRUpgradeComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/DataTable.h"
#include "GameplayEffect.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRMovementAttributeSet.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "Text/RRTextContentSubsystem.h"
#include "Upgrades/RRUpgradeRarityRow.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"
#include "Weapons/RRWeaponCatalogueEntry.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRUpgradeComponent)

#define LOCTEXT_NAMESPACE "RRUpgrades"

const TCHAR* URRUpgradeComponent::UpgradeValuesPath = TEXT("RogueRigs/Data/Tables/UpgradeValues.csv");
const TCHAR* URRUpgradeComponent::UpgradeRaritiesPath = TEXT("RogueRigs/Data/Tables/UpgradeRarities.csv");

URRUpgradeComponent::URRUpgradeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void URRUpgradeComponent::InitializeComponent()
{
	Super::InitializeComponent();
	LoadTables();
	RandomStream.GenerateNewSeed();
}

void URRUpgradeComponent::LoadTables()
{
	ValueRows.Reset();
	StatDrawWeights.Reset();
	for (int32 Rank = 0; Rank < TierCount; ++Rank)
	{
		BaseWeights[Rank] = 0.f;
		LuckFactors[Rank] = 0.f;
	}

	if (const UDataTable* Values = FRRBaseStatsLoader::GetCachedTable(UpgradeValuesPath, FRRUpgradeValueRow::StaticStruct()))
	{
		for (const TPair<FName, uint8*>& Pair : Values->GetRowMap())
		{
			const FRRUpgradeValueRow* Row = reinterpret_cast<const FRRUpgradeValueRow*>(Pair.Value);
			if (Row == nullptr)
			{
				continue;
			}
			TArray<TOptional<FRRUpgradeValueRow>>& ByTier = ValueRows.FindOrAdd(Row->Stat);
			ByTier.SetNum(TierCount);
			const int32 Rank = static_cast<int32>(Row->Tier);
			if (ByTier[Rank].IsSet())
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("URRUpgradeComponent: duplicate (stat, tier) at row '%s' in %s; first row kept."), *Pair.Key.ToString(), UpgradeValuesPath);
				continue;
			}
			ByTier[Rank] = *Row;
		}
		// Draw weight per stat: the lowest tier row of the stat.
		for (const TPair<ERRUpgradeStat, TArray<TOptional<FRRUpgradeValueRow>>>& Pair : ValueRows)
		{
			for (const TOptional<FRRUpgradeValueRow>& Row : Pair.Value)
			{
				if (Row.IsSet())
				{
					StatDrawWeights.Add(Pair.Key, FMath::Max(Row->DrawWeight, 0.f));
					break;
				}
			}
		}
	}

	if (const UDataTable* Rarities = FRRBaseStatsLoader::GetCachedTable(UpgradeRaritiesPath, FRRUpgradeRarityRow::StaticStruct()))
	{
		const UEnum* TierEnum = StaticEnum<ERRUpgradeRarity>();
		for (const TPair<FName, uint8*>& Pair : Rarities->GetRowMap())
		{
			const int64 Rank = TierEnum->GetValueByNameString(Pair.Key.ToString());
			const FRRUpgradeRarityRow* Row = reinterpret_cast<const FRRUpgradeRarityRow*>(Pair.Value);
			if (Rank == INDEX_NONE || Rank < 0 || Rank >= TierCount || Row == nullptr)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("URRUpgradeComponent: unknown tier row '%s' in %s."), *Pair.Key.ToString(), UpgradeRaritiesPath);
				continue;
			}
			BaseWeights[Rank] = FMath::Max(Row->BaseWeight, 0.f);
			LuckFactors[Rank] = Row->LuckFactor;
		}
	}
}

bool URRUpgradeComponent::IsWeaponStat(ERRUpgradeStat Stat)
{
	return Stat <= ERRUpgradeStat::MagazineSize || Stat == ERRUpgradeStat::Pierce;
}

FGameplayAttribute URRUpgradeComponent::GetStatAttribute(ERRUpgradeStat Stat)
{
	switch (Stat)
	{
	case ERRUpgradeStat::Damage:       return URRWeaponAttributeSet::GetDamageAttribute();
	case ERRUpgradeStat::FireRate:     return URRWeaponAttributeSet::GetFireRateAttribute();
	case ERRUpgradeStat::ReloadSpeed:  return URRWeaponAttributeSet::GetReloadSpeedAttribute();
	case ERRUpgradeStat::CritChance:   return URRWeaponAttributeSet::GetCritChanceAttribute();
	case ERRUpgradeStat::CritDamage:   return URRWeaponAttributeSet::GetCritDamageAttribute();
	case ERRUpgradeStat::MagazineSize: return URRWeaponAttributeSet::GetMagazineSizeAttribute();
	case ERRUpgradeStat::MaxHealth:    return URRHealthAttributeSet::GetMaxHealthAttribute();
	case ERRUpgradeStat::MoveSpeed:    return URRMovementAttributeSet::GetMoveSpeedAttribute();
	case ERRUpgradeStat::PickupRadius: return URRPlayerAttributeSet::GetPickupRadiusAttribute();
	case ERRUpgradeStat::XPGain:       return URRPlayerAttributeSet::GetXPGainAttribute();
	case ERRUpgradeStat::MiningSpeed:  return URRPlayerAttributeSet::GetMiningSpeedAttribute();
	case ERRUpgradeStat::Luck:         return URRPlayerAttributeSet::GetLuckAttribute();
	case ERRUpgradeStat::Pierce:       return URRWeaponAttributeSet::GetPierceAttribute();
	}
	return FGameplayAttribute();
}

FText URRUpgradeComponent::GetStatDisplayName(ERRUpgradeStat Stat)
{
	switch (Stat)
	{
	case ERRUpgradeStat::Damage:       return LOCTEXT("Stat_Damage", "Damage");
	case ERRUpgradeStat::FireRate:     return LOCTEXT("Stat_FireRate", "Fire rate");
	case ERRUpgradeStat::ReloadSpeed:  return LOCTEXT("Stat_ReloadSpeed", "Reload speed");
	case ERRUpgradeStat::CritChance:   return LOCTEXT("Stat_CritChance", "Crit chance");
	case ERRUpgradeStat::CritDamage:   return LOCTEXT("Stat_CritDamage", "Crit damage");
	case ERRUpgradeStat::MagazineSize: return LOCTEXT("Stat_MagazineSize", "Magazine size");
	case ERRUpgradeStat::MaxHealth:    return LOCTEXT("Stat_MaxHealth", "Max health");
	case ERRUpgradeStat::MoveSpeed:    return LOCTEXT("Stat_MoveSpeed", "Move speed");
	case ERRUpgradeStat::PickupRadius: return LOCTEXT("Stat_PickupRadius", "Pickup radius");
	case ERRUpgradeStat::XPGain:       return LOCTEXT("Stat_XPGain", "XP gain");
	case ERRUpgradeStat::MiningSpeed:  return LOCTEXT("Stat_MiningSpeed", "Mining speed");
	case ERRUpgradeStat::Luck:         return LOCTEXT("Stat_Luck", "Luck");
	case ERRUpgradeStat::Pierce:       return LOCTEXT("Stat_Pierce", "Piercing");
	}
	return FText::GetEmpty();
}

FText URRUpgradeComponent::FormatValueText(ERRUpgradeStat Stat, float Value)
{
	const bool bFlat = Stat == ERRUpgradeStat::MaxHealth || Stat == ERRUpgradeStat::Luck || Stat == ERRUpgradeStat::MagazineSize
		|| Stat == ERRUpgradeStat::Pierce;
	const double Shown = bFlat ? Value : Value * 100.0;

	// One decimal at most, decimal point, no trailing ".0" (English UI, readability-rules).
	const double Rounded = FMath::RoundToDouble(Shown * 10.0) / 10.0;
	FString Number = FMath::IsNearlyEqual(Rounded, FMath::RoundToDouble(Rounded))
		? FString::Printf(TEXT("%lld"), static_cast<long long>(FMath::RoundToDouble(Rounded)))
		: FString::Printf(TEXT("%.1f"), Rounded);
	const FString Sign = Rounded >= 0.0 ? TEXT("+") : TEXT("");
	return FText::FromString(bFlat ? Sign + Number : Sign + Number + TEXT("%"));
}

bool URRUpgradeComponent::GetUpgradeValue(ERRUpgradeStat Stat, ERRUpgradeRarity Tier, bool bTargeted, float& OutValue) const
{
	const TArray<TOptional<FRRUpgradeValueRow>>* ByTier = ValueRows.Find(Stat);
	const int32 Rank = static_cast<int32>(Tier);
	if (ByTier == nullptr || !ByTier->IsValidIndex(Rank) || !(*ByTier)[Rank].IsSet())
	{
		return false;
	}
	const FRRUpgradeValueRow& Row = (*ByTier)[Rank].GetValue();
	OutValue = bTargeted ? Row.GlobalValue * Row.TargetedMultiplier : Row.GlobalValue;
	return true;
}

TArray<float> URRUpgradeComponent::GetTierWeights(float Luck) const
{
	TArray<float> Weights;
	Weights.SetNumZeroed(TierCount);
	double Total = 0.0;
	for (int32 Rank = 0; Rank < TierCount; ++Rank)
	{
		const double Growth = FMath::Max(0.0, 1.0 + static_cast<double>(LuckFactors[Rank]) * Luck);
		const double Weight = BaseWeights[Rank] * FMath::Pow(Growth, static_cast<double>(Rank));
		Weights[Rank] = static_cast<float>(Weight);
		Total += Weight;
	}
	if (Total > 0.0)
	{
		for (float& Weight : Weights)
		{
			Weight = static_cast<float>(Weight / Total);
		}
	}
	return Weights;
}

ERRUpgradeRarity URRUpgradeComponent::DrawTier(float Luck)
{
	const TArray<float> Weights = GetTierWeights(Luck);
	float Roll = RandomStream.FRand();
	for (int32 Rank = 0; Rank < TierCount; ++Rank)
	{
		if (Roll < Weights[Rank])
		{
			return static_cast<ERRUpgradeRarity>(Rank);
		}
		Roll -= Weights[Rank];
	}
	// Rounding left-over: the highest tier with a weight.
	for (int32 Rank = TierCount - 1; Rank >= 0; --Rank)
	{
		if (Weights[Rank] > 0.f)
		{
			return static_cast<ERRUpgradeRarity>(Rank);
		}
	}
	return ERRUpgradeRarity::Grey;
}

bool URRUpgradeComponent::MakeCard(ERRUpgradeStat Stat, ERRUpgradeRarity Tier, ERRUpgradeTarget Target, ARRWeapon* TargetWeapon, FRRUpgradeCard& OutCard) const
{
	const bool bWeaponStat = IsWeaponStat(Stat);
	if (bWeaponStat == (Target == ERRUpgradeTarget::Player) || (Target == ERRUpgradeTarget::Weapon && !IsValid(TargetWeapon)))
	{
		return false;
	}

	float Value = 0.f;
	if (!GetUpgradeValue(Stat, Tier, Target == ERRUpgradeTarget::Weapon, Value))
	{
		return false;
	}
	const TArray<TOptional<FRRUpgradeValueRow>>& ByTier = ValueRows.FindChecked(Stat);

	FRRUpgradeCard Card;
	Card.Stat = Stat;
	Card.Tier = Tier;
	Card.Target = Target;
	Card.TargetWeapon = Target == ERRUpgradeTarget::Weapon ? TargetWeapon : nullptr;
	Card.Value = Value;
	Card.Operation = ByTier[static_cast<int32>(Tier)]->Operation;
	Card.DisplayName = GetStatDisplayName(Stat);
	Card.ValueText = FormatValueText(Stat, Value);
	// Flavor line from upgrade_texts.json: alone in LoreText, and under the stat name in DescriptionText (kept for the current widget);
	// no line (file or stat missing) keeps both empty.
	if (URRTextContentSubsystem* TextContent = URRTextContentSubsystem::Get(this))
	{
		FText Flavor;
		if (TextContent->GetUpgradeFlavor(Card.DisplayName.ToString(), Flavor))
		{
			Card.DescriptionText = URRTextContentSubsystem::ComposeUpgradeDescription(Card.DisplayName, Flavor);
			Card.LoreText = Flavor;
		}
	}
	switch (Target)
	{
	case ERRUpgradeTarget::Player:     Card.TargetText = FText::GetEmpty(); break;
	case ERRUpgradeTarget::AllWeapons: Card.TargetText = LOCTEXT("Target_AllWeapons", "All weapons"); break;
	case ERRUpgradeTarget::Weapon:     Card.TargetText = TargetWeapon->GetDisplayName(); break;
	}
	OutCard = Card;
	return true;
}

bool URRUpgradeComponent::MakeWeaponCard(const FRRWeaponCatalogueEntry& Entry, FRRUpgradeCard& OutCard, const UObject* WorldContext)
{
	if (!Entry.WeaponClass)
	{
		return false;
	}
	FRRUpgradeCard Card;
	Card.CardType = ERRUpgradeCardType::NewWeapon;
	Card.WeaponClass = Entry.WeaponClass;
	Card.Target = ERRUpgradeTarget::Weapon;
	Card.DisplayName = LOCTEXT("Card_NewWeapon", "New weapon");
	Card.TargetText = ARRWeapon::GetClassDisplayName(Entry.WeaponClass);
	Card.DescriptionText = Entry.Description;
	// Lore line from weapon_texts.json, keyed by the weapon display name; empty when missing (task-169-03).
	if (URRTextContentSubsystem* TextContent = URRTextContentSubsystem::Get(WorldContext))
	{
		FText Flavor;
		if (TextContent->GetWeaponFlavor(Card.TargetText.ToString(), Flavor))
		{
			Card.LoreText = Flavor;
		}
	}
	OutCard = Card;
	return true;
}

TArray<ARRWeapon*> URRUpgradeComponent::GetTargetableWeapons() const
{
	TArray<ARRWeapon*> Result;
	if (const ARRMiner* Miner = Cast<ARRMiner>(GetOwner()))
	{
		for (ARRWeapon* Weapon : Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAbilitySystemComponent() != nullptr)
			{
				Result.Add(Weapon);
			}
		}
	}
	return Result;
}

TArray<URRUpgradeComponent::FChoice> URRUpgradeComponent::GatherChoices(const TArray<FRRUpgradeCard>& Excluded) const
{
	const TArray<ARRWeapon*> Weapons = GetTargetableWeapons();
	const bool bHasPlayer = GetOwnerASC() != nullptr;

	TArray<FChoice> Choices;
	for (const TPair<ERRUpgradeStat, TArray<TOptional<FRRUpgradeValueRow>>>& Pair : ValueRows)
	{
		const ERRUpgradeStat Stat = Pair.Key;
		// A stat with rows at some tiers only (Pierce) is offered; a drawn tier without a row drops the choice (DrawCards).
		if (!Pair.Value.ContainsByPredicate([](const TOptional<FRRUpgradeValueRow>& Row) { return Row.IsSet(); }))
		{
			continue;
		}

		TArray<FChoice> StatChoices;
		if (IsWeaponStat(Stat))
		{
			if (Weapons.IsEmpty())
			{
				continue;
			}
			StatChoices.Add({ Stat, ERRUpgradeTarget::AllWeapons, nullptr, 0.f });
			for (ARRWeapon* Weapon : Weapons)
			{
				StatChoices.Add({ Stat, ERRUpgradeTarget::Weapon, Weapon, 0.f });
			}
		}
		else if (bHasPlayer)
		{
			StatChoices.Add({ Stat, ERRUpgradeTarget::Player, nullptr, 0.f });
		}

		// A stat's odds follow its DrawWeight, whatever its number of targets.
		const float Weight = StatChoices.IsEmpty() ? 0.f : GetStatDrawWeight(Stat) / StatChoices.Num();
		for (FChoice& Choice : StatChoices)
		{
			const bool bExcluded = Excluded.ContainsByPredicate([&Choice](const FRRUpgradeCard& Card)
			{
				return Card.Stat == Choice.Stat && Card.Target == Choice.Target && Card.TargetWeapon == Choice.Weapon;
			});
			if (!bExcluded && Weight > 0.f)
			{
				Choice.Weight = Weight;
				Choices.Add(Choice);
			}
		}
	}

	// Stable order (TMap order is not part of the contract) so a seed always gives the same offer.
	Choices.StableSort([](const FChoice& A, const FChoice& B)
	{
		return A.Stat != B.Stat ? A.Stat < B.Stat : A.Target < B.Target;
	});
	return Choices;
}

TArray<FRRUpgradeCard> URRUpgradeComponent::DrawCards(TArray<FChoice> Choices, int32 CardCount, float Luck)
{
	TArray<FRRUpgradeCard> Cards;
	while (Cards.Num() < CardCount && !Choices.IsEmpty())
	{
		float Total = 0.f;
		for (const FChoice& Choice : Choices)
		{
			Total += Choice.Weight;
		}
		float Roll = RandomStream.FRand() * Total;
		int32 Picked = Choices.Num() - 1;
		for (int32 Index = 0; Index < Choices.Num(); ++Index)
		{
			if (Roll < Choices[Index].Weight)
			{
				Picked = Index;
				break;
			}
			Roll -= Choices[Index].Weight;
		}

		const FChoice Choice = Choices[Picked];
		Choices.RemoveAt(Picked);

		// MakeCard fails when the stat has no row at the drawn tier (Pierce below blue): the choice is dropped for this offer.
		FRRUpgradeCard Card;
		if (MakeCard(Choice.Stat, DrawTier(Luck), Choice.Target, Choice.Weapon, Card))
		{
			Cards.Add(Card);
		}
	}
	return Cards;
}

float URRUpgradeComponent::GetStatDrawWeight(ERRUpgradeStat Stat) const
{
	const float* Weight = StatDrawWeights.Find(Stat);
	return Weight ? *Weight : 0.f;
}

int32 URRUpgradeComponent::GetNewWeaponCardCount(int32 InOfferLevel, int32 Acquirable) const
{
	if (InOfferLevel <= 0 || WeaponCardLevelInterval <= 0 || InOfferLevel % WeaponCardLevelInterval != 0)
	{
		return 0;
	}
	return FMath::Clamp(Acquirable, 0, MaxNewWeaponCardsPerOffer);
}

TArray<FRRUpgradeCard> URRUpgradeComponent::DrawOffer(int32 CardCount, float Luck, const TArray<FRRUpgradeCard>& Excluded)
{
	// New-weapon slots first, only on weapon levels. Nothing is drawn from the stream otherwise, so stat draws keep their sequence.
	TArray<FRRWeaponCatalogueEntry> Candidates;
	const ARRMiner* Miner = Cast<ARRMiner>(GetOwner());
	const int32 WeaponCount = Miner ? FMath::Min(GetNewWeaponCardCount(OfferLevel, Miner->GetAcquirableWeapons().Num()), CardCount) : 0;
	if (WeaponCount > 0)
	{
		Candidates = Miner->GetAcquirableWeapons();
		Candidates.RemoveAll([&Excluded](const FRRWeaponCatalogueEntry& Entry)
		{
			return Excluded.ContainsByPredicate([&Entry](const FRRUpgradeCard& Card) { return Card.IsNewWeapon() && Card.WeaponClass == Entry.WeaponClass; });
		});
	}
	TArray<int32> WeaponSlots;
	TArray<FRRUpgradeCard> WeaponCards;
	for (int32 Slot = 0; Slot < CardCount && WeaponCards.Num() < WeaponCount && !Candidates.IsEmpty(); ++Slot)
	{
		const int32 Index = RandomStream.RandRange(0, Candidates.Num() - 1);
		FRRUpgradeCard Card;
		if (MakeWeaponCard(Candidates[Index], Card, this))
		{
			WeaponSlots.Add(Slot);
			WeaponCards.Add(Card);
		}
		Candidates.RemoveAt(Index);
	}

	// Stat cards for the other slots.
	const TArray<FRRUpgradeCard> StatExcluded = Excluded.FilterByPredicate([](const FRRUpgradeCard& Card) { return !Card.IsNewWeapon(); });
	const int32 StatCount = CardCount - WeaponCards.Num();
	TArray<FChoice> Choices = GatherChoices(StatExcluded);
	if (!StatExcluded.IsEmpty() && Choices.Num() < StatCount)
	{
		// Pool too small for fresh choices: allow the same (stat, target) again; a card may then differ only by tier.
		Choices = GatherChoices({});
	}
	const TArray<FRRUpgradeCard> StatCards = DrawCards(MoveTemp(Choices), StatCount, Luck);

	TArray<FRRUpgradeCard> Cards;
	int32 NextWeapon = 0;
	int32 NextStat = 0;
	for (int32 Slot = 0; Slot < CardCount; ++Slot)
	{
		if (WeaponSlots.IsValidIndex(NextWeapon) && WeaponSlots[NextWeapon] == Slot)
		{
			Cards.Add(WeaponCards[NextWeapon++]);
		}
		else if (StatCards.IsValidIndex(NextStat))
		{
			Cards.Add(StatCards[NextStat++]);
		}
	}
	return Cards;
}

TArray<FRRUpgradeCard> URRUpgradeComponent::BuildOffer(int32 CardCount, float Luck, int32 InOfferLevel)
{
	OfferLevel = InOfferLevel;
	CurrentOffer = DrawOffer(CardCount, Luck, {});
	OfferLuck = Luck;
	bHasOffer = true;
	if (CurrentOffer.Num() < CardCount)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRUpgradeComponent: offer of %d cards asked, %d available."), CardCount, CurrentOffer.Num());
	}
	return CurrentOffer;
}

TArray<FRRUpgradeCard> URRUpgradeComponent::BuildOfferWithOwnerLuck(int32 CardCount, float BonusLuck, int32 InOfferLevel)
{
	return BuildOffer(CardCount, GetOwnerLuck() + BonusLuck, InOfferLevel);
}

bool URRUpgradeComponent::Reroll(TArray<FRRUpgradeCard>& OutCards)
{
	if (!CanReroll())
	{
		OutCards = CurrentOffer;
		return false;
	}

	CurrentOffer = DrawOffer(CurrentOffer.Num(), OfferLuck, CurrentOffer);
	OutCards = CurrentOffer;
	return true;
}

bool URRUpgradeComponent::ApplyUpgrade(const FRRUpgradeCard& Card)
{
	if (Card.IsNewWeapon())
	{
		return ApplyNewWeapon(Card);
	}

	TArray<UAbilitySystemComponent*> Targets;
	switch (Card.Target)
	{
	case ERRUpgradeTarget::Player:
		if (UAbilitySystemComponent* ASC = GetOwnerASC())
		{
			Targets.Add(ASC);
		}
		break;
	case ERRUpgradeTarget::AllWeapons:
		for (ARRWeapon* Weapon : GetTargetableWeapons())
		{
			Targets.Add(Weapon->GetAbilitySystemComponent());
		}
		break;
	case ERRUpgradeTarget::Weapon:
		if (GetTargetableWeapons().Contains(Card.TargetWeapon.Get()))
		{
			Targets.Add(Card.TargetWeapon->GetAbilitySystemComponent());
		}
		break;
	}

	const bool bTargetRequired = Card.Target != ERRUpgradeTarget::AllWeapons;
	if (bTargetRequired && Targets.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRUpgradeComponent: %s upgrade refused, its target is missing or not owned by the miner."),
			*UEnum::GetValueAsString(Card.Stat));
		return false;
	}

	for (UAbilitySystemComponent* ASC : Targets)
	{
		ApplyCardToASC(Card, *ASC);
	}

	FinishPick(Card);
	return true;
}

bool URRUpgradeComponent::ApplyNewWeapon(const FRRUpgradeCard& Card)
{
	ARRMiner* Miner = Cast<ARRMiner>(GetOwner());
	if (!Miner || !Miner->CanAcquireWeapon(Card.WeaponClass))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRUpgradeComponent: new weapon %s refused (no miner, class unset or already owned, or weapon limit reached)."),
			*GetNameSafe(Card.WeaponClass.Get()));
		return false;
	}
	// AddWeapon acquires the weapon, which replays the global upgrades on it (ReplayGlobalUpgrades).
	ARRWeapon* Weapon = Miner->AddWeapon(Card.WeaponClass);
	if (!Weapon)
	{
		return false;
	}
	FRRUpgradeCard Picked = Card;
	Picked.TargetWeapon = Weapon;
	FinishPick(Picked);
	return true;
}

void URRUpgradeComponent::FinishPick(const FRRUpgradeCard& Picked)
{
	PickedUpgrades.Add(Picked);
	CurrentOffer.Reset();
	bHasOffer = false;
	OnUpgradeApplied.Broadcast(Picked);
}

void URRUpgradeComponent::ReplayGlobalUpgrades(ARRWeapon& Weapon)
{
	UAbilitySystemComponent* ASC = Weapon.GetAbilitySystemComponent();
	if (ASC == nullptr)
	{
		return;
	}
	for (const FRRUpgradeCard& Card : PickedUpgrades)
	{
		if (!Card.IsNewWeapon() && Card.Target == ERRUpgradeTarget::AllWeapons)
		{
			ApplyCardToASC(Card, *ASC);
		}
	}
}

void URRUpgradeComponent::ApplyCardToASC(const FRRUpgradeCard& Card, UAbilitySystemComponent& ASC)
{
	const FGameplayAttribute Attribute = GetStatAttribute(Card.Stat);
	if (!ASC.HasAttributeSetForAttribute(Attribute))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRUpgradeComponent: %s has no attribute %s; upgrade skipped on it."),
			*GetNameSafe(ASC.GetOwner()), *Attribute.GetName());
		return;
	}

	if (Card.Operation == ERRUpgradeOperation::MultiplyAdditive)
	{
		ApplyModifier(ASC, Attribute, EGameplayModOp::MultiplyAdditive, false, 1.f + Card.Value);
	}
	else
	{
		ApplyModifier(ASC, Attribute, EGameplayModOp::AddBase, false, Card.Value);
	}

	// A max health upgrade also heals by the same amount (Health is not raised when MaxHealth grows).
	if (Card.Stat == ERRUpgradeStat::MaxHealth && Card.Value > 0.f && ASC.HasAttributeSetForAttribute(URRHealthAttributeSet::GetHealthAttribute()))
	{
		ApplyModifier(ASC, URRHealthAttributeSet::GetHealthAttribute(), EGameplayModOp::AddBase, true, Card.Value);
	}
}

void URRUpgradeComponent::ApplyModifier(UAbilitySystemComponent& ASC, const FGameplayAttribute& Attribute, EGameplayModOp::Type Op, bool bInstant, float Magnitude)
{
	UGameplayEffect* Effect = GetOrCreateEffect(Attribute, Op, bInstant);
	FGameplayEffectSpec Spec(Effect, ASC.MakeEffectContext(), 1.f);
	Spec.SetSetByCallerMagnitude(RRGameplayTags::Data_UpgradeValue, Magnitude);
	ASC.ApplyGameplayEffectSpecToSelf(Spec);
}

UGameplayEffect* URRUpgradeComponent::GetOrCreateEffect(const FGameplayAttribute& Attribute, EGameplayModOp::Type Op, bool bInstant)
{
	const FString OpName = Op == EGameplayModOp::MultiplyAdditive ? TEXT("MultiplyAdditive") : TEXT("AddBase");
	const FString Key = FString::Printf(TEXT("%s_%s_%s"), *Attribute.GetName(), *OpName, bInstant ? TEXT("Instant") : TEXT("Infinite"));
	if (TObjectPtr<UGameplayEffect>* Found = EffectCache.Find(Key))
	{
		return *Found;
	}

	// Runtime GE (single-player, never replicated): one modifier reading SetByCaller Data.UpgradeValue.
	const FName ObjectName = MakeUniqueObjectName(this, UGameplayEffect::StaticClass(), FName(*(TEXT("GE_Upgrade_") + Key)));
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(this, ObjectName, RF_Transient);
	Effect->DurationPolicy = bInstant ? EGameplayEffectDurationType::Instant : EGameplayEffectDurationType::Infinite;

	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = RRGameplayTags::Data_UpgradeValue;

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = Attribute;
	Modifier.ModifierOp = Op;
	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
	Effect->Modifiers.Add(Modifier);

	EffectCache.Add(Key, Effect);
	return Effect;
}

UAbilitySystemComponent* URRUpgradeComponent::GetOwnerASC() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
}

float URRUpgradeComponent::GetOwnerLuck() const
{
	const UAbilitySystemComponent* ASC = GetOwnerASC();
	if (ASC == nullptr || !ASC->HasAttributeSetForAttribute(URRPlayerAttributeSet::GetLuckAttribute()))
	{
		return 0.f;
	}
	return ASC->GetNumericAttribute(URRPlayerAttributeSet::GetLuckAttribute());
}

#undef LOCTEXT_NAMESPACE
