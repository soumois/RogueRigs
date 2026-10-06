// Rogue Rigs - enemy spawner: wave director (calm, lone robots, packs of one type from several sides, announced hordes) or the older trickle then packs, capped alive count, on reachable floor cells just outside the camera view.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemies/RRWaveDirector.h"
#include "UObject/ObjectKey.h"
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

/** Sector of a spawn point around the miner relative to its run heading (front within FrontHalfAngle, back within BackHalfAngle of the reverse heading, sides between). */
UENUM(BlueprintType)
enum class ERRSpawnSector : uint8
{
	/** No heading (the miner stands still) or no origin. */
	None,
	/** Within FrontHalfAngle of the heading. */
	Front,
	/** Between the front and the back cones (both sides). */
	Side,
	/** Within BackHalfAngle of the reverse heading. */
	Back
};

/** Wave director entered phase PhaseIndex (DirectorConfig.Phases), named PhaseName. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnDirectorPhaseChangedSignature, int32, PhaseIndex, FName, PhaseName);

/** Horde HordeIndex: its lull starts (nothing spawns until its front). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnHordeLullSignature, int32, HordeIndex);

/** Horde HordeIndex is announced: its front starts in SecondsUntil seconds (the HUD banner). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnHordeIncomingSignature, int32, HordeIndex, float, SecondsUntil);

/** Horde HordeIndex: its front starts spawning beyond Side of the view (None when no cell was found). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnHordeStartedSignature, int32, HordeIndex, ERRSpawnSide, Side);

/** Horde HordeIndex: its front is over, Spawned robots spawned. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnHordeFinishedSignature, int32, HordeIndex, int32, Spawned);

/** Enemy was recycled (removed far off screen without dying); called just before its Destroy. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnEnemyRecycledSignature, ARREnemy*, Enemy);

/** One pack spawned by the wave director: its type, side and robots (front robots first). */
struct FRRSpawnedPack
{
	/** Index in DirectorConfig.PackTypes. */
	int32 PackType = INDEX_NONE;

	/** Side of the view it came from. */
	ERRSpawnSide Side = ERRSpawnSide::None;

	/** Robots spawned, front members first. */
	TArray<ARREnemy*> Robots;

	/** Spawn cell of each robot of Robots (the actor may be nudged off it when capsules overlap). */
	TArray<FIntPoint> Cells;
};

/** One planned robot of a horde front: class and cell, in spawn order. */
struct FRRHordeSlot
{
	/** Resolved robot class. */
	TSubclassOf<ARREnemy> Class;

	/** Spawn cell. */
	FIntPoint Cell = FIntPoint::ZeroValue;
};

