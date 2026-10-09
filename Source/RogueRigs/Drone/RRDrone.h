// Rogue Rigs - the helper drone: separate actor spawned by the miner; orbits it, cuts an arc of blocks with a laser (dig) and links to an objective (hack).

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
	/** Hovers over the arc of blocks in front of the miner and cuts them with the laser. */
	Dig,
	/** Links to a console (or later a bonus chest) inside its objective zone: StartHack / StopHack, driven by the objective. */
	Hack
};

/** Fired when a block enters the dug arc (it starts shaking and taking damage): its cell. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnDigBlockStartedSignature, FIntPoint, Cell);

/** Fired when a block leaves the dug arc: its cell, and true when it left because it was destroyed (dug out or blown up). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnDigBlockStoppedSignature, FIntPoint, Cell, bool, bDestroyed);

/**
 * The miner's helper drone. Spawned and destroyed by ARRMiner (DroneClass), which calls SetOwnerMiner.
 * Orbit: each tick the orbit angle advances by 2*pi / OrbitPeriod per second around the miner's location,
 * the orbit point is OrbitRadius away in the floor plane at HoverHeight above the miner's floor (capsule
 * bottom), and the drone interpolates toward it with FollowInterpSpeed.
 * Dig (arc mining, drone gdd): while the miner touches rock it calls RequestArcDig every tick with the arc direction;
 * the drone selects up to MaxArcBlocks block cells in front of the miner (SelectArcCells: center within ArcReachCells
 * cells of the miner's front point, inside an ArcAngleDegrees cone around the direction) and cuts all of them in
 * parallel, each taking DeltaSeconds * MiningSpeed dig-seconds of terrain damage per tick. A block keeps being cut while
 * it stays in the arc; one that leaves it stops shaking and keeps its damage. ReleaseDig (miner no longer touching rock)
 * returns to Orbit. The drone hovers over the arc centre (mean of the dug cell centers, HoverHeight above the miner's
 * floor) and shows one laser beam per dug block, from itself to the block's face (LaserBeams pool); OnDigBlockStarted / OnDigBlockStopped fire per
 * block for Blueprint lasers and effects.
 * Hack (StartHack, called every tick by the objective while the miner is in its zone): the drone stops any dig, hovers
 * HackHoverDistance from the target toward the miner (same HoverHeight) and shows LaserMesh from itself to the target's
 * location + HackLinkOffset. One target at a time; RequestArcDig is refused while hacking. StopHack (or the target or miner
 * gone) returns it to Orbit. The objective owns the hack progress.
 * The drone has no ASC: it reads MiningSpeed on the miner's ASC. Its meshes never collide. The grid is assumed unrotated
 * and unscaled (world X / Y = grid X / Y).
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
	 * Requests a state change. Orbit is always accepted (stops any dig). Dig is accepted only when the dug arc holds at
	 * least one block (use RequestArcDig to select one). Hack is refused here: use StartHack. Refusals are logged.
	 * Returns true when the drone is in the requested state afterwards.
	 */
	UFUNCTION(BlueprintCallable, Category = "Drone")
	bool SetState(ERRDroneState NewState);

	/**
	 * Arc dig request, called every tick by the miner while it touches rock. ArcDirection (world, only XY used) is where
	 * the arc points: the move direction when it pushes into the wall, else toward the wall. Selects the arc
	 * (SelectArcCells) from the miner's current location; blocks entering it start shaking (OnDigBlockStarted), blocks
	 * leaving it stop (OnDigBlockStopped, damage kept). Enters Dig when the arc holds a block; an empty arc leaves Dig.
	 * Returns true when the drone digs at least one block afterwards; false without a miner or grid, with a zero
	 * direction, with an empty arc, or while hacking (it cannot dig and hack at once).
	 */
	UFUNCTION(BlueprintCallable, Category = "Drone|Dig")
	bool RequestArcDig(ARRArenaGrid* Grid, FVector ArcDirection);

	/** Stops digging (miner no longer touching rock): every dug block stops shaking and keeps its damage; back to Orbit. No effect outside Dig. */
	UFUNCTION(BlueprintCallable, Category = "Drone|Dig")
	void ReleaseDig();

	/**
	 * Fills OutCells with the block cells of Grid the arc would dig if the miner stood at MinerLocation: block cells whose
	 * center is within ArcReachCells * cell size of the front point (MinerLocation + ArcDirection * miner capsule radius)
	 * and whose direction from MinerLocation is within ArcAngleDegrees / 2 of ArcDirection, nearest to the front point
	 * first (ties: smaller angle, then lower Y, then lower X), at most MaxArcBlocks. 2D only. Returns the count; 0 without
	 * a grid or with a zero direction. Without an owner miner the capsule radius is 0.
	 */
	int32 SelectArcCells(const ARRArenaGrid* Grid, const FVector& MinerLocation, const FVector& ArcDirection, TArray<FIntPoint>& OutCells) const;

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

	/** Cells being cut, nearest to the miner's front first; empty outside Dig. */
	UFUNCTION(BlueprintPure, Category = "Drone|Dig")
	const TArray<FIntPoint>& GetDigCells() const { return DigCells; }

	/** Grid holding the dug cells; null outside Dig. */
	UFUNCTION(BlueprintPure, Category = "Drone|Dig")
	ARRArenaGrid* GetDigGrid() const { return DigGrid; }

	/** Direction of the dug arc (unit, Z = 0) given by the last accepted RequestArcDig. */
	UFUNCTION(BlueprintPure, Category = "Drone|Dig")
	FVector GetDigArcDirection() const { return DigArcDirection; }

	/**
	 * Point a laser aims at to cut Cell: the center of its face toward the owner miner (dominant axis of the miner's
	 * offset), at half block height. Use it every frame for a per-block Blueprint laser. Zero without a dig grid or miner.
	 */
	UFUNCTION(BlueprintPure, Category = "Drone|Dig")
	FVector GetDigTargetPoint(FIntPoint Cell) const;

	/** MiningSpeed attribute of the owner miner's ASC; 1 if there is no miner, no ASC or no MiningSpeed attribute. */
	UFUNCTION(BlueprintPure, Category = "Drone")
	float GetMiningSpeed() const;

	/** Runs the behaviour of the current state (Orbit: moves along the orbit; Dig: hovers, cuts, draws the laser; Hack: hovers, draws the link). */
	virtual void Tick(float DeltaSeconds) override;

	/** Broadcast when a block enters the dug arc (also when the drone enters Dig). Blueprint: start a laser beam or effect on Cell. */
	UPROPERTY(BlueprintAssignable, Category = "Drone|Dig")
	FRROnDigBlockStartedSignature OnDigBlockStarted;

	/** Broadcast when a block leaves the dug arc: out of the arc, miner released the wall, drone left Dig, or block destroyed (bDestroyed). */
	UPROPERTY(BlueprintAssignable, Category = "Drone|Dig")
	FRROnDigBlockStoppedSignature OnDigBlockStopped;

