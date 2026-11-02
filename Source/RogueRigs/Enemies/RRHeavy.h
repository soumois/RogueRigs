// Rogue Rigs - heavy robot: slow, tough biped with a telegraphed ground slam (Chase, Telegraph, Slam, Recover), high knockback resistance.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/RREnemy.h"
#include "RRHeavy.generated.h"

class UCameraShakeBase;
class UGameplayEffect;
class URRGroundTelegraphComponent;

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
 * Camera shake (task-419-01): each slam frame plays SlamShakeClass once at the feet through
 * URRCameraShakeLibrary::PlayCameraShakeAtLocation, max distance Stats.AttackRange x SlamShakeMaxDistanceFactor (722 uu), hit or not.
 * Knockback resistance comes from the CSV (base ApplyKnockback). The capsule (48 x 124) fits one-cell corridors (200 uu); the
 * Blueprint mesh is twice the chaser's.
 * GroundTelegraph (sys-ground-telegraph, task-115-01): shown in Danger mode at radius GetSlamRadius on entering Telegraph,
 * progress = GetTelegraphProgress each telegraph frame, hidden on leaving Telegraph (Slam, death) and on pool reuse (progress 0).
 * Animation (task-284-05, ARREnemy clip writer): walk loop (clip 0, random phase) while the movement is on (Chase). Slam clips
 * (task-285-02, baked in task-285-01): entering Telegraph plays TelegraphClipId (1, clamped) with a timer of Stats.WindUp -
 * SlamContactLead; each Telegraph step (task-293-01, no extra tick) rewrites its start so the shown frame follows the telegraph
 * progress: frame TelegraphRaiseTopFrame (13, fists up) is reached when the timer ends instead of 0.43 s in; its end (OnAnimClipEnded, still in Telegraph) plays SlamClipId (2, looped by the
 * shader, contact pose at 0.133 s), so the fists reach the ground just before the slam frame; the slam frame (PerformSlam) plays
 * RecoverClipId (3, clamped, contact held 3 frames, back to rest by 0.467 s < RecoverDuration). Clip 3 ends at the rest pose
 * (row 61), so RestClipId stays 3. Chase again: walk loop (base SetMovementActive). Death leaves the clip.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRHeavy : public ARREnemy
{
	GENERATED_BODY()

public:
	/** Sets the stats row (Heavy), the damage effect, the arrival ratio, the corridor-fitting capsule and the ground telegraph. */
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

	/** Radius (uu) of the shockwave = Stats.AttackRange (361). */
	UFUNCTION(BlueprintPure, Category = "Heavy")
	float GetSlamRadius() const { return Stats.AttackRange; }

	/** Telegraph duration (s) = Stats.WindUp (1.0). */
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

	/** Ground signal of the slam (danger zone at the slam radius, progress circle growing to it over the telegraph). */
	UFUNCTION(BlueprintPure, Category = "Heavy")
	URRGroundTelegraphComponent* GetGroundTelegraph() const { return GroundTelegraph; }

protected:
	/** Sets the movement arrival distance from the slam radius. */
	virtual void BeginPlay() override;

	/** Runs one step of the state machine. */
	virtual void UpdateBehavior(float DeltaSeconds) override;

	/** Enters Dead (resets the telegraph visuals through OnRecover) then runs the base death. */
	virtual void HandleDeath(AActor* DeathInstigator) override;

	/** Recycling relocation: back to Chase (leaving the current state as EnterState does), OnRecover when it left Telegraph, Slam or Recover; nothing while Dead. */
	virtual void ResetStateMachine() override;

	/** Pool reuse: base reset, then slam time and counters cleared, back to Chase, ground signal hidden with progress 0. */
	virtual void ResetForNewLife() override;

	/** Leaves the current state and enters NewState (movement on in Chase only). */
	void EnterState(ERRHeavyState NewState);

	/** True when the cadence allows a new telegraph at Now (the slam it leads to lands 1 / AttackRate after the previous one). */
	bool IsSlamReady(double Now) const;

	/** Telegraph clip end (WindUp - SlamContactLead into the telegraph): plays the slam-down clip; nothing once Dead; else base. */
	virtual void OnAnimClipEnded() override;

	/**
	 * While the telegraph clip plays (its timer pending), rewrites its start (Custom Primitive Data AnimStartDataIndex) so the
	 * shader shows frame TelegraphRaiseTopFrame x GetTelegraphProgress x WindUp / (WindUp - SlamContactLead): the raise lasts until
	 * the slam-down clip. Called each Telegraph step; between two writes the shader runs on from that frame at 30 fps.
	 */
	void UpdateTelegraphClipFrame();

	/** Performs the slam: damage to the miner within the radius, counters, recover clip, slam camera shake at the feet, OnSlam. */
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

	/**
	 * Camera shake played once on each slam frame at the heavy's feet, miner hit or not, never during the telegraph (task-419,
	 * owner 2026-11-02): default URRHeavySlamCameraShake (8 uu, 0.3 s, 25 Hz). None = no slam shake.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy|Slam")
	TSubclassOf<UCameraShakeBase> SlamShakeClass;

	/**
	 * Slam shake max distance as a multiple of the slam radius (Stats.AttackRange): scale 1 at the feet, falling linearly to the
	 * edge scale (50 %) at radius x this factor, nothing beyond (2: 722 uu, provisional owner 2026-11-02, task-419 Q9). 0 = no shake.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy|Slam", meta = (ClampMin = 0))
	float SlamShakeMaxDistanceFactor = 2.f;

	/** Blink frequency of the meshes during the telegraph (base UpdateTelegraphBlink). 0 = no blink: the Blueprint glows instead. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy|Slam", meta = (ClampMin = 0, Units = "Hz"))
	float TelegraphFlashRate = 0.f;

	/** Baked clip played from the telegraph start (rows 16..30, clamped: fists raised, then held). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy|Animation")
	float TelegraphClipId = 1.f;

	/** Frame of TelegraphClipId (0-based, 30 fps bake) where the fists reach the top; shown when the slam-down clip starts. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy|Animation", meta = (ClampMin = 0, ClampMax = 14))
	float TelegraphRaiseTopFrame = 13.f;

	/** Frame rate of the baked clamped clips in MF_RobotAnimWPO (frame = clip age x 30; RunRate applies to clips 0 and 2 only). */
	static constexpr double BakedClipFrameRate = 30.0;

	/** Baked clip played SlamContactLead seconds before the slam (rows 31..46, looped by the shader: must be replaced before 0.533 s). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy|Animation")
	float SlamClipId = 2.f;

	/** Baked clip played on the slam frame (rows 47..61, clamped: contact held, back to the rest pose by 0.467 s). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy|Animation")
	float RecoverClipId = 3.f;

	/** Seconds before the slam frame at which SlamClipId starts (its contact pose comes 0.133 s in, so the fists land on the slam). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heavy|Animation", meta = (ClampMin = 0, Units = "s"))
	float SlamContactLead = 0.15f;

	/** Ground signal of the slam, centered on the heavy's feet; visible only during Telegraph. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URRGroundTelegraphComponent> GroundTelegraph;

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
