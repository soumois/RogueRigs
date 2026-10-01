// Rogue Rigs - test-only listener: records dynamic delegate broadcasts (pickup collected, cell destroyed) for the automation tests.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Grid/RRArenaGrid.h"
#include "Pickups/RRPickup.h"
#include "RRTestEventRecorder.generated.h"

/**
 * Records the broadcasts of ARRMiner::OnPickupCollected and ARRArenaGrid::OnCellDestroyed so automation tests
 * (Source/RogueRigs/Tests) can check that the events fire. Dynamic delegates need a UFunction to bind to, so this
 * is a UCLASS and is compiled in every configuration (UHT does not support WITH_DEV_AUTOMATION_TESTS guards);
 * it is never used by gameplay code. Create with NewObject, keep it alive with a TStrongObjectPtr.
 */
UCLASS(Transient, NotBlueprintable, NotBlueprintType, HideDropdown)
class ROGUERIGS_API URRTestEventRecorder : public UObject
{
	GENERATED_BODY()

public:
	/** Bind to ARRMiner::OnPickupCollected: appends the collected pickup type. */
	UFUNCTION()
	void HandlePickupCollected(ERRPickupType Type) { CollectedPickups.Add(Type); }

	/** Bind to ARRArenaGrid::OnCellDestroyed: appends the destroyed cell. */
	UFUNCTION()
	void HandleCellDestroyed(FIntPoint Cell, ERRCellType Type, ERRTerrainDamageSource Source) { DestroyedCells.Add(Cell); }

	/** Pickup types received through HandlePickupCollected, in order. */
	const TArray<ERRPickupType>& GetCollectedPickups() const { return CollectedPickups; }

	/** Cells received through HandleCellDestroyed, in order. */
	const TArray<FIntPoint>& GetDestroyedCells() const { return DestroyedCells; }

protected:
	/** Pickup types collected so far. */
	TArray<ERRPickupType> CollectedPickups;

	/** Cells destroyed so far. */
	TArray<FIntPoint> DestroyedCells;
};
