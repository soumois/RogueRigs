// Rogue Rigs - robot base: own ASC with health, movement and enemy sets, CSV stats, flow-field movement, single death with pooled puff, shrink, pooled debris and XP drop, floating damage numbers.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "Enemies/RRDebrisPiece.h"
#include "Enemies/RREnemyStatsRow.h"
#include "Pool/RRPoolable.h"
#include "RREnemy.generated.h"

class ARRMiner;
class ARRPickup;
class UAbilitySystemComponent;
class UCapsuleComponent;
class URRAbilitySystemComponent;
class URRBlobShadowComponent;
class URREnemyAttributeSet;
class URREnemyMovementComponent;
class URRHealthAttributeSet;
class URRMovementAttributeSet;
class UGameplayEffect;
class UMaterialInterface;
class UMeshComponent;
class UNiagaraSystem;
class UStaticMeshComponent;
class USceneComponent;
struct FOnAttributeChangeData;

/**
 * Base of every robot (sys-enemies). Owns its URRAbilitySystemComponent (owner and avatar: the robot) with the health,
 * movement and enemy AttributeSets. At BeginPlay it reads row StatsRowName of EnemyStats.csv: MaxHealth, Health and
 * MoveSpeed become base values (MoveSpeed is mirrored to CharacterMovement->MaxWalkSpeed), the rest is kept in Stats.
 * Moves without a controller: URREnemyMovementComponent feeds the flow-field direction to CharacterMovement (RVO
 * off since task-034-01: URRRobotSeparationSubsystem pushes overlapping robots apart). The root capsule has the chaser radius
 * against the world (TerrainCapsuleRadius); HitCapsule keeps the robot's own size for the miner's shots. HitCapsule overlaps the Projectile channel, so shots hit it through the ASC (sys-projectiles).
 * HitCapsule is a URRHitCapsuleComponent: it runs no overlap query of its own when the robot moves (task-093-01; projectiles find it).
 * Crowd: when bIgnoreOtherRobots is on, the capsule carries RRCollisionChannels::RobotMaskFilter and its movement sweeps ignore
 * that mask, so robots pass through each other (the light separation spaces them) while the miner, terrain and every query still
 * collide with them. At BeginPlay the robot also writes the crowd RVO settings to the world avoidance manager.
 * Light movement: CharacterMovement is URRRobotMovementComponent (light kinematic step on the flat arena behind
 * rr.Enemy.LightMovement, cached floor on the engine path); at BeginPlay the attached parts are made cheap to move
 * (bLightAttachedParts) and, with rr.Enemy.LightMovement on, the root capsule stops generating overlap events; a walking robot far from the miner and off screen moves at a slower
 * tick interval (FarMovementDistance, FarMovementTickInterval; UpdateMovementLOD).
 * Death (health set OnDeath, once, State.Dead on the ASC): stops, disables collision, plays the pooled DeathPuffSystem at its
 * centre (URRNiagaraPoolSubsystem, at most DeathPuffPoolSize), shrinks to 0 over DeathShrinkDuration, throws 2 to 4
 * DebrisPieces from the last hit direction through the pooled DebrisSystem (LaunchDebris, at most DebrisPoolSize),
 * then bursts XPCubeCount pickups of XPPickupClass at its feet (Stats.XPValue split over them) and goes back to the actor pool
 * (URRActorPoolSubsystem::Release; a robot not created by the pool is destroyed instead).
 * Pool (task-091-01, IRRPoolable): BeginPlay runs once per actor; every later life starts in OnPoolAcquired, which calls
 * ResetForNewLife (subclasses extend it): GAS effects and State.Dead removed, CSV stats and attributes back, death flags,
 * movement, far-movement LOD, visual parts (relative transforms, visibility, materials, light intensity captured at the first
 * life) restored, separation registration. OnPoolReleased leaves the separation and cancels latent actions. A released robot
 * keeps State.Dead while idle, so every IsDead filter also skips it. Each hit (the killing one too) is sent to
 * URRDamageNumberSubsystem at DamageNumberHeight above the capsule top (one pooled number per hit, starting in a DamageNumberSpawnRadius disc).
 * Blob shadow (task-264-02): BlobShadow, a flat dark disc attached under the capsule (diameter = own capsule radius x
 * BlobShadowDiameterScale), hidden when the death starts, shown again with the visual parts on pool reuse; never blinks with
 * the wind-up flash.
 * Baked animation clips (task-284-05, moved from ARRChaser): the static mesh component named AnimatedMeshName plays clips baked in
 * a texture (MF_RobotAnimWPO). The robot writes Custom Primitive Data 0 (clip id) and 1 (clip start, world seconds) on events
 * only, never per tick (SetAnimClip). LoopClipId loops from a random phase at BeginPlay and pool reuse; with RestClipId >= 0,
 * SetMovementActive switches between the loop (movement on) and the held rest clip (movement off). Subclasses add their own
 * clips on state events with PlayClip / HoldClipEnd and return with PlayMovementClip (default OnAnimClipEnded).
 * Subclasses implement their behavior in
 * UpdateBehavior (alive only) and may extend HandleDeath. Subclass in Blueprint for the mesh.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARREnemy : public ACharacter, public IAbilitySystemInterface, public IRRPoolable
{
	GENERATED_BODY()

public:
	/** Creates the ASC, the AttributeSets and the enemy movement component; sets collision and movement defaults. */
	ARREnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ IAbilitySystemInterface
	/** Returns the robot's ASC. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//~ IRRPoolable
	/**
	 * Start of a life handed out by the actor pool. First delivery: captures the visual parts. Every delivery after a release:
	 * ResetForNewLife, so the robot behaves like a fresh one.
	 */
	virtual void OnPoolAcquired_Implementation() override;

	/** Back to the pool: leaves the separation subsystem, stops the movement, undoes the death shrink, cancels latent actions (Blueprint Delay nodes). */
	virtual void OnPoolReleased_Implementation() override;

	/** Lives started from the actor pool (1 after the first delivery, +1 per reuse; 0 for a robot spawned outside the pool). */
	int32 GetPoolLifeCount() const { return PoolLifeCount; }

	/** Runs the death shrink when dead, else UpdateBehavior. */
	virtual void Tick(float DeltaSeconds) override;

	/** Reads row RowName of EnemyStats.csv into OutRow. Returns false (logged) when the file or the row is missing. */
	static bool LoadStatsRow(FName RowName, FRREnemyStatsRow& OutRow);

	/** Path of the enemy stats CSV, relative to FPaths::ProjectContentDir(). */
	static const TCHAR* EnemyStatsPath;

	/** Stats read from the CSV at BeginPlay (zeros before, or when the row is missing). */
	const FRREnemyStatsRow& GetStats() const { return Stats; }

	/** Row of EnemyStats.csv this robot reads (Chaser, Kamikaze...): the robot type name of the run log. */
	FName GetStatsRowName() const { return StatsRowName; }

	/** The robot health set (Health, MaxHealth, IncomingDamage; OnDamageReceived and OnDeath native events). */
	URRHealthAttributeSet* GetHealthSet() const { return HealthSet; }

	/** The flow-field movement component. */
	URREnemyMovementComponent* GetEnemyMovement() const { return EnemyMovement; }

	/** True once the robot died (its ASC has State.Dead). */
	UFUNCTION(BlueprintPure, Category = "Enemy")
	bool IsDead() const;

	/**
	 * True while the robot is dead for damage and targeting (State.Dead) but has not left the fight yet: a kamikaze killed by
	 * the player runs its fuse first (task-096-01). Such a robot still holds its spawner slot and class cap. Base: false.
	 */
	virtual bool IsDeathPending() const { return false; }

	/** True while the robot holds its spawner slot: not dead, or dead with a pending death (IsDeathPending). */
	bool HoldsSpawnSlot() const { return !IsDead() || IsDeathPending(); }

	/** Knockback resistance from the CSV stats, clamped to [0, 1] (0 = full knockback). */
	UFUNCTION(BlueprintPure, Category = "Enemy")
	float GetKnockbackResistance() const;

	/**
	 * Pushes a living robot horizontally: launches it (ACharacter::LaunchCharacter, XY overridden, Z kept) with Velocity's XY
	 * scaled by (1 - GetKnockbackResistance()), added to a launch still pending this frame (several shotgun pellets hitting in
	 * the same frame stack). Ignored when dead or when the scaled velocity is zero. Called by projectiles with a knockback (shotgun pellets).
	 */
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void ApplyKnockback(FVector Velocity);

	/** Target of the attacks: the first living ARRMiner of the world (cached); null when there is none. */
	ARRMiner* FindTarget();

	/**
	 * Lets the movement of this robot pass through other robots (true) or collide with them (false), applied at once: sets
	 * RobotMaskFilter on the capsule body and as the capsule move-ignore mask (both cleared when false). Called at BeginPlay with
	 * bIgnoreOtherRobots; tests call it to compare both crowds.
	 */
	void SetIgnoreOtherRobots(bool bIgnore);

	/** True when the capsule move sweeps currently ignore other robots. */
	bool IsIgnoringOtherRobots() const;

	/**
	 * Switches the far-movement level of detail (tick interval FarMovementTickInterval on CharacterMovement and EnemyMovement)
	 * on or off at once. Driven by UpdateMovementLOD; tests call it directly.
	 */
	void SetFarMovement(bool bFar);

	/** True while the movement runs at the far tick interval. */
	bool IsFarMovementActive() const { return bFarMovementActive; }

	/** Radius (uu) of the robot body: its own capsule radius before the terrain capsule shrink; used by the hit capsule; the separation scales it (GetSeparationRadius). */
	float GetBodyRadius() const { return BodyRadius; }

	/** Radius (uu) the separation uses: GetBodyRadius x SeparationRadiusScale (task-295-01). */
	float GetSeparationRadius() const { return BodyRadius * SeparationRadiusScale; }

	/** Capsule of the robot's own size that takes the miner's shots (Projectile overlap); the root capsule only moves. */
	UCapsuleComponent* GetHitCapsule() const { return HitCapsule; }

	/** Fake round shadow on the floor under the robot (task-264-02). */
	URRBlobShadowComponent* GetBlobShadow() const { return BlobShadow; }

	/**
	 * Teleports the robot to Location (the capsule center) for the spawner's recycling and resets its motion: velocity, pending
	 * knockback and movement input cleared, far-movement level of detail off, meshes shown, then ResetStateMachine (attack,
	 * wind-up, flare or aim cancelled). Class, health, stats and attack cadence are kept. Does nothing while dead or dying;
	 * returns false then.
	 */
	bool RelocateTo(const FVector& Location);

	/**
	 * Records the direction of a hit about to be applied (enemy-death debris): projectiles pass their flight velocity,
	 * explosions the radial direction from their centre. Kept horizontal and normalized; a zero direction, or a dead robot,
	 * changes nothing. Read when the debris is launched; cleared each life (ResetForNewLife).
	 */
	void RecordHitDirection(const FVector& Direction);

	/** Horizontal unit direction of the last recorded hit of this life; zero when none was recorded. */
	const FVector& GetLastHitDirection() const { return LastHitDirection; }

	/** Niagara user parameter: int array, debris mesh index of each thrown piece (FRRDebrisPiece::MeshIndex). */
	static const FName DebrisMeshIndexParameter;

	/** Niagara user parameter: vector array, world-space offset of each thrown piece from the system location (robot yaw applied). */
	static const FName DebrisOffsetParameter;

	/** Niagara user parameter: vector, horizontal unit direction the pieces are thrown toward. */
	static const FName DebrisHitDirectionParameter;

	/** XP pickup class dropped on death (the spawner prewarms it from the class defaults, task-111-01). */
	TSubclassOf<ARRPickup> GetXPPickupClass() const { return XPPickupClass; }

	/** Clip id last written to the animated mesh (Custom Primitive Data 0); -1 before any write or without animated mesh. Tests and debug. */
	float GetAnimClipId() const { return CurrentAnimClipId; }

	/** Start (world seconds) of the clip last written to the animated mesh (Custom Primitive Data 1). Tests and debug. */
	double GetAnimClipStart() const { return CurrentAnimClipStart; }

	/** Hit flash value last written to the animated mesh (Custom Primitive Data 2): 1 while flashing, else 0. Tests and debug. */
	float GetHitFlashValue() const { return bHitFlashOn ? 1.f : 0.f; }

