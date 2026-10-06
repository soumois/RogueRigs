// Rogue Rigs - the helper drone: separate actor spawned by the miner; orbits it, cuts blocks with a laser (dig) and links to an objective (hack).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRDrone.generated.h"

class ARRArenaGrid;
class ARRMiner;
class UStaticMeshComponent;

/** What the drone is doing. */
UENUM(BlueprintType)
enum class ERRDroneState : uint8
{
	/** Circles the miner in the floor plane (default). */
	Orbit,
	/** Hovers beside the block the miner pushed into and cuts it with the laser. */
	Dig,
	/** Links to a console (or later a bonus chest) inside its objective zone: StartHack / StopHack, driven by the objective. */
	Hack
};

/**
 * The miner's helper drone. Spawned and destroyed by ARRMiner (DroneClass), which calls SetOwnerMiner.
 * Orbit: each tick the orbit angle advances by 2*pi / OrbitPeriod per second around the miner's location,
 * the orbit point is OrbitRadius away in the floor plane at HoverHeight above the miner's floor (capsule
 * bottom), and the drone interpolates toward it with FollowInterpSpeed.
 * Dig (started by RequestDig, from the miner): the drone flies to a hover point DigHoverDistance off the
 * target block's face on the miner's side (same HoverHeight), applies DeltaSeconds * MiningSpeed dig-seconds
 * of terrain damage to the block each tick, keeps it shaking, and shows LaserMesh from itself to the face
 * center. It returns to Orbit when the block is gone or the miner is farther than DigCancelDistanceCells
 * cells from the block center; damage done stays on the block.
 * Hack (StartHack, called every tick by the objective while the miner is in its zone): the drone stops any dig, hovers
 * HackHoverDistance from the target toward the miner (same HoverHeight) and shows LaserMesh from itself to the target's
 * location + HackLinkOffset. One target at a time; RequestDig is refused while hacking. StopHack (or the target or miner
 * gone) returns it to Orbit. The objective owns the hack progress.
 * The drone has no ASC: it reads MiningSpeed on the miner's ASC. Its meshes never collide.
 * Subclass in Blueprint for the drone mesh and the laser mesh and material.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRDrone : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the root mesh and the hidden laser mesh (no collision) and enables ticking. */
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
	 * Requests a state change. Orbit is always accepted (stops any dig). Dig is accepted only when a dig
	 * target is set and still holds a block (use RequestDig to set one). Hack is refused here: use StartHack.
	 * Refusals are logged. Returns true when the drone is in the requested state afterwards.
	 */
	UFUNCTION(BlueprintCallable, Category = "Drone")
	bool SetState(ERRDroneState NewState);

	/**
	 * Asks the drone to cut the block in Cell of Grid. MinerSide is the unit grid step from the block toward
	 * the miner (the face the laser hits and the side the drone hovers on). Called every tick by the miner
	 * while it pushes into the block: the same cell only refreshes MinerSide; another cell switches the target
	 * (the old block stops shaking, keeps its damage). Returns false when Cell holds no block, there is no miner, or the
	 * drone is hacking (it cannot dig and hack at once).
	 */
	UFUNCTION(BlueprintCallable, Category = "Drone|Dig")
	bool RequestDig(ARRArenaGrid* Grid, FIntPoint Cell, FIntPoint MinerSide);

	/**
	 * Links the drone to Target (an objective actor) and enters Hack: stops a dig in progress. Called every tick by the
	 * objective while the miner is inside its zone; the same target again just returns true. Refused (false) without a miner
	 * or target, or while hacking another target (one target at a time).
	 */
	UFUNCTION(BlueprintCallable, Category = "Drone|Hack")
	bool StartHack(AActor* Target);

	/** Leaves Hack if Target is the current hack target (the miner left the zone or the hack completed): back to Orbit. */
	UFUNCTION(BlueprintCallable, Category = "Drone|Hack")
	void StopHack(AActor* Target);

	/** Objective the drone is hacking; null outside Hack. */
	UFUNCTION(BlueprintPure, Category = "Drone|Hack")
	AActor* GetHackTarget() const { return HackTarget; }

	/** Cell being cut; meaningful only while GetState() is Dig. */
	UFUNCTION(BlueprintPure, Category = "Drone|Dig")
	FIntPoint GetDigCell() const { return DigCell; }

	/** MiningSpeed attribute of the owner miner's ASC; 1 if there is no miner, no ASC or no MiningSpeed attribute. */
	UFUNCTION(BlueprintPure, Category = "Drone")
	float GetMiningSpeed() const;

	/** Runs the behaviour of the current state (Orbit: moves along the orbit; Dig: hovers, cuts, draws the laser; Hack: hovers, draws the link). */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Stops a dig in progress so the target block does not keep shaking after the drone is gone. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Orbit tick: advances the orbit angle and moves toward the orbit point. */
	void TickOrbit(float DeltaSeconds);

	/** Dig tick: checks the cancel rules, moves to the hover point, damages the block and updates the laser. */
	void TickDig(float DeltaSeconds);

	/** Hack tick: back to Orbit when the target or miner is gone, else moves to the hack hover point and draws the link. */
	void TickHack(float DeltaSeconds);

	/** Leaves Hack: hides the laser, clears the target and resumes the orbit from the drone's current angle. */
	void EndHack();

	/** Sets the orbit angle to the drone's current angle around the miner, so the orbit resumes where the drone is. */
	void ResumeOrbitFromCurrentAngle();

	/** World position of the orbit point for the current angle around the owner miner. Requires a valid miner. */
	FVector ComputeOrbitPoint() const;

	/** World position the drone hovers at while digging. Requires a valid miner and dig grid. */
	FVector ComputeDigHoverPoint() const;

	/** Leaves Dig: stops the block shaking, hides the laser, clears the target and resumes the orbit from the drone's current angle. */
	void StopDig();

	/** Places LaserMesh as a cylinder from Start to End with LaserDiameter, and shows it. */
	void UpdateLaser(const FVector& Start, const FVector& End);

	/** Root visual. Collision disabled, so it never blocks or overlaps pawns. Mesh set in Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/**
	 * Cutting laser shown only in Dig. Expected mesh: a cylinder along local Z with its pivot at the center
	 * (the engine cylinder); size given by LaserMeshBaseHeight and LaserMeshBaseDiameter. Mesh and material
	 * set in Blueprint. Uses an absolute transform, set every Dig tick. No collision, no shadow.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LaserMesh;

	/** Horizontal distance from the miner's location to the orbit point, in uu. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm"))
	float OrbitRadius = 150.f;

	/** Time for one full turn around the miner, in seconds. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0.01, Units = "s"))
	float OrbitPeriod = 2.f;

	/** Height of the drone above the miner's floor (capsule bottom), in uu. Used in Orbit and Dig. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (Units = "cm"))
	float HoverHeight = 150.f;

	/** Speed at which the drone moves toward its orbit or dig hover point (FMath::VInterpTo speed). 0 = snap to it. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0))
	float FollowInterpSpeed = 10.f;

	/** Horizontal distance of the dig hover point from the cut face, on the miner's side, in uu (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig", meta = (ClampMin = 0, Units = "cm"))
	float DigHoverDistance = 60.f;

	/** The dig stops when the miner's horizontal distance to the block center exceeds this many cells (provisional, drone spec; 4.5 cells of 67 uu = the former 1.5 cells of 200 uu). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig", meta = (ClampMin = 0))
	float DigCancelDistanceCells = 4.5f;

	/** Diameter of the laser beam in uu. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Laser", meta = (ClampMin = 0, Units = "cm"))
	float LaserDiameter = 8.f;

	/** Length of LaserMesh's mesh along its local Z at scale 1, in uu (engine cylinder: 100). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Laser", meta = (ClampMin = 0.01, Units = "cm"))
	float LaserMeshBaseHeight = 100.f;

	/** Diameter of LaserMesh's mesh at scale 1, in uu (engine cylinder: 100). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Laser", meta = (ClampMin = 0.01, Units = "cm"))
	float LaserMeshBaseDiameter = 100.f;

	/** Horizontal distance of the hack hover point from the target, toward the miner, in uu (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Hack", meta = (ClampMin = 0, Units = "cm"))
	float HackHoverDistance = 100.f;

	/** Offset from the hack target's location to the point the link beam aims at, in uu (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Hack")
	FVector HackLinkOffset = FVector(0.f, 0.f, 50.f);

	/** Miner the drone follows. Set by SetOwnerMiner. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone")
	TObjectPtr<ARRMiner> OwnerMiner;

	/** Current state. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone")
	ERRDroneState State = ERRDroneState::Orbit;

	/** Grid holding the dig target. Set by RequestDig, cleared by StopDig. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone|Dig")
	TObjectPtr<ARRArenaGrid> DigGrid;

	/** Cell being cut. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone|Dig")
	FIntPoint DigCell = FIntPoint(-1, -1);

	/** Unit grid step from the dig cell toward the miner: the face the laser hits. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone|Dig")
	FIntPoint DigSide = FIntPoint(0, 0);

	/** Objective being hacked. Set by StartHack, cleared by EndHack. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone|Hack")
	TObjectPtr<AActor> HackTarget;

	/** Current orbit angle in radians, in [0, 2*pi). */
	float OrbitAngle = 0.f;
};
