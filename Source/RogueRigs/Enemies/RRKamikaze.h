// Rogue Rigs - kamikaze robot: hovering charger that arms near the miner, explodes after a short fuse or when killed (C++ state machine Chase, Armed, Dead).

#pragma once

#include "CoreMinimal.h"
#include "Enemies/RREnemy.h"
#include "RRKamikaze.generated.h"

class ARRKamikaze;
class ARRMiner;
class UStaticMeshComponent;

/** Native event of ARRKamikaze: its core flare started (tests, debug listeners). */
DECLARE_MULTICAST_DELEGATE_OneParam(FRROnKamikazeFlareStart, ARRKamikaze* /*Kamikaze*/);

/** State of the kamikaze's machine. */
UENUM(BlueprintType)
enum class ERRKamikazeState : uint8
{
	/** Follows the path provider toward the miner. */
	Chase,
	/** Target came within the arm range (CSV AttackRange): stopped, the core flares (pulses faster, grows, Blueprint light) for the fuse (CSV WindUp, 0.65 s), then it explodes. */
	Armed,
	/** Exploded by fuse or killed: the base runs the death shrink. */
	Dead
};

/**
 * Second robot (enemy-kamikaze). The capsule walks the flow field on the ground like the chaser; the visuals (VisualRoot:
 * BodyMesh, CoreMesh and whatever the Blueprint attaches, e.g. the eyes) hover HoverHeight above the capsule bottom with a
 * slight bob. CoreMesh always pulses its scale at CorePulseRate. Each tick, alive: Chase (movement on) -> Armed when the
 * target (first living miner) is within the arm range (CSV AttackRange), 2D between centers: movement off, the core pulses at ArmedPulseRate and
 * grows to ArmedCoreScale over the fuse (CSV WindUp) -> after the fuse it queues one ExplosionRowName explosion (URRExplosionSubsystem,
 * instigator and source = this robot) and dies without dropping XP (State.Dead added directly, so its own explosion is
 * refused by its ASC and never counts as a kill). Killed before that (shots, another explosion): it queues the same explosion
 * at once (no flare) and drops its XP as any robot. The robot being the explosion source, the subsystem never explodes it twice.
 * The Armed phase is the flare (enemy-kamikaze, owner 2026-10-05: strong core light for 0.5 s before a proximity explosion):
 * OnFlareStart(Duration) fires on arming, GetFlareProgress runs 0 -> 1 over the fuse, OnFlareEnd fires when it leaves Armed.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRKamikaze : public ARREnemy
{
	GENERATED_BODY()

public:
	/** Sets the stats row (Kamikaze), the small capsule and the hover visual components. */
	ARRKamikaze(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Current state of the machine. */
	UFUNCTION(BlueprintPure, Category = "Kamikaze")
	ERRKamikazeState GetState() const { return State; }

	/** True once the robot exploded by its fuse (it then dies without XP). */
	UFUNCTION(BlueprintPure, Category = "Kamikaze")
	bool HasDetonated() const { return bDetonated; }

	/** World time (s) at which the robot armed; negative before. */
	double GetArmTime() const { return ArmTime; }

	/** World time (s) at which its explosion was queued (fuse or death); negative before. */
	double GetExplosionQueuedTime() const { return ExplosionQueuedTime; }

	/** Distance (uu, 2D between centers) to the miner under which the robot arms: CSV AttackRange (enemy-kamikaze: 150). */
	float GetArmRange() const { return Stats.AttackRange; }

	/** Seconds between arming and the explosion: CSV WindUp (enemy-kamikaze: 0.5). */
	float GetFuseTime() const { return Stats.WindUp; }

	/** Current core scale multiplier (pulse x arming growth), for tests and debug. */
	float GetCoreScaleFactor() const { return CoreScaleFactor; }

	/** True during the flare (Armed): the core light should be on. */
	UFUNCTION(BlueprintPure, Category = "Kamikaze|Flare")
	bool IsFlaring() const { return State == ERRKamikazeState::Armed; }

	/** Flare duration in seconds: the fuse (CSV WindUp, enemy-kamikaze 0.65 s). */
	UFUNCTION(BlueprintPure, Category = "Kamikaze|Flare")
	float GetFlareDuration() const { return GetFuseTime(); }

	/** Flare progress 0 -> 1 over the fuse while Armed (drive the core light intensity); 0 before arming, 1 once it detonated, 0 when killed before. */
	UFUNCTION(BlueprintPure, Category = "Kamikaze|Flare")
	float GetFlareProgress() const;

	/** Broadcast when the flare starts (arming), after OnFlareStart. */
	FRROnKamikazeFlareStart OnFlareStarted;

protected:
	/** Sets the movement arrival distance from the arm range and records the core base scale. */
	virtual void BeginPlay() override;

	/** Runs one step of the state machine. */
	virtual void UpdateBehavior(float DeltaSeconds) override;

	/** Enters Dead; queues the explosion unless the fuse already did; then runs the base death. */
	virtual void HandleDeath(AActor* DeathInstigator) override;

	/** Drops the XP only when the robot was killed (not after its fuse). */
	virtual void DropXP() override;

	/** Leaves the current state and enters NewState (movement on in Chase only). */
	void EnterState(ERRKamikazeState NewState);

	/** True when Target is alive and within the arm range (CSV AttackRange) (2D between centers). */
	bool IsTargetInArmRange(const ARRMiner* Target) const { return IsTargetWithin(Target, GetArmRange()); }

	/** Called when the flare starts (arming): ramp the core light to a strong orange-white over Duration seconds. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kamikaze|Flare")
	void OnFlareStart(float Duration);

	/** Called when the flare ends: bDetonated true at the fuse explosion, false when the robot was killed during the flare (light off). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Kamikaze|Flare")
	void OnFlareEnd(bool bDetonatedByFuse);

	/** Fuse end: queues the explosion, adds State.Dead and runs the death without XP. */
	void Detonate();

	/** Queues one ExplosionRowName explosion at the robot (instigator and source: this robot). Once. */
	void QueueOwnExplosion();

	/** Places VisualRoot HoverHeight above the capsule bottom plus the bob. */
	void UpdateHover();

	/** Advances the core pulse phase and applies the pulse and arming growth to CoreMesh. */
	void UpdateCore(float DeltaSeconds);

	/** Parent of the visual meshes, lifted to the hover height (the Blueprint attaches the eyes and logo plate here). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> VisualRoot;

	/** Body (dark gunmetal hull). Mesh set by the Blueprint. No collision. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	/** Pulsing orange core. Mesh and material set by the Blueprint; its relative scale at BeginPlay is the pulse base. No collision. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CoreMesh;

	/** Row of ExplosionStats.csv queued on fuse or death. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kamikaze|Fuse")
	FName ExplosionRowName = TEXT("Kamikaze");

	/** Height (uu) of VisualRoot above the capsule bottom (the floor). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kamikaze|Hover", meta = (ClampMin = 0, Units = "cm"))
	float HoverHeight = 40.f;

	/** Amplitude (uu) of the vertical hover bob. 0 = no bob. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kamikaze|Hover", meta = (ClampMin = 0, Units = "cm"))
	float HoverBobAmplitude = 4.f;

	/** Frequency of the hover bob, in full cycles per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kamikaze|Hover", meta = (ClampMin = 0, Units = "Hz"))
	float HoverBobRate = 1.5f;

	/** Core pulse frequency while chasing (base rate), in full cycles per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kamikaze|Core", meta = (ClampMin = 0, Units = "Hz"))
	float CorePulseRate = 1.5f;

	/** Core pulse frequency while armed, in full cycles per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kamikaze|Core", meta = (ClampMin = 0, Units = "Hz"))
	float ArmedPulseRate = 8.f;

	/** Relative amplitude of the core pulse (scale x (1 +/- amplitude)). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kamikaze|Core", meta = (ClampMin = 0, ClampMax = 1))
	float CorePulseAmplitude = 0.15f;

	/** Core scale multiplier reached at the end of the fuse (linear growth from 1 while armed). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Kamikaze|Core", meta = (ClampMin = 1))
	float ArmedCoreScale = 1.8f;

	/** Current state. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Kamikaze")
	ERRKamikazeState State = ERRKamikazeState::Chase;

	/** Relative scale of CoreMesh at BeginPlay (the pulse multiplies it). */
	FVector CoreBaseScale = FVector::OneVector;

	/** Relative location of VisualRoot at BeginPlay, X and Y kept (Z is driven by the hover). */
	FVector VisualRootBaseLocation = FVector::ZeroVector;

	/** Core pulse phase in cycles (accumulated so a rate change does not jump). */
	double CorePulsePhase = 0.0;

	/** Current core scale multiplier. */
	float CoreScaleFactor = 1.f;

	/** World time (s) of arming; negative before. */
	double ArmTime = -1.0;

	/** World time (s) at which the explosion was queued; negative before. */
	double ExplosionQueuedTime = -1.0;

	/** True once the explosion was requested (fuse or death); it is never requested twice. */
	bool bExplosionRequested = false;

	/** True when the fuse exploded the robot: no XP drop. */
	bool bDetonated = false;
};