/** One entry of the spawner weighted robot list: a class, its relative weight and an optional alive cap for that class. */
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

	/**
	 * Most robots of this class (IsA, subclasses included) of this spawner alive at once; while reached, the entry is left out of
	 * the draw (its weight goes to the others). 0 = no cap (heavy: 5, enemy-heavy).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = 0))
	int32 MaxAlive = 0;
};

/**
 * Placed actor that keeps robots coming from outside the screen (sys-spawner).
 * Wave director (DirectorConfig.bEnabled, default): FRRWaveDirector schedules phases of lone robots and waves of packs of one
 * type (each pack from a distinct side, front members on the cells nearest to the view) and announced hordes (lull, banner event,
 * a front along one side spawned over a few seconds, front members first). Robot classes of the director data are resolved to the
 * WeightedEnemyClasses entries (ResolveRobotClass) and respect their MaxAlive caps. Without the director, the older timers below:
 * Trickle phase, the first TrickleDuration seconds of spawning: one robot every SpawnInterval seconds. Pack phase after it: a pack
 * of PackSizeMin to PackSizeMax robots every PackInterval seconds (the first one when the phase starts), all from one side of the
 * view rectangle, on distinct cells, clamped to the free slots; between packs, every SpawnInterval, a lone robot with chance
 * LoneSpawnChance. Nothing spawns beyond MaxAlive robots alive. Each robot class is a weighted draw among WeightedEnemyClasses,
 * else EnemyClass. A robot spawns on a random cell that is walkable and reachable in the flow field (URRFlowFieldSubsystem), not occupied by a pawn, and lies in a
 * band around the visible floor rectangle: the cell center is SpawnBandMinDistance to SpawnBandMaxDistance uu outside the
 * rectangle (box distance); when the band has no such cell, the nearest band of the same width farther out (up to
 * SpawnBandFallbackMaxDistance); cells whose path to the miner detours more than MaxPathDetourRatio are left out while others
 * remain. The robot spawns at the cell center, its capsule resting on the floor.
 * Run heading: the miner's (flow-field goal) movement over the last HeadingWindowSeconds; at HeadingMinSpeed or faster, each lone
 * robot, each director pack and each horde side first draws a sector around the miner (front / sides / back, weights
 * FrontSectorWeight / SideSectorWeight / BackSectorWeight, empty sectors left out) and spawns there; slower = uniform draw.
 * View rectangle source, first available: the override (SetViewRectangleOverride, used by tests); the first local player's
 * camera manager (its camera cache view, viewport aspect ratio); the first camera component of the flow-field goal actor (the
 * miner's camera, works headless). Each is the bounding box of the four frustum corner rays projected on the floor plane.
 * A robot stops counting as alive when it dies (IsDead) or is destroyed. Off when bEnabled is false or rr.Spawner.Enabled is 0.
 * Recycling (bRecycleEnabled, every tick, spawning on or off): a robot of this spawner farther than RecycleDistance from the miner
 * and more than RecycleOffScreenMargin beyond the view rectangle for RecycleDelay seconds in a row is destroyed: no death, no XP,
 * no kill, no explosion; its slot (MaxAlive, per-class caps, shooter cap) is free at once.
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

	/** Advances the run clock and the director (or the older trickle, pack and lone-robot timers) while enabled; spawns what they call for. */
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
	 * Class of the next robot: a weighted draw (Stream) among WeightedEnemyClasses entries with a class, a positive weight and
	 * room under their MaxAlive cap; EnemyClass when no entry has a class and a positive weight. Null when nothing is set or every
	 * valid entry is capped.
	 */
	TSubclassOf<ARREnemy> PickEnemyClass(const FRandomStream& Stream) const;

	/** PickEnemyClass with the spawner random stream (used by TrySpawnEnemy and TrySpawnPack). */
	TSubclassOf<ARREnemy> PickEnemyClass() const;

	/** Robot classes this spawner can spawn: EnemyClass, then each WeightedEnemyClasses entry with a class (no duplicates). */
	void GetSpawnableClasses(TArray<TSubclassOf<ARREnemy>>& OutClasses) const;

	/** Number of robots of Class (IsA) spawned by this spawner that are alive (not dead, not destroyed). Does not prune. */
	int32 GetAliveCountOfClass(TSubclassOf<ARREnemy> Class) const;

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

	// --- Run heading ---

	/**
	 * Sector of Point seen from Origin for a run toward Heading (unit or not): Front within FrontHalfAngle degrees of Heading,
	 * Back within BackHalfAngle degrees of -Heading, Side otherwise. None for a zero Heading or Point on Origin.
	 */
	static ERRSpawnSector GetSectorOfPoint(const FVector2D& Origin, const FVector2D& Heading, const FVector2D& Point, float FrontHalfAngle, float BackHalfAngle);

	/**
	 * Run heading (unit, world XY) of the miner: its movement over the last HeadingWindowSeconds, or the override. False (and a
	 * zero direction) while it moves slower than HeadingMinSpeed or without a miner: the spawns are uniform then.
	 */
	UFUNCTION(BlueprintCallable, Category = "Spawner|Heading")
	bool GetRunHeading(FVector2D& OutDirection) const;

	/** Miner speed (uu/s) over the last HeadingWindowSeconds; 0 without samples. The override reports HeadingMinSpeed or 0. */
	UFUNCTION(BlueprintPure, Category = "Spawner|Heading")
	float GetRunSpeed() const;

	/** Sector of Point for the current heading, around the miner; None while still. Used by tests and debug views. */
	ERRSpawnSector GetSpawnSector(const FVector2D& Point) const;

	/** Forces the run heading to Direction (tests); a zero Direction means standing still. */
	void SetHeadingOverride(const FVector2D& Direction);

	/** Returns to the heading measured from the miner's movement. */
	void ClearHeadingOverride() { bHasHeadingOverride = false; }

	// --- Recycling ---

	/** A robot was recycled (far off screen for RecycleDelay seconds); broadcast before it is destroyed. */
	UPROPERTY(BlueprintAssignable, Category = "Spawner|Recycling")
	FRROnEnemyRecycledSignature OnEnemyRecycled;

	/** Robots recycled since BeginPlay. */
	UFUNCTION(BlueprintPure, Category = "Spawner|Recycling")
	int32 GetRecycledCount() const { return RecycledCount; }

	/** Seconds Enemy has been recyclable in a row (far and off screen); 0 when it is not or not tracked. */
	float GetRecycleSeconds(const ARREnemy* Enemy) const;

	// --- Wave director ---

	/** Wave director phase changes (index in DirectorConfig.Phases, name). */
	UPROPERTY(BlueprintAssignable, Category = "Spawner|Director")
	FRROnDirectorPhaseChangedSignature OnDirectorPhaseChanged;

	/** A horde lull starts (no spawn until the front). */
	UPROPERTY(BlueprintAssignable, Category = "Spawner|Director")
	FRROnHordeLullSignature OnHordeLull;

	/** A horde is announced, SecondsUntil its front (HUD banner "HORDE EN APPROCHE !"). */
	UPROPERTY(BlueprintAssignable, Category = "Spawner|Director")
	FRROnHordeIncomingSignature OnHordeIncoming;

	/** A horde front starts spawning beyond a side of the view. */
	UPROPERTY(BlueprintAssignable, Category = "Spawner|Director")
	FRROnHordeStartedSignature OnHordeStarted;

	/** A horde front is fully spawned. */
	UPROPERTY(BlueprintAssignable, Category = "Spawner|Director")
	FRROnHordeFinishedSignature OnHordeFinished;

	/** True when the wave director drives the timed spawning (DirectorConfig.bEnabled). */
	UFUNCTION(BlueprintPure, Category = "Spawner|Director")
	bool IsDirectorEnabled() const { return DirectorConfig.bEnabled; }

	/** Director data (phases, pack types, hordes). */
	const FRRWaveDirectorConfig& GetDirectorConfig() const { return DirectorConfig; }

	/** Director scheduler state (phase, hordes). */
	const FRRWaveDirector& GetDirector() const { return Director; }

	/** Current director phase index; INDEX_NONE before the first. */
	UFUNCTION(BlueprintPure, Category = "Spawner|Director")
	int32 GetDirectorPhase() const { return Director.GetPhaseIndex(); }

	/** What starts the hordes (run time or alert). */
	UFUNCTION(BlueprintPure, Category = "Spawner|Director")
	ERRHordeTrigger GetHordeTrigger() const { return DirectorConfig.HordeTrigger; }

	/** Number of hordes in the director data. */
	UFUNCTION(BlueprintPure, Category = "Spawner|Director")
	int32 GetHordeCount() const { return DirectorConfig.Hordes.Num(); }

	/** RunTime trigger: seconds of spawning at which the front of horde Index starts; -1 out of range. */
	UFUNCTION(BlueprintPure, Category = "Spawner|Director")
	float GetHordeTriggerTime(int32 Index) const { return DirectorConfig.Hordes.IsValidIndex(Index) ? DirectorConfig.Hordes[Index].TriggerTime : -1.f; }

	/** Alert trigger: alert (0..1) at which the lull of horde Index starts; -1 out of range. */
	UFUNCTION(BlueprintPure, Category = "Spawner|Director")
	float GetHordeTriggerAlert(int32 Index) const { return DirectorConfig.Hordes.IsValidIndex(Index) ? DirectorConfig.Hordes[Index].TriggerAlert : -1.f; }

	/** State of horde Index. */
	UFUNCTION(BlueprintPure, Category = "Spawner|Director")
	ERRHordeState GetHordeState(int32 Index) const { return Director.GetHordeState(Index); }

	/** Horde in its lull, announced or spawning; INDEX_NONE (-1) when none. */
	UFUNCTION(BlueprintPure, Category = "Spawner|Director")
	int32 GetActiveHorde() const { return Director.GetActiveHorde(); }

	/** Seconds until the front of the active horde (0 while spawning); -1 without an active horde. */
	UFUNCTION(BlueprintPure, Category = "Spawner|Director")
	float GetSecondsUntilHordeFront() const { return Director.GetSecondsUntilHordeFront(DirectorConfig); }

	/**
	 * Class to spawn for a director class: the WeightedEnemyClasses entry (then EnemyClass) whose nearest native class is Wanted;
	 * Wanted itself when none (or when Wanted is already a Blueprint class). Null for null.
	 */
	TSubclassOf<ARREnemy> ResolveRobotClass(TSubclassOf<ARREnemy> Wanted) const;

	/**
	 * Spawns one wave now (ignores the timers and the enabled flags): one pack per entry of PackTypeIndices (DirectorConfig.PackTypes),
	 * each from a distinct side of the view while sides remain, on adjacent cells, front members nearest to the view. Packs are
	 * clamped to the free slots and the per-class caps. One candidate scan. Returns the packs spawned.
	 */
	TArray<FRRSpawnedPack> TrySpawnDirectorWave(const TArray<int32>& PackTypeIndices);

	/** Spawns one robot of SpawnClass now (resolved, per-class cap and MaxAlive respected); none = weighted mix. Null when nothing spawned. */
	ARREnemy* TrySpawnEnemyOfClass(TSubclassOf<ARREnemy> SpawnClass);

