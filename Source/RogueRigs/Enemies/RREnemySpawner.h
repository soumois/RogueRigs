// Rogue Rigs - enemy spawner: a trickle of single robots, then packs from one side, capped alive count, on reachable floor cells just outside the camera view.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RREnemySpawner.generated.h"

class ARREnemy;
class ARRArenaGrid;
class URRFlowFieldSubsystem;
struct FMinimalViewInfo;

/** Side of the visible floor rectangle a cell lies beyond (the axis of its larger gap; X wins a tie). */
UENUM(BlueprintType)
enum class ERRSpawnSide : uint8
{
	/** Inside the rectangle (no gap). */
	None,
	/** Beyond the -X edge. */
	NegativeX,
	/** Beyond the +X edge. */
	PositiveX,
	/** Beyond the -Y edge. */
	NegativeY,
	/** Beyond the +Y edge. */
	PositiveY
};

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
 * Trickle phase, the first TrickleDuration seconds of spawning: one robot every SpawnInterval seconds. Pack phase after it: a pack
 * of PackSizeMin to PackSizeMax robots every PackInterval seconds (the first one when the phase starts), all from one side of the
 * view rectangle, on distinct cells, clamped to the free slots; between packs, every SpawnInterval, a lone robot with chance
 * LoneSpawnChance. Nothing spawns beyond MaxAlive robots alive. Each robot class is a weighted draw among WeightedEnemyClasses,
 * else EnemyClass. A robot spawns on a random cell that is walkable and reachable in the flow field (URRFlowFieldSubsystem), not occupied by a pawn, and lies in a
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

	/** Seeds the random stream (a new seed per run). */
	virtual void BeginPlay() override;

	/** Advances the run clock and the trickle, pack and lone-robot timers while enabled; spawns what they call for. */
	virtual void Tick(float DeltaSeconds) override;

	/**
	 * Spawns one pack now (ignores the timers and the enabled flags): size drawn in [PackSizeMin, PackSizeMax], clamped to the
	 * free slots and to the cells of the chosen side. The side is drawn among the sides with at least that many candidate cells,
	 * else the side with the most. One candidate scan per pack. Returns the robots spawned (empty when none).
	 */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	TArray<ARREnemy*> TrySpawnPack();

	/** Reseeds the spawner random stream (cells, sides, pack sizes, classes, lone rolls): reproducible draws in tests. */
	void SeedRandomStream(int32 Seed) { RandomStream.Initialize(Seed); }

	/** Seconds of enabled spawning since BeginPlay (the trickle phase lasts TrickleDuration of them). */
	UFUNCTION(BlueprintPure, Category = "Spawner")
	float GetRunTime() const { return RunTime; }

	/** True once the trickle phase is over and packs come. */
	UFUNCTION(BlueprintPure, Category = "Spawner")
	bool IsInPackPhase() const { return bPackPhase; }

	/** Side of Rectangle that Point lies beyond: the axis of the larger gap (X on a tie), None inside. */
	static ERRSpawnSide GetSideOfRectangle(const FBox2D& Rectangle, const FVector2D& Point);

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
	TSubclassOf<ARREnemy> PickEnemyClass(const FRandomStream& Stream) const;

	/** PickEnemyClass with the spawner random stream (used by TrySpawnEnemy and TrySpawnPack). */
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

	/**
	 * Visible floor rectangle (world XY) of World's camera on the plane Z = FloorZ, without any override: the first player
	 * controller's camera manager (camera cache view, viewport aspect ratio), else the first camera component of
	 * FallbackCameraOwner (the miner: works headless). False when neither exists. Shared by the spawner and the shooter's
	 * on-screen check.
	 */
	static bool ComputeCameraViewRectangle(UWorld* World, float FloorZ, const AActor* FallbackCameraOwner, FBox2D& OutRectangle);

	/** Box distance in uu from Point to Rectangle (0 inside): the larger of the X and Y gaps. */
	static float GetDistanceToRectangle(const FBox2D& Rectangle, const FVector2D& Point);

protected:
	/** Flow field of the world (null outside a game world). */
	URRFlowFieldSubsystem* GetFlowField() const;

	/** Grid the flow field uses; null without one. */
	ARRArenaGrid* GetGrid() const;

	/** Removes dead or destroyed robots from AliveEnemies. */
	void PruneAlive();

	/** Spawns one robot of SpawnClass on Cell (capsule on the floor) and tracks it; null when the spawn fails. */
	ARREnemy* SpawnEnemyOnCell(TSubclassOf<ARREnemy> SpawnClass, const FIntPoint& Cell);

	/** Robot class spawned when WeightedEnemyClasses has no valid entry (a Blueprint subclass of ARRChaser for the mesh). None = no spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	TSubclassOf<ARREnemy> EnemyClass;

	/** Weighted robot classes (default of the placed spawner: chaser 70, kamikaze 15, shooter 15). Empty or all invalid = EnemyClass. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	TArray<FRRWeightedEnemyClass> WeightedEnemyClasses;

	/** Trickle phase: seconds between two single spawns. Pack phase: seconds between two lone-robot rolls. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = 0.01, Units = "s"))
	float SpawnInterval = 2.f;

	/** Seconds of enabled spawning spent in the trickle phase before packs start. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Packs", meta = (ClampMin = 0, Units = "s"))
	float TrickleDuration = 30.f;

	/** Pack phase: seconds between two packs. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Packs", meta = (ClampMin = 0.01, Units = "s"))
	float PackInterval = 6.f;

	/** Smallest pack (robots), before the clamp to free slots and side cells. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Packs", meta = (ClampMin = 1))
	int32 PackSizeMin = 8;

	/** Largest pack (robots); below PackSizeMin it counts as PackSizeMin. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Packs", meta = (ClampMin = 1))
	int32 PackSizeMax = 15;

	/**
	 * Pack phase: chance of a lone robot at each SpawnInterval roll between packs (a pack resets the roll timer). With 0.1, a 2 s
	 * interval and 6 s between packs: two rolls per gap, about one lone robot every 5 gaps.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Packs", meta = (ClampMin = 0, ClampMax = 1))
	float LoneSpawnChance = 0.1f;

	/** Maximum number of robots of this spawner alive at once; spawns beyond it are skipped (packs are clamped). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = 0))
	int32 MaxAlive = 100;

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

	/** Trickle phase: seconds accumulated toward the next single spawn. */
	float SpawnTimer = 0.f;

	/** Seconds of enabled spawning since BeginPlay. */
	float RunTime = 0.f;

	/** True once RunTime reached TrickleDuration. */
	bool bPackPhase = false;

	/** Pack phase: seconds accumulated toward the next pack. */
	float PackTimer = 0.f;

	/** Pack phase: seconds accumulated toward the next lone-robot roll (reset by each pack). */
	float LoneTimer = 0.f;

	/** Random stream of every spawner draw; seeded in BeginPlay, reseeded by SeedRandomStream. */
	FRandomStream RandomStream;

	/** View rectangle set by SetViewRectangleOverride. */
	FBox2D ViewOverride = FBox2D(ForceInit);

	/** True while ViewOverride replaces the camera. */
	bool bHasViewOverride = false;
};
