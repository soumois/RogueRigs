// Rogue Rigs - chaser robot: melee rusher driven by a C++ state machine (Chase, WindUp, Attack, Recover).

#pragma once

#include "CoreMinimal.h"
#include "Enemies/RREnemy.h"
#include "RRChaser.generated.h"

class UGameplayEffect;

/** State of the chaser's machine. */
UENUM(BlueprintType)
enum class ERRChaserState : uint8
{
	/** Follows the path provider toward the miner. */
	Chase,
	/** Target within AttackRange: stopped, flashing for Stats.WindUp seconds. */
	WindUp,
	/** One frame: the hit is applied if the target is still within AttackRange. */
	Attack,
	/** Stopped until 1 / Stats.AttackRate seconds after the attack. */
	Recover,
	/** Health reached 0: the base runs the death shrink. */
	Dead
};

/**
 * First robot (enemy-chaser). Each tick, alive: Chase (movement on) -> WindUp when the target (first living miner) is within
 * Stats.AttackRange, 2D between centers: movement off, velocity cut, meshes blink at WindUpFlashRate for Stats.WindUp
 * seconds -> Attack: if the target is still in range and alive, applies DamageEffectClass (URRGE_DamageFlat, SetByCaller
 * Data.Damage = Stats.Damage, source = this robot's ASC, so AttackPower scales it); the miner's i-frames may refuse it ->
 * Recover (movement off) until 1 / Stats.AttackRate seconds after the attack -> Chase. The movement component's arrival
 * distance is set to Stats.AttackRange * ArrivalRangeRatio so the robot stops inside its range.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRChaser : public ARREnemy
{
	GENERATED_BODY()

public:
	/** Sets the stats row (Chaser), the damage effect and the small capsule. */
	ARRChaser(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Current state of the machine. */
	UFUNCTION(BlueprintPure, Category = "Chaser")
	ERRChaserState GetState() const { return State; }

	/** World time (s) of the last attack (hit applied or not); negative before the first. */
	double GetLastAttackTime() const { return LastAttackTime; }

	/** Number of attacks performed (hit applied or not), for tests and debug. */
	int32 GetAttackCount() const { return AttackCount; }

protected:
	/** Sets the movement arrival distance from the attack range. */
	virtual void BeginPlay() override;

	/** Runs one step of the state machine. */
	virtual void UpdateBehavior(float DeltaSeconds) override;

	/** Enters Dead (stops the flash) then runs the base death. */
	virtual void HandleDeath(AActor* DeathInstigator) override;

	/** Recycling relocation: back to Chase (leaving the current state as EnterState does); nothing while Dead. */
	virtual void ResetStateMachine() override;

	/** Leaves the current state and enters NewState (movement on in Chase only; flash only in WindUp). */
	void EnterState(ERRChaserState NewState);

	/** True when Target is alive and within Stats.AttackRange (2D between centers). */
	bool IsTargetInRange(const ARRMiner* Target) const { return IsTargetWithin(Target, Stats.AttackRange); }

	/** Applies DamageEffectClass with SetByCaller Data.Damage = Stats.Damage from this ASC to the target's ASC. */
	void ApplyAttack(ARRMiner* Target);

	/** Damage effect of the attack; must read SetByCaller Data.Damage (URRGE_DamageFlat). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chaser|Attack")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	/** Blink frequency of the meshes during the wind-up, in full hide/show cycles per second. 0 = no blink. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chaser|Attack", meta = (ClampMin = 0, Units = "Hz"))
	float WindUpFlashRate = 12.f;

	/** Current state. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Chaser")
	ERRChaserState State = ERRChaserState::Chase;

	/** World time (s) of the last attack; negative before the first. */
	double LastAttackTime = -1.0;

	/** Attacks performed. */
	int32 AttackCount = 0;
};
