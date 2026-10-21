// Rogue Rigs - pickup actor dropped by ore and heal ore blocks and by dying robots: pops out in an arc, rests, then is magnetized into the miner within its pickup radius.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pool/RRPoolable.h"
#include "RRPickup.generated.h"

class ARRMiner;
class UGameplayEffect;
class UPhysicalMaterial;
class UStaticMesh;
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
	/** Kinematic ejection arc with a light spin (task-229-02); without physics it ends with a small kinematic bounce (PopDuration). With bAttractDuringPop the magnet may latch mid-air once PopAttractDelay has elapsed since spawn. */
	Pop,
	/** Landed: a Chaos rigid body rolling then sleeping on the floor (task-229-02), or lying at its rest point without physics; attraction allowed once AttractDelay has elapsed (or, with bAttractDuringPop, PopAttractDelay since spawn). */
	Rest,
	/** Accelerating toward the miner until collected. */
	Magnet,
	/** Collected: released to the actor pool (or destroyed outside it) this frame. */
	Collected
};

/**
 * Start orientation of a pickup, drawn once per life when it is ejected (task-192-01). Since task-229-02 it is the orientation
 * the flight spin starts from; the landed orientation is decided by physics (a pickup without a physics body keeps the spun
 * orientation). BP_PickupXP and BP_PickupXPBig still turn to face the miner on magnet start.
 */
UENUM(BlueprintType)
enum class ERRPickupRestOrientation : uint8
{
	/** Rotation left as spawned (zero for grid and robot drops, restored to zero by the pool): today's look. */
	None,
	/** Random yaw only, pitch and roll zero: the pickup keeps its upward face (XP chip, one interesting face). */
	YawOnly,
	/** Random pitch, roll and yaw (uniform over every orientation): chunks that land in any position (ore). */
	Full
};

