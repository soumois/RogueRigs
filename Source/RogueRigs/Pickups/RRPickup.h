// Rogue Rigs - pickup actor dropped by ore and heal ore blocks and by dying robots: pops out in an arc, rests, then is magnetized into the miner within its pickup radius.

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
	Heal,
	/** Experience cube, dropped by robots on death (sys-enemies). Adds XPAmount XP to the miner progression on collection. */
	XP
};

/** Motion phase of a pickup (drives the Blueprint visuals: trail while magnetized). */
UENUM(BlueprintType)
enum class ERRPickupPhase : uint8
{
	/** Ejection arc with a small bounce (PopDuration). */
	Pop,
	/** Lying at its rest point; attraction allowed once AttractDelay has elapsed. */
	Rest,
	/** Accelerating toward the miner until collected. */
	Magnet,
	/** Collected: destroyed this frame. */
	Collected
};

/**
 * A pickup lying on the arena floor (xp-level-up: DRGS-like ejection then suction). On spawn it is ejected along a random
 * horizontal direction (or the one given by SetPopYaw) to a distance in [PopMinDistance, PopMaxDistance]: an arc of
 * PopArcHeight then a small bounce of PopBounceHeight, over PopDuration in total, ending HoverHeight above the spawn Z.
 * It then rests; once AttractDelay has elapsed after the pop, as soon as its horizontal distance to the player miner is
 * within the miner's PickupRadius attribute it latches the magnet: it flies straight at the miner, starting at
 * MagnetStartSpeed and accelerating by MagnetAcceleration up to MagnetMaxSpeed (nearby cubes form a stream), even if the
 * miner moves away. Within CollectDistance it is collected: logs its type (Verbose), an XP pickup adds XPAmount to the
 * miner progression (scaled by XPGain there), ARRMiner::NotifyPickupCollected broadcasts the type and the location, then
 * it destroys itself. No physics, no collision; movement is driven by URRPickupSubsystem in one loop per frame (actor tick
 * only as a fallback when the subsystem is missing). Spawned by ARRArenaGrid at a cell center (floor top), and by ARREnemy
 * at its feet on death (XP, split over several cubes). Subclass in Blueprint: set PickupType and the mesh.
 */
UCLASS(Abstract, Blueprintable)
class ROGUERIGS_API ARRPickup : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the mesh root without collision; actor tick disabled unless the pickup subsystem is missing. */
	ARRPickup();

	/** Fallback only (no URRPickupSubsystem): runs UpdateMotion with the player miner. */
	virtual void Tick(float DeltaSeconds) override;

	/**
	 * Advances the pickup by DeltaSeconds: pop, rest, magnet and collection. Miner may be null (no attraction).
	 * PickupRadiusSquared: the miner's PickupRadius squared (read once per frame by the caller). Called by URRPickupSubsystem.
	 */
	void UpdateMotion(float DeltaSeconds, ARRMiner* Miner, float PickupRadiusSquared);

	/** What this pickup gives when collected. */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	ERRPickupType GetPickupType() const { return PickupType; }

	/** Current motion phase. */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	ERRPickupPhase GetPhase() const { return Phase; }

	/** True while flying to the miner (Magnet phase): show the trail. */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool IsMagnetized() const { return Phase == ERRPickupPhase::Magnet; }

	/** Current magnet speed in uu/s (0 before the magnet latches). */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	float GetMagnetSpeed() const { return MagnetSpeed; }

	/** Raw XP given on collection by an XP pickup. */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	float GetXPAmount() const { return XPAmount; }

	/** Sets the raw XP given on collection (ARREnemy splits a robot's XP over its cubes). Call before collection, ideally before BeginPlay (deferred spawn). */
	void SetXPAmount(float InXPAmount) { XPAmount = FMath::Max(InXPAmount, 0.f); }

	/** Forces the ejection direction (world yaw in degrees) instead of a random one. Call before BeginPlay (deferred spawn); ARREnemy spreads its cubes evenly. */
	void SetPopYaw(float InYawDegrees) { PopYawDegrees = InYawDegrees; bHasPopYaw = true; }

	/** World location where the pop started (spawn point). */
	const FVector& GetPopStart() const { return PopStart; }

	/** World rest location at the end of the pop. */
	const FVector& GetPopEnd() const { return PopEnd; }

