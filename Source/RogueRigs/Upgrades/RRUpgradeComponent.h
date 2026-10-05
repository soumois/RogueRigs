// Rogue Rigs - upgrade offer and application on the miner: CSV values, luck-weighted rarity draw, owned-weapon targets, new-weapon cards, GAS application, replay.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "Math/RandomStream.h"
#include "Upgrades/RRUpgradeCard.h"
#include "Upgrades/RRUpgradeValueRow.h"
#include "RRUpgradeComponent.generated.h"

class ARRWeapon;
class UAbilitySystemComponent;
struct FRRWeaponCatalogueEntry;
class UGameplayEffect;

/** Fired after an upgrade was applied, with the card. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnUpgradeAppliedSignature, const FRRUpgradeCard&, Card);

/**
 * Upgrade system of the miner (a default subobject of ARRMiner; sys-upgrades). Reads UpgradeValues.csv (FRRUpgradeValueRow)
 * and UpgradeRarities.csv (FRRUpgradeRarityRow) when it initializes.
 * Offer: BuildOffer draws distinct cards; each card picks a stat uniformly among the offerable stats (at least one row;
 * weapon stats only while the miner owns a weapon), then a target uniformly among that stat's targets (player stats: the
 * player; weapon stats: all weapons or one weapon of ARRMiner::GetWeapons, so a new weapon becomes a target by itself),
 * then a tier by the luck-weighted draw; when the stat has no row at the drawn tier (Pierce below blue) that choice is
 * dropped for this offer and the card is drawn again from the other choices. Two cards of one offer never share stat and target. Reroll replaces the offer
 * once with cards sharing no (stat, target) with it. ApplyUpgrade ends the offer.
 * New-weapon cards (upgrades gdd): before the stat cards, while the owner miner can acquire a catalogue weapon
 * (ARRMiner::GetAcquirableWeapons: not owned, fewer than MaxWeapons owned), each slot in order rolls NewWeaponCardChance; a
 * success makes that slot a "Nouvelle arme" card for a weapon picked uniformly among the acquirable ones, at most
 * MaxNewWeaponCardsPerOffer per offer; the other slots get stat cards. No roll is made when the chance is 0 or nothing can be
 * acquired, so the stat draws of a seed are unchanged. Picking a new-weapon card calls ARRMiner::AddWeapon (acquisition
 * replays the global upgrades), and the new weapon becomes a target of later stat cards. A reroll excludes the weapons offered.
 * Application: an infinite runtime GE per (attribute, operation) with SetByCaller Data.UpgradeValue (AddBase: Value;
 * MultiplyAdditive: 1 + Value, so percentages add before multiplying). Player stats go to the owner's ASC (MaxHealth also
 * raises Health by Value), one-weapon cards to that weapon's ASC, all-weapons cards to every owned weapon ASC; picked cards
 * are kept and all-weapons cards are replayed on weapons acquired later (ReplayGlobalUpgrades, from ARRWeapon::Acquire).
 * Upgrades are never removed. The level-up flow (pause, heal, chained screens) is not here.
 */
