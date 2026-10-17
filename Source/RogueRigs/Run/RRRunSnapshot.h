// Rogue Rigs - miner snapshot taken at the end of a level of a run and re-applied to the miner of the next level.

#pragma once

#include "CoreMinimal.h"
// RRUpgradeCard.h converts TSubclassOf<ARRWeapon> inline (IsSameChoice): the complete type is needed by every includer.
#include "Upgrades/RRUpgradeCard.h"
#include "Weapons/RRWeapon.h"
#include "RRRunSnapshot.generated.h"

/** One upgrade card picked during the run, stored without actor pointers so it survives a map travel. */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRRunPickedCard
{
	GENERATED_BODY()

	/** The picked card as URRUpgradeComponent recorded it, TargetWeapon cleared (the actor dies with the map). */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	FRRUpgradeCard Card;

	/** Exact class of the weapon the card targeted (Target Weapon, or the weapon a NewWeapon card gave); null otherwise. */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	TSubclassOf<ARRWeapon> TargetWeaponClass;
};

/**
 * State of the miner carried into the next level of a run (run-structure): upgrade cards, weapons, level, XP, health, plus the
 * ore figures of the level for the intermission recap. Ore itself is carried as secured ore in URROreBankSubsystem (the game
 * instance keeps it across OpenLevel); it is not re-applied to the miner.
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRRunMinerSnapshot
{
	GENERATED_BODY()

	/** True once a snapshot was captured (CaptureLevelEndSnapshot succeeded). */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	bool bValid = false;

	/** Run level (1-based) the snapshot was taken at the end of. */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	int32 LevelIndex = 0;

	/** Upgrade cards picked since the run started, in pick order (URRUpgradeComponent::GetPickedUpgrades). */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	TArray<FRRRunPickedCard> PickedCards;

	/** Exact classes of the owned weapons, in acquisition order (ARRMiner::GetWeapons). */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	TArray<TSubclassOf<ARRWeapon>> WeaponClasses;

	/** Miner level (URRProgressionComponent::GetLevel). */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	int32 MinerLevel = 1;

	/** XP inside the miner level (URRProgressionComponent::GetXP). */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	float MinerXP = 0.f;

	/** Current Health at capture (re-applied, at most the new miner's MaxHealth). */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	float Health = 0.f;

	/** MaxHealth at capture (information only: the new miner's MaxHealth comes from its base stats and the replayed cards). */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	float MaxHealth = 0.f;

	/** Ore secured during this level (recap): bank total at capture minus the total at the level start. */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	int32 OreSecuredThisLevel = 0;

	/** Ore secured since the run started (recap run total). */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	int32 RunSecuredOre = 0;

	/** Carried ore still held at capture (0 after a successful extraction, which secures it); never re-applied. */
	UPROPERTY(BlueprintReadOnly, Category = "Run")
	int32 CarriedOreAtCapture = 0;
};
