// Rogue Rigs - the helper drone: separate actor spawned by the miner; patrols around it (Orbit state), cuts an arc of blocks with a laser (dig) and links to an objective (hack).

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
	/** Patrols around the miner (default): flies to random patrol points 60-160 uu from it, with a lag that grows with the distance (task-251-01). */
	Orbit,
	/** Hovers over the arc of blocks in front of the miner and cuts them with the laser. */
	Dig,
	/** Links to a console (or later a bonus chest) inside its objective zone: StartHack / StopHack, driven by the objective. */
	Hack
};

/** Idle gestures of the drone at rest in Orbit (task-249-01). Internal to ARRDrone, not exposed to Blueprint. */
enum class ERRDroneIdleGesture : uint8
{
	/** No gesture playing. */
	None,
	/** Yaw swings to one side then the other. */
	ScanSweep,
	/** Faces the miner and dips its nose. */
	Nod,
	/** Short drop in height. */
	Dip
};

/** Fired when a block enters the dug arc (it starts shaking and taking damage): its cell. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnDigBlockStartedSignature, FIntPoint, Cell);

/** Fired when a block leaves the dug arc: its cell, and true when it left because it was destroyed (dug out or blown up). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnDigBlockStoppedSignature, FIntPoint, Cell, bool, bDestroyed);

/**
 * The miner's helper drone. Spawned and destroyed by ARRMiner (DroneClass), which calls SetOwnerMiner.
 * Orbit (patrol movement, task-251-01, owner 2026-10-20, replaces the task-247-01 escort; the state keeps its name): the drone
 * flies to a patrol point picked at random in the zone PatrolMinRadius-PatrolMaxRadius around the miner. The point is an offset
 * from the miner, so the miner moving only drags the zone; it never depends on the miner's facing, aim or movement direction.
 * A new point is picked when the drone comes within PatrolReachDistance of it, after a random PatrolMinInterval-PatrolMaxInterval
 * time, or when it becomes invalid. Follow (horizontal): the desired speed toward the point grows with the distance on a
 * smoothstep from PatrolNearSpeed to PatrolFarSpeed (reached at PatrolSpeedRampDistance), slowed further at arrival; the drone's
 * velocity approaches it at PatrolVelocityResponse (the lag), and never passes the point. Height: toward GetEscortHeight above
 * the miner's floor (capsule bottom, task-249-01, see Alive) at PatrolHeightInterpSpeed.
 * Walls (task-248-01, owner 2026-10-20): the drone never hovers over a block. A patrol point is valid when the 2D segment from
 * the miner to DroneWallClearance past it crosses no block cell (ARRArenaGrid::HasLineOfSight); invalid candidates are rejected
 * and drawn again. No valid candidate = just above the miner, EscortFallbackExtraHeight higher, retried every
 * PatrolFallbackRetryInterval. When the drone has no clear line to its point it heads for the miner instead. As a last guard, a
 * move that would put the drone in a block cell is cut on the blocked axis (Orbit, Dig and Hack).
 * Alive (task-249-01, owner 2026-10-20, visual only): the Orbit height is EscortCalmHeight, blended toward EscortAlertHeight
 * while an enemy is within EscortAlertRadius of the miner, plus two bobs of different periods, clamped to [EscortMinHeight,
 * EscortMaxHeight]. The actor yaw turns with delay toward a point of interest: the dig impact point (Dig) or hack target (Hack),
 * else the nearest living enemy near the miner, else the nearest deposit block or objective near the miner, else the miner
 * (enemy, deposit and objective lookups refreshed every InterestRefreshInterval only). Pitch and roll follow the miner's
 * acceleration in the drone's frame (nose down when it speeds forward, banked into turns), damped, clamped to MaxTiltDegrees.
 * While the miner rests in Orbit, an idle gesture plays every 4 to 8 s at random: scan sweep (yaw), nod toward the miner
 * (pitch) or short dip (height).
 * Dig (arc mining, drone gdd): while the miner touches rock it calls RequestArcDig every tick with the arc direction;
 * the drone selects up to MaxArcBlocks block cells in front of the miner (SelectArcCells: center within ArcReachCells
 * cells of the miner's front point, inside an ArcAngleDegrees cone around the direction) and cuts all of them in
 * parallel, each taking DeltaSeconds * MiningSpeed dig-seconds of terrain damage per tick. A block keeps being cut while
 * it stays in the arc; one that leaves it stops shaking and keeps its damage. ReleaseDig (miner no longer touching rock)
 * returns to Orbit. The drone hovers on the free side of the wall face, DigHoverFaceDistance from GetDigImpactPoint along
 * the face normal, DigHoverHeight above the miner's floor (task-248-01; pulled back toward the miner when that point is not
 * clear), and shows one laser beam (LaserMesh), from itself to GetDigImpactPoint (the middle of the dug blocks' faces, on the
 * wall face), whatever the number of blocks (drone gdd, owner 2026-10-19); OnDigBlockStarted / OnDigBlockStopped fire per
 * block for Blueprint effects. While it cuts, DigImpactComponent plays DigImpactSystem (NS_DigImpact, task-233-02) at
 * GetDigImpactPoint: User.WallNormal = outward normal of the impact face, User.ChipColor = colour of the majority type of
 * the dug blocks, User.Intensity 1; when the dig stops it fades to 0 over DigImpactFadeTime, then deactivates. Never in Hack.
 * Hack (StartHack, called every tick by the objective while the miner is in its zone): the drone stops any dig, hovers
 * HackHoverDistance from the target toward the miner (same HoverHeight; pulled back toward the miner when that point is
 * not clear, task-248-01) and shows LaserMesh from itself to the target's
 * location + HackLinkOffset. One target at a time; RequestArcDig is refused while hacking. StopHack (or the target or miner
 * gone) returns it to Orbit. The objective owns the hack progress.
 * Parts (task-253-01, owner 2026-10-20, provisional, visual only): the root Mesh is the base (body); its yaw is the actor yaw,
 * which turns toward the dig or hack target at BaseYawTurnRate at most (never instantly). LaserPivot (the laser arm, hanging
 * under the base) pitches about its local Y axis so its emitter tip (LaserEmitterLocal) aims at the dig impact point or the hack
 * link point, within [LaserPitchMinDegrees, LaserPitchMaxDegrees] (0 = straight down) at LaserPitchTurnRate at most, and rests
 * straight down in Orbit; the beam (LaserMesh) starts at that emitter tip. AntennaMesh spins about its Z axis at
 * AntennaSpinSpeed, always.
 * Thrusters (task-254-02, owner 2026-10-20, provisional, visual only): ThrusterLeft and ThrusterRight play ThrusterSystem
 * (NS_DroneThruster) under the two side pods, always active (Orbit, Dig, Hack; idle glow at rest); every tick User.Speed =
 * the actor's own world speed (location delta / DeltaSeconds, smoothed over ThrusterSpeedSmoothing). Never active under -nullrhi.
 * The drone has no ASC: it reads MiningSpeed on the miner's ASC. Its meshes never collide. The grid is assumed unrotated
 * and unscaled (world X / Y = grid X / Y).
 * Subclass in Blueprint for the base, laser arm and antenna meshes and the beam mesh and material.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRDrone : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the root base mesh, the laser arm (LaserPivot) and antenna children, the hidden beam mesh (all no collision), the impact effect and the two thrusters, and enables ticking. */
	ARRDrone();

	/** Places the two thrusters at ThrusterLeftOffset and ThrusterRightOffset (so a Blueprint override of the offsets applies). */
	virtual void OnConstruction(const FTransform& Transform) override;

	/** Smoothed world speed of the drone written to the thrusters' User.Speed, in uu/s (0 at rest). */
	UFUNCTION(BlueprintPure, Category = "Drone|Thrusters")
	float GetThrusterSpeed() const { return ThrusterSpeed; }

	/**
	 * Sets the miner the drone follows, makes the drone tick after it, seeds the patrol stream from the drone's unique id, picks
	 * a patrol point and snaps the drone to it (velocity zero). Called by ARRMiner right after spawning the drone. Null stops all
	 * movement.
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

	/**
	 * World location of the laser emitter tip, where the beam starts (task-253-01): LaserEmitterLocal transformed by LaserPivot's
	 * world transform (pitch, base yaw and tilt included). Valid with or without a mesh on LaserPivot.
	 */
	UFUNCTION(BlueprintPure, Category = "Drone|Laser")
	FVector GetLaserEmitterLocation() const;

	/** MiningSpeed attribute of the owner miner's ASC; 1 if there is no miner, no ASC or no MiningSpeed attribute. */
	UFUNCTION(BlueprintPure, Category = "Drone")
	float GetMiningSpeed() const;

	/** Runs the behaviour of the current state (Orbit: patrols around the miner; Dig: hovers, cuts, draws the laser; Hack: hovers, draws the link). */
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

	/** Orbit tick (patrol): advances the bob phases, re-picks the patrol point when reached, timed out or invalid, then moves the drone toward it (UpdatePatrolPoint, ComputeOrbitPoint, distance-scaled lag follow). */
	void TickOrbit(float DeltaSeconds);

	/** Dig tick: checks the cancel rules, drops destroyed blocks, moves to the arc centre, updates the laser and damages every dug block. */
	void TickDig(float DeltaSeconds);

	/** Hack tick: back to Orbit when the target or miner is gone, else moves to the hack hover point and draws the link. */
	void TickHack(float DeltaSeconds);

	/** Leaves Hack: hides the laser, clears the target and resumes the escort from where the drone is. */
	void EndHack();

	/** Resumes the patrol from the drone's current location: velocity zero and a fresh patrol point (PickPatrolPoint), so the drone flies back smoothly, never snaps. */
	void ResumeOrbit();

	/**
	 * Restarts the patrol around the miner (SetOwnerMiner, tests): velocity zero, a fresh patrol point, and the drone snapped to
	 * it (ComputeOrbitPoint). Does not reseed PatrolRandom. Requires a valid miner.
	 */
	void RestartPatrol();

	/**
	 * World position of the current patrol point (task-251-01): miner XY + PatrolOffset at GetEscortHeight above the miner's floor
	 * (task-249-01). The fallback point (bPatrolFallback) is the miner's XY, EscortFallbackExtraHeight higher. Requires a valid miner.
	 */
	FVector ComputeOrbitPoint() const;

	/**
	 * True when the patrol offset Offset (from the miner, XY) is valid on Grid (task-248-01 rule): the 2D segment from the miner to
	 * DroneWallClearance past the point crosses no block cell. Always true without a grid. Requires a valid miner.
	 */
	bool IsPatrolOffsetClear(const ARRArenaGrid* Grid, const FVector2D& Offset) const;

	/**
	 * Picks a new patrol point (task-251-01): up to PatrolPickAttempts random candidates (angle uniform, radius uniform in
	 * [PatrolMinRadius, PatrolMaxRadius]) from PatrolRandom; invalid ones (IsPatrolOffsetClear) are rejected. Among the valid ones
	 * the first whose straight path from the drone stays at least PatrolMinRadius from the miner and that lies at least
	 * PatrolMinHopDistance from the drone wins, else the valid one whose path passes farthest from the miner. None valid =
	 * fallback above the miner. Draws the next timeout in [PatrolMinInterval, PatrolMaxInterval]. Requires a valid miner.
	 */
	void PickPatrolPoint();

	/** Re-picks the patrol point when the drone is within PatrolReachDistance of it, when its timeout ran out, when it became invalid, or (fallback) every PatrolFallbackRetryInterval. Requires a valid miner. */
	void UpdatePatrolPoint(float DeltaSeconds);

	/** Desired horizontal speed toward a target Distance uu away: smoothstep from PatrolNearSpeed to PatrolFarSpeed over [0, PatrolSpeedRampDistance], then at most Distance * PatrolArrivalRate. */
	float GetPatrolSpeed(double Distance) const;

	/**
	 * World position the drone hovers at while digging (task-248-01): GetDigImpactPoint moved DigHoverFaceDistance along the
	 * outward normal of the impact face (the free side, toward the miner), at DigHoverHeight above the miner's floor; the
	 * drone's XY when the arc is empty. Pulled back toward the miner when not clear (ResolveClearHoverPoint). Requires a valid
	 * miner and dig grid, in Dig.
	 */
	FVector ComputeDigHoverPoint() const;

	/** Grid the drone avoids: the dig grid in Dig, else the owner miner's arena grid. Null when there is none. */
	const ARRArenaGrid* GetTerrainGrid() const;

	/**
	 * Desired hover point made clear (Dig and Hack, task-248-01): unchanged when the 2D segment from the miner to
	 * DroneWallClearance past it crosses no block cell, else moved back along that segment to DroneWallClearance before the
	 * first block (toward the miner, at most onto it). Z kept. Without a grid, Desired. Requires a valid miner.
	 */
	FVector ResolveClearHoverPoint(const ARRArenaGrid* Grid, const FVector& Desired) const;

	/**
	 * Last guard against hovering over rock (visual only, no collision): To when its cell is not a block on Grid (or when From
	 * is already in one, so the drone never gets stuck); else the move is cut on the blocked axis (X only, then Y only, else no
	 * horizontal move; Z kept from To). When InOutVelocity is set, the cut components are zeroed. Without a grid, To.
	 */
	FVector ConstrainToClearCells(const ARRArenaGrid* Grid, const FVector& From, const FVector& To, FVector* InOutVelocity = nullptr) const;

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

	/** Leaves Dig: stops the whole-vein shake, stops every dug block, hides the laser, clears the arc and resumes the escort from where the drone is. */
	void StopDig();

	/** Places Beam as a cylinder from Start to End with LaserDiameter, and shows it (hidden when Start and End coincide). */
	void UpdateLaser(UStaticMeshComponent* Beam, const FVector& Start, const FVector& End);

	/** Side of Cell's face toward the owner miner: unit step on the dominant axis of the miner's offset from the cell center. Requires a valid miner and dig grid. */
	FIntPoint GetDigFaceSide(FIntPoint Cell) const;

	/** Dig laser: LaserMesh from the emitter tip (GetLaserEmitterLocation) to GetDigImpactPoint; hidden when the arc is empty. */
	void UpdateDigLaser();

	/**
	 * Point the laser arm aims at (task-253-01): GetDigImpactPoint in Dig (false with an empty arc), the hack target's location +
	 * HackLinkOffset in Hack; false in Orbit (the arm rests straight down).
	 */
	bool GetLaserAimPoint(FVector& OutPoint) const;

	/**
	 * Pitch of LaserPivot, in degrees (0 = straight down, positive = toward the base's forward +X), that puts the line from the pivot
	 * through the emitter tip on Point, in the base's XZ plane (the out-of-plane part is left to the base yaw). Not clamped.
	 */
	float ComputeLaserPitchToward(const FVector& Point) const;

	/**
	 * After the attitude (task-253-01): spins AntennaMesh by AntennaSpinSpeed, turns LaserPivot's pitch toward the aim point (or 0 in
	 * Orbit) at LaserPitchTurnRate at most, clamped to [LaserPitchMinDegrees, LaserPitchMaxDegrees], then re-places a visible beam
	 * from the new emitter location to LaserBeamEnd. Runs with or without a miner.
	 */
	void UpdateParts(float DeltaSeconds);

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

	/**
	 * Thrusters (task-254-02), every tick in every state: measures the actor's world speed from its location delta over
	 * DeltaSeconds, smooths it (exponential, ThrusterSpeedSmoothing), writes it to User.Speed of both thrusters and activates
	 * an inactive thruster. No activation when the process can never render (-nullrhi) or without ThrusterSystem.
	 */
	void UpdateThrusters(float DeltaSeconds);

	/** Chip colour of the dug arc: the colour of the cell type held by most dug cells (ties: the type met first, nearest the miner's front). */
	FLinearColor GetDigChipColor() const;

	/** Chip colour of one block type: rock, Duskite (Ore and SpecialOre), heal ore, barrel; any other type gives RockChipColor. */
	FLinearColor GetChipColorForType(ERRCellType Type) const;

	/** Root visual, the drone base (body, SM_DroneBase, task-253-01): turned in yaw by the actor rotation. Collision disabled, so it never blocks or overlaps pawns. Mesh set in Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/**
	 * Laser arm (SM_DroneLaser, task-253-01) hanging under the base: attached to Mesh at (1.606, 0, -17.248) (bp-drone-materials),
	 * pitched about its local Y axis by UpdateParts (pitch 0 = hanging straight down). No collision, no shadow. Mesh set in
	 * Blueprint; works with none (its transform still places the beam start).
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LaserPivot;

	/**
	 * Antenna (SM_DroneAntenna, task-253-01, origin at the flange foot on its spin axis): attached to Mesh at (-4.257, -0.837, 28.752)
	 * (bp-drone-materials), spun about its local Z by UpdateParts. No collision, no shadow. Mesh set in Blueprint.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> AntennaMesh;

	/** Beam origin, local to LaserPivot: the emitter tip of SM_DroneLaser, in uu (bp-drone-materials, Y mirrored from Blender; task-253-01, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Parts")
	FVector LaserEmitterLocal = FVector(-0.054f, 0.143f, -19.f);

	/** Spin speed of AntennaMesh about its Z axis, in degrees per second, always on (Orbit, Dig, Hack; task-253-01, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Parts", meta = (Units = "deg/s"))
	float AntennaSpinSpeed = 90.f;

	/**
	 * Largest turn rate of the base yaw (the actor yaw), in degrees per second (task-253-01, provisional): in Dig and Hack the yaw
	 * turns toward the target at this constant rate (no instant turn); for the other points of interest the task-249-01 delay
	 * (LookYawInterpSpeed) is kept and capped by this same rate.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Parts", meta = (ClampMin = 0, Units = "deg/s"))
	float BaseYawTurnRate = 120.f;

	/** Largest turn rate of LaserPivot's pitch, in degrees per second (task-253-01, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Parts", meta = (ClampMin = 0, Units = "deg/s"))
	float LaserPitchTurnRate = 240.f;

	/** Lowest LaserPivot pitch (negative = toward the base's back), in degrees from straight down (task-253-01, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Parts", meta = (ClampMin = -180, ClampMax = 180, Units = "deg"))
	float LaserPitchMinDegrees = -20.f;

	/** Highest LaserPivot pitch (positive = toward the base's front, where the dig face is), in degrees from straight down (task-253-01, provisional; the dig aim is about 50-60 deg). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Parts", meta = (ClampMin = -180, ClampMax = 180, Units = "deg"))
	float LaserPitchMaxDegrees = 80.f;

	/** Current LaserPivot pitch, in degrees (0 = straight down). */
	float LaserPitch = 0.f;

	/** Current AntennaMesh yaw relative to the base, in degrees, in [0, 360). */
	float AntennaYaw = 0.f;

	/** End of the beam last placed by UpdateLaser; UpdateParts re-places a visible beam from the moved emitter to it. */
	FVector LaserBeamEnd = FVector::ZeroVector;

	/**
	 * Cutting laser: the single dig beam in Dig (from the emitter tip, GetLaserEmitterLocation, toward GetDigImpactPoint) and the link beam in Hack. Expected mesh: a cylinder along local Z with its
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

	/**
	 * Left pod thruster (NS_DroneThruster, bp-drone-thruster, task-254-02): attached to Mesh (the base) at ThrusterLeftOffset,
	 * exhaust along the component's -Z. Created in the constructor, never spawned per frame; not auto-activated (UpdateThrusters
	 * activates it); no collision.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> ThrusterLeft;

	/** Right pod thruster: same as ThrusterLeft, at ThrusterRightOffset. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> ThrusterRight;

	/**
	 * Niagara system both thrusters play. Default /Game/RogueRigs/FX/Drone/NS_DroneThruster (constructor reference). None = no
	 * effect. The system reads User.Speed in uu/s (700 = top speed).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Thrusters")
	TObjectPtr<UNiagaraSystem> ThrusterSystem;

	/**
	 * Relative transform of ThrusterLeft in the base frame, in uu: the bottom of the left pod nozzle, identity rotation (exhaust
	 * -Z). Provisional (task-254-02), to fine-tune on the real meshes; the pods are mirror-symmetric in Y, so the sign only picks the side.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Thrusters")
	FTransform ThrusterLeftOffset = FTransform(FVector(0.0, -27.0, -17.0));

	/** Relative transform of ThrusterRight in the base frame, in uu: the bottom of the right pod nozzle (provisional, see ThrusterLeftOffset). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Thrusters")
	FTransform ThrusterRightOffset = FTransform(FVector(0.0, 27.0, -17.0));

	/** Time constant of the exponential smoothing of the measured speed, in seconds (provisional). 0 = raw speed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Thrusters", meta = (ClampMin = 0, Units = "s"))
	float ThrusterSpeedSmoothing = 0.1f;

	/** Smoothed world speed written to User.Speed, in uu/s. */
	float ThrusterSpeed = 0.f;

	/** Actor location at the previous thruster update; valid when bHasThrusterLocation. */
	FVector ThrusterPreviousLocation = FVector::ZeroVector;

	/** False until the first thruster update, and after SetOwnerMiner's snap, so a teleport never reads as speed. */
	bool bHasThrusterLocation = false;

	/** Inner radius of the patrol zone around the miner: a patrol point is at least this far from it (2D), in uu (task-251-01; provisional, owner 2026-10-20). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm"))
	float PatrolMinRadius = 60.f;

	/** Outer radius of the patrol zone around the miner: a patrol point is at most this far from it (2D), in uu (task-251-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm"))
	float PatrolMaxRadius = 160.f;

	/** The patrol point counts as reached (a new one is picked) when the drone is within this 2D distance of it, in uu (task-251-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm"))
	float PatrolReachDistance = 25.f;

	/** Shortest random lifetime of a patrol point before a new one is picked, in seconds (task-251-01: 3.5-5 s; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0.1, Units = "s"))
	float PatrolMinInterval = 3.5f;

	/** Longest random lifetime of a patrol point before a new one is picked, in seconds (task-251-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0.1, Units = "s"))
	float PatrolMaxInterval = 5.f;

	/** A new patrol point should lie at least this far from the drone (2D), so reaching one does not pick one right beside it, in uu (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm"))
	float PatrolMinHopDistance = 60.f;

	/** Random candidates drawn per patrol pick before falling back above the miner (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 1))
	int32 PatrolPickAttempts = 12;

	/** Desired follow speed right at the patrol point (before the arrival slowdown), in uu/s (task-251-01: about 40; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm/s"))
	float PatrolNearSpeed = 40.f;

	/** Desired follow speed far from the patrol point (the clamp), in uu/s (task-251-01: about 700, above the miner's 600; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm/s"))
	float PatrolFarSpeed = 700.f;

	/** Distance to the patrol point at which the follow speed reaches PatrolFarSpeed (smoothstep from PatrolNearSpeed below), in uu; sets the lag behind a running miner (about 225 uu at 600 uu/s; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 1, Units = "cm"))
	float PatrolSpeedRampDistance = 300.f;

	/** Arrival slowdown: the desired speed is at most the distance times this rate, so the drone settles on a point without overshoot (1/s; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0))
	float PatrolArrivalRate = 4.f;

	/** Rate at which the drone's velocity approaches the desired one (1/s, exponential): the inertia of the lag; lower = softer (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0.01))
	float PatrolVelocityResponse = 5.f;

	/** Speed at which the drone's height follows the Orbit height (FMath::FInterpTo speed; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0))
	float PatrolHeightInterpSpeed = 6.f;

	/** Horizontal miner speed at or below which the miner counts as resting (idle gestures), in uu/s (provisional; was EscortHeadingMinSpeed). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm/s"))
	float MinerRestSpeed = 50.f;

	/** Distance from the patrol point beyond which the drone snaps to it and stops (miner teleported), in uu (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm"))
	float EscortSnapDistance = 2000.f;

	/** Height of the drone above the miner's floor (capsule bottom) while hacking, in uu. Orbit uses EscortCalmHeight / EscortAlertHeight (task-249-01), Dig uses DigHoverHeight. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (Units = "cm"))
	float HoverHeight = 150.f;

	/** Amplitude of the slow vertical bob in Orbit (first of the two bob frequencies), in uu (task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm"))
	float EscortBobAmplitude = 9.f;

	/** Period of the slow vertical bob in Orbit, in seconds (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0.1, Units = "s"))
	float EscortBobPeriod = 2.4f;

	/** Amplitude of the fast vertical bob in Orbit (second bob frequency, added to the slow one), in uu (task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0, Units = "cm"))
	float EscortBobSecondAmplitude = 4.f;

	/** Period of the fast vertical bob in Orbit, in seconds; not a divisor of EscortBobPeriod, so the sum never repeats exactly (task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0.1, Units = "s"))
	float EscortBobSecondPeriod = 0.9f;

	/** Orbit centre height above the miner's floor when no enemy is near the miner, in uu (task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (Units = "cm"))
	float EscortCalmHeight = 158.f;

	/** Orbit centre height above the miner's floor when an enemy is within EscortAlertRadius of the miner, in uu (task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (Units = "cm"))
	float EscortAlertHeight = 174.f;

	/** Lowest Orbit slot height above the miner's floor (bob and dip included), in uu (drone gdd band 130-190, task-249-01; provisional). Not applied to the fallback slot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (Units = "cm"))
	float EscortMinHeight = 130.f;

	/** Highest Orbit slot height above the miner's floor (bob included), in uu (drone gdd band 130-190, task-249-01; provisional). The fallback slot adds EscortFallbackExtraHeight on top. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (Units = "cm"))
	float EscortMaxHeight = 190.f;

	/** A living enemy within this 2D distance of the miner raises the Orbit height toward EscortAlertHeight, in uu (task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0, Units = "cm"))
	float EscortAlertRadius = 700.f;

	/** Speed of the calm-to-alert height blend (FMath::FInterpTo speed; task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0))
	float EscortAlertBlendSpeed = 1.5f;

	/** Time between two refreshes of the point of interest (enemy, deposit and objective lookups), in seconds: 0.25 = 4 Hz (task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0.02, Units = "s"))
	float InterestRefreshInterval = 0.25f;

	/** The nearest living enemy within this 2D distance of the miner becomes the point of interest, in uu (task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0, Units = "cm"))
	float InterestEnemyRadius = 900.f;

	/** Without an enemy, the nearest standing deposit block or objective (console, shuttle) within this 2D distance of the miner is the point of interest, in uu (task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0, Units = "cm"))
	float InterestSiteRadius = 500.f;

	/** Speed at which the drone's yaw turns toward the point of interest (FMath::FInterpTo speed on the yaw delta): the delay of the look (task-249-01; provisional). Outside Dig and Hack only, capped by BaseYawTurnRate (task-253-01). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0))
	float LookYawInterpSpeed = 6.f;

	/** Tilt per unit of miner acceleration (pitch from its forward part, roll from its side part in the drone's frame), in degrees per uu/s^2 (task-249-01; provisional: 15 deg at the miner's 1000 uu/s^2). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0))
	float TiltDegreesPerAcceleration = 0.015f;

	/** Largest pitch or roll of the drone, gestures included, in degrees (task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0, ClampMax = 90, Units = "deg"))
	float MaxTiltDegrees = 15.f;

	/** Damping of the tilt toward its target (FMath::FInterpTo speed; task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0))
	float TiltInterpSpeed = 6.f;

	/** Smoothing of the measured miner acceleration (FMath::VInterpTo speed), so one-frame spikes do not jerk the tilt (task-249-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0))
	float AccelerationSmoothingSpeed = 10.f;

	/** Shortest time between two idle gestures while the miner rests in Orbit, in seconds (drone gdd: 4 to 8 s at random; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0.1, Units = "s"))
	float IdleGestureMinInterval = 4.f;

	/** Longest time between two idle gestures while the miner rests in Orbit, in seconds (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0.1, Units = "s"))
	float IdleGestureMaxInterval = 8.f;

	/** Half width of the scan sweep gesture: the yaw swings this far to one side then the other, in degrees (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0, Units = "deg"))
	float ScanSweepDegrees = 35.f;

	/** Duration of the scan sweep gesture, in seconds (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0.1, Units = "s"))
	float ScanSweepDuration = 1.6f;

	/** Nose-down pitch at the middle of the nod gesture (the drone faces the miner during it), in degrees (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0, Units = "deg"))
	float NodPitchDegrees = 12.f;

	/** Duration of the nod gesture, in seconds (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0.1, Units = "s"))
	float NodDuration = 1.2f;

	/** Depth of the short dip gesture at its middle, in uu (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0, Units = "cm"))
	float DipDepth = 12.f;

	/** Duration of the short dip gesture, in seconds (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Alive", meta = (ClampMin = 0.1, Units = "s"))
	float DipDuration = 0.9f;

	/** While on the fallback point above the miner (no valid patrol candidate), a new pick is tried this often, in seconds (task-251-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Walls", meta = (ClampMin = 0.02, Units = "s"))
	float PatrolFallbackRetryInterval = 0.3f;

	/** Extra height of the fallback point just above the miner (no valid patrol candidate), added to the Orbit height, in uu (task-248-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Walls", meta = (ClampMin = 0, Units = "cm"))
	float EscortFallbackExtraHeight = 40.f;

	/** Margin kept between a patrol or hover point and the nearest block along the checked segments, in uu (task-248-01; provisional; the grey box drone is a 40 uu sphere). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Walls", meta = (ClampMin = 0, Units = "cm"))
	float DroneWallClearance = 20.f;

	/** Distance of the dig hover point from the wall face, along its outward normal (free side), in uu (task-248-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig", meta = (ClampMin = 0, Units = "cm"))
	float DigHoverFaceDistance = 90.f;

	/** Height of the dig hover point above the miner's floor (capsule bottom), in uu (task-248-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig", meta = (Units = "cm"))
	float DigHoverHeight = 130.f;

	/** Speed at which the drone moves toward its dig or hack hover point (FMath::VInterpTo speed). 0 = snap to it. Orbit uses the patrol follow instead. */
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

	/** Phase of the vertical bob in radians, in [0, 2*pi). */
	float EscortBobPhase = 0.f;

	/** Horizontal velocity of the drone in Orbit, in uu/s (Z = 0); reset to zero when the patrol resumes or restarts. */
	FVector PatrolVelocity = FVector::ZeroVector;

	/** Current patrol point as a 2D offset from the miner's location (moves with the miner); zero on the fallback. */
	FVector2D PatrolOffset = FVector2D::ZeroVector;

	/** True when no valid patrol candidate was found: the point is just above the miner, EscortFallbackExtraHeight higher. */
	bool bPatrolFallback = false;

	/** Time left before the patrol point is re-picked (random lifetime, or the fallback retry), in seconds. */
	float PatrolTimer = 0.f;

	/** Own random stream for the patrol picks (seeded in SetOwnerMiner), so they never consume the global FMath stream other systems and tests seed. */
	FRandomStream PatrolRandom;

	/** Phase of the fast vertical bob in radians, in [0, 2*pi). */
	float EscortBobSecondPhase = 0.f;

	/** Calm-to-alert height blend in [0, 1], smoothed toward 1 while an enemy is within EscortAlertRadius of the miner. */
	float EscortAlert = 0.f;

	/** True when the last interest refresh found a living enemy within EscortAlertRadius of the miner. */
	bool bEnemyNearMiner = false;

	/** Time left before the next point-of-interest refresh, in seconds. */
	float InterestRefreshTimer = 0.f;

	/** Moving point of interest (enemy, console, shuttle) found by the last refresh; null when none or when it is a deposit. */
	TWeakObjectPtr<AActor> InterestActor;

	/** Fixed point of interest (deposit block centre) found by the last refresh; used when bHasInterestLocation. */
	FVector InterestLocation = FVector::ZeroVector;

	/** True when the last refresh found a deposit block as the point of interest. */
	bool bHasInterestLocation = false;

	/** Yaw of the drone in degrees, turned toward the point of interest with delay (LookYawInterpSpeed). */
	float LookYaw = 0.f;

	/** Current pitch from the miner's acceleration, damped (degrees; negative = nose down). */
	float TiltPitch = 0.f;

	/** Current roll from the miner's acceleration, damped (degrees; positive = right side down). */
	float TiltRoll = 0.f;

	/** Miner velocity of the previous tick, for the acceleration estimate. */
	FVector PreviousMinerVelocity = FVector::ZeroVector;

	/** Smoothed miner acceleration on XY, in uu/s^2. */
	FVector SmoothedMinerAcceleration = FVector::ZeroVector;

	/** Gesture being played; None between gestures. */
	ERRDroneIdleGesture IdleGesture = ERRDroneIdleGesture::None;

	/** Time since the current gesture started, in seconds. */
	float IdleGestureTime = 0.f;

	/** Time left before the next gesture while the miner rests, in seconds; drawn again in [IdleGestureMinInterval, IdleGestureMaxInterval] whenever the miner moves or a gesture starts. */
	float IdleGestureCountdown = 0.f;

	/** Own random stream for the gestures (seeded in SetOwnerMiner), so they never consume the global FMath stream other systems and tests seed. */
	FRandomStream GestureRandom;

	/** Interest lookups, every InterestRefreshInterval: nearest living enemy within InterestEnemyRadius of the miner, else the nearest standing deposit block or objective within InterestSiteRadius; also sets bEnemyNearMiner. Requires a valid miner. */
	void RefreshInterest();

	/** Before the state tick: refresh timer, alert blend, fast bob phase and idle gesture timing (gestures only in Orbit with the miner at rest). Requires a valid miner. */
	void UpdateLiveliness(float DeltaSeconds);

	/** Draws the time to the next idle gesture in [IdleGestureMinInterval, IdleGestureMaxInterval]. */
	void ResetIdleGestureCountdown();

	/** Orbit slot height above the miner's floor: calm-to-alert height, both bobs and the dip gesture, clamped to [EscortMinHeight, EscortMaxHeight]. */
	float GetEscortHeight() const;

	/** Envelope of the current gesture: sin(pi * progress) in [0, 1], 0 without a gesture. */
	float GetIdleGestureEnvelope() const;

	/** World point the drone looks at now: dig impact point in Dig, hack target in Hack, the miner during a nod, else the point of interest or the miner. Requires a valid miner. */
	FVector GetLookTarget() const;

	/** After the state tick: turns the yaw toward GetLookTarget (plus the scan sweep; Dig and Hack at BaseYawTurnRate, else the LookYawInterpSpeed delay capped by it), tilts from the miner's acceleration (plus the nod), damped and clamped, and sets the actor rotation. Requires a valid miner. */
	void UpdateAttitude(float DeltaSeconds);
};
