// Rogue Rigs - pickup actor dropped by ore and heal ore blocks: pops out, then flies to the miner within its pickup radius.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRPickup.generated.h"

class ARRMiner;
class UStaticMeshComponent;

/** Kind of a pickup; tells the miner what was collected. */
UENUM(BlueprintType)
enum class ERRPickupType : uint8
{
	/** Ore currency, dropped by ore blocks. */
	Ore,
	/** Heal, dropped by heal ore blocks. */
	Heal
};

/**
 * A pickup lying on the arena floor. On spawn it pops toward a random horizontal offset (within PopMaxOffset of
 * the spawn point, i.e. inside the cell) along a short arc, then rests HoverHeight above the spawn Z. Once the pop
 * is over, each tick it measures its horizontal distance to the player miner; when that distance is within the
 * miner's PickupRadius attribute it flies to the miner at FlySpeed (and keeps flying even if the miner moves away)
 * and is collected within CollectDistance: logs its type on LogRogueRigs, calls ARRMiner::NotifyPickupCollected,
 * destroys itself. No collision at all (pawns and projectiles go through). No heal or ore effect yet.
 * Spawned by ARRArenaGrid at a cell center (floor top). Subclass in Blueprint: set PickupType and the mesh.
 */
UCLASS(Abstract, Blueprintable)
class ROGUERIGS_API ARRPickup : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the mesh root without collision and enables tick. */
	ARRPickup();

	/** Runs the pop, then the attraction and collection check. */
	virtual void Tick(float DeltaSeconds) override;

	/** What this pickup gives when collected. */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	ERRPickupType GetPickupType() const { return PickupType; }

protected:
	/** Picks the random pop target and stores the pop start point. */
	virtual void BeginPlay() override;

	/** Advances the pop arc; returns true once the pop is finished. */
	bool UpdatePop(float DeltaSeconds);

	/** Returns the player miner (player 0 pawn), cached once found; null when the player pawn is not a miner. */
	ARRMiner* FindMiner();

	/** Moves toward the miner at FlySpeed; collects when within CollectDistance. */
	void FlyToMiner(ARRMiner* Miner, float DeltaSeconds);

	/** Logs the type, notifies the miner and destroys this pickup. */
	void Collect(ARRMiner* Miner);

	/** Visual root. Collision disabled: pickups find the miner by distance. Assign the mesh in the Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	/** What this pickup gives when collected (set in each Blueprint subclass). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	ERRPickupType PickupType = ERRPickupType::Ore;

	/** Maximum horizontal offset (per axis) of the pop target from the spawn point; keep under half a cell to stay in the cell. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (ClampMin = 0, Units = "cm"))
	float PopMaxOffset = 80.f;

	/** Duration of the pop arc in seconds (0 = appear directly at the target). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (ClampMin = 0, Units = "s"))
	float PopDuration = 0.35f;

	/** Peak height of the pop arc above the rest height. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (ClampMin = 0, Units = "cm"))
	float PopArcHeight = 80.f;

	/** Rest height above the spawn point (the floor top when spawned by the grid). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (Units = "cm"))
	float HoverHeight = 30.f;

	/** Flight speed toward the miner once attracted; keep above the miner's move speed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Collect", meta = (ClampMin = 1, Units = "CentimetersPerSecond"))
	float FlySpeed = 1500.f;

	/** Distance to the miner's location at which a flying pickup is collected. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Collect", meta = (ClampMin = 0, Units = "cm"))
	float CollectDistance = 50.f;

	/** Miner this pickup checks and flies to; found from the player 0 pawn. */
	UPROPERTY(Transient)
	TObjectPtr<ARRMiner> TargetMiner;

	/** World location at spawn: the pop starts here. */
	FVector PopStart = FVector::ZeroVector;

	/** World rest location at the end of the pop. */
	FVector PopEnd = FVector::ZeroVector;

	/** Seconds elapsed in the pop. */
	float PopElapsed = 0.f;

	/** True once the pop has finished. */
	bool bPopDone = false;

	/** True once the miner came within its pickup radius: the pickup flies until collected. */
	bool bAttracted = false;

	/** True once collected, so it is never collected twice. */
	bool bCollected = false;
};
