// Rogue Rigs - shooter robot: closes in to its hold distance and never backs away from the miner, fires dodgeable shots with grid line of sight while on screen, telegraphs each shot with a targeting laser (C++ state machine Approach, Hold, Reposition, Aim, Dead).

#pragma once

#include "CoreMinimal.h"
#include "Enemies/RREnemy.h"
#include "RRShooter.generated.h"

class ARRArenaGrid;
class ARRMiner;
class ARRProjectile;
class UGameplayEffect;
class UStaticMeshComponent;

/** State of the shooter's machine. */
UENUM(BlueprintType)
enum class ERRShooterState : uint8
{
	/** Follows the flow field toward the miner: to the preferred distance with line of sight, closer until it has sight. */
	Approach,
	/** Stands still within the hold distance with line of sight, however close the miner comes. */
	Hold,
	/** Within the hold distance without line of sight: follows the flow field like a chaser (never stepping away from the miner) until it has sight. */
	Reposition,
	/** Stopped, facing the miner, targeting laser on for Stats.WindUp seconds, then fires. */
	Aim,
	/** Health reached 0: the base runs the death shrink. */
	Dead
};

/**
 * Third robot (enemy-shooter). Each tick, alive, it measures the 2D distance to the target (first living miner), grid line of
 * sight (ARRArenaGrid::HasLineOfSight, computed only within Stats.AttackRange) and whether it stands inside the visible floor
 * rectangle (the spawner's camera projection, ARREnemySpawner::ComputeCameraViewRectangle, or SetViewRectangleOverride).
 * Fire: when it can fire (sight, within Stats.AttackRange, on screen) and the cadence allows it (an aim may start
 * 1 / Stats.AttackRate - Stats.WindUp s after the previous shot, so continuous shots are 1 / AttackRate apart), it enters Aim:
 * movement off, faces the target, laser on (OnAimStart); losing the fire condition cancels the aim (OnAimEnd, no shot);
 * after Stats.WindUp it fires one ProjectileClass shot (ARREnemyProjectile: hurts the miner only, stops on rock without
 * damaging it) from the muzzle at the target's current position, speed Stats.ProjectileSpeed, lifetime AttackRange / speed,
 * carrying a DamageEffectClass spec with Data.Damage = Stats.Damage (OnAimEnd, then OnFire).
 * Movement (task-031-02: it never backs away from the miner): hold distance = Stats.PreferredDistance + PreferredDistanceTolerance,
 * clamped to AttackRange. Approach (flow field, arrival at the preferred distance with sight, 0 without) -> Hold once within the
 * preferred distance with sight. Hold -> Approach beyond the hold distance; with sight it stays put however close the miner comes.
 * Without sight within the hold distance (task-038-01): Reposition, the flow field again (no arrival stop, the same detour
 * around rock as the chasers) until sight returns, then Hold and fire; beyond the hold distance, Approach. Within the hold
 * distance every flow-field step goes through MakeNonRetreatingDirection (URREnemyMovementComponent direction filter), so the
 * robot never moves away from the target. The C++ drives a
 * LaserBeam mesh between the muzzle and the line-of-sight-clipped aim point while aiming; Blueprint events add the rest.
 * Animation (task-284-05, ARREnemy clip writer): walk loop (clip 0, random phase) while the movement is on (Approach, Reposition),
 * rest pose (RestClipId 2 held) while it is off (Hold); task-287-02: entering Aim plays AimClipId (1, clamped: cannon raised by
 * 0.3 s, then held); a cancelled aim returns to the movement clip (rest held); a shot skips that return and plays FireClipId
 * (3, clamped: recoil, back to the aim pose by FireClipDuration 0.2 s), whose end (OnAnimClipEnded) plays the movement clip.
 * Death leaves the clip.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRShooter : public ARREnemy
{
	GENERATED_BODY()

public:
	/** Sets the stats row (Shooter), the miner-sized capsule, the muzzle point and the hidden laser beam. */
	ARRShooter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Current state of the machine. */
	UFUNCTION(BlueprintPure, Category = "Shooter")
	ERRShooterState GetState() const { return State; }

	/** True while the targeting laser is on (Aim state). */
	UFUNCTION(BlueprintPure, Category = "Shooter")
	bool IsAiming() const { return State == ERRShooterState::Aim; }

	/** Actor aimed at during Aim (the miner); null otherwise. */
	UFUNCTION(BlueprintPure, Category = "Shooter")
	AActor* GetAimTarget() const;

	/** World location the shots and the laser start from (MuzzlePoint). */
	UFUNCTION(BlueprintPure, Category = "Shooter")
	FVector GetAimOrigin() const;

	/** Point aimed at: the target's location at the muzzle height (the muzzle itself when there is no target). */
	UFUNCTION(BlueprintPure, Category = "Shooter")
	FVector GetAimTargetPoint() const;

	/** Laser length (uu): distance from the muzzle to the aim point, clipped at the first rock block (grid line of sight). */
	UFUNCTION(BlueprintPure, Category = "Shooter")
	float GetLaserLength() const;

	/** Fraction of the aim elapsed in [0, 1] (0 outside Aim): lets the Blueprint ramp the laser before the shot. */
	UFUNCTION(BlueprintPure, Category = "Shooter")
	float GetAimProgress() const;

	/** Grid line of sight to the target at the last behavior step (false beyond Stats.AttackRange). */
	UFUNCTION(BlueprintPure, Category = "Shooter")
	bool HasSightOfTarget() const { return bHasSight; }

	/** True when the robot stands inside the visible floor rectangle (override or camera); false without any view. */
	UFUNCTION(BlueprintPure, Category = "Shooter")
	bool IsOnScreen() const;

	/** Uses Rectangle (world XY, uu) as the visible floor rectangle for IsOnScreen instead of the camera (tests, cinematics). */
	UFUNCTION(BlueprintCallable, Category = "Shooter")
	void SetViewRectangleOverride(FBox2D Rectangle);

	/** Returns IsOnScreen to the camera-computed view rectangle. */
	UFUNCTION(BlueprintCallable, Category = "Shooter")
	void ClearViewRectangleOverride() { bHasViewOverride = false; }

	/** Number of shots fired. */
	int32 GetShotCount() const { return ShotCount; }

	/** World time (s) of the last shot; negative before the first. */
	double GetLastShotTime() const { return LastShotTime; }

	/** World time (s) at which the last aim started; negative before the first. */
	double GetLastAimStartTime() const { return LastAimStartTime; }