protected:
	/**
	 * Per-life reset of a robot reused from the actor pool (called by OnPoolAcquired from the second life on). Base: every
	 * active gameplay effect removed, State.Dead cleared, CSV stats re-applied (MaxHealth, Health, MoveSpeed, Shield,
	 * AttackPower default), death state (bDying, bDeathFinished, DeathElapsed, drop point), movement (velocity, pending launch,
	 * input, default movement mode, movement on), far-movement LOD off, cached target, visual parts restored, separation
	 * registration. Overrides call Super first, then reset their own state machine and counters.
	 */
	virtual void ResetForNewLife();

	/** True when the dead robot may go back to the pool now (after the shrink and the XP drop). Kamikaze: once its blast left the queue. */
	virtual bool CanReleaseToPool() const { return true; }

	/** Captures the visual state of every attached scene component (relative transform, visibility, materials, light intensity). */
	void CaptureVisualParts();

	/** Restores the state captured by CaptureVisualParts (Blueprint events such as a broken shield shell or a slam pose undone). */
	void RestoreVisualParts();

	/** Returns the subclass state machine to its first state (Chase, Approach) after a relocation. Base: nothing. */
	virtual void ResetStateMachine() {}

	/**
	 * Sizes the hit capsule to the robot's own capsule (BodyRadius, half height), then shrinks the root capsule radius to
	 * TerrainCapsuleRadius, so every robot meets the rock like a chaser (enemies-index fourth test).
	 */
	virtual void PostInitializeComponents() override;

	/** Initializes the ASC, applies the CSV stats, enables controller-less walking and binds the health set events. */
	virtual void BeginPlay() override;

	/** Unbinds the health set and attribute events. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Behavior step of a living robot, called from Tick. Empty in the base. */
	virtual void UpdateBehavior(float DeltaSeconds) {}

	/**
	 * Health set death callback (runs inside the GE execution): stops movement, disables collision, stores the drop point and
	 * starts the shrink. Overrides call Super.
	 */
	virtual void HandleDeath(AActor* DeathInstigator);

	/** Applies Stats to the ASC (MaxHealth, Health, MoveSpeed). */
	void ApplyStats();

	/** ASC callback: MoveSpeed changed; pushes it to CharacterMovement->MaxWalkSpeed. */
	void HandleMoveSpeedChanged(const FOnAttributeChangeData& ChangeData);

	/**
	 * End of the death shrink: draws 2 to 4 meshes (equal odds) of DebrisPieces (FRRDebrisSelection::Choose) and plays
	 * DebrisSystem once through URRNiagaraPoolSubsystem (cap DebrisPoolSize) at the robot feet (DropLocation), rotated by the
	 * robot yaw, with User.PieceMeshIndex, User.PieceOffset (offsets rotated by the yaw, pieces at scale 1) and
	 * User.HitDirection (last hit, else a random horizontal direction). Nothing when DebrisSystem is null, DebrisPieces is
	 * empty, or the pool is full.
	 */
	void LaunchDebris();

	/** Advances the shrink; at its end launches the debris, drops the XP pickups, then releases the robot to the actor pool (destroys it outside the pool). */
	void UpdateDeath(float DeltaSeconds);

	/** Requests XPCubeCount pickups of XPPickupClass at DropLocation (no owner) from URRPickupSubsystem::RequestXPCube (actor pool, created within 3 frames; direct spawn without the subsystem), each worth Stats.XPValue / XPCubeCount, ejected in spread directions. Overridden to suppress the drop (kamikaze fuse). */
	virtual void DropXP();

	/** Health set hit callback (runs inside the GE execution): sends the hit to URRDamageNumberSubsystem (capsule top + DamageNumberHeight, disc DamageNumberSpawnRadius) and starts the hit flash. */
	void HandleDamageReceived(float Damage, bool bIsCritical, float HealthLeft);

	/** Sets the flash value to 1 on the animated mesh and (re)starts HitFlashTimer: a new hit restarts the flash. Nothing without animated mesh. */
	void StartHitFlash();

	/** Sets the flash value back to 0 and stops HitFlashTimer. */
	void EndHitFlash();

	/**
	 * Makes the attached parts cheap to move (bLightAttachedParts): an inherited CharacterMesh0 without skeletal mesh and
	 * without children stops ticking, colliding and rendering and goes fully absolute (skipped by move transform updates);
	 * every other non-capsule primitive with no collision stops generating overlap events.
	 */
	void ApplyLightAttachedParts();

	/**
	 * Every MovementLODCheckInterval s: far movement on when the robot is beyond FarMovementDistance from the miner and was not
	 * rendered during the last OffScreenGraceSeconds, off otherwise. Only in a world with a game viewport (never in tests).
	 */
	void UpdateMovementLOD(float DeltaSeconds);

	/**
	 * Wind-up flash (task-119-01). false hides every visible mesh component and remembers it; true shows again only the meshes
	 * a previous false hid. A mesh hidden by the Blueprint (effect meshes kept hidden at rest) is never turned on here.
	 */
	void SetMeshesVisible(bool bVisible);

	/**
	 * Turns the flow-field movement on or off. Off also drops the pending movement input and cuts the velocity at once, so a
	 * robot that stops to attack or telegraph does not slide. Subclass state machines call it when entering a state.
	 */
	void SetMovementActive(bool bActive);

	/** Finds the static mesh component named AnimatedMeshName and stores it in AnimatedMesh (null when none). Called at BeginPlay. */
	void ResolveAnimatedMesh();

	/**
	 * Writes the clip id (Custom Primitive Data AnimClipDataIndex) and its start in world seconds (AnimStartDataIndex) to the
	 * animated mesh in one render update. The only CPD write of the animation; nothing without animated mesh.
	 */
	void SetAnimClip(float ClipId, double StartTime);

	/** Loops ClipId from a random phase (start = now - uniform [0, LoopPhaseSpread)) so a crowd does not step in sync; stops the clip timer. */
	void PlayLoopClip(float ClipId);

	/**
	 * Plays ClipId from now (a clamped clip holds its last frame). Duration > 0: OnAnimClipEnded runs Duration seconds later
	 * (AnimClipTimer), unless another clip is played first; Duration <= 0: no timer. Use it on state events (attack, telegraph, aim).
	 */
	void PlayClip(float ClipId, double Duration);

	/** Holds the last frame of the clamped clip ClipId (start = now - AnimHoldLead); stops the clip timer. Used for the rest pose. */
	void HoldClipEnd(float ClipId);

	/** Plays the movement clip: RestClipId held when RestClipId >= 0 and the movement is off, else LoopClipId looped (new phase). */
	void PlayMovementClip();

	/** End of a timed clip (PlayClip with Duration > 0). Base: PlayMovementClip. Override to chain clips. */
	virtual void OnAnimClipEnded();

	/** True while a timed clip plays (its OnAnimClipEnded is pending). */
	bool IsTimedClipPlaying() const;

	/** True when Target is a living miner within Range (uu, 2D between actor centers, range inclusive). */
	bool IsTargetWithin(const ARRMiner* Target, float Range) const;

	/** Records the current world time as the start of a new state (StateStartTime). Subclasses call it in their EnterState. */
	void MarkStateStart();

	/** Seconds spent in the current state (world time minus StateStartTime). */
	double GetTimeInState() const;

	/** True once at least Duration seconds were spent in the current state (with TimeTolerance). */
	bool HasStateLasted(double Duration) const;

	/**
	 * Applies EffectClass from this robot's ASC to TargetASC with SetByCaller Data.Damage = Damage (source = this ASC, so
	 * AttackPower scales it). The target's tag requirements (i-frames, death) may refuse it. Returns false (logged when the spec
	 * could not be made) when nothing was applied.
	 */
	bool ApplyDamageSpecTo(UAbilitySystemComponent* TargetASC, TSubclassOf<UGameplayEffect> EffectClass, float Damage);

	/**
	 * Outgoing spec of EffectClass from this robot's ASC with SetByCaller Data.Damage = Damage (AttackPower scales it on
	 * execution). Invalid handle (logged) when the spec could not be made. Used by ApplyDamageSpecTo and by ranged robots, whose
	 * projectile carries the spec to the impact.
	 */
	FGameplayEffectSpecHandle MakeDamageSpec(TSubclassOf<UGameplayEffect> EffectClass, float Damage) const;

	/**
	 * Telegraph blink: meshes hidden during the first half of each cycle at Rate full hide/show cycles per second, measured from
	 * the state start. Rate <= 0 leaves the meshes as they are. Call every tick of the telegraph state; show the meshes again
	 * (SetMeshesVisible(true)) when leaving it.
	 */
	void UpdateTelegraphBlink(float Rate);

	/** Tolerance on state timers (s): absorbs float drift of the accumulated world time. */
	static constexpr double TimeTolerance = 1e-4;

	/** The robot's ability system component (owner and avatar: this robot). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<URRAbilitySystemComponent> AbilitySystemComponent;

	/** Health, MaxHealth, IncomingDamage. Registered on the ASC as a default subobject. */
	UPROPERTY()
	TObjectPtr<URRHealthAttributeSet> HealthSet;

	/** MoveSpeed. Registered on the ASC as a default subobject. */
	UPROPERTY()
	TObjectPtr<URRMovementAttributeSet> MovementSet;

	/** AttackPower (starts at 1). Registered on the ASC as a default subobject. */
	UPROPERTY()
	TObjectPtr<URREnemyAttributeSet> EnemySet;

	/** Asks the path provider (flow field) for a direction each tick and adds it as movement input. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URREnemyMovementComponent> EnemyMovement;

	/** Row of EnemyStats.csv read at BeginPlay. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Stats")
	FName StatsRowName;

	/** Pickup class dropped on death, Stats.XPValue times (a Blueprint subclass of ARRPickup with type XP). None = no drop. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Death")
	TSubclassOf<ARRPickup> XPPickupClass;

	/**
	 * Number of XP cubes the robot bursts on death; its Stats.XPValue is split evenly over them (task-027-01, DRGS-like burst):
	 * chaser 3, kamikaze 4, shooter 6, heavy 12 (set in each robot constructor). 0 = one cube per XP point (pre-027 rule).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Death", meta = (ClampMin = 0))
	int32 XPCubeCount = 0;

	/** Duration of the death shrink to scale 0, in seconds (enemy-death M1: 0.15 s). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Death", meta = (ClampMin = 0, Units = "s"))
	float DeathShrinkDuration = 0.15f;

	/**
	 * One-shot Niagara puff of sparks played at the robot centre when the death starts, hiding the shrink (enemy-death:
	 * about 12 sprites, 0.3 s; task-167-01). Played through URRNiagaraPoolSubsystem; null = no puff.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Death")
	TObjectPtr<UNiagaraSystem> DeathPuffSystem;

	/**
	 * Most puff components alive at once for DeathPuffSystem (enemy-death: 40, provisional); prewarmed at BeginPlay. When all
	 * are playing, a death plays no puff. Robots sharing the system share the pool (the last cap given wins).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Death", meta = (ClampMin = 0))
	int32 DeathPuffPoolSize = 40;

	/**
	 * One-shot Niagara system throwing the debris meshes when the robot disappears (enemy-death, task-166-02): mesh particles,
	 * one per entry of User.PieceMeshIndex, placed by User.PieceOffset, thrown toward User.HitDirection, floor plane = system Z.
	 * Null = M1 death (no debris).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Death")
	TObjectPtr<UNiagaraSystem> DebrisSystem;

	/**
	 * Most debris components alive at once for DebrisSystem (enemy-death: 40, provisional); prewarmed at BeginPlay. When all
	 * are playing, a death throws no debris (the puff still plays). Robots sharing the system share the pool.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Death", meta = (ClampMin = 0))
	int32 DebrisPoolSize = 40;

	/**
	 * Debris meshes this robot can throw, one entry per mesh of the DebrisSystem mesh renderer (chaser: the 13 pieces of
	 * debris_pieces.json, set on BP_Chaser). Empty = M1 death (no debris).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Death")
	TArray<FRRDebrisPiece> DebrisPieces;

	/** Robots pass through each other when moving (crowd spacing left to the light separation); applied at BeginPlay. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Crowd")
	bool bIgnoreOtherRobots = true;

	/**
	 * Light separation (task-034-01, replaces RVO, which made robots give way to each other): each living tick, while the
	 * movement is on, the push of URRRobotSeparationSubsystem::GetSeparation x SeparationWeight is added to the movement input.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Crowd")
	bool bUseSeparation = true;

	/** Weight of the separation push against the flow-field input (1). 0.5 = a full overlap steers at half the path weight (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Crowd", meta = (ClampMin = "0"))
	float SeparationWeight = 0.5f;

	/**
	 * Multiplier of the body radius used only by URRRobotSeparationSubsystem (contact distance between two robots = sum of
	 * both separation radii). 1 = the body radius; kamikaze and heavy 1.3 (task-295-01). Capsules and GetBodyRadius unchanged.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Crowd", meta = (ClampMin = "0"))
	float SeparationRadiusScale = 1.f;

	/**
	 * Root capsule radius against the world (rock, floor, the miner) for every robot: the chaser's 24 uu (enemies-index fourth
	 * test), so no robot gets stuck where a chaser passes. The robot keeps its look and its body-size hit capsule. 0 = own radius.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Collision", meta = (ClampMin = "0", Units = "cm"))
	float TerrainCapsuleRadius = 24.f;

	/** Body-size capsule taking the miner's shots (Pawn object type, overlaps Projectile only; URRHitCapsuleComponent, passive on moves since task-093-01); sized in PostInitializeComponents. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Collision")
	TObjectPtr<UCapsuleComponent> HitCapsule;

	/** Flat dark disc on the floor under the robot (task-264-02): attached to the root, absolute zero rotation, no collision or shadow. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Blob Shadow")
	TObjectPtr<URRBlobShadowComponent> BlobShadow;

	/** Blob shadow diameter = own capsule radius (before the terrain shrink) x this (2.2: a bit wider than the body). 0 = no disc. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Blob Shadow", meta = (ClampMin = "0"))
	float BlobShadowDiameterScale = 2.2f;

	/** Own capsule radius before the shrink (uu). */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Enemy|Collision")
	float BodyRadius = 0.f;

	/** Adds the separation push to the movement input (alive, movement on, bUseSeparation). */
	void UpdateSeparation();

	/**
	 * RVO radius multiplier written to the world avoidance manager at BeginPlay (ArtificialRadiusExpansion, engine default 1.5):
	 * robots keep about 2 x capsule radius x this value between centers. Lower = denser packs. 0 = leave the manager as is.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Crowd", meta = (ClampMin = "0"))
	float AvoidanceRadiusExpansion = 1.1f;

	/**
	 * Seconds an RVO-diverted velocity stays locked before the next avoidance query, written to the world avoidance manager at
	 * BeginPlay (LockTimeAfterAvoid, engine default 0.2 s). Higher = fewer queries. Negative = leave the manager as is.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Crowd", meta = (Units = "s"))
	float AvoidanceLockTimeAfterAvoid = 0.3f;

	/** At BeginPlay, strips the cost of moving the attached parts (see ApplyLightAttachedParts). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Performance")
	bool bLightAttachedParts = true;

	/** Distance (uu, 2D) from the miner beyond which an off-screen robot moves at FarMovementTickInterval. 0 = never. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Performance", meta = (ClampMin = "0", Units = "cm"))
	float FarMovementDistance = 2000.f;

	/** Tick interval (s) of CharacterMovement and EnemyMovement while far movement is on. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Performance", meta = (ClampMin = "0", Units = "s"))
	float FarMovementTickInterval = 0.1f;

	/** Seconds between two far-movement checks (each robot starts at a random phase). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Performance", meta = (ClampMin = "0.01", Units = "s"))
	float MovementLODCheckInterval = 0.25f;

	/** A robot rendered within this many seconds counts as on screen (AActor::WasRecentlyRendered tolerance). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Performance", meta = (ClampMin = "0", Units = "s"))
	float OffScreenGraceSeconds = 0.2f;

	/** Height (uu) of the damage number above the capsule top. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Damage Numbers", meta = (Units = "cm"))
	float DamageNumberHeight = 40.f;

	/** Radius (uu) of the horizontal disc around the hit where each damage number starts (damage-numbers: 30, heavy 45 in ARRHeavy). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Damage Numbers", meta = (ClampMin = "0", Units = "cm"))
	float DamageNumberSpawnRadius = 30.f;

	/**
	 * Arrival distance of the movement = the subclass's engage range (chaser AttackRange, kamikaze arm range = AttackRange, shooter PreferredDistance) * this ratio (< 1 so
	 * the robot stops inside its range). Defaults set per subclass constructor (chaser 0.9, kamikaze 0.5).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Movement", meta = (ClampMin = 0, ClampMax = 1))
	float ArrivalRangeRatio = 0.9f;

	/**
	 * Name of the static mesh component that plays the baked clips (BP_Chaser, BP_ShieldChaser, BP_Heavy, BP_Shooter, BP_Kamikaze:
	 * BodyMesh). Resolved once at BeginPlay; no component of that name (native classes in tests) or None = no animation.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Animation")
	FName AnimatedMeshName = TEXT("BodyMesh");

	/** Clip id looped while the robot moves (all the time when RestClipId < 0): chaser run, heavy and shooter walk, kamikaze sway = 0. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Animation")
	float LoopClipId = 0.f;

	/**
	 * Clip id held at its last frame while the movement is off (rest pose); negative = no rest clip, the loop never stops on
	 * SetMovementActive (chaser, kamikaze). Heavy 3, shooter 2 (rows whose last frame is the rest pose).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Animation")
	float RestClipId = -1.f;

	/**
	 * Range (s) of the random loop phase: PlayLoopClip starts the loop at now - uniform [0, LoopPhaseSpread). Longer than any loop
	 * period at its material RunRate (chaser 0.762 s at 0.7, kamikaze 1.524 s at 0.35) so the phase covers the whole loop.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Animation", meta = (ClampMin = 0, Units = "s"))
	float LoopPhaseSpread = 2.f;

	/** Custom Primitive Data index of the clip id on the animated mesh (MF_RobotAnimWPO ClipId). */
	static constexpr int32 AnimClipDataIndex = 0;

	/** Custom Primitive Data index of the clip start time (world seconds) on the animated mesh (MF_RobotAnimWPO AnimStart). */
	static constexpr int32 AnimStartDataIndex = 1;

	/** Custom Primitive Data index of the hit flash on the animated mesh (MF_RobotHitFlash): 1 = plain white, 0 = normal look. */
	static constexpr int32 HitFlashDataIndex = 2;

	/** Seconds a hit keeps the robot white (provisional, owner 2026-11-05, inside his 0.05 to 0.1 s). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Hit Flash", meta = (ClampMin = 0, Units = "s"))
	float HitFlashDuration = 0.08f;

	/** Fires EndHitFlash; active only while the robot flashes (no tick). */
	FTimerHandle HitFlashTimer;

	/** True while the flash value on the animated mesh is 1 (repeated hits restart the timer without a new render update). */
	bool bHitFlashOn = false;

	/** Age (s) given to a held clip by HoldClipEnd: far beyond any clamped clip (15 frames at 30 fps = 0.5 s). */
	static constexpr double AnimHoldLead = 10.0;

	/** Body mesh playing the baked clips (null when the class has none). */
	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> AnimatedMesh;

	/** Fires OnAnimClipEnded at the end of a timed clip; active only while it plays. */
	FTimerHandle AnimClipTimer;

	/** Movement state the movement clip was last chosen for (SetMovementActive acts only on a change; reset each life). */
	bool bMovementClipActive = true;

	/** Clip id last written (GetAnimClipId); -1 = none. */
	float CurrentAnimClipId = -1.f;

	/** Clip start last written, world seconds (GetAnimClipStart). */
	double CurrentAnimClipStart = 0.0;

	/** Stats read at BeginPlay. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Stats")
	FRREnemyStatsRow Stats;

	/** World time (s) at which the subclass state machine entered its current state (MarkStateStart). */
	double StateStartTime = 0.0;

	/** Cached attack target. */
	TWeakObjectPtr<ARRMiner> CachedTarget;

	/** Horizontal unit direction of the last hit of this life (RecordHitDirection); zero = none. */
	FVector LastHitDirection = FVector::ZeroVector;

	/** Feet location at death: the XP pickups and the debris system spawn there. */
	FVector DropLocation = FVector::ZeroVector;

	/** Actor scale at death (the shrink starts from it). */
	FVector ScaleAtDeath = FVector::OneVector;

	/** Seconds elapsed in the death shrink. */
	float DeathElapsed = 0.f;

	/** Seconds left before the next far-movement check. */
	float MovementLODTimer = 0.f;

	/** True while the movement runs at FarMovementTickInterval. */
	bool bFarMovementActive = false;

	/** True once HandleDeath ran (reset only by ResetForNewLife). */
	bool bDying = false;

	/** True once the shrink ended and the XP was dropped (release or Destroy follows). */
	bool bDeathFinished = false;

	/** Lives started from the actor pool (see GetPoolLifeCount). */
	int32 PoolLifeCount = 0;

	/** Visual state of one attached scene component, captured at the first pooled life. */
	struct FVisualPartState
	{
		/** The component (owned by this robot). */
		TWeakObjectPtr<USceneComponent> Component;
		/** Relative transform. */
		FTransform RelativeTransform;
		/** USceneComponent::IsVisible. */
		bool bVisible = true;
		/** USceneComponent::bHiddenInGame. */
		bool bHiddenInGame = false;
		/** Materials of a primitive component, by slot (empty otherwise). */
		TArray<TWeakObjectPtr<UMaterialInterface>> Materials;
		/** Intensity of a light component; negative for other components. */
		float LightIntensity = -1.f;
	};

	/** True once CaptureVisualParts ran (first pooled delivery). */
	bool bVisualPartsCaptured = false;

	/** True once the robot went back to the pool (every later delivery runs ResetForNewLife; also covers prewarmed robots). */
	bool bPoolReleasedOnce = false;

	/** Captured visual parts (CaptureVisualParts). */
	TArray<FVisualPartState> VisualParts;

	/** Meshes hidden by the wind-up flash (SetMeshesVisible(false)), the only ones SetMeshesVisible(true) shows again. */
	TArray<TWeakObjectPtr<UMeshComponent>> BlinkHiddenMeshes;

	/** Handle of the MoveSpeed change subscription on the ASC. */
	FDelegateHandle MoveSpeedChangedHandle;

	/** Handle of the binding on HealthSet->OnDeath. */
	FDelegateHandle DeathHandle;

	/** Handle of the binding on HealthSet->OnDamageReceived. */
	FDelegateHandle DamageReceivedHandle;
};
