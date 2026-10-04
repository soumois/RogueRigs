// Rogue Rigs - flow field world subsystem: one grid BFS from the miner's cell gives every ground robot its direction.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlowField/RRPathProvider.h"
#include "Grid/RRArenaGrid.h"
#include "RRFlowFieldSubsystem.generated.h"

/**
 * Flow field on the arena grid, implementing IRRPathProvider.
 * BFS from the goal cell (the miner's cell) over walkable cells (Floor, Spawn; blocks and void are not walkable),
 * 8-neighbour: a diagonal step is allowed only when both orthogonal cells it passes are walkable (no corner cutting past
 * blocks). Every step costs 1 cell. Per cell: distance (in steps, INDEX_NONE when unreachable) and the index of the
 * neighbour to move to (lowest distance, ties broken by the neighbour closest to the goal in a straight line).
 * Recomputed when the goal enters a new cell, when the grid broadcasts OnCellDestroyed, or when the grid changes, at most
 * once per frame (GFrameCounter): the check runs in Tick and lazily in every query.
 * Grid and goal: set with SetGrid / SetGoalActor, otherwise the first ARRArenaGrid and the first living ARRMiner of the world.
 * Debug: console variable rr.FlowField.Debug 1 draws the directions within rr.FlowField.DebugRadius cells of the goal.
 */
UCLASS()
class ROGUERIGS_API URRFlowFieldSubsystem : public UTickableWorldSubsystem, public IRRPathProvider
{
	GENERATED_BODY()

public:
	/** Refreshes the field when needed and draws it when rr.FlowField.Debug is set. */
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

	/** Recomputes the field if the goal changed cell, the terrain changed or the grid changed; at most once per frame. */
	UFUNCTION(BlueprintCallable, Category = "FlowField")
	void UpdateField();

	/** Forces a recompute at the next update (e.g. after a terrain change the grid does not broadcast). */
	UFUNCTION(BlueprintCallable, Category = "FlowField")
	void MarkDirty() { bDirty = true; }

	/** BFS steps from Cell to the goal cell (0 at the goal); INDEX_NONE when unreachable or outside the field. Updates first. */
	UFUNCTION(BlueprintCallable, Category = "FlowField")
	int32 GetCellDistance(FIntPoint Cell);

	/** Next cell on the path from Cell; (INDEX_NONE, INDEX_NONE) at the goal, when unreachable or outside the field. Updates first. */
	UFUNCTION(BlueprintCallable, Category = "FlowField")
	FIntPoint GetNextCell(FIntPoint Cell);

	/** Cell the field was last computed from; (INDEX_NONE, INDEX_NONE) before the first computation. */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	FIntPoint GetGoalCell() const { return FieldGoalCell; }

	/** True when a robot can stand on Cell (Floor or Spawn of the current grid). */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	bool IsWalkableCell(FIntPoint Cell) const;

	/**
	 * Reachable-cell query shared by every placement search (console, shuttle, spawner, stress, shooter reposition). Updates
	 * the field once, then scans the grid row by row (Y outer, X inner, so seeded callers keep drawing the same cells) and
	 * appends to OutCells (not reset) every cell that is walkable (Floor or Spawn; Floor only when bFloorOnly), has a path to
	 * the goal, is not in ExcludedCells (optional) and passes Filter(Cell, Steps), Steps being its BFS distance to the goal.
	 * Filter runs only on cells that passed the other checks, in scan order. Nothing is added without a grid or a goal.
	 */
	void GatherReachableCells(TArray<FIntPoint>& OutCells, TFunctionRef<bool(FIntPoint Cell, int32 Steps)> Filter,
		bool bFloorOnly = false, const TSet<FIntPoint>* ExcludedCells = nullptr);

	/** Adds the cell of every valid pawn of the world (robots, miner) to OutCells, on the current grid; spawn searches exclude them. */
	void GatherPawnCells(TSet<FIntPoint>& OutCells) const;

	/** Duration of the last BFS (distances and directions) in milliseconds. */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	float GetLastComputeMilliseconds() const { return LastComputeMilliseconds; }

	/** Number of computations since the subsystem started (tests use it to check the recompute triggers). */
	UFUNCTION(BlueprintPure, Category = "FlowField")
	int32 GetComputeCount() const { return ComputeCount; }

protected:
	/** Value of Directions for a cell without a next cell. */
	static constexpr uint8 NoDirection = 0xFF;

	/** Finds the grid and the goal when not set or no longer valid; binds OnCellDestroyed. False when either is missing. */
	bool ResolveGridAndGoal();

	/** Runs the BFS from GoalCell over a field of Size cells and fills Distances and Directions. */
	void ComputeField(FIntPoint GoalCell, FIntPoint Size);

	/** True when the step from Cell along neighbour offset Index stays on walkable cells without cutting a corner. Uses Walkable. */
	bool CanStep(FIntPoint Cell, int32 Index) const;

	/** True when Cell is inside the current field. */
	bool IsInField(FIntPoint Cell) const { return Cell.X >= 0 && Cell.Y >= 0 && Cell.X < FieldSize.X && Cell.Y < FieldSize.Y; }

	/** Row-major index of a cell of the field. */
	int32 GetFieldIndex(FIntPoint Cell) const { return Cell.Y * FieldSize.X + Cell.X; }

	/** Marks the field dirty when the grid destroys a block. */
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

	/** Walkable flag per cell, row-major, rebuilt with each computation. */
	TArray<bool> Walkable;

	/** BFS steps to the goal per cell, row-major; INDEX_NONE when unreachable. */
	TArray<int32> Distances;

	/** Neighbour offset index (into the 8-neighbour table) to move to per cell, row-major; NoDirection when none. */
	TArray<uint8> Directions;

	/** Dimensions of the computed field (grid size at computation). */
	FIntPoint FieldSize = FIntPoint::ZeroValue;

	/** Goal cell of the computed field. */
	FIntPoint FieldGoalCell = FIntPoint(INDEX_NONE, INDEX_NONE);

	/** Set by terrain changes, grid or goal changes: forces a computation at the next update. */
	bool bDirty = true;

	/** GFrameCounter of the last computation; limits computations to one per frame. */
	uint64 LastComputeFrame = MAX_uint64;

	/** Duration of the last computation in ms. */
	float LastComputeMilliseconds = 0.f;

	/** Computations done so far. */
	int32 ComputeCount = 0;
};
