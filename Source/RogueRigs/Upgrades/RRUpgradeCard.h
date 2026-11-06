// Rogue Rigs - upgrade card: the enums of the upgrade system (card type, stat, rarity tier, target, operation) and the card shown to the player.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "RRUpgradeCard.generated.h"

class ARRWeapon;

/** Kind of card of an offer: a stat upgrade, or a new weapon to acquire (sys-upgrades). */
UENUM(BlueprintType)
enum class ERRUpgradeCardType : uint8
{
	/** Raises Stat on Target by Value, with a rarity Tier. */
	StatUpgrade,
	/** "New weapon": picking it gives the miner WeaponClass (ARRMiner::AddWeapon). No tier: the UI shows it with its own style. */
	NewWeapon
};

/** Stat an upgrade raises. Player stats live on the miner's ASC, weapon stats on each weapon's ASC (sys-upgrades). */
UENUM(BlueprintType)
enum class ERRUpgradeStat : uint8
{
	/** Weapon: Damage (%). */
	Damage,
	/** Weapon: FireRate (%). */
	FireRate,
	/** Weapon: ReloadSpeed (%). */
	ReloadSpeed,
	/** Weapon: CritChance (points, 0.01 = 1 %). */
	CritChance,
	/** Weapon: CritDamage multiplier (points, 0.1 = +10 %). */
	CritDamage,
	/** Weapon: MagazineSize (points). Not offered in M1: UpgradeValues.csv has no row for it. */
	MagazineSize,
	/** Player: MaxHealth (flat points); current Health is raised by the same amount. */
	MaxHealth,
	/** Player: MoveSpeed (%). */
	MoveSpeed,
	/** Player: PickupRadius (%). */
	PickupRadius,
	/** Player: XPGain (%). */
	XPGain,
	/** Player: MiningSpeed (%). */
	MiningSpeed,
	/** Player: Luck (points). */
	Luck,
	/** Weapon: Pierce (points: +1 = one more robot damaged per projectile). Rows only for blue, purple and yellow. */
	Pierce
};

/** Rarity tier of an upgrade card, low to high; the enum value is the rank used by the luck-weighted draw (rarities). */
UENUM(BlueprintType)
enum class ERRUpgradeRarity : uint8
{
	/** Rank 0. */
	Grey,
	/** Rank 1. */
	Green,
	/** Rank 2. */
	Blue,
	/** Rank 3. */
	Purple,
	/** Rank 4. */
	Yellow
};

/** Who receives an upgrade. */
UENUM(BlueprintType)
enum class ERRUpgradeTarget : uint8
{
	/** The miner's ASC (player stats). */
	Player,
	/** Every weapon the miner owns, and every weapon acquired later (global weapon upgrade, replayed on acquisition). */
	AllWeapons,
	/** One owned weapon (targeted weapon upgrade, value x TargetedMultiplier). */
	Weapon
};

/** GAS operation of an upgrade row (UpgradeValues.csv column Operation). */
UENUM(BlueprintType)
enum class ERRUpgradeOperation : uint8
{
	/** Points added to the attribute (EGameplayModOp::AddBase); bonuses add. */
	AddBase,
	/** Percentage bonus (EGameplayModOp::MultiplyAdditive, magnitude 1 + value); bonuses add before multiplying. */
	MultiplyAdditive
};

/**
 * One upgrade card of an offer, built by URRUpgradeComponent::BuildOffer and applied by ApplyUpgrade. Value is in attribute
 * units (0.12 = +12 % for a percentage stat, 0.05 = +5 points of CritChance, 20 = +20 MaxHealth), already scaled by the
 * targeted multiplier for a one-weapon card. The texts are ready for the UI (French).
 * A NewWeapon card uses only CardType, WeaponClass, Target (Weapon), TargetWeapon (null in the offer, the acquired weapon
 * once picked), DisplayName ("New weapon"), TargetText (weapon name), DescriptionText and LoreText; Stat, Tier, Value, Operation and
 * ValueText keep their defaults.
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRUpgradeCard
{
	GENERATED_BODY()

	/** Stat upgrade or new weapon; the UI picks the card style from it (a new weapon has no rarity color). */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade")
	ERRUpgradeCardType CardType = ERRUpgradeCardType::StatUpgrade;

	/** Weapon class given by a NewWeapon card; null for a stat upgrade. */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade")
	TSubclassOf<ARRWeapon> WeaponClass;

	/** Stat raised by the card. */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade")
	ERRUpgradeStat Stat = ERRUpgradeStat::Damage;

	/** Rarity tier of the card (UI color). */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade")
	ERRUpgradeRarity Tier = ERRUpgradeRarity::Grey;

	/** Who receives the card. */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade")
	ERRUpgradeTarget Target = ERRUpgradeTarget::Player;

	/** The weapon receiving the card when Target is Weapon; null otherwise. */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade")
	TObjectPtr<ARRWeapon> TargetWeapon;

	/** Bonus in attribute units, targeted multiplier included. */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade")
	float Value = 0.f;

	/** GAS operation used to apply Value. */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade")
	ERRUpgradeOperation Operation = ERRUpgradeOperation::AddBase;

	/** Stat name for the UI (e.g. "Damage"); "New weapon" on a NewWeapon card. */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade|UI")
	FText DisplayName;

	/** Target for the UI: empty for a player stat, "All weapons" or the weapon's display name (also on a NewWeapon card). */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade|UI")
	FText TargetText;

	/** Bonus for the UI (e.g. "+12 %", "+20"); empty on a NewWeapon card. */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade|UI")
	FText ValueText;

	/**
	 * NewWeapon card: short weapon description (ARRMiner weapon catalogue). Stat card: the stat name, then its Rook flavor line on a
	 * second line (upgrade_texts.json, URRTextContentSubsystem); empty when the file or the stat line is missing. Never holds the value.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade|UI")
	FText DescriptionText;

	/**
	 * Lore line shown apart from the stat name. Stat card: the Rook flavor line of upgrade_texts.json alone (the second line of
	 * DescriptionText); empty when the file or the stat line is missing. NewWeapon card: the weapon lore
	 * line of weapon_texts.json (looked up by TargetText); empty when the file or the weapon line is missing.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade|UI")
	FText LoreText;

	/** True for a "New weapon" card. */
	bool IsNewWeapon() const { return CardType == ERRUpgradeCardType::NewWeapon; }

	/** True when both cards raise the same stat on the same target (tier ignored), or offer the same weapon: such cards never share an offer. */
	bool IsSameChoice(const FRRUpgradeCard& Other) const;
};
