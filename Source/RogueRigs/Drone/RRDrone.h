// Rogue Rigs - the helper drone: separate actor spawned by the miner; escorts it (Orbit state), cuts an arc of blocks with a laser (dig) and links to an objective (hack).

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
	/** Escorts the miner (default): holds a slot beside and behind it on a damped spring, slow figure-eight at rest (task-247-01). */
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
 * Orbit (escort movement, task-247-01, the state keeps its name): the escort heading turns toward the miner's horizontal
 * velocity direction (EscortHeadingTurnRate, only above EscortHeadingMinSpeed; kept at rest). The slot sits EscortDistance
 * from the miner at EscortRestAngleDegrees from the heading on the escort side at rest (the shoulder), EscortMoveAngleDegrees
 * when moving (behind and to the side), blended smoothly by the miner's speed fraction. At rest a slow horizontal
 * figure-eight (EscortFigureEightPeriod, long axis tangential) is added around the slot; height above the
 * miner's floor (capsule bottom) from GetEscortHeight (task-249-01, see Alive). The drone follows the slot with a damped spring (EscortSpringFrequency,
 * EscortDampingRatio, part of the miner velocity fed forward), so it lags a little and overshoots on stop.
 * Walls (task-248-01, owner 2026-10-20): the drone never hovers over a block. The escort tries candidate slots in order (the
 * ideal slot, +-EscortAlternateAngleDegrees, +-EscortWideAngleDegrees from it, then straight behind the miner); a candidate is
 * clear when the 2D segment from the miner to DroneWallClearance past it, and the figure-eight span across it, cross no block
 * cell (ARRArenaGrid::HasLineOfSight). The first clear one is used; none = just above the miner, EscortFallbackExtraHeight
 * higher. A clear slot is held at least EscortSlotChangeDelay; a slot that becomes blocked is left at once. As a last guard, a
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
	 * Sets the miner the drone follows, makes the drone tick after it and snaps the drone to its rest escort slot (heading =
	 * the miner's facing, spring at rest). Called by ARRMiner right after spawning the drone. Null stops all movement.
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

	/** Runs the behaviour of the current state (Orbit: escorts the miner; Dig: hovers, cuts, draws the laser; Hack: hovers, draws the link). */
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

	/** Orbit tick (escort): updates the heading, the move blend and the figure-eight and bob phases, then moves the drone toward the escort slot on the damped spring. */
	void TickOrbit(float DeltaSeconds);

	/** Dig tick: checks the cancel rules, drops destroyed blocks, moves to the arc centre, updates the laser and damages every dug block. */
	void TickDig(float DeltaSeconds);

	/** Hack tick: back to Orbit when the target or miner is gone, else moves to the hack hover point and draws the link. */
	void TickHack(float DeltaSeconds);

	/** Leaves Hack: hides the laser, clears the target and resumes the escort from where the drone is. */
	void EndHack();

	/** Resumes the escort from the drone's current location: the spring starts at rest (zero velocity) and pulls the drone to the slot, so it never snaps; the escort slot is chosen afresh. */
	void ResumeOrbit();

	/**
	 * World position of the escort slot for the current heading, move blend, phases and EscortSlotIndex (task-247-01,
	 * task-248-01): EscortDistance from the miner at the candidate yaw (GetEscortCandidateYaw), plus the rest figure-eight
	 * scaled by (1 - move blend), at GetEscortHeight above the miner's floor (task-249-01). The fallback slot is the miner's XY,
	 * EscortFallbackExtraHeight higher, without figure-eight. Requires a valid miner.
	 */
	FVector ComputeOrbitPoint() const;

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
	 * Yaw in degrees of escort candidate SlotIndex for the current heading and move blend: 0 the ideal slot, 1 and 2 the ideal
	 * +- EscortAlternateAngleDegrees (toward behind first), 3 and 4 the ideal +- EscortWideAngleDegrees (same order), 5 straight
	 * behind the miner (heading + 180). Not used for EscortFallbackSlotIndex.
	 */
	float GetEscortCandidateYaw(int32 SlotIndex) const;

	/**
	 * True when escort candidate SlotIndex is clear on Grid: the 2D segment from the miner to DroneWallClearance past the slot,
	 * and the segments from the slot across the rest figure-eight span (tangential half width scaled by 1 - move blend, plus
	 * DroneWallClearance) on both sides, cross no block cell. The fallback slot (above the miner) is always clear; so is every
	 * slot without a grid. Requires a valid miner.
	 */
	bool IsEscortSlotClear(const ARRArenaGrid* Grid, int32 SlotIndex) const;

	/** First clear escort candidate (0..5, see GetEscortCandidateYaw); EscortFallbackSlotIndex when none is clear. Requires a valid miner. */
	int32 SelectEscortSlot() const;

	/**
	 * Escort slot choice, every Orbit tick: switches to the best clear candidate when the current slot is blocked (at once) or
	 * when it has been held for at least EscortSlotChangeDelay (so the drone does not hesitate on a corner). Requires a valid miner.
	 */
	void UpdateEscortSlot(float DeltaSeconds);

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

	/** Horizontal distance from the miner's location to the escort slot, in uu (drone gdd: 110-140 band; provisional, 2026-10-20). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm"))
	float EscortDistance = 125.f;

	/** Slot angle from the escort heading at rest, toward the escort side, in degrees: 90 = beside the miner, 100 = the shoulder, a little behind (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, ClampMax = 180, Units = "deg"))
	float EscortRestAngleDegrees = 100.f;

	/** Slot angle from the escort heading at full speed, toward the escort side, in degrees: 145 = behind and to the side (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, ClampMax = 180, Units = "deg"))
	float EscortMoveAngleDegrees = 145.f;

	/** Escort side: true = the miner's right of its movement heading (+90 degrees of yaw), false = its left (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit")
	bool bEscortOnRightSide = true;

	/** Turn rate of the escort heading toward the miner's velocity direction, in degrees per second, so the slot swings around the miner instead of snapping (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "deg/s"))
	float EscortHeadingTurnRate = 360.f;

	/** Horizontal miner speed below which the escort heading is kept (the miner is at rest), in uu/s (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm/s"))
	float EscortHeadingMinSpeed = 50.f;

	/** Speed of the move blend (rest slot to move slot, figure-eight fading) toward the miner's speed fraction of its max speed (FMath::FInterpTo speed; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0))
	float EscortBlendSpeed = 4.f;

	/** Period of the rest figure-eight around the slot, in seconds (drone gdd: about 6 s; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0.1, Units = "s"))
	float EscortFigureEightPeriod = 6.f;

	/** Half width of the figure-eight along its long axis, tangential to the miner (perpendicular to the miner-to-slot line), in uu (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm"))
	float EscortFigureEightTangential = 30.f;

	/** Half width of the figure-eight along its short axis, radial (toward or away from the miner), in uu; keeps the rest distance near 115-137 uu (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, Units = "cm"))
	float EscortFigureEightRadial = 10.f;

	/** Natural frequency of the follow spring, in Hz: higher follows tighter (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0.01, Units = "Hz"))
	float EscortSpringFrequency = 1.5f;

	/** Damping ratio of the follow spring: 1 = critically damped, below 1 overshoots a little on stop (provisional: 0.75, slightly under-damped). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0))
	float EscortDampingRatio = 0.75f;

	/** Share of the miner's velocity the spring damps toward (feed-forward): 1 = no steady lag behind the slot, 0 = full spring lag (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Orbit", meta = (ClampMin = 0, ClampMax = 1))
	float EscortVelocityFeedForward = 0.8f;

	/** Distance from the slot beyond which the drone snaps to it and the spring stops (miner teleported), in uu (provisional). */
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

	/** Speed at which the drone's yaw turns toward the point of interest (FMath::FInterpTo speed on the yaw delta): the delay of the look (task-249-01; provisional). */
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

	/** Angle of the first alternate escort slots, either side of the ideal slot, in degrees (task-248-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Walls", meta = (ClampMin = 0, ClampMax = 180, Units = "deg"))
	float EscortAlternateAngleDegrees = 45.f;

	/** Angle of the second alternate escort slots, either side of the ideal slot, in degrees (task-248-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Walls", meta = (ClampMin = 0, ClampMax = 180, Units = "deg"))
	float EscortWideAngleDegrees = 90.f;

	/** Minimum time a clear escort slot is held before the drone switches to a better one, in seconds (task-248-01; provisional). A blocked slot is left at once. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Walls", meta = (ClampMin = 0, Units = "s"))
	float EscortSlotChangeDelay = 0.3f;

	/** Extra height of the fallback slot just above the miner (no clear candidate), added to HoverHeight, in uu (task-248-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Walls", meta = (ClampMin = 0, Units = "cm"))
	float EscortFallbackExtraHeight = 40.f;

	/** Margin kept between a slot or hover point and the nearest block along the checked segments, in uu (task-248-01; provisional; the grey box drone is a 40 uu sphere). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Walls", meta = (ClampMin = 0, Units = "cm"))
	float DroneWallClearance = 20.f;

	/** Distance of the dig hover point from the wall face, along its outward normal (free side), in uu (task-248-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig", meta = (ClampMin = 0, Units = "cm"))
	float DigHoverFaceDistance = 90.f;

	/** Height of the dig hover point above the miner's floor (capsule bottom), in uu (task-248-01; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drone|Dig", meta = (Units = "cm"))
	float DigHoverHeight = 130.f;

	/** Speed at which the drone moves toward its dig or hack hover point (FMath::VInterpTo speed). 0 = snap to it. Orbit uses the escort spring instead. */
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

	/** Escort heading yaw in degrees: the miner's last movement direction (its facing until it first moves). */
	float EscortHeadingYaw = 0.f;

	/** Move blend in [0, 1]: 0 = rest slot with the full figure-eight, 1 = move slot behind and to the side, no figure-eight. */
	float EscortMoveBlend = 0.f;

	/** Phase of the rest figure-eight in radians, in [0, 2*pi). */
	float EscortFigureEightPhase = 0.f;

	/** Phase of the vertical bob in radians, in [0, 2*pi). */
	float EscortBobPhase = 0.f;

	/** Velocity of the drone on the escort spring, in uu/s; reset to zero when the escort resumes. */
	FVector EscortVelocity = FVector::ZeroVector;

	/** Escort slot index meaning "just above the miner" (no clear candidate). */
	static constexpr int32 EscortFallbackSlotIndex = 6;

	/** Current escort slot: a candidate of GetEscortCandidateYaw (0 = ideal) or EscortFallbackSlotIndex. */
	int32 EscortSlotIndex = 0;

	/** Time the current escort slot has been held, in seconds (reset on every slot change). */
	float EscortSlotHoldTime = 0.f;

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

	/** After the state tick: turns the yaw toward GetLookTarget (plus the scan sweep), tilts from the miner's acceleration (plus the nod), damped and clamped, and sets the actor rotation. Requires a valid miner. */
	void UpdateAttitude(float DeltaSeconds);
};
