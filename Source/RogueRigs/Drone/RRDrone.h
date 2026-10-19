// Rogue Rigs - the helper drone: separate actor spawned by the miner; orbits it, cuts an arc of blocks with a laser (dig) and links to an objective (hack).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid/RRCellType.h"
#include "RRDrone.generated.h"

class ARRArenaGrid;
class ARRMiner;
class UNiagaraComponent;
class UNiagaraSystem;
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
 * floor) and shows one laser beam (LaserMesh), from itself to GetDigImpactPoint (the middle of the dug blocks' faces, on the
 * wall face), whatever the number of blocks (drone gdd, owner 2026-10-19); OnDigBlockStarted / OnDigBlockStopped fire per
 * block for Blueprint effects. While it cuts, DigImpactComponent plays DigImpactSystem (NS_DigImpact, task-233-02) at
 * GetDigImpactPoint: User.WallNormal = outward normal of the impact face, User.ChipColor = colour of the majority type of
 * the dug blocks, User.Intensity 1; when the dig stops it fades to 0 over DigImpactFadeTime, then deactivates. Never in Hack.
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
	 * direction, with an empty arc, or while hacking (it cannot dig and hack at once). No contact fallback: see the overload.
	 */
	UFUNCTION(BlueprintCallable, Category = "Drone|Dig")
	bool RequestArcDig(ARRArenaGrid* Grid, FVector ArcDirection);

	/**
	 * Same as RequestArcDig, with the block cells the miner touches (task-065-01). When the arc is empty (the move
	 * direction steers the cone away from every touching block, e.g. moving diagonally into a corner or along a wall),
	 * the drone digs the touching block nearest the arc direction instead (SelectContactFallbackCells), so contact
	 * always digs at least one block. Called by ARRMiner::UpdateDigTrigger.
	 */
	bool RequestArcDig(ARRArenaGrid* Grid, FVector ArcDirection, const TArray<FIntPoint>& TouchingCells);

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
	 * Contact fallback for an empty arc: fills OutCells with the block cells of TouchingCells whose direction from
	 * MinerLocation is closest to ArcDirection (largest cosine; cells tied within 0.01 all kept, so a symmetric corner
	 * gives both), at most MaxArcBlocks, sorted by lower Y then lower X. 2D only. Returns the count; 0 without a grid,
	 * with a zero direction or without a touching block.
	 */
	int32 SelectContactFallbackCells(const ARRArenaGrid* Grid, const FVector& MinerLocation, const FVector& ArcDirection, const TArray<FIntPoint>& TouchingCells, TArray<FIntPoint>& OutCells) const;

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
	 * offset), at half block height. Zero without a dig grid or miner.
	 */
	UFUNCTION(BlueprintPure, Category = "Drone|Dig")
	FVector GetDigTargetPoint(FIntPoint Cell) const;

	/**
	 * Where the single dig beam ends (task-232-01): the mean of GetDigTargetPoint over the dug cells, projected on the plane of
	 * the wall face nearest the miner (the face of the dug cell whose target point is closest to the miner, along that face's
	 * axis), so the beam hits the middle of the dug blocks on the wall face. Changes every tick with the arc. Zero outside Dig,
	 * with an empty arc, or without a dig grid or miner. Use it for an impact effect at the beam's end.
	 */
	UFUNCTION(BlueprintPure, Category = "Drone|Dig")
	FVector GetDigImpactPoint() const;

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
	 * Every block of the deposits of DigCells (ARRArenaGrid::GetDepositCells, by deposit id, duplicates removed): the whole-deposit dig targets
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

	/** Side of Cell's face toward the owner miner: unit step on the dominant axis of the miner's offset from the cell center. Requires a valid miner and dig grid. */
	FIntPoint GetDigFaceSide(FIntPoint Cell) const;

	/** Dig laser: LaserMesh from the drone to GetDigImpactPoint; hidden when the arc is empty. */
	void UpdateDigLaser();

	/**
	 * Computes the impact point (see GetDigImpactPoint) and the grid side of the face it lies on (the face of the dug cell
	 * whose target point is nearest the miner). Returns false (outputs zero) outside Dig, with an empty arc, or without a dig
	 * grid or miner.
	 */
	bool ComputeDigImpact(FVector& OutPoint, FIntPoint& OutFaceSide) const;

	/**
	 * Dig impact effect, called in the Dig tick after the laser (before the damage, so the dug cells still hold their type):
	 * places DigImpactComponent at the impact point, sets User.WallNormal, User.ChipColor and User.Intensity 1, activates it.
	 * With an empty arc it fades instead (FadeDigImpact).
	 */
	void UpdateDigImpact(float DeltaSeconds);

	/** Lowers User.Intensity toward 0 over DigImpactFadeTime; at 0 deactivates the component (live particles finish). No effect when inactive. */
	void FadeDigImpact(float DeltaSeconds);

	/** Stops the dig impact effect at once (Hack, EndPlay): intensity 0 and deactivate, no fade. */
	void StopDigImpact();

	/** Chip colour of the dug arc: the colour of the cell type held by most dug cells (ties: the type met first, nearest the miner's front). */
	FLinearColor GetDigChipColor() const;

	/** Chip colour of one block type: rock, Duskite (Ore and SpecialOre), heal ore, barrel; any other type gives RockChipColor. */
	FLinearColor GetChipColorForType(ERRCellType Type) const;

	/** Root visual. Collision disabled, so it never blocks or overlaps pawns. Mesh set in Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/**
	 * Cutting laser: the single dig beam in Dig (toward GetDigImpactPoint) and the link beam in Hack. Expected mesh: a cylinder along local Z with its
	 * pivot at the center (the engine cylinder); size given by LaserMeshBaseHeight and LaserMeshBaseDiameter. Mesh and
	 * material set in Blueprint. Uses an absolute transform, set every tick. No collision, no shadow.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LaserMesh;

	/**
	 * Dig impact effect (NS_DigImpact, bp-dig-impact): created in the constructor, never spawned per frame. Absolute transform,
	 * moved every Dig tick to GetDigImpactPoint. Not auto-activated; uses DigImpactSystem.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> DigImpactComponent;

	/** Niagara system played at the dig impact point. Default /Game/RogueRigs/FX/Dig/NS_DigImpact (constructor reference). None = no effect. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig Impact")
	TObjectPtr<UNiagaraSystem> DigImpactSystem;

	/** Time for User.Intensity to fall from 1 to 0 after the dig stops, before the effect deactivates, in seconds (provisional). 0 = stop at once. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig Impact", meta = (ClampMin = 0, Units = "s"))
	float DigImpactFadeTime = 0.15f;

	/** Chip colour for rock blocks: light grey tinted violet (color-code), the NS_DigImpact default lavender. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig Impact")
	FLinearColor RockChipColor = FLinearColor(0.35f, 0.28f, 0.5f, 1.f);

	/** Chip colour for Duskite ore blocks (Ore and SpecialOre): glowing violet (color-code; value provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig Impact")
	FLinearColor OreChipColor = FLinearColor(0.6f, 0.15f, 1.f, 1.f);

	/** Chip colour for heal ore blocks: red (color-code; value provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig Impact")
	FLinearColor HealOreChipColor = FLinearColor(0.9f, 0.06f, 0.05f, 1.f);

	/** Chip colour for barrel blocks: orange (color-code; value provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig Impact")
	FLinearColor BarrelChipColor = FLinearColor(1.f, 0.35f, 0.05f, 1.f);

	/** Current User.Intensity of the dig impact effect: 1 while cutting, falls to 0 during the fade. */
	float DigImpactIntensity = 0.f;

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
