// Rogue Rigs - shield chaser: a chaser whose CSV Shield (30) absorbs damage before health; Blueprint events and getters for its shell.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/RRChaser.h"
#include "RRShieldChaser.generated.h"

/**
 * Chaser variant (enemy-chaser, shield variant): same melee state machine, speed and damage as ARRChaser, stats row ShieldChaser
 * (XP 2, Shield 30). The shield is the Shield attribute of the health set: each hit is taken from it first, the rest of the same
 * hit from Health (URRHealthAttributeSet). No tick cost: the robot only listens to the set's OnShieldBroken and forwards it to
 * the Blueprint (OnShieldBroken: hide the shell, play the burst), keeping the breaking hit direction for the shield debris
 * (GetShieldDebrisDirection). Its own native class so the spawner resolves it to its own
 * Blueprint (WeightedEnemyClasses entry with the same nearest native class); a Blueprint of it must derive from this class.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRShieldChaser : public ARRChaser
{
	GENERATED_BODY()

public:
	/** Sets the stats row ShieldChaser; the rest is the chaser's. */
	ARRShieldChaser(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Shield points left (Shield attribute, 0 once broken). */
	UFUNCTION(BlueprintPure, Category = "Shield Chaser")
	float GetShield() const;

	/** Shield points at spawn (CSV column Shield of the row). */
	UFUNCTION(BlueprintPure, Category = "Shield Chaser")
	float GetMaxShield() const { return Stats.Shield; }

	/** Shield left over shield at spawn, in [0, 1]; 0 when the row has no shield. Drive the shell opacity or size with it. */
	UFUNCTION(BlueprintPure, Category = "Shield Chaser")
	float GetShieldFraction() const;

	/** True while some shield is left. */
	UFUNCTION(BlueprintPure, Category = "Shield Chaser")
	bool HasShield() const { return GetShield() > 0.f; }

	/** Times the shield broke (0 or 1), for tests and debug. */
	int32 GetShieldBreakCount() const { return ShieldBreakCount; }

	/**
	 * Horizontal unit direction to throw the shield fragments toward (NS_ShieldDebris User.HitDirection, task-224-01). Base =
	 * the travel direction of the hit that broke the shield (ShieldBreakDirection), else the robot backward. A hit from behind
	 * (base along the robot forward, dot > 0) returns the base as is; any other base is turned ShieldDebrisSideAngleDeg about
	 * Z toward the shield side (ShieldSide). Valid any time; meant to be read in OnShieldBroken.
	 */
	UFUNCTION(BlueprintPure, Category = "Shield Chaser")
	FVector GetShieldDebrisDirection() const;

	/** Horizontal unit travel direction of the hit that broke the shield this life; zero when none was recorded. */
	const FVector& GetShieldBreakDirection() const { return ShieldBreakDirection; }

protected:
	/** Binds the health set's OnShieldBroken (after the base applied the CSV shield). */
	virtual void BeginPlay() override;

	/** Removes the OnShieldBroken binding. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Pool reuse: chaser reset (shield back through the CSV stats, shell through the restored visual parts), break count cleared. */
	virtual void ResetForNewLife() override;

	/** Health set callback: counts the break and calls OnShieldBroken. Runs inside the GE execution (sys-damage safety rules). */
	void HandleShieldBroken();

	/** Blueprint: the hit took the shield to 0 (fired once, before the death event when the same hit kills). Hide the shell, play the burst. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shield Chaser")
	void OnShieldBroken();

	/** Handle of the binding on HealthSet->OnShieldBroken. */
	FDelegateHandle ShieldBrokenHandle;

	/** Times the shield broke. */
	int32 ShieldBreakCount = 0;

	/** Degrees the shield debris direction is turned toward the shield side, unless the breaking hit came from behind. */
	UPROPERTY(EditDefaultsOnly, Category = "Shield Chaser|Debris", meta = (ClampMin = "0", ClampMax = "90", Units = "Degrees"))
	float ShieldDebrisSideAngleDeg = 20.f;

	/**
	 * Side the shield is held on, robot space: -1 = robot left, 1 = robot right, 0 = no turn. Default -1: the shield fragment
	 * offsets of shield_debris_pieces.json have a negative Unreal Y (left of the +X front).
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Shield Chaser|Debris", meta = (ClampMin = "-1", ClampMax = "1"))
	int32 ShieldSide = -1;

	/** Last recorded hit direction (ARREnemy::GetLastHitDirection) copied when the shield broke; zero = none. Cleared each life. */
	FVector ShieldBreakDirection = FVector::ZeroVector;
};
