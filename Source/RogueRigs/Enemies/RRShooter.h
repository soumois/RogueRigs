// Rogue Rigs - shooter robot: keeps a ring of distance from the miner, fires dodgeable shots with grid line of sight while on screen, telegraphs each shot with a targeting laser (C++ state machine Approach, Hold, Reposition, Aim, Dead).

#pragma once

#include "CoreMinimal.h"
#include "Enemies/RREnemy.h"
#include "RRShooter.generated.h"

class ARRArenaGrid;
class ARRMiner;
class ARRProjectile;
class UGameplayEffect;
class URRFlowFieldSubsystem;
class UStaticMeshComponent;

/** State of the shooter's machine. */
UENUM(BlueprintType)
enum class ERRShooterState : uint8
{
	/** Follows the flow field toward the miner: to the preferred distance with line of sight, closer until it has sight. */
	Approach,
	/** Stands still in the preferred ring with line of sight (or nowhere better to go). */
	Hold,
	/** Walks a local grid path to a cell of the preferred ring that has line of sight to the miner. */
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
 * Movement, ring = Stats.PreferredDistance +/- Stats.PreferredDistanceTolerance: Approach (flow field, arrival at the preferred
 * distance with sight, 0 without) -> Hold once within the preferred distance with sight. Hold -> Approach beyond the ring.
 * Without sight in the ring, or too close: a repositioning search (at most every RepositionSearchInterval): among the cells of
 * URRFlowFieldSubsystem::GatherReachableCells within RepositionSearchRadiusCells, those in the ring with grid sight to the
 * target; the one with the shortest local grid path (8-neighbour BFS around the robot, no corner cutting) wins and the robot
 * walks that path (Reposition). No cell: Approach without sight (closing in gives sight), Hold with sight. The C++ drives a
 * LaserBeam mesh between the muzzle and the line-of-sight-clipped aim point while aiming; Blueprint events add the rest.
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

	/** Cell the current repositioning path ends on; (INDEX_NONE, INDEX_NONE) when none. */
	FIntPoint GetRepositionCell() const { return RepositionCell; }

protected:
	/** Sets the movement arrival distance from the preferred distance and enters Approach. */
	virtual void BeginPlay() override;

	/** Runs one step of the state machine. */
	virtual void UpdateBehavior(float DeltaSeconds) override;

	/** Ends a running aim, enters Dead, then runs the base death. */
	virtual void HandleDeath(AActor* DeathInstigator) override;

	/** Leaves the current state and enters NewState (flow-field movement on in Approach only). Leaving Aim hides the laser and calls OnAimEnd. */
	void EnterState(ERRShooterState NewState);

	/** Movement decisions of Approach, Hold and Reposition for the current target, distance and sight. */
	void UpdateMovement(const ARRMiner* Target, float Distance);

	/** True when Distance (uu) lies in the preferred ring (PreferredDistance +/- PreferredDistanceTolerance). */
	bool IsInRing(float Distance) const;

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

	/**
	 * Runs a repositioning search if RepositionSearchInterval has passed since the last one: enters Reposition with the found
	 * path, else Approach without sight or Hold with sight. When the search is throttled, nothing changes.
	 */
	void TryReposition(const ARRMiner* Target);

	/**
	 * Repositioning search around the robot for Target: candidate cells from URRFlowFieldSubsystem::GatherReachableCells within
	 * RepositionSearchRadiusCells (Chebyshev), in the preferred ring and with grid sight to the target; the one with the fewest
	 * steps of a local 8-neighbour BFS from the robot's cell (no corner cutting) wins, ties to the ring center then scan order.
	 * Fills OutWaypoints (cell centers from the first step to the chosen cell) and OutCell. False when no cell qualifies.
	 */
	bool FindRepositionPath(const ARRMiner* Target, TArray<FVector>& OutWaypoints, FIntPoint& OutCell);

	/** Walks the repositioning path: skips a waypoint already in direct reach, steers at the next one. True once arrived. */
	bool FollowRepositionPath();

	/** Arena grid (cached; RRWorld::FindArenaGrid). */
	ARRArenaGrid* GetGrid();

	/** Flow field subsystem of the world. */
	URRFlowFieldSubsystem* GetFlowField() const;

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

	/** Half size (cells) of the square searched around the robot for a firing position. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Reposition", meta = (ClampMin = 1))
	int32 RepositionSearchRadiusCells = 18;

	/** Minimum seconds between two repositioning searches of this robot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Reposition", meta = (ClampMin = 0, Units = "s"))
	float RepositionSearchInterval = 0.5f;

	/** Distance (uu, 2D) under which a path waypoint counts as reached. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Reposition", meta = (ClampMin = 1, Units = "cm"))
	float WaypointAcceptanceRadius = 40.f;

	/** Seconds after which an unfinished repositioning walk is abandoned (Hold, then a new search). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shooter|Reposition", meta = (ClampMin = 0, Units = "s"))
	float RepositionMaxDuration = 4.f;

	/** Current state. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Shooter")
	ERRShooterState State = ERRShooterState::Approach;

	/** Target of the running aim. */
	TWeakObjectPtr<ARRMiner> AimTarget;

	/** Cached arena grid. */
	TWeakObjectPtr<ARRArenaGrid> CachedGrid;

	/** Waypoints (cell centers) left on the repositioning path. */
	TArray<FVector> RepositionWaypoints;

	/** Last cell of the repositioning path; (INDEX_NONE, INDEX_NONE) when none. */
	FIntPoint RepositionCell = FIntPoint(INDEX_NONE, INDEX_NONE);

	/** Target cell when the repositioning path was computed (a new search runs when the target leaves it). */
	FIntPoint RepositionTargetCell = FIntPoint(INDEX_NONE, INDEX_NONE);

	/** View rectangle set by SetViewRectangleOverride. */
	FBox2D ViewOverride = FBox2D(ForceInit);

	/** True while ViewOverride replaces the camera. */
	bool bHasViewOverride = false;

	/** Grid line of sight to the target at the last step. */
	bool bHasSight = false;

	/** World time (s) of the last repositioning search; very negative before the first. */
	double LastSearchTime = -1.0e9;

	/** World time (s) of the last shot; negative before the first. */
	double LastShotTime = -1.0;

	/** World time (s) at which the last aim started; negative before the first. */
	double LastAimStartTime = -1.0;

	/** Shots fired. */
	int32 ShotCount = 0;
};