/**
 * A pickup lying on the arena floor (xp-level-up: DRGS-like ejection then suction). On spawn it is ejected along a random
 * horizontal direction (or the one given by SetPopYaw) to a distance in [PopMinDistance, PopMaxDistance]: an arc of
 * PopArcHeight then a small bounce of PopBounceHeight, over PopDuration in total, ending HoverHeight above the spawn Z.
 * PopFallTimeRatio (task-195-01) makes the main arc fall slower than it rises (floaty DRGS-like pop; 1 = symmetric arc).
 * With bAttractDuringPop the magnet may latch mid-air once PopAttractDelay has elapsed since spawn (same radius, heal
 * refusal and latch rules as from the rest; the rest of the pop is dropped and the pickup flies straight to the miner).
 * It then rests; once AttractDelay has elapsed after the pop, as soon as its horizontal distance to the player miner is
 * within the miner's PickupRadius attribute it latches the magnet: it flies straight at the miner, starting at
 * MagnetStartSpeed and accelerating by MagnetAcceleration up to MagnetMaxSpeed (nearby cubes form a stream), even if the
 * miner moves away. A Heal pickup is ignored while the miner is at full health (IsRefusedByMiner): it stays where it is
 * and is attracted once the miner is damaged and within its radius. Within CollectDistance it is collected: logs its type (Verbose), an XP pickup adds XPAmount to the
 * miner progression (scaled by XPGain there), an Ore pickup adds OreValue to URRRunFlowSubsystem::AddCarriedOre, ARRMiner::NotifyPickupCollected broadcasts the type and the location, then
 * it returns to URRActorPoolSubsystem when the pool created it (task-092-01: URRPickupSubsystem::RequestSpawn and
 * RequestXPCube acquire every grid and robot drop from the pool), else it destroys itself. Pool reset (IRRPoolable):
 * OnPoolReleased unregisters from the pickup subsystem, deactivates every FX component (BP_PickupXP TrailFX) at once and
 * resets motion state (phase, magnet latch and speed, pop timers) and per-drop values (XPAmount, OreValue, HealAmount back
 * to the class defaults, pop yaw cleared); OnPoolAcquired starts a new life (pop from the new location, registration) after
 * the caller set the values, and re-activates auto-activate FX components. The pool resets the rotation (BP_PickupXP faces
 * the miner on magnet start). Glow fade (bGlowFade, task-148-01): a 0..1 value written to PickupMesh Custom Primitive Data
 * at GlowFadeDataIndex, 0 in the air, fading to 1 over GlowFadeDuration on landing and back to 0 on magnet; advanced in
 * UpdateMotion only while it moves, reset to 0 at each life start and pool release. Look per life (task-192-01): StartLife
 * picks one mesh of MeshVariants for PickupMesh (empty = keep the Blueprint mesh) and sets the actor rotation from
 * RestOrientation (None = unchanged); the PickupMesh override materials are kept across a mesh swap. Pulse desync
 * (task-257-02): Ore and Heal pickups write a seeded pulse phase and rate (FRRPulseDesync of PulseSeed and the world's pulse
 * spawn id) to PickupMesh Custom Primitive Data PulsePhaseDataIndex / PulseRateDataIndex each life, neutral 0 / 1 written back
 * at pool release; XP pickups never write them (material defaults). Physics (task-229-02):
 * PickupMesh uses the "Pickup" collision profile (physics only, blocks WorldStatic = the grid floor and rock collision,
 * ignores pawns, robots, projectiles, other pickups; no query, no overlap, no hit event). The pop stays kinematic and spins
 * at PopSpinDegPerSec about a random axis; with bPhysicsOnLanding (and rr.Pickup.Physics 1, and a physics body) the pop ends
 * at the touch-down of the main arc: PickupMesh simulates Chaos physics from the arc's landing velocity (scaled by
 * LandingHorizontalSpeedScale / LandingVerticalSpeedScale) and the spin as angular velocity, with the Physics* damping,
 * friction, restitution and sleep tunables, and the Rest phase starts there. The magnet latch, pool release and the fall
 * guard (PhysicsFallLimit) turn physics off; a refused heal in flight falls back under physics. Without physics the kinematic
 * bounce and the rest point apply as before. Movement is driven by URRPickupSubsystem in one loop per frame (actor tick
 * only as a fallback when the subsystem is missing). Spawned by ARRArenaGrid at a cell center (floor top), and by ARREnemy
 * at its feet on death (XP, split over several cubes). Subclass in Blueprint: set PickupType and the mesh.
 */
UCLASS(Abstract, Blueprintable)
class ROGUERIGS_API ARRPickup : public AActor, public IRRPoolable
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

	/** Big XP block class the cubes of this class merge into (null = never merged); read on the class default by URRPickupSubsystem. */
	TSubclassOf<ARRPickup> GetXPBigPickupClass() const { return XPBigPickupClass; }

	/** Ore value of an Ore pickup (ore-economy: 1 per chunk), set by the grid from the ore type table (ChunkValue). Ignored by other types. */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	int32 GetOreValue() const { return OreValue; }

	/** Health restored on collection by a Heal pickup (0 = none). */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	float GetHealAmount() const { return HealAmount; }

	/** Sets the raw XP given on collection (ARREnemy splits a robot's XP over its cubes). Call before collection, ideally before BeginPlay (deferred spawn). */
	void SetXPAmount(float InXPAmount) { XPAmount = FMath::Max(InXPAmount, 0.f); }

	/** Sets the ore value of an Ore pickup (clamped at 0). Call before collection, ideally before BeginPlay (deferred spawn); the grid passes the ore type ChunkValue. */
	void SetOreValue(int32 InOreValue) { OreValue = FMath::Max(InOreValue, 0); }

	/** Sets the health a Heal pickup restores (clamped at 0). Call before collection, ideally before BeginPlay (deferred spawn); the grid passes the ore type HealValue. */
	void SetHealAmount(float InHealAmount) { HealAmount = FMath::Max(InHealAmount, 0.f); }

	/** Forces the ejection direction (world yaw in degrees) instead of a random one. Call before BeginPlay (deferred spawn); ARREnemy spreads its cubes evenly. */
	void SetPopYaw(float InYawDegrees) { PopYawDegrees = InYawDegrees; bHasPopYaw = true; }

	/** True between the start of a life (BeginPlay or OnPoolAcquired) and its end (OnPoolReleased). */
	bool IsLifeStarted() const { return bLifeStarted; }

	/** Pulse spawn id of the current life (task-257-02; Ore and Heal pickups only), INDEX_NONE for XP pickups and between lives. */
	int32 GetPulseSpawnId() const { return PulseSpawnId; }

	/** True for the pickup types whose glow pulse is desynchronised (Ore and Heal; XP keeps the neutral material values). */
	bool UsesPulseDesync() const { return PickupType == ERRPickupType::Ore || PickupType == ERRPickupType::Heal; }

	/** IRRPoolable: starts a new life at the current location (values already set by the caller) and re-activates auto-activate FX. */
	virtual void OnPoolAcquired_Implementation() override;

	/** IRRPoolable: unregisters, stops every FX component and resets motion state and per-drop values (see the class comment). */
	virtual void OnPoolReleased_Implementation() override;

	/** World location where the pop started (spawn point). */
	const FVector& GetPopStart() const { return PopStart; }

	/** World rest location at the end of the pop (without physics; with physics the pickup settles wherever it rolls). */
	const FVector& GetPopEnd() const { return PopEnd; }

	/** True while PickupMesh simulates physics (landed and not yet asleep, latched or released; task-229-02). */
	bool IsSimulatingPhysics() const;

	/**
	 * Builds the physical material of this pickup class from PhysicsFriction, PhysicsRestitution and the sleep thresholds
	 * (task-229-02), outered to Outer. Called on the class default object by URRPickupSubsystem, which shares one per class.
	 */
	UPhysicalMaterial* MakePhysicalMaterial(UObject* Outer) const;

