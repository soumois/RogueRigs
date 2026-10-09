// Rogue Rigs - flow field world subsystem: one grid BFS from the miner's cell gives every ground robot its direction,
// repaired incrementally when cells open and refreshed when the miner changes cell, on a worker thread (double buffer) or time-sliced.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlowField/RRPathProvider.h"
#include "FlowField/RRFlowFieldData.h"
#include "Grid/RRArenaGrid.h"
#include "Tasks/Task.h"
#include "RRFlowFieldSubsystem.generated.h"

/**
 * Flow field on the arena grid, implementing IRRPathProvider.
 * BFS from the goal cell (the miner's cell) over walkable cells (Floor, Spawn; blocks and void are not walkable),
 * 8-neighbour: a diagonal step is allowed only when both orthogonal cells it passes are walkable (no corner cutting past
 * blocks). Every step costs 1 cell. Per cell: distance (in steps, INDEX_NONE when unreachable) and the index of the
 * neighbour to move to (lowest distance, ties broken by the neighbour closest to the goal in a straight line, then by
 * neighbour order).
 *
 * Updates (task-029-02). During play cells only open, so distances only decrease and a field a few frames late stays valid
 * (every stored distance is the length of a real path, every direction leads to a strictly closer cell):
 * - full rebuild, synchronous: first computation, grid changed (SetGrid, size), MarkDirty; at most once per frame.
 * - opened cell (grid OnCellDestroyed): queued; Tick marks it walkable and relaxes distances outward from it (decrease-only,
 *   lowest distance first), under the per-frame budget rr.FlowField.BudgetMs (0.3 ms; <= 0 means no budget).
 * - goal changed cell: Tick runs a BFS into a second buffer under the same budget (after the opening work) and swaps it in when
 *   done; cells opened meanwhile are re-relaxed on the new field. Until then queries answer from the field of the previous
 *   goal cell. A goal move during a rebuild is picked up by the next one.
 * Worker thread (task-066-01, rr.FlowField.Async 1, the default): openings and goal rebuilds are computed by a UE::Tasks job
 * over a private copy (FRRFlowFieldJob: walkability with the validated openings applied, plus the distances for an incremental
 * update). The live field stays untouched and readable until the job is complete; the next Tick swaps it in (double buffer).
 * Cells destroyed while a job runs stay queued and go into the next job. One job at a time; the game thread never waits for it
 * (except FlushPendingWork). rr.FlowField.Async 0 keeps the time-sliced game-thread path above (platforms without spare cores).
 * Queries never wait for pending work: they return the last valid field. FlushPendingWork completes it on demand.
 * Grid and goal: set with SetGrid / SetGoalActor, otherwise the first ARRArenaGrid and the first living ARRMiner of the world.
 * Debug: console variable rr.FlowField.Debug 1 draws the directions within rr.FlowField.DebugRadius cells of the goal.
 */
UCLASS()
class ROGUERIGS_API URRFlowFieldSubsystem : public UTickableWorldSubsystem, public IRRPathProvider
{
	GENERATED_BODY()

public:
	/** Refreshes the field when needed, runs the budgeted incremental work and draws it when rr.FlowField.Debug is set. */
	virtual void Tick(float DeltaTime) override;

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

	/** Unbinds from the grid. */
	virtual void Deinitialize() override;

	// IRRPathProvider
	/** Direction from From toward the center of the next cell of its path; zero in the goal cell, off the field or unreachable. */
	virtual FVector GetMoveDirection(const FVector& From) override;
	/** True when From's cell has a path to the goal cell. */
	virtual bool IsReachable(const FVector& From) override;
	/** Path length from From's cell to the goal cell: steps x CellSize (a diagonal step counts one cell); -1 when unreachable. */
	virtual float GetPathDistance(const FVector& From) override;
	/** Location of the goal actor; false without one. */
	virtual bool GetGoalLocation(FVector& OutLocation) override;
	/** Grid walk from From to To: every cell crossed is walkable and no diagonal crossing cuts a block corner. */
	virtual bool HasDirectPath(const FVector& From, const FVector& To) override;

	/** Uses Grid as the terrain (instead of the first grid found) and marks the field dirty. */
	UFUNCTION(BlueprintCallable, Category = "FlowField")
	void SetGrid(ARRArenaGrid* InGrid);

	/** Uses Goal as the target of the field (instead of the first living miner) and marks the field dirty. */
	UFUNCTION(BlueprintCallable, Category = "FlowField")
	void SetGoalActor(AActor* Goal);

	/** The grid the field is built on (resolved on demand); null without one. */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	ARRArenaGrid* GetGrid() const { return Grid.Get(); }

