// Rogue Rigs - enemy spawner: one robot per interval, capped alive count, on reachable floor cells just outside the camera view.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RREnemySpawner.generated.h"

class ARREnemy;
class ARRArenaGrid;
class URRFlowFieldSubsystem;
struct FMinimalViewInfo;

/** One entry of the spawner weighted robot list: a class and its relative weight. */
USTRUCT(BlueprintType)
struct FRRWeightedEnemyClass
{
	GENERATED_BODY()

	/** Robot class (a Blueprint subclass of ARRChaser, ARRKamikaze...). None = entry ignored. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	TSubclassOf<ARREnemy> EnemyClass;

	/** Relative weight: chance = Weight / sum of the weights of the valid entries. 0 or less = never picked. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = 0))
	float Weight = 1.f;
};

/**
 * Placed actor that keeps robots coming from outside the screen (sys-spawner).
 * Every SpawnInterval seconds, when fewer than MaxAlive robots it spawned are alive, it spawns one robot (weighted draw among
 * WeightedEnemyClasses, else EnemyClass) on a
 * random cell that is walkable and reachable in the flow field (URRFlowFieldSubsystem), not occupied by a pawn, and lies in a
 * band around the visible floor rectangle: the cell center is at least ViewMargin + 0.5 cells and at most
 * MaxSpawnDistanceCells + 0.5 cells outside the rectangle (box distance), so with ViewMargin 1 the whole cell is at least one
 * cell beyond the view edge. The robot spawns at the cell center, its capsule resting on the floor.
 * View rectangle source, first available: the override (SetViewRectangleOverride, used by tests); the first local player's
 * camera manager (its camera cache view, viewport aspect ratio); the first camera component of the flow-field goal actor (the
 * miner's camera, works headless). Each is the bounding box of the four frustum corner rays projected on the floor plane.
 * A robot stops counting as alive when it dies (IsDead) or is destroyed. Off when bEnabled is false or rr.Spawner.Enabled is 0.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARREnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	/** Ticks the spawn timer. */
	ARREnemySpawner();

	/** Advances the timer and spawns one robot each SpawnInterval while enabled. */
	virtual void Tick(float DeltaSeconds) override;

	/**
	 * Spawns one robot now if a slot is free and a valid cell exists (ignores the timer and the enabled flags).
	 * Returns the robot, null when nothing spawned.
	 */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	ARREnemy* TrySpawnEnemy();

	/**
	 * Class of the next robot: a weighted draw (Stream) among WeightedEnemyClasses entries with a class and a positive weight;
	 * EnemyClass when there is none. Null when nothing is set.
	 */
	TSubclassOf<ARREnemy> PickEnemyClass(FRandomStream& Stream) const;

	/** PickEnemyClass with the global random generator (used by TrySpawnEnemy). */
	TSubclassOf<ARREnemy> PickEnemyClass() const;

	/** Robot classes this spawner can spawn: EnemyClass, then each WeightedEnemyClasses entry with a class (no duplicates). */
	void GetSpawnableClasses(TArray<TSubclassOf<ARREnemy>>& OutClasses) const;

	/** Number of robots spawned by this spawner that are alive (not dead, not destroyed). Prunes the list. */
	UFUNCTION(BlueprintPure, Category = "Spawner")
	int32 GetAliveCount();

	/** Robots spawned by this spawner that are alive. Prunes the list. */
	TArray<ARREnemy*> GetAliveEnemies();

	/** True when the spawner runs on its own: bEnabled and rr.Spawner.Enabled != 0. */
	UFUNCTION(BlueprintPure, Category = "Spawner")
	bool IsSpawningEnabled() const;

	/** Turns the timed spawning on or off (bEnabled). */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SetSpawningEnabled(bool bInEnabled) { bEnabled = bInEnabled; }

	/** Uses Rectangle (world XY, uu) as the visible floor rectangle instead of a camera (tests, cinematics). */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SetViewRectangleOverride(FBox2D Rectangle);

	/** Returns to the camera-computed view rectangle. */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void ClearViewRectangleOverride() { bHasViewOverride = false; }

	/** Current visible floor rectangle (world XY) from the override, the player camera or the goal's camera; false without one. */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	bool GetViewRectangle(FBox2D& OutRectangle) const;

	/** Every cell a robot may spawn on now (band around the view, walkable, reachable, free of pawns). */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void GetSpawnCandidates(TArray<FIntPoint>& OutCells);

	/**
	 * Bounding box (world XY) of the frustum of View projected on the plane Z = FloorZ, horizontal field of view View.FOV and
	 * width / height AspectRatio. A corner ray that does not point down is clamped to a shallow downward slope.
	 */
	static FBox2D ProjectViewOnFloor(const FMinimalViewInfo& View, float AspectRatio, float FloorZ);

	/** Box distance in uu from Point to Rectangle (0 inside): the larger of the X and Y gaps. */
	static float GetDistanceToRectangle(const FBox2D& Rectangle, const FVector2D& Point);

protected:
	/** Flow field of the world (null outside a game world). */
	URRFlowFieldSubsystem* GetFlowField() const;

	/** Grid the flow field uses; null without one. */
	ARRArenaGrid* GetGrid() const;

	/** Removes dead or destroyed robots from AliveEnemies. */
	void PruneAlive();

	/** Robot class spawned when WeightedEnemyClasses has no valid entry (a Blueprint subclass of ARRChaser for the mesh). None = no spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	TSubclassOf<ARREnemy> EnemyClass;

	/** Weighted robot classes (e.g. chaser 3, kamikaze 1 = 25 % kamikazes). Empty or all invalid = EnemyClass. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	TArray<FRRWeightedEnemyClass> WeightedEnemyClasses;

	/** Seconds between two spawn attempts (one robot per attempt). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = 0.01, Units = "s"))
	float SpawnInterval = 2.f;

	/** Maximum number of robots of this spawner alive at once; an attempt at the cap spawns nothing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = 0))
	int32 MaxAlive = 20;

	/** Minimum gap in cells between the visible floor rectangle and a spawn cell (whole cell beyond it). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = 0))
	int32 ViewMargin = 1;

	/** Maximum distance in cells beyond the view edge of a spawn cell (cell center within this many cells + 0.5). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = 0))
	int32 MaxSpawnDistanceCells = 6;

	/** Timed spawning on or off (also see rr.Spawner.Enabled). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	bool bEnabled = true;

	/** Height above the floor added to the capsule half height at spawn, so the capsule starts just over the floor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = 0, Units = "cm"))
	float SpawnFloorClearance = 2.f;

	/** Robots spawned and still alive (pruned on query). */
	TArray<TWeakObjectPtr<ARREnemy>> AliveEnemies;

	/** Seconds accumulated toward the next attempt. */
	float SpawnTimer = 0.f;

	/** View rectangle set by SetViewRectangleOverride. */
	FBox2D ViewOverride = FBox2D(ForceInit);

	/** True while ViewOverride replaces the camera. */
	bool bHasViewOverride = false;
};
