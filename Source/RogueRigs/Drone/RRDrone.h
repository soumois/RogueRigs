// Rogue Rigs - the helper drone: separate actor spawned by the miner; orbits it (dig and hack states declared, not implemented).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRDrone.generated.h"

class ARRMiner;
class UStaticMeshComponent;

/** What the drone is doing. Only Orbit has a behaviour for now; Dig and Hack are added by later tasks. */
UENUM(BlueprintType)
enum class ERRDroneState : uint8
{
	/** Circles the miner in the floor plane (default). */
	Orbit,
	/** Cuts the block the miner stands against (not implemented yet). */
	Dig,
	/** Links to a console or bonus chest inside an objective zone (not implemented yet). */
	Hack
};

/**
 * The miner's helper drone. Spawned and destroyed by ARRMiner (DroneClass), which calls SetOwnerMiner.
 * In Orbit, each tick the orbit angle advances by 2*pi / OrbitPeriod per second around the miner's location,
 * the orbit point is OrbitRadius away in the floor plane at HoverHeight above the miner's floor (capsule
 * bottom), and the drone interpolates toward it with FollowInterpSpeed. The drone has no ASC: it reads
 * MiningSpeed on the miner's ASC. Its root mesh never collides. Subclass in Blueprint for the mesh.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRDrone : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the root mesh (no collision) and enables ticking. */
	ARRDrone();

	/**
	 * Sets the miner the drone follows, makes the drone tick after it and snaps the drone to the start of
	 * its orbit (angle 0). Called by ARRMiner right after spawning the drone. Null stops all movement.
	 */
	UFUNCTION(BlueprintCallable, Category = "Drone")
	void SetOwnerMiner(ARRMiner* NewOwnerMiner);

	/** Miner the drone follows (may be null). */
	UFUNCTION(BlueprintPure, Category = "Drone")
	ARRMiner* GetOwnerMiner() const { return OwnerMiner; }

	/** Current state of the drone. */
	UFUNCTION(BlueprintPure, Category = "Drone")
	ERRDroneState GetState() const { return State; }

	/**
	 * Requests a state change. Only Orbit is implemented: any other state is refused (logged) and the
	 * function returns false. Returns true when the drone is in the requested state afterwards.
	 */
	UFUNCTION(BlueprintCallable, Category = "Drone")
	bool SetState(ERRDroneState NewState);

	/** MiningSpeed attribute of the owner miner's ASC; 1 if there is no miner, no ASC or no MiningSpeed attribute. */
	UFUNCTION(BlueprintPure, Category = "Drone")
	float GetMiningSpeed() const;

	/** Runs the behaviour of the current state (Orbit: moves along the orbit). */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Orbit tick: advances the orbit angle and moves toward the orbit point. */
	void TickOrbit(float DeltaSeconds);

	/** World position of the orbit point for the current angle around the owner miner. Requires a valid miner. */
	FVector ComputeOrbitPoint() const;

	/** Root visual. Collision disabled, so it never blocks or overlaps pawns. Mesh set in Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Horizontal distance from the miner's location to the orbit point, in uu. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm"))
	float OrbitRadius = 150.f;

	/** Time for one full turn around the miner, in seconds. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0.01, Units = "s"))
	float OrbitPeriod = 2.f;

	/** Height of the orbit above the miner's floor (capsule bottom), in uu. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (Units = "cm"))
	float HoverHeight = 150.f;

	/** Speed at which the drone moves toward the orbit point (FMath::VInterpTo speed). 0 = snap to it. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0))
	float FollowInterpSpeed = 10.f;

	/** Miner the drone follows. Set by SetOwnerMiner. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone")
	TObjectPtr<ARRMiner> OwnerMiner;

	/** Current state. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone")
	ERRDroneState State = ERRDroneState::Orbit;

	/** Current orbit angle in radians, in [0, 2*pi). */
	float OrbitAngle = 0.f;
};
