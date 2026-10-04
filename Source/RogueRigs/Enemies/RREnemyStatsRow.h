// Rogue Rigs - DataTable row holding one robot's stats (EnemyStats.csv).

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RREnemyStatsRow.generated.h"

/**
 * One row of Content/RogueRigs/Data/Tables/EnemyStats.csv (row name = robot, e.g. Chaser). Column names match the
 * property names. Read by ARREnemy at BeginPlay: MaxHealth and MoveSpeed become attribute base values (Health starts
 * at MaxHealth); the others are CSV constants kept on the robot (no upgrade or status changes them during a run).
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRREnemyStatsRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Base MaxHealth (URRHealthAttributeSet), in health points. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float MaxHealth = 0.f;

	/** Base MoveSpeed (URRMovementAttributeSet), in uu/s. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float MoveSpeed = 0.f;

	/** Flat damage of one attack (SetByCaller Data.Damage of URRGE_DamageFlat, scaled by AttackPower). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float Damage = 0.f;

	/** Attacks per second at most (an attack is followed by a recovery of 1 / AttackRate seconds). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float AttackRate = 0.f;

	/** 2D distance between the robot and its target centers within which it attacks, in uu (kamikaze: arm range; shooter: fire range). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float AttackRange = 0.f;

	/** Telegraph before each attack, in seconds (chaser flash, kamikaze fuse, shooter targeting laser). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float WindUp = 0.f;

	/** Number of XP pickups dropped on death. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	int32 XPValue = 0;

	/** Knockback resistance in [0, 1] (0 = full knockback). Read by ARREnemy::ApplyKnockback (shotgun): launch speed x (1 - resistance). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float KnockbackResistance = 0.f;

	/** Ranged robots: distance (uu, 2D) to the miner the robot tries to keep (shooter 800). 0 for melee robots. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float PreferredDistance = 0.f;

	/** Ranged robots: half width (uu) of the preferred ring, PreferredDistance +/- this (shooter 200). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float PreferredDistanceTolerance = 0.f;

	/** Ranged robots: speed of the robot's shots, in uu/s (shooter 1200). Lifetime = AttackRange / ProjectileSpeed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float ProjectileSpeed = 0.f;
};