	/** The goal actor (resolved on demand); null without one. */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	AActor* GetGoalActor() const { return GoalActor.Get(); }

	/**
	 * Runs a synchronous full rebuild when there is no valid field, the grid changed or MarkDirty was called (at most once per
	 * frame). Openings and goal moves are left to the budgeted work of Tick (see FlushPendingWork).
	 */
	UFUNCTION(BlueprintCallable, Category = "FlowField")
	void UpdateField();

	/** Forces a synchronous full rebuild at the next update (fallback, e.g. after a terrain change the grid does not broadcast). */
	UFUNCTION(BlueprintCallable, Category = "FlowField")
	void MarkDirty() { bDirty = true; }

	/** Completes now, without budget, every queued opening and the goal rebuild (starting one if the goal changed cell). */
	UFUNCTION(BlueprintCallable, Category = "FlowField")
	void FlushPendingWork();

	/** True while openings, a relaxation, a goal rebuild or a worker job are pending, or the goal is in another cell than the field's. */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	bool IsUpdatePending() const;

	/** BFS steps from Cell to the goal cell (0 at the goal); INDEX_NONE when unreachable or outside the field. Updates first. */
	UFUNCTION(BlueprintCallable, Category = "FlowField")
	int32 GetCellDistance(FIntPoint Cell);

	/** Next cell on the path from Cell; (INDEX_NONE, INDEX_NONE) at the goal, when unreachable or outside the field. Updates first. */
	UFUNCTION(BlueprintCallable, Category = "FlowField")
	FIntPoint GetNextCell(FIntPoint Cell);

	/** Cell the current field leads to; (INDEX_NONE, INDEX_NONE) before the first computation. Lags the goal while a rebuild runs. */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	FIntPoint GetGoalCell() const { return Field.GoalCell; }

	/** True when a robot can stand on Cell (Floor or Spawn of the current grid). */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	bool IsWalkableCell(FIntPoint Cell) const;

	/**
	 * Reachable-cell query shared by every placement search (console, shuttle, spawner, stress, shooter reposition). Updates
	 * the field once, then scans the grid row by row (Y outer, X inner, so seeded callers keep drawing the same cells) and
	 * appends to OutCells (not reset) every cell that is walkable (Floor or Spawn; Floor only when bFloorOnly), has a path to
	 * the goal, is not in ExcludedCells (optional) and passes Filter(Cell, Steps), Steps being its BFS distance to the goal.
	 * Filter runs only on cells that passed the other checks, in scan order. Nothing is added without a grid or a goal.
	 * While work is pending, cells opened in the last frames may be missing and Steps may be a little long: never wrong.
	 */
	void GatherReachableCells(TArray<FIntPoint>& OutCells, TFunctionRef<bool(FIntPoint Cell, int32 Steps)> Filter,
		bool bFloorOnly = false, const TSet<FIntPoint>* ExcludedCells = nullptr);

	/** Adds the cell of every valid pawn of the world (robots, miner) to OutCells, on the current grid; spawn searches exclude them. */
	void GatherPawnCells(TSet<FIntPoint>& OutCells) const;

	/** True when openings and goal rebuilds run on a worker thread (rr.FlowField.Async), false for the time-sliced path. */
	static bool IsAsyncEnabled();

	/** Duration of the last complete field computation in ms: a synchronous rebuild, the summed slices or the worker time of a goal rebuild. */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	float GetLastComputeMilliseconds() const { return LastComputeMilliseconds; }

	/** Complete field computations since the subsystem started (synchronous rebuilds and finished goal rebuilds). */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	int32 GetComputeCount() const { return ComputeCount; }

	/** Opened cells applied incrementally since the subsystem started (each one without a full rebuild). */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	int32 GetOpenedCellCount() const { return OpenedCellCount; }

	/** Game-thread flow-field time (rebuilds, incremental work, or the swap and launch of worker jobs) of the last frame that did any, in ms. */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	float GetLastFrameWorkMilliseconds() const { return FrameWorkMilliseconds; }

	/** Frames that did flow-field work since the subsystem started; samplers compare it to detect a new value of GetLastFrameWorkMilliseconds. */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	int32 GetWorkFrameCount() const { return WorkFrameCount; }

protected:
	/** Finds the grid and the goal when not set or no longer valid; binds OnCellDestroyed. False when either is missing. */
	bool ResolveGridAndGoal();

	/** Synchronous full rebuild from GoalCell over a field of Size cells: walkability from the grid, distances, directions. Drops pending work. */
	void ComputeField(FIntPoint GoalCell, FIntPoint Size);

	/** Budgeted work of a frame: queued openings, relaxation, then the goal rebuild. DeadlineSeconds <= 0: run to completion. */
	void ProcessPendingWork(double DeadlineSeconds);

