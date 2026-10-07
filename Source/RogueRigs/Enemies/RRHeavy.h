// Rogue Rigs - heavy robot: slow, tough biped with a telegraphed ground slam (Chase, Telegraph, Slam, Recover), high knockback resistance.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/RREnemy.h"
#include "RRHeavy.generated.h"

class UGameplayEffect;

/** State of the heavy's machine. */
UENUM(BlueprintType)
enum class ERRHeavyState : uint8
{
	/** Follows the path provider toward the miner. */
	Chase,
	/** Miner within the trigger range and the slam ready: stopped, arms raised, glowing for Stats.WindUp seconds. */
	Telegraph,
	/** One frame: the shockwave hits the miner if within Stats.AttackRange of the heavy. */
	Slam,
	/** Stopped for RecoverDuration seconds after the slam. */
	Recover,
	/** Health reached 0: the base runs the death shrink. */
	Dead
};

/**
 * Heavy robot (enemy-heavy). Each tick, alive: Chase (movement on) -> Telegraph when the target (first living miner) is within
 * Stats.AttackRange * SlamTriggerRatio (2D between centers) and the cadence allows it (LastSlamTime + 1 / Stats.AttackRate -
 * Stats.WindUp): movement off, OnTelegraphStart, for Stats.WindUp seconds (committed: the miner may walk out) -> Slam (one frame):
 * the shockwave around the heavy, radius Stats.AttackRange, applies DamageEffectClass (URRGE_DamageFlat, SetByCaller Data.Damage =
 * Stats.Damage, scaled by AttackPower) to the miner if within the radius; it never touches terrain or other robots; OnSlam ->
 * Recover (movement off) for RecoverDuration, then OnRecover -> Chase. In contact, slams are 1 / Stats.AttackRate seconds apart.
 * Knockback resistance comes from the CSV (base ApplyKnockback). The capsule (48 x 124) fits one-cell corridors (200 uu); the
 * Blueprint mesh is twice the chaser's.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRHeavy : public ARREnemy
{
	GENERATED_BODY()

public:
	/** Sets the stats row (Heavy), the damage effect, the arrival ratio and the corridor-fitting capsule. */
	ARRHeavy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Current state of the machine. */
	UFUNCTION(BlueprintPure, Category = "Heavy")
	ERRHeavyState GetState() const { return State; }

	/** True during the telegraph (arms raised, glow on). */
	UFUNCTION(BlueprintPure, Category = "Heavy")
	bool IsTelegraphing() const { return State == ERRHeavyState::Telegraph; }

	/** Telegraph progress in [0, 1] (0 at its start, 1 at the slam); 0 outside the telegraph. Drives the body tilt and the glow. */
	UFUNCTION(BlueprintPure, Category = "Heavy")
	float GetTelegraphProgress() const;

	/** Radius (uu) of the shockwave = Stats.AttackRange (340). */
	UFUNCTION(BlueprintPure, Category = "Heavy")
	float GetSlamRadius() const { return Stats.AttackRange; }

	/** Telegraph duration (s) = Stats.WindUp (0.6). */
	UFUNCTION(BlueprintPure, Category = "Heavy")
	float GetTelegraphDuration() const { return Stats.WindUp; }

	/** World time (s) of the last slam; negative before the first. */
	UFUNCTION(BlueprintPure, Category = "Heavy")
	double GetLastSlamTime() const { return LastSlamTime; }

	/** Number of slams performed (miner hit or not). */
	UFUNCTION(BlueprintPure, Category = "Heavy")
	int32 GetSlamCount() const { return SlamCount; }

	/** Number of slams that applied their damage to the miner (for tests and debug). */
	int32 GetSlamHitCount() const { return SlamHitCount; }

protected:
	/** Sets the movement arrival distance from the slam radius. */
	virtual void BeginPlay() override;

	/** Runs one step of the state machine. */
	virtual void UpdateBehavior(float DeltaSeconds) override;

	/** Enters Dead (resets the telegraph visuals through OnRecover) then runs the base death. */
	virtual void HandleDeath(AActor* DeathInstigator) override;

	/** Recycling relocation: back to Chase (leaving the current state as EnterState does); nothing while Dead. */
	virtual void ResetStateMachine() override;

	/** Leaves the current state and enters NewState (movement on in Chase only). */
	void EnterState(ERRHeavyState NewState);

	/** True when the cadence allows a new telegraph at Now (the slam it leads to lands 1 / AttackRate after the previous one). */
	bool IsSlamReady(double Now) const;

	/** Performs the slam: damage to the miner within the radius, counters, OnSlam. */
	void PerformSlam(ARRMiner* Target, double Now);

	/** Called when the telegraph starts (raise the arms, light the eyes and stripes). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Heavy")
	void OnTelegraphStart();

	/** Called on the slam frame with the shockwave center (heavy feet) and radius (spawn the shockwave visual, arms down, glow off). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Heavy")
	void OnSlam(FVector Location, float Radius);

	/** Called when the recovery ends and the heavy walks again; also when it dies during a telegraph or recovery (reset the pose). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Heavy")
	void OnRecover();

	/** Damage effect of the slam; must read SetByCaller Data.Damage (URRGE_DamageFlat). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy|Slam")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	/** The telegraph starts when the miner is within Stats.AttackRange * this ratio, so a miner who stays put is inside the slam radius. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy|Slam", meta = (ClampMin = 0, ClampMax = 1))
	float SlamTriggerRatio = 0.7f;

	/** Seconds the heavy stays still after a slam before walking again (its punish window). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy|Slam", meta = (ClampMin = 0, Units = "s"))
	float RecoverDuration = 0.5f;

	/** Blink frequency of the meshes during the telegraph (base UpdateTelegraphBlink). 0 = no blink: the Blueprint glows instead. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy|Slam", meta = (ClampMin = 0, Units = "Hz"))
	float TelegraphFlashRate = 0.f;

	/** Current state. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Heavy")
	ERRHeavyState State = ERRHeavyState::Chase;

	/** World time (s) of the last slam; negative before the first. */
	double LastSlamTime = -1.0;

	/** Slams performed. */
	int32 SlamCount = 0;

	/** Slams that applied their damage. */
	int32 SlamHitCount = 0;
};