protected:
	/** Runs the director for one frame and executes its orders. */
	void TickDirector(float DeltaSeconds);

	/** Executes one director order. */
	void ExecuteDirectorAction(const FRRDirectorAction& Action);

	/** Alert of the run flow (0..1); 0 without one. */
	float GetDirectorAlert() const;

	/** Free slots for SpawnClass under its WeightedEnemyClasses MaxAlive cap (MAX_int32 when uncapped). */
	int32 GetClassRoom(TSubclassOf<ARREnemy> SpawnClass) const;

	/**
	 * Draws the robots of Members (resolved classes) into OutFront and OutBack, within Room (per resolved class free slots, updated).
	 * OutBack is shuffled so escorts mix with the main group; bBehind members are then inserted at random in its back half.
	 * Shooters beyond ShooterRoom (DirectorConfig.ShooterMaxAlive minus the shooters alive, updated) become
	 * DirectorConfig.ShooterCapReplacement robots in the same list (no extra random draw).
	 */
	void DrawMembers(const TArray<FRRDirectorPackMember>& Members, TMap<UClass*, int32>& Room, int32& ShooterRoom,
		TArray<TSubclassOf<ARREnemy>>& OutFront, TArray<TSubclassOf<ARREnemy>>& OutBack);

	/** Free shooter slots under DirectorConfig.ShooterMaxAlive (ARRShooter and subclasses alive). */
	int32 GetDirectorShooterRoom() const;

	/** Splits Candidates by the side of View they lie beyond (index = ERRSpawnSide - 1). */
	void SplitCandidatesBySide(const FBox2D& View, const TArray<FIntPoint>& Candidates, TArray<FIntPoint> (&OutBySide)[4]) const;

	/** Records the miner's location for the run heading (every tick, spawning on or off); drops samples older than the window. */
	void RecordHeadingSample();

	/** Miner location (world XY) the sectors are measured from; false without a miner. */
	bool GetHeadingOrigin(FVector2D& OutOrigin) const;

	/**
	 * Draws the sector of the next spawn among Cells while running (weights FrontSectorWeight, SideSectorWeight,
	 * BackSectorWeight; sectors without a cell of Cells left out); None while still, without cells or when every weight is 0.
	 */
	ERRSpawnSector DrawSpawnSector(const TArray<FIntPoint>& Cells);

	/** Sector of the center of Cell for the current heading; None while still. */
	ERRSpawnSector GetCellSector(const FIntPoint& Cell) const;

	/**
	 * Side index (ERRSpawnSide - 1) and anchor cell for a pack or horde of Needed robots. Sector None: RRSpawner::PickSide (uniform
	 * among the unused sides with room) and a random anchor of it. Otherwise a random cell of Sector among the unused sides with
	 * at least Needed cells, else among the unused sides, else among all sides; its side wins. OutAnchor (optional; no extra draw when null) gets that cell. INDEX_NONE when every side is empty.
	 */
	int32 PickSideForSector(const TArray<FIntPoint> (&BySide)[4], const bool (&Used)[4], int32 Needed, ERRSpawnSector Sector, FIntPoint* OutAnchor);

	/** Plans the front of horde HordeIndex (side, classes, cells) and returns its side; None when no cell. */
	ERRSpawnSide PrepareHorde(int32 HordeIndex);

	/** Spawns the planned horde robots up to Fraction (0..1) of the plan, skipping cells that entered the spawn margin. */
	void SpawnHordeUpTo(float Fraction);

	/** Wave director data: phases, pack types, hordes (provisional defaults, FRRWaveDirectorConfig::MakeDefault). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Director")
	FRRWaveDirectorConfig DirectorConfig;

	/** Wave director scheduler (reset in BeginPlay). */
	FRRWaveDirector Director;

	/** Orders of the current frame (reused buffer). */
	TArray<FRRDirectorAction> DirectorActions;

	/** Current horde front, in spawn order (front members and nearest rows first). */
	TArray<FRRHordeSlot> HordePlan;

	/** Entries of HordePlan already handled (spawned or skipped). */
	int32 HordePlanCursor = 0;

	/** Robots of the current horde actually spawned. */
	int32 HordeSpawned = 0;

	/** Horde whose plan HordePlan holds; INDEX_NONE when none. */
	int32 HordePlanIndex = INDEX_NONE;

	/** Side of the current horde front. */
	ERRSpawnSide HordeSide = ERRSpawnSide::None;

	/** Flow field of the world (null outside a game world). */
	URRFlowFieldSubsystem* GetFlowField() const;

	/** Grid the flow field uses; null without one. */
	ARRArenaGrid* GetGrid() const;

	/** Removes dead or destroyed robots from AliveEnemies. */
	void PruneAlive();

	/**
	 * Advances the recycle timer of every alive robot by DeltaSeconds and destroys those far off screen for RecycleDelay seconds
	 * (Destroy only: no death, no XP, no kill). A robot near the miner or within RecycleOffScreenMargin of the view resets its timer.
	 */
	void RecycleFarEnemies(float DeltaSeconds);

	/** Spawns one robot of SpawnClass on Cell (capsule on the floor) and tracks it; null when the spawn fails. */
	ARREnemy* SpawnEnemyOnCell(TSubclassOf<ARREnemy> SpawnClass, const FIntPoint& Cell);

	/** Robot class spawned when WeightedEnemyClasses has no valid entry (a Blueprint subclass of ARRChaser for the mesh). None = no spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	TSubclassOf<ARREnemy> EnemyClass;

	/** Weighted robot classes (default of the placed spawner: chaser 65, kamikaze 12, shooter 15, heavy 8 with MaxAlive 5). Empty or all invalid = EnemyClass. */
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

	/** Maximum number of robots of this spawner alive at once (150, owner 2026-10-05); spawns beyond it are skipped (packs and hordes are clamped). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = 0))
	int32 MaxAlive = 150;

	/** Nearest a spawn cell center lies beyond the visible floor rectangle edge (box distance, uu; owner 2026-10-06, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Band", meta = (ClampMin = 0, Units = "cm"))
	float SpawnBandMinDistance = 100.f;

	/** Farthest a spawn cell center lies beyond the visible floor rectangle edge (box distance, uu; owner 2026-10-06, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Band", meta = (ClampMin = 0, Units = "cm"))
	float SpawnBandMaxDistance = 500.f;

	/**
	 * Fallback search limit (uu beyond the view edge): when the band holds no candidate (rock, arena edge), the nearest band of
	 * the same width beyond it is used, as long as its near edge is within this distance.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Band", meta = (ClampMin = 0, Units = "cm"))
	float SpawnBandFallbackMaxDistance = 3000.f;

	/**
	 * Longest walk accepted for a spawn cell, as a ratio of its straight distance to the miner (flow-field steps x cell size /
	 * straight distance; open floor is about 0.7 to 1). Longer detours (behind rock) are used only when the band has no other
	 * cell, so the first robot arrives quickly. 0 = no check.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Band", meta = (ClampMin = 0))
	float MaxPathDetourRatio = 1.5f;

	/** Seconds of miner movement the run heading is measured over. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Heading", meta = (ClampMin = 0.05, Units = "s"))
	float HeadingWindowSeconds = 1.f;

	/** Average miner speed over the window (uu/s) from which it counts as running; slower = standing still (uniform spawns). Miner walk speed: 600. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Heading", meta = (ClampMin = 0, Units = "CentimetersPerSecond"))
	float HeadingMinSpeed = 150.f;

	/** Relative weight of the front sector while running (55, owner 2026-10-06, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Heading", meta = (ClampMin = 0))
	float FrontSectorWeight = 55.f;

	/** Relative weight of the two side sectors together while running (35, owner 2026-10-06, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Heading", meta = (ClampMin = 0))
	float SideSectorWeight = 35.f;

	/** Relative weight of the back sector while running (10, owner 2026-10-06, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Heading", meta = (ClampMin = 0))
	float BackSectorWeight = 10.f;

	/** Half angle (degrees) of the front sector around the heading (45: NW to NE when running north). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Heading", meta = (ClampMin = 0, ClampMax = 180, Units = "deg"))
	float FrontHalfAngle = 45.f;

	/** Half angle (degrees) of the back sector around the reverse heading (45: SW to SE when running north). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Heading", meta = (ClampMin = 0, ClampMax = 180, Units = "deg"))
	float BackHalfAngle = 45.f;

	/** Recycling on or off: far off-screen robots are removed so the alive cap never starves spawns ahead of a running miner. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Recycling")
	bool bRecycleEnabled = true;

	/** Straight distance (2D, uu) from the miner beyond which an off-screen robot counts down to recycling (2500, owner 2026-10-06, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Recycling", meta = (ClampMin = 0, Units = "cm"))
	float RecycleDistance = 2500.f;

	/** Seconds a robot must stay far and off screen in a row before it is recycled (4, owner 2026-10-06, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Recycling", meta = (ClampMin = 0, Units = "s"))
	float RecycleDelay = 4.f;

	/**
	 * Box distance (uu) beyond the view rectangle under which a robot still counts as on screen for recycling. Equal to
	 * SpawnBandMaxDistance by default, so a fresh robot walking in from the spawn band is never recycled (implementation choice).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner|Recycling", meta = (ClampMin = 0, Units = "cm"))
	float RecycleOffScreenMargin = 500.f;

	/** Timed spawning on or off (also see rr.Spawner.Enabled). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner")
	bool bEnabled = true;

	/** Height above the floor added to the capsule half height at spawn, so the capsule starts just over the floor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (ClampMin = 0, Units = "cm"))
	float SpawnFloorClearance = 2.f;

	/** Robots spawned and still alive (pruned on query). */
	TArray<TWeakObjectPtr<ARREnemy>> AliveEnemies;

	/** Seconds in a row each recyclable robot has been far and off screen (entries removed when it comes back or goes). */
	TMap<TObjectKey<ARREnemy>, float> RecycleTimers;

	/** Robots recycled since BeginPlay. */
	int32 RecycledCount = 0;

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

	/** One miner location sample of the run heading. */
	struct FHeadingSample
	{
		/** World time (s). */
		double Time = 0.0;

		/** Miner location (world XY). */
		FVector2D Location = FVector2D::ZeroVector;
	};

	/** Miner locations of the last HeadingWindowSeconds (oldest first; the first one may be just older than the window). */
	TArray<FHeadingSample> HeadingSamples;

	/** Heading set by SetHeadingOverride (zero = still). */
	FVector2D HeadingOverride = FVector2D::ZeroVector;

	/** True while HeadingOverride replaces the measured heading. */
	bool bHasHeadingOverride = false;
};