	/** Marks the queued opened cells walkable and seeds the relaxation around them (time-sliced path). */
	void ApplyPendingOpenings();

	/** Worker path of a frame: applies a completed job, then launches the next one when work is queued. True when it did either. */
	bool ProcessAsyncWork();

	/** Swaps the completed job's field into the live field (dropped when outdated) and keeps the job as the spare buffer. */
	void ApplyCompletedJob();

	/** Fills a job from the live field and the queued openings and launches it on a worker. True when launched or openings consumed. */
	bool LaunchJob();

	/** True when Cell is inside the live field. */
	bool IsInField(FIntPoint Cell) const { return Field.IsInField(Cell); }

	/** Row-major index of a cell of the live field. */
	int32 GetFieldIndex(FIntPoint Cell) const { return Field.GetIndex(Cell); }

	/** Adds Milliseconds to the work total of the current frame (resets it on a new frame). */
	void AddFrameWork(double Milliseconds);

	/** Queues the opened cell for the incremental update. */
	UFUNCTION()
	void HandleCellDestroyed(FIntPoint Cell, ERRCellType Type, ERRTerrainDamageSource Source);

	/** Draws the direction arrows around the goal (rr.FlowField.Debug). */
	void DrawDebugField() const;

	/** Grid the field is built on. */
	TWeakObjectPtr<ARRArenaGrid> Grid;

	/** Grid whose OnCellDestroyed is bound to HandleCellDestroyed. */
	TWeakObjectPtr<ARRArenaGrid> BoundGrid;

	/** Actor the field leads to. */
	TWeakObjectPtr<AActor> GoalActor;

	/** True when the goal was set explicitly (SetGoalActor): no automatic miner lookup. */
	bool bExplicitGoal = false;

	/** Live field read by every query (walkability, distances, directions, goal cell, relaxation heap); only the game thread touches it. */
	FRRFlowFieldData Field;

	/** Opened cells received from the grid, not applied yet. */
	TArray<FIntPoint> PendingOpenings;

	/** Worker job in flight (null when none); only the worker touches it until JobTask is complete. */
	TSharedPtr<FRRFlowFieldJob, ESPMode::ThreadSafe> Job;

	/** Completed job kept as the buffer of the next one (its arrays hold the previous live field, so copies reuse the allocations). */
	TSharedPtr<FRRFlowFieldJob, ESPMode::ThreadSafe> SpareJob;

	/** Task running Job. */
	UE::Tasks::FTask JobTask;

	/** Incremented by every synchronous full rebuild: a job launched before it is dropped on completion. */
	uint32 JobGeneration = 0;

	/** True while a time-sliced goal rebuild runs over several frames. */
	bool bGoalRebuildActive = false;

	/** Goal cell of the running goal rebuild. */
	FIntPoint RebuildGoalCell = FIntPoint(INDEX_NONE, INDEX_NONE);

	/** Distances of the running goal rebuild (swapped into Distances when done). */
	TArray<int32> RebuildDistances;

	/** Directions of the running goal rebuild (swapped into Directions when done). */
	TArray<uint8> RebuildDirections;

	/** Breadth-first queue of the running goal rebuild. */
	TArray<int32> RebuildQueue;

	/** Next queue entry to expand in the running goal rebuild. */
	int32 RebuildHead = 0;

	/** Next queue entry whose direction the running goal rebuild computes. */
	int32 RebuildDirectionHead = 0;

	/** Cells opened while the goal rebuild runs: re-relaxed on the new field when it is swapped in. */
	TArray<FIntPoint> OpenedDuringRebuild;

	/** Time spent so far in the slices of the running goal rebuild, in ms. */
	double RebuildMilliseconds = 0.0;

	/** Set by grid or goal-actor changes and MarkDirty: forces a synchronous full rebuild at the next update. */
	bool bDirty = true;

	/** GFrameCounter of the last full rebuild; limits full rebuilds to one per frame. */
	uint64 LastComputeFrame = MAX_uint64;

	/** GFrameCounter of the last budgeted work pass (one per frame). */
	uint64 LastWorkFrame = MAX_uint64;

	/** GFrameCounter of FrameWorkMilliseconds. */
	uint64 FrameWorkFrame = MAX_uint64;

	/** Duration of the last complete computation in ms. */
	float LastComputeMilliseconds = 0.f;

	/** Flow-field time of the last frame with work, in ms. */
	float FrameWorkMilliseconds = 0.f;

	/** Complete computations done so far. */
	int32 ComputeCount = 0;

	/** Opened cells applied incrementally so far. */
	int32 OpenedCellCount = 0;

	/** Frames with flow-field work so far. */
	int32 WorkFrameCount = 0;
};