protected:
	/** Picks the pop target, stores the pop start point and registers with URRPickupSubsystem (else enables the actor tick). */
	virtual void BeginPlay() override;

	/** Unregisters from URRPickupSubsystem. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Advances the pop arc and bounce; returns true once the pop is finished. */
	bool UpdatePop(float DeltaSeconds);

	/** Returns the player miner (player 0 pawn), cached once found; null when the player pawn is not a miner. Fallback tick only. */
	ARRMiner* FindMiner();

	/** Accelerates toward the miner; collects when within CollectDistance or when the step reaches it. */
	void FlyToMiner(ARRMiner* Miner, float DeltaSeconds);

	/** Logs the type, adds XPAmount XP to the miner for an XP pickup, notifies the miner (type, location) and destroys this pickup. */
	void Collect(ARRMiner* Miner);

	/** Called once when the magnet latches (start the trail). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnMagnetStart();

	/** Called once on the collection frame, at the collection location, just before the pickup is destroyed (spawn a detached sparkle here, or bind ARRMiner::OnPickupCollectedAt). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnCollected(FVector Location);

	/** Visual root. Collision disabled: pickups find the miner by distance. Assign the mesh in the Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	/** What this pickup gives when collected (set in each Blueprint subclass). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	ERRPickupType PickupType = ERRPickupType::Ore;

	/** Raw XP given by an XP pickup on collection (before the miner XPGain); set per cube by ARREnemy. Ignored by other types. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = 0))
	float XPAmount = 1.f;

	/** Shortest horizontal ejection distance from the spawn point (task-027-01: 80 uu). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (ClampMin = 0, Units = "cm"))
	float PopMinDistance = 80.f;

	/** Longest horizontal ejection distance from the spawn point (task-027-01: 220 uu). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (ClampMin = 0, Units = "cm"))
	float PopMaxDistance = 220.f;

	/** Duration of the whole pop (arc + bounce) in seconds (0 = appear directly at the target). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (ClampMin = 0, Units = "s"))
	float PopDuration = 0.35f;

	/** Peak height of the main arc above the straight line from the spawn point to the rest point. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (ClampMin = 0, Units = "cm"))
	float PopArcHeight = 80.f;

	/** Peak height of the small bounce after the main arc lands. 0 = no bounce. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (ClampMin = 0, Units = "cm"))
	float PopBounceHeight = 15.f;

	/** Fraction of PopDuration spent on the bounce (the main arc takes the rest). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (ClampMin = 0, ClampMax = 0.9))
	float PopBounceFraction = 0.25f;

	/** Rest height above the spawn point (the floor top when spawned by the grid, the robot feet for XP). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (Units = "cm"))
	float HoverHeight = 30.f;

	/** Seconds the pickup rests after the pop ends before it may be attracted to the miner. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Collect", meta = (ClampMin = 0, Units = "s"))
	float AttractDelay = 1.f;

	/** Speed toward the miner when the magnet latches (task-027-01: 400 uu/s). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Collect", meta = (ClampMin = 1, Units = "CentimetersPerSecond"))
	float MagnetStartSpeed = 400.f;

	/** Acceleration toward the miner while magnetized (task-027-01: 4000 uu/s2). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Collect", meta = (ClampMin = 0))
	float MagnetAcceleration = 4000.f;

	/** Top magnet speed; keep well above the miner's move speed (task-027-01: 2500 uu/s). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Collect", meta = (ClampMin = 1, Units = "CentimetersPerSecond"))
	float MagnetMaxSpeed = 2500.f;

	/** Distance to the miner's location at which a flying pickup is collected. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Collect", meta = (ClampMin = 0, Units = "cm"))
	float CollectDistance = 50.f;

	/** Miner this pickup flies to (latched with the magnet), or found from the player 0 pawn by the fallback tick. */
	UPROPERTY(Transient)
	TObjectPtr<ARRMiner> TargetMiner;

	/** Current phase. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup")
	ERRPickupPhase Phase = ERRPickupPhase::Pop;

	/** World location at spawn: the pop starts here. */
	FVector PopStart = FVector::ZeroVector;

	/** World rest location at the end of the pop. */
	FVector PopEnd = FVector::ZeroVector;

	/** Seconds elapsed in the pop. */
	float PopElapsed = 0.f;

	/** Seconds elapsed since the pop finished; attraction is allowed once it reaches AttractDelay. */
	float RestElapsed = 0.f;

	/** Current speed toward the miner (Magnet phase). */
	float MagnetSpeed = 0.f;

	/** Ejection yaw in degrees, used when bHasPopYaw. */
	float PopYawDegrees = 0.f;

	/** True when SetPopYaw chose the ejection direction. */
	bool bHasPopYaw = false;
};
