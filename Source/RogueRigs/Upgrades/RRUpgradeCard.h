// Rogue Rigs - upgrade card: the enums of the upgrade system (stat, rarity tier, target, operation) and the card shown to the player.

#pragma once

#include "CoreMinimal.h"
#include "RRUpgradeCard.generated.h"

class ARRWeapon;

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
	Luck
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
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRUpgradeCard
{
	GENERATED_BODY()

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

	/** Stat name for the UI (e.g. "Dégâts"). */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade|UI")
	FText DisplayName;

	/** Target for the UI: empty for a player stat, "Toutes les armes" or the weapon's display name. */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade|UI")
	FText TargetText;

	/** Bonus for the UI (e.g. "+12 %", "+20"). */
	UPROPERTY(BlueprintReadOnly, Category = "Upgrade|UI")
	FText ValueText;

	/** True when both cards raise the same stat on the same target (tier ignored): such cards never share an offer. */
	bool IsSameChoice(const FRRUpgradeCard& Other) const
	{
		return Stat == Other.Stat && Target == Other.Target && TargetWeapon == Other.TargetWeapon;
	}
};