protected:
	/** Sets the movement arrival distance from the preferred distance and enters Approach. */
	virtual void BeginPlay() override;

	/** Runs one step of the state machine. */
	virtual void UpdateBehavior(float DeltaSeconds) override;

	/** Ends a running aim, enters Dead, then runs the base death. */
	virtual void HandleDeath(AActor* DeathInstigator) override;

	/** Recycling relocation: back to Approach (leaving the current state as EnterState does); nothing while Dead. */
	virtual void ResetStateMachine() override;

	/** Pool reuse: base reset, then aim, sight, shot counters, laser and view override cleared, back to Approach. */
	virtual void ResetForNewLife() override;

	/** Fire clip end: nothing once Dead (death leaves the clip); else the base movement clip (rest held in Hold, walk if moving). */
	virtual void OnAnimClipEnded() override;

	/** Leaves the current state and enters NewState. Flow-field movement on in Approach and Reposition (arrival 0 in Reposition). Leaving Aim hides the laser and calls OnAimEnd. Clips: Aim plays AimClipId; leaving Aim to a stopped state plays the movement clip unless bLeavingAimForShot. */
	void EnterState(ERRShooterState NewState);

	/** Movement decisions of Approach, Hold and Reposition for the current target, distance and sight. */
	void UpdateMovement(const ARRMiner* Target, float Distance);

	/**
	 * Hold distance: PreferredDistance + PreferredDistanceTolerance, clamped to AttackRange (task-030-02: range 960 is inside the
	 * 1000 uu edge; a shooter parked beyond its range never had sight and never fired). Beyond it the robot approaches; within it
	 * it holds with sight. There is no inner edge (task-031-02: it never backs away).
	 */
	float GetRingOuterDistance() const;

	/**
	 * Steering direction toward a waypoint that never moves away from TargetLocation: the component of Direction pointing away
	 * from the target is removed and RepositionInwardBias of the inward direction added. Zero when nothing is left.
	 */
	FVector MakeNonRetreatingDirection(const FVector& Direction, const FVector& TargetLocation) const;

	/** Earliest world time (s) at which a new aim may start (cadence). */
	double GetNextAimTime() const;

	/** Enters Aim on Target: faces it, shows the laser, calls OnAimStart. */
	void StartAim(ARRMiner* Target);

	/** Spawns one ProjectileClass shot at Target's current position with the flat damage spec. */
	void Fire(const ARRMiner* Target);

	/** Turns the robot (yaw only) toward Target. */
	void FaceTarget(const AActor* Target);

	/** Places, scales and shows LaserBeam between the muzzle and the clipped aim point. */
	void UpdateLaserBeam();

	/** Arena grid (cached; RRWorld::FindArenaGrid). */
	ARRArenaGrid* GetGrid();

	/** Called when an aim starts (laser on). Target = the miner. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shooter")
	void OnAimStart(AActor* Target);

	/** Called when an aim ends, by the shot or cancelled (sight, range or screen lost, death). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shooter")
	void OnAimEnd();

	/** Called right after a shot is spawned (after OnAimEnd). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shooter")
	void OnFire();

	/** Start point of the shots and the laser (move it to the cannon's tip in the Blueprint; keep it below the block height). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> MuzzlePoint;

	/**
	 * Targeting laser, driven by C++ while aiming (hidden otherwise): centered between the muzzle and the clipped aim point,
	 * X along the beam, scaled to (length, LaserThickness, LaserThickness) / LaserMeshSize. The Blueprint sets a mesh of
	 * LaserMeshSize uu centered on its pivot (engine Cube) and the red material. No collision.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LaserBeam;

	/** Shot class (a Blueprint subclass of ARREnemyProjectile for the red mesh). Default: the native ARREnemyProjectile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Fire")
	TSubclassOf<ARRProjectile> ProjectileClass;

	/** Damage effect carried by the shot; must read SetByCaller Data.Damage (URRGE_DamageFlat). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Fire")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	/** Size (uu) of the laser mesh along each axis at scale 1 (engine Cube: 100). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Laser", meta = (ClampMin = 1, Units = "cm"))
	float LaserMeshSize = 100.f;

	/** Width and height (uu) of the laser beam. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Laser", meta = (ClampMin = 0, Units = "cm"))
	float LaserThickness = 4.f;

	/**
	 * Inward part (0..1) added to a repositioning step whose away-from-the-miner component was removed, so that sliding around
	 * the miner never increases the distance (task-031-02).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Reposition", meta = (ClampMin = 0, ClampMax = 1))
	float RepositionInwardBias = 0.1f;

	/** Baked clip played on entering Aim (rows 16..30, clamped: cannon raised by 0.3 s, then held at the aim pose). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Animation")
	float AimClipId = 1.f;

	/** Baked clip played on each shot (rows 47..61, clamped: recoil peak at 0.067 s, back to the aim pose at 0.2 s, then held). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Animation")
	float FireClipId = 3.f;

	/** Seconds the fire clip plays before the movement clip returns (the recoil is back to the aim pose at 0.2 s). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Animation", meta = (ClampMin = 0, Units = "s"))
	float FireClipDuration = 0.2f;

	/** True while the shot path leaves Aim: EnterState skips the movement clip, Fire plays FireClipId instead. */
	bool bLeavingAimForShot = false;

	/** Current state. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Shooter")
	ERRShooterState State = ERRShooterState::Approach;

	/** Target of the running aim. */
	TWeakObjectPtr<ARRMiner> AimTarget;

	/** Cached arena grid. */
	TWeakObjectPtr<ARRArenaGrid> CachedGrid;

	/** View rectangle set by SetViewRectangleOverride. */
	FBox2D ViewOverride = FBox2D(ForceInit);

	/** True while ViewOverride replaces the camera. */
	bool bHasViewOverride = false;

	/** Grid line of sight to the target at the last step. */
	bool bHasSight = false;

	/** World time (s) of the last shot; negative before the first. */
	double LastShotTime = -1.0;

	/** World time (s) at which the last aim started; negative before the first. */
	double LastAimStartTime = -1.0;

	/** Shots fired. */
	int32 ShotCount = 0;
};