UCLASS(ClassGroup = (RogueRigs), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Path of the upgrade value CSV, relative to FPaths::ProjectContentDir(). */
	static const TCHAR* UpgradeValuesPath;

	/** Path of the rarity draw CSV, relative to FPaths::ProjectContentDir(). */
	static const TCHAR* UpgradeRaritiesPath;

	/** Number of rarity tiers (Grey to Yellow). */
	static constexpr int32 TierCount = 5;

	/** No tick; initializes the component so the tables load before any BeginPlay. */
	URRUpgradeComponent();

	/**
	 * Builds a new offer of up to CardCount distinct cards drawn with Luck (the owner's Luck plus any chest bonus; see
	 * BuildOfferWithOwnerLuck), stores it as the current offer and re-arms the reroll. Returns the cards (fewer when the
	 * pool is smaller, empty when the tables failed to load).
	 */
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	TArray<FRRUpgradeCard> BuildOffer(int32 CardCount, float Luck);

	/** BuildOffer with the owner's current Luck attribute + BonusLuck (a bonus chest's temporary luck). */
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	TArray<FRRUpgradeCard> BuildOfferWithOwnerLuck(int32 CardCount = 3, float BonusLuck = 0.f);

	/**
	 * Replaces the current offer, once per offer, with the same number of cards drawn with the offer's luck, none sharing
	 * stat and target with the replaced cards (if the pool is too small, only identical cards are excluded). Returns false
	 * (offer unchanged) when there is no current offer or its reroll was used.
	 */
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	bool Reroll(TArray<FRRUpgradeCard>& OutCards);

	/** True while a current offer exists and its reroll is unused. */
	UFUNCTION(BlueprintPure, Category = "Upgrades")
	bool CanReroll() const { return bHasOffer && !bRerollUsed; }

	/** True between BuildOffer and ApplyUpgrade. */
	UFUNCTION(BlueprintPure, Category = "Upgrades")
	bool HasOffer() const { return bHasOffer; }

	/** Cards of the current offer (empty when there is none). */
	UFUNCTION(BlueprintPure, Category = "Upgrades")
	const TArray<FRRUpgradeCard>& GetCurrentOffer() const { return CurrentOffer; }

	/**
	 * Applies Card (see the class comment), adds it to the picked list, ends the current offer and broadcasts OnUpgradeApplied.
	 * Returns false (logged, nothing applied) if the target is missing: no player ASC, or a weapon the miner does not own.
	 */
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	bool ApplyUpgrade(const FRRUpgradeCard& Card);

	/** Upgrades applied this run, in order (replay, stat sheet, run log). */
	UFUNCTION(BlueprintPure, Category = "Upgrades")
	const TArray<FRRUpgradeCard>& GetPickedUpgrades() const { return PickedUpgrades; }

	/** Applies every picked all-weapons card to Weapon's ASC. Called by ARRWeapon::Acquire after the base values. */
	void ReplayGlobalUpgrades(ARRWeapon& Weapon);

	/**
	 * Value of Stat at Tier from the table: GlobalValue, or GlobalValue x TargetedMultiplier when bTargeted. Returns false when
	 * the table has no such row.
	 */
	UFUNCTION(BlueprintPure, Category = "Upgrades")
	bool GetUpgradeValue(ERRUpgradeStat Stat, ERRUpgradeRarity Tier, bool bTargeted, float& OutValue) const;

	/** Normalized draw weight of each tier at Luck, index = rank (all 0 when the rarity table failed to load). */
	UFUNCTION(BlueprintPure, Category = "Upgrades")
	TArray<float> GetTierWeights(float Luck) const;

	/** Draws one tier with the luck-weighted odds, from the component's random stream. */
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	ERRUpgradeRarity DrawTier(float Luck);

	/**
	 * Builds a card with its value and UI texts. TargetWeapon is used only for a Weapon target; the target must match the stat
	 * (player stat: Player; weapon stat: AllWeapons or Weapon). Returns false when the table has no row or the target is invalid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	bool MakeCard(ERRUpgradeStat Stat, ERRUpgradeRarity Tier, ERRUpgradeTarget Target, ARRWeapon* TargetWeapon, FRRUpgradeCard& OutCard) const;

	/**
	 * Builds the "Nouvelle arme" card of Entry: CardType NewWeapon, WeaponClass, Target Weapon (no TargetWeapon yet), TargetText
	 * = the weapon's CSV name (ARRWeapon::GetClassDisplayName), DescriptionText = Entry.Description. False when the class is unset.
	 */
	static bool MakeWeaponCard(const FRRWeaponCatalogueEntry& Entry, FRRUpgradeCard& OutCard);

	/** Chance (0 to 1) that one offer slot becomes a new-weapon card while a weapon can be acquired (NewWeaponCardChance). */
	UFUNCTION(BlueprintPure, Category = "Upgrades")
	float GetNewWeaponCardChance() const { return NewWeaponCardChance; }

	/** Most new-weapon cards in one offer (MaxNewWeaponCardsPerOffer). */
	UFUNCTION(BlueprintPure, Category = "Upgrades")
	int32 GetMaxNewWeaponCardsPerOffer() const { return MaxNewWeaponCardsPerOffer; }

	/** Weapons that can be targeted: the owner miner's valid weapons with an ASC (empty when the owner is not a miner). */
	TArray<ARRWeapon*> GetTargetableWeapons() const;

	/** Reseeds the random stream (deterministic offers in tests). */
	void SetRandomSeed(int32 Seed) { RandomStream.Initialize(Seed); }

	/** True for the stats held by the weapons (Damage to MagazineSize, Pierce), false for player stats. */
	static bool IsWeaponStat(ERRUpgradeStat Stat);

	/** Attribute raised by Stat. */
	static FGameplayAttribute GetStatAttribute(ERRUpgradeStat Stat);

	/** French UI name of Stat. */
	static FText GetStatDisplayName(ERRUpgradeStat Stat);

	/** UI text of a value of Stat: percentages and crit stats as "+12 %", flat stats as "+20"; French decimal comma. */
	static FText FormatValueText(ERRUpgradeStat Stat, float Value);

	/** Broadcast after each ApplyUpgrade that succeeded. */
	UPROPERTY(BlueprintAssignable, Category = "Upgrades")
	FRROnUpgradeAppliedSignature OnUpgradeApplied;

protected:
	/** One possible (stat, target) choice of a draw, with its pick weight. */
	struct FChoice
	{
		/** Stat of the choice. */
		ERRUpgradeStat Stat;
		/** Target kind of the choice. */
		ERRUpgradeTarget Target;
		/** Weapon of a Weapon target. */
		ARRWeapon* Weapon;
		/** Pick weight: 1 / number of targets of the stat, so each stat is equally likely. */
		float Weight;
	};

	/** Loads the tables (LoadTables) and seeds the random stream. */
	virtual void InitializeComponent() override;

	/** Reads both CSVs into ValueRows and RarityRows. Logs problems; a stat is offered only at the tiers it has rows for. */
	void LoadTables();

	/** Every (stat, target) choice available now, minus those matching a card of Excluded. */
	TArray<FChoice> GatherChoices(const TArray<FRRUpgradeCard>& Excluded) const;

	/**
	 * Draws a full offer of up to CardCount cards (see the class comment): new-weapon slots first, then stat cards from the
	 * choices not matching Excluded (all choices when too few remain). Excluded weapon classes are not offered again.
	 */
	TArray<FRRUpgradeCard> DrawOffer(int32 CardCount, float Luck, const TArray<FRRUpgradeCard>& Excluded);

	/** Applies a new-weapon card: ARRMiner::AddWeapon if the miner can acquire the class. False (logged) otherwise. */
	bool ApplyNewWeapon(const FRRUpgradeCard& Card);

	/** Records Picked, ends the current offer and broadcasts OnUpgradeApplied. */
	void FinishPick(const FRRUpgradeCard& Picked);

	/** Draws up to CardCount cards from Choices without replacement (each with its own tier drawn at Luck). */
	TArray<FRRUpgradeCard> DrawCards(TArray<FChoice> Choices, int32 CardCount, float Luck);

	/** Applies one card's GE (and the Health raise for MaxHealth) on ASC. */
	void ApplyCardToASC(const FRRUpgradeCard& Card, UAbilitySystemComponent& ASC);

	/** Applies a runtime GE of Attribute/Op/DurationPolicy with SetByCaller Data.UpgradeValue = Magnitude on ASC. */
	void ApplyModifier(UAbilitySystemComponent& ASC, const FGameplayAttribute& Attribute, EGameplayModOp::Type Op, bool bInstant, float Magnitude);

	/** Returns the cached runtime GE for Attribute/Op/DurationPolicy, creating it on first use. */
	UGameplayEffect* GetOrCreateEffect(const FGameplayAttribute& Attribute, EGameplayModOp::Type Op, bool bInstant);

	/** The owner's ASC (the miner's), or null. */
	UAbilitySystemComponent* GetOwnerASC() const;

	/** The owner's current Luck attribute; 0 when it has no player attribute set. */
	float GetOwnerLuck() const;

	/** Table rows by stat and tier rank (TierCount entries, unset when the tier has no row); a stat is offerable when one is set. */
	TMap<ERRUpgradeStat, TArray<TOptional<FRRUpgradeValueRow>>> ValueRows;

	/** Base weight and luck factor per tier rank (0 when the tier is missing). */
	float BaseWeights[TierCount] = {};

	/** Luck factor k per tier rank. */
	float LuckFactors[TierCount] = {};

	/** Runtime upgrade GEs by "<Attribute>_<Op>_<Instant|Infinite>"; outer this component. */
	UPROPERTY(Transient)
	TMap<FString, TObjectPtr<UGameplayEffect>> EffectCache;

	/** Chance (0 to 1) that each offer slot becomes a new-weapon card while a weapon can be acquired (upgrades gdd: 30 %, provisional). */
	UPROPERTY(EditDefaultsOnly, Category = "Upgrades|New weapons", meta = (ClampMin = 0, ClampMax = 1))
	float NewWeaponCardChance = 0.3f;

	/** Most new-weapon cards in one offer (upgrades gdd: 1, provisional). */
	UPROPERTY(EditDefaultsOnly, Category = "Upgrades|New weapons", meta = (ClampMin = 0))
	int32 MaxNewWeaponCardsPerOffer = 1;

	/** Cards of the current offer. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Upgrades")
	TArray<FRRUpgradeCard> CurrentOffer;

	/** Upgrades applied this run, in order. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Upgrades")
	TArray<FRRUpgradeCard> PickedUpgrades;

	/** Luck the current offer was drawn with (reused by Reroll). */
	float OfferLuck = 0.f;

	/** True between BuildOffer and ApplyUpgrade. */
	bool bHasOffer = false;

	/** True once the current offer was rerolled. */
	bool bRerollUsed = false;

	/** Random stream of the draws; seeded at initialization, reseeded by SetRandomSeed. */
	FRandomStream RandomStream;
};