protected:
	/** Turns shadow casting off on every primitive component, Blueprint ones included (task-263-01: only rocks cast; holds across pool reuse). */
	virtual void PostInitializeComponents() override;

	/** Turns collision off on every primitive component (Blueprint ones included) except PickupMesh, which gets the physics setup (ConfigurePhysicsBody); then StartLife. */
	virtual void BeginPlay() override;

	/**
	 * task-229-02: collision profile PhysicsCollisionProfile (NoCollision when bPhysicsOnLanding is false), no overlap, no hit
	 * event, damping, depenetration cap and the physical material of the class (shared by URRPickupSubsystem). Once per actor, in BeginPlay; the body stays kinematic until landing.
	 */
	void ConfigurePhysicsBody();

	/** True when the landing hands the pickup to physics: bPhysicsOnLanding, rr.Pickup.Physics 1 and a valid physics body on PickupMesh. */
	bool CanSimulateOnLanding() const;

	/** Enables physics on PickupMesh with LinearVelocity (uu/s) and, when bWithSpin, the flight spin as angular velocity. */
	void StartPhysics(const FVector& LinearVelocity, bool bWithSpin);

	/** Zeroes the velocities and turns physics off on PickupMesh (kinematic again, where it is); no-op when not simulating. */
	void StopPhysics();

	/** Location on the pop path Elapsed seconds after the life started (no side effect; Elapsed clamped to [0, PopDuration]). */
	FVector EvaluatePop(float Elapsed) const;

	/** Moves the actor to Location along the pop and turns it by the flight spin for DeltaSeconds (teleport, no sweep). */
	void ApplyPopStep(const FVector& Location, float DeltaSeconds);

	/**
	 * Starts a life once (no-op while one is running): picks the pop target from the current location, resets the motion
	 * state and registers with URRPickupSubsystem (else enables the actor tick). Called by BeginPlay and OnPoolAcquired.
	 */
	void StartLife();

	/**
	 * Draws the look of a new life (task-192-01), called by StartLife before the pop starts: a random entry of MeshVariants
	 * on PickupMesh (null entries skipped; nothing when the list is empty or holds no valid mesh) and the actor rotation from
	 * RestOrientation (None leaves it untouched). Override materials of PickupMesh are not touched, so they apply to every variant.
	 */
	void ApplyLifeLook();

	/** Unregisters from URRPickupSubsystem. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Advances the pop arc with the flight spin; returns true once the pop is finished: at the touch-down of the main arc when
	 * CanSimulateOnLanding (physics started there with the arc's landing velocity), else after the kinematic bounce.
	 */
	bool UpdatePop(float DeltaSeconds);

	/** Returns the player miner (player 0 pawn), cached once found; null when the player pawn is not a miner. Fallback tick only. */
	ARRMiner* FindMiner();

	/**
	 * heal-ore (owner 2026-10-10): true when this is a Heal pickup and Miner is at full health (Health >= MaxHealth). Such a
	 * pickup is neither attracted nor collected: it rests where it is until the miner is damaged and within PickupRadius.
	 * False for every other type, for a null miner and for a miner without a health set. Read only by Heal pickups each frame.
	 */
	bool IsRefusedByMiner(const ARRMiner* Miner) const;

	/** Leaves the Magnet phase when a flying Heal pickup is refused (the miner got back to full health): back in Rest where it is, falling under physics from zero velocity when CanSimulateOnLanding, else snapped to its rest height (attracted again as soon as the miner is damaged and in radius). */
	void ReturnToRest();

	/** Sets the glow fade target (0 or 1) when bGlowFade; no-op otherwise. Called on each phase change. */
	void SetGlowFadeTarget(float InTarget) { if (bGlowFade) { GlowFadeTarget = InTarget; } }

	/** Moves GlowFade toward GlowFadeTarget at 1 / GlowFadeDuration per second and writes it to PickupMesh. Called by UpdateMotion only while they differ. */
	void UpdateGlowFade(float DeltaSeconds);

	/** Sets GlowFade and GlowFadeTarget to 0 and writes 0 to PickupMesh (bGlowFade only): start of a life and pool release, so a reused pickup never flashes. */
	void ResetGlowFade();

	/**
	 * task-257-02: for Ore and Heal pickups (UsesPulseDesync), draws the pulse spawn id of this life (URRPickupSubsystem::
	 * DrawPulseSpawnId, or a per-actor life count without the subsystem) and writes FRRPulseDesync phase and rate of
	 * (PulseSeed, id) to PickupMesh Custom Primitive Data PulsePhaseDataIndex and PulseRateDataIndex. XP: nothing written. Called by StartLife.
	 */
	void ApplyPulseDesync();

	/** task-257-02: Ore and Heal pickups write the neutral phase 0 and rate 1 back and clear the spawn id (pool release); XP: nothing written. */
	void ResetPulseDesync();

	/**
	 * Latches the magnet when Miner is within PickupRadiusSquared (2D) and does not refuse this pickup: Magnet phase,
	 * glow target 0, MagnetStartSpeed, OnMagnetStart, then a first FlyToMiner step. Returns true when latched. Used from
	 * Rest and, with bAttractDuringPop, from Pop.
	 */
	bool TryLatchMagnet(ARRMiner* Miner, float PickupRadiusSquared, float DeltaSeconds);

	/** Life time in seconds (since StartLife) for the spawn-based delay: PopElapsed + RestElapsed (magnet flight not counted). */
	float GetLifeElapsed() const { return PopElapsed + RestElapsed; }

	/** Accelerates toward the miner; collects when within CollectDistance or when the step reaches it. A refused Heal pickup (IsRefusedByMiner) returns to Rest instead. */
	void FlyToMiner(ARRMiner* Miner, float DeltaSeconds);

	/** Heal pickup: applies HealEffectClass with SetByCaller Data.Heal = HealAmount to Miner (skipped when 0, dead or no ASC). */
	void ApplyHeal(ARRMiner* Miner) const;

	/** Logs the type, adds XPAmount XP to the miner for an XP pickup (OreValue carried ore to the run flow for an Ore pickup), notifies the miner (type, location), then releases this pickup to the pool (destroys it outside the pool). */
	void Collect(ARRMiner* Miner);

	/** Called once when the magnet latches (start the trail). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnMagnetStart();

	/** Called once on the collection frame, at the collection location, just before the pickup is released or destroyed (spawn a detached sparkle here, or bind ARRMiner::OnPickupCollectedAt). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnCollected(FVector Location);

	/** Visual root and physics body (task-229-02: "Pickup" profile, physics only, floor and walls only; the miner is still found by distance, no query). Assign the mesh in the Blueprint; its simple collision is the body. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	/** What this pickup gives when collected (set in each Blueprint subclass). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	ERRPickupType PickupType = ERRPickupType::Ore;

	/** Raw XP given by an XP pickup on collection (before the miner XPGain); set per cube by ARREnemy. Ignored by other types. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = 0))
	float XPAmount = 1.f;

	/**
	 * Big XP block (task-110-01, xp-level-up): pooled pickup class that groups of XP cubes of this class merge into when more
	 * than XPMergeThreshold of them fall in one URRPickupSubsystem bucket in a frame (its XPAmount = the sum of the group).
	 * Set on the small cube Blueprint (BP_PickupXP class defaults); null = no merge for this class (feature off).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|XP")
	TSubclassOf<ARRPickup> XPBigPickupClass;

	/** Ore value of an Ore pickup (ore-economy, owner 2026-10-10: 1 per chunk); the grid sets it from OreTypes.csv ChunkValue. Ignored by other types. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = 0))
	int32 OreValue = 1;

	/**
	 * Health a Heal pickup restores on collection, through HealEffectClass (SetByCaller Data.Heal, clamped to MaxHealth).
	 * The grid sets it from OreTypes.csv HealValue (row Heal: 15 per pickup, a 2 x 2 vein restores 60; heal-ore, owner
	 * 2026-10-10). 0 by default: a heal pickup spawned outside the grid heals nothing. Only read by Heal pickups.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Heal", meta = (ClampMin = 0))
	float HealAmount = 0.f;

	/** Instant effect applied to the miner by a Heal pickup (URRGE_Heal by default). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Heal")
	TSubclassOf<UGameplayEffect> HealEffectClass;

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

	/**
	 * task-195-01 (owner 2026-10-15, provisional): fall time / rise time of the main arc. 1 (default) = today's symmetric arc,
	 * exactly unchanged; 2 = the apex comes at one third of the arc and the descent lasts twice the ascent (fall gravity 1 / 4
	 * of the rise one). The arc keeps its duration ((1 - PopBounceFraction) x PopDuration), height and spatial path: only the
	 * timing along it changes (horizontal motion follows the same remapped time, so the pickup also drifts slower on the way down).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (ClampMin = 0.1, UIMin = 0.25, UIMax = 4))
	float PopFallTimeRatio = 1.f;

	/** Rest height above the spawn point (the floor top when spawned by the grid, the robot feet for XP). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (Units = "cm"))
	float HoverHeight = 30.f;

	/** Seconds the pickup rests after the pop ends before it may be attracted to the miner (with bAttractDuringPop, PopAttractDelay since spawn may end the wait earlier). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Collect", meta = (ClampMin = 0, Units = "s"))
	float AttractDelay = 1.f;

	/**
	 * task-195-01 (owner 2026-10-15, provisional): lets the magnet latch during the pop (mid-air) once PopAttractDelay has
	 * elapsed since spawn (life start), with the same radius test, heal refusal, latch, OnMagnetStart and glow rules as from
	 * the rest. A latched pickup drops the rest of its pop and flies straight to the miner. On the ground the wait ends at
	 * AttractDelay after landing or PopAttractDelay after spawn, whichever comes first. False (default) = today: attraction
	 * only after AttractDelay from the end of the pop.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Collect")
	bool bAttractDuringPop = false;

	/** Seconds from spawn (life start: fresh spawn or pool reuse) before the magnet may latch, in the air or on the ground (bAttractDuringPop only). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Collect", meta = (ClampMin = 0, Units = "s", EditCondition = "bAttractDuringPop"))
	float PopAttractDelay = 1.2f;

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

	/**
	 * task-148-01 (owner 2026-10-13, provisional): drives a glow fade value 0..1 written to PickupMesh as Custom Primitive
	 * Data at GlowFadeDataIndex (the glow disc material of BP_PickupXP and BP_PickupXPBig reads it): 0 during Pop, fades to
	 * 1 when the pickup lands (start of Rest), back to 0 at the start of Magnet and Collected. False = no fade, no cost (ore, heal).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Glow")
	bool bGlowFade = false;

	/** Seconds the glow fade takes from 0 to 1 or from 1 to 0 (0 = instant). Provisional 0.2 s, owner 2026-10-13. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Glow", meta = (ClampMin = 0, Units = "s", EditCondition = "bGlowFade"))
	float GlowFadeDuration = 0.2f;

	/** Custom Primitive Data index of PickupMesh the glow fade is written to (the material scalar parameter uses the same index). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Glow", meta = (ClampMin = 0, EditCondition = "bGlowFade"))
	int32 GlowFadeDataIndex = 0;

	/**
	 * task-257-02 (owner 2026-10-20, provisional): seed of the pulse desync of Ore and Heal pickups. Combined with the pulse
	 * spawn id of each life by FRRPulseDesync: the same seed and spawn order give the same phases and rates.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Glow")
	int32 PulseSeed = 1;

	/** Lowest pulse rate drawn per life (scale of the material PulseFrequency; owner 2026-10-20: -20 percent, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Glow", meta = (ClampMin = 0.01))
	float PulseRateMin = 0.8f;

	/** Highest pulse rate drawn per life (owner 2026-10-20: +20 percent, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Glow", meta = (ClampMin = 0.01))
	float PulseRateMax = 1.2f;

	/** Custom Primitive Data index of PickupMesh receiving the pulse phase in turns (M_XPNutGlow PulsePhase, task-257-01). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Glow", meta = (ClampMin = 0))
	int32 PulsePhaseDataIndex = 1;

	/** Custom Primitive Data index of PickupMesh receiving the pulse rate (M_XPNutGlow PulseRate, task-257-01). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Glow", meta = (ClampMin = 0))
	int32 PulseRateDataIndex = 2;

	/**
	 * task-192-01 (owner 2026-10-15, provisional per Blueprint): rotation drawn once per life when the pickup is ejected
	 * (fresh spawn and every pool reuse). Since task-229-02 it is only the start of the flight spin; physics decides the landed
	 * orientation (never forced at landing). None (default) = rotation as spawned.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Look")
	ERRPickupRestOrientation RestOrientation = ERRPickupRestOrientation::None;

	/**
	 * task-192-01: meshes one of which is put on PickupMesh at random each life (fresh spawn and every pool reuse). Empty
	 * (default) = keep the Blueprint mesh. The PickupMesh override materials are kept, so every variant must share the slot
	 * layout they target (ore chunks: crystal_bright, crystal_dark); leave the overrides empty to use each mesh's own slot defaults.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Look")
	TArray<TObjectPtr<UStaticMesh>> MeshVariants;

	/**
	 * task-229-02 (owner 2026-10-17 "très légère rotation", provisional): spin speed in flight about a random axis drawn each
	 * life; also the angular velocity given to physics at landing. 0 = no spin.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Pop", meta = (ClampMin = 0, Units = "DegreesPerSecond"))
	float PopSpinDegPerSec = 90.f;

	/**
	 * task-229-02 (owner 2026-10-17 "vraie physique Chaos"): at the touch-down of the main arc PickupMesh becomes a Chaos rigid
	 * body that rolls and settles (the kinematic bounce is skipped). False = no physics body at all (NoCollision), kinematic
	 * bounce and rest point as before task-229. The console variable rr.Pickup.Physics 0 turns the hand-off off at runtime.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics")
	bool bPhysicsOnLanding = true;

	/** Collision profile of PickupMesh (Config/DefaultEngine.ini "Pickup": physics only, blocks WorldStatic only). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics", meta = (EditCondition = "bPhysicsOnLanding"))
	FName PhysicsCollisionProfile = TEXT("Pickup");

	/** Scale of the horizontal velocity of the arc at touch-down given to physics (1 = the arc's own speed, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics", meta = (ClampMin = 0, EditCondition = "bPhysicsOnLanding"))
	float LandingHorizontalSpeedScale = 1.f;

	/** Scale of the vertical (falling) velocity of the arc at touch-down given to physics (0 = horizontal part only, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics", meta = (ClampMin = 0, EditCondition = "bPhysicsOnLanding"))
	float LandingVerticalSpeedScale = 0.f;

	/** Linear damping of the landed body (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics", meta = (ClampMin = 0, EditCondition = "bPhysicsOnLanding"))
	float PhysicsLinearDamping = 0.1f;

	/** Angular damping of the landed body (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics", meta = (ClampMin = 0, EditCondition = "bPhysicsOnLanding"))
	float PhysicsAngularDamping = 0.5f;

	/** Friction of the pickup physical material (combined with the floor's by the project combine mode, average by default; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics", meta = (ClampMin = 0, EditCondition = "bPhysicsOnLanding"))
	float PhysicsFriction = 0.7f;

	/** Restitution of the pickup physical material (combined like the friction; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics", meta = (ClampMin = 0, ClampMax = 1, EditCondition = "bPhysicsOnLanding"))
	float PhysicsRestitution = 0.2f;

	/** Linear speed in uu/s under which the body counts toward sleep (engine material default 1; higher sleeps sooner; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics", meta = (ClampMin = 0, EditCondition = "bPhysicsOnLanding"))
	float PhysicsSleepLinearThreshold = 5.f;

	/** Angular speed in rad/s under which the body counts toward sleep (engine material default 0.05; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics", meta = (ClampMin = 0, EditCondition = "bPhysicsOnLanding"))
	float PhysicsSleepAngularThreshold = 0.2f;

	/** Physics frames under both thresholds before the body sleeps (engine material default 4). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics", meta = (ClampMin = 0, EditCondition = "bPhysicsOnLanding"))
	int32 PhysicsSleepCounterThreshold = 4;

	/** Cap in uu/s of the push out of an initial overlap (a pickup landing inside a rock block; provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics", meta = (ClampMin = 0, Units = "CentimetersPerSecond", EditCondition = "bPhysicsOnLanding"))
	float PhysicsMaxDepenetrationVelocity = 200.f;

	/**
	 * Fall guard: a landed pickup more than this far below its rest point (fell through a hole or the floor) stops physics and is
	 * put back at the rest point, kinematic. 0 = no guard (provisional 500 uu).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Physics", meta = (ClampMin = 0, Units = "cm", EditCondition = "bPhysicsOnLanding"))
	float PhysicsFallLimit = 500.f;

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

	/** Seconds elapsed in the pop (since the life started, while in Pop; reset per life in StartLife and OnPoolReleased). */
	float PopElapsed = 0.f;

	/** Seconds elapsed since the pop finished; attraction is allowed once it reaches AttractDelay. */
	float RestElapsed = 0.f;

	/** Current speed toward the miner (Magnet phase). */
	float MagnetSpeed = 0.f;

	/** Ejection yaw in degrees, used when bHasPopYaw. */
	float PopYawDegrees = 0.f;

	/** True when SetPopYaw chose the ejection direction. */
	bool bHasPopYaw = false;

	/** True while a life runs (see IsLifeStarted). */
	bool bLifeStarted = false;

	/** Current glow fade value 0..1, last written to PickupMesh (bGlowFade only). */
	float GlowFade = 0.f;

	/** Glow fade value GlowFade moves toward: 0 in Pop, Magnet and Collected, 1 in Rest (bGlowFade only). */
	float GlowFadeTarget = 0.f;

	/** Unit axis of the flight spin, drawn each life in StartLife (task-229-02). */
	FVector SpinAxis = FVector::UpVector;

	/** Pulse spawn id of the current life (ApplyPulseDesync), INDEX_NONE for XP and between lives. */
	int32 PulseSpawnId = INDEX_NONE;

	/** Lives started by this actor: the pulse spawn id when no URRPickupSubsystem exists. */
	int32 LocalLifeCount = 0;
};