protected:
	/** Fills the laser beam pool up to MaxArcBlocks beams, so the fan does not create components mid-dig. */
	virtual void BeginPlay() override;

	/** Stops a dig in progress so the target blocks do not keep shaking after the drone is gone. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Orbit tick: advances the orbit angle and moves toward the orbit point. */
	void TickOrbit(float DeltaSeconds);

	/** Dig tick: checks the cancel rules, drops destroyed blocks, moves to the arc centre, updates the laser and damages every dug block. */
	void TickDig(float DeltaSeconds);

	/** Hack tick: back to Orbit when the target or miner is gone, else moves to the hack hover point and draws the link. */
	void TickHack(float DeltaSeconds);

	/** Leaves Hack: hides the laser, clears the target and resumes the orbit from the drone's current angle. */
	void EndHack();

	/** Sets the orbit angle to the drone's current angle around the miner, so the orbit resumes where the drone is. */
	void ResumeOrbitFromCurrentAngle();

	/** World position of the orbit point for the current angle around the owner miner. Requires a valid miner. */
	FVector ComputeOrbitPoint() const;

	/** World position the drone hovers at while digging: the arc centre (mean of the dug cell centers, or the drone's XY when none) at HoverHeight above the miner's floor. Requires a valid miner and dig grid. */
	FVector ComputeDigHoverPoint() const;

	/** Stops one dug block: broadcasts OnDigBlockStopped (bDestroyed = the cell no longer holds a block). Touches neither DigCells nor the shake (RefreshDigShake). */
	void StopDigBlock(FIntPoint Cell);

	/**
	 * Every block of the veins of DigCells (ARRArenaGrid::GetVeinCells, duplicates removed): the whole-vein dig targets
	 * (ore-blocks, heal-ore). Requires a valid dig grid.
	 */
	void CollectDigTargets(TArray<FIntPoint>& OutTargets) const;

	/**
	 * Makes ShakeCells match the dig targets (heal-ore, owner 2026-10-07 fifth test: a vein being mined shakes whole): blocks
	 * entering start shaking, blocks leaving (out of the arc veins or destroyed) stop. Without a dig grid, stops them all.
	 */
	void RefreshDigShake();

	/** Leaves Dig: stops the whole-vein shake, stops every dug block, hides the laser, clears the arc and resumes the orbit from the drone's current angle. */
	void StopDig();

	/** Places Beam as a cylinder from Start to End with LaserDiameter, and shows it (hidden when Start and End coincide). */
	void UpdateLaser(UStaticMeshComponent* Beam, const FVector& Start, const FVector& End);

	/** Grows LaserBeams to at least Count beams; each new beam copies LaserMesh's mesh and materials, absolute transform, no collision, hidden. */
	void EnsureLaserBeams(int32 Count);

	/** Aims beam i at GetDigTargetPoint(DigCells[i]) and hides the beams past the arc. Grows the pool when the arc is larger. */
	void UpdateDigLasers();

	/** Hides every beam of LaserBeams (LaserMesh included). */
	void HideLaserBeams();

	/** Root visual. Collision disabled, so it never blocks or overlaps pawns. Mesh set in Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/**
	 * Cutting laser: beam 0 of LaserBeams in Dig (toward the first dug block) and the link beam in Hack; the other beams copy its mesh and materials. Expected mesh: a cylinder along local Z with its
	 * pivot at the center (the engine cylinder); size given by LaserMeshBaseHeight and LaserMeshBaseDiameter. Mesh and
	 * material set in Blueprint. Uses an absolute transform, set every tick. No collision, no shadow.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LaserMesh;

	/**
	 * Laser fan: one beam per dug block, DigCells[i] cut by LaserBeams[i]. LaserBeams[0] is LaserMesh; the others are
	 * created at runtime (BeginPlay, or when the arc outgrows the pool) as copies of LaserMesh. Unused beams are hidden.
	 */
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone|Laser")
	TArray<TObjectPtr<UStaticMeshComponent>> LaserBeams;

	/** Horizontal distance from the miner's location to the orbit point, in uu. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm"))
	float OrbitRadius = 150.f;

	/** Time for one full turn around the miner, in seconds. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0.01, Units = "s"))
	float OrbitPeriod = 2.f;

	/** Height of the drone above the miner's floor (capsule bottom), in uu. Used in Orbit, Dig and Hack. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (Units = "cm"))
	float HoverHeight = 150.f;

	/** Speed at which the drone moves toward its orbit, dig or hack hover point (FMath::VInterpTo speed). 0 = snap to it. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0))
	float FollowInterpSpeed = 10.f;

	/** Reach of the arc: a block is dug when its center is within this many cells of the miner's front point (drone gdd depth: 2.5, to test; about two rows deep). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig", meta = (ClampMin = 0))
	float ArcReachCells = 2.5f;

	/** Full opening angle of the arc cone around the arc direction, in degrees (drone gdd: 120, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig", meta = (ClampMin = 0, ClampMax = 360, Units = "deg"))
	float ArcAngleDegrees = 120.f;

	/** Most blocks dug at once (drone gdd depth: 8, to test); nearest first, so the front row goes before the second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig", meta = (ClampMin = 1))
	int32 MaxArcBlocks = 8;

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

	/** Grid holding the dug cells. Set by RequestArcDig, cleared by StopDig. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone|Dig")
	TObjectPtr<ARRArenaGrid> DigGrid;

	/** Cells being cut (the arc), nearest to the miner's front first. Their whole veins shake (ShakeCells). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone|Dig")
	TArray<FIntPoint> DigCells;

	/** Cells this drone makes shake on DigGrid: every block of the veins of DigCells. Kept in sync by RefreshDigShake. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone|Dig")
	TArray<FIntPoint> ShakeCells;

	/** Arc direction (unit, Z = 0) of the last accepted RequestArcDig. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone|Dig")
	FVector DigArcDirection = FVector::ForwardVector;

	/** Objective being hacked. Set by StartHack, cleared by EndHack. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Drone|Hack")
	TObjectPtr<AActor> HackTarget;

	/** Current orbit angle in radians, in [0, 2*pi). */
	float OrbitAngle = 0.f;
};
