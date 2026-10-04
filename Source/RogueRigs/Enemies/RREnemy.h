// Rogue Rigs - robot base: own ASC with health, movement and enemy sets, CSV stats, flow-field movement, single death with shrink and XP drop, floating damage numbers.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Enemies/RREnemyStatsRow.h"
#include "RREnemy.generated.h"

class ARRFloatingDamageText;
class ARRMiner;
class ARRPickup;
class UAbilitySystemComponent;
class URRAbilitySystemComponent;
class URREnemyAttributeSet;
class URREnemyMovementComponent;
class URRHealthAttributeSet;
class URRMovementAttributeSet;
struct FOnAttributeChangeData;

/**
 * Base of every robot (sys-enemies). Owns its URRAbilitySystemComponent (owner and avatar: the robot) with the health,
 * movement and enemy AttributeSets. At BeginPlay it reads row StatsRowName of EnemyStats.csv: MaxHealth, Health and
 * MoveSpeed become base values (MoveSpeed is mirrored to CharacterMovement->MaxWalkSpeed), the rest is kept in Stats.
 * Moves without a controller: URREnemyMovementComponent feeds the flow-field direction to CharacterMovement (RVO
 * avoidance on). The capsule overlaps the Projectile channel, so shots hit it through the ASC (sys-projectiles).
 * Crowd: when bIgnoreOtherRobots is on, the capsule carries RRCollisionChannels::RobotMaskFilter and its movement sweeps ignore
 * that mask, so robots pass through each other (RVO alone separates them) while the miner, terrain and every query still
 * collide with them. At BeginPlay the robot also writes the crowd RVO settings to the world avoidance manager.
 * Death (health set OnDeath, once, State.Dead on the ASC): stops, disables collision, shrinks to 0 over DeathShrinkDuration,
 * then spawns Stats.XPValue pickups of XPPickupClass at its feet and destroys itself. Each hit (the killing one too) spawns a
 * FloatingTextClass damage number above the capsule top when bShowDamageNumbers is on. Subclasses implement their behavior in
 * UpdateBehavior (alive only) and may extend HandleDeath. Subclass in Blueprint for the mesh.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARREnemy : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	/** Creates the ASC, the AttributeSets and the enemy movement component; sets collision and movement defaults. */
	ARREnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ IAbilitySystemInterface
	/** Returns the robot's ASC. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Runs the death shrink when dead, else UpdateBehavior. */
	virtual void Tick(float DeltaSeconds) override;

	/** Reads row RowName of EnemyStats.csv into OutRow. Returns false (logged) when the file or the row is missing. */
	static bool LoadStatsRow(FName RowName, FRREnemyStatsRow& OutRow);

	/** Path of the enemy stats CSV, relative to FPaths::ProjectContentDir(). */
	static const TCHAR* EnemyStatsPath;

	/** Stats read from the CSV at BeginPlay (zeros before, or when the row is missing). */
	const FRREnemyStatsRow& GetStats() const { return Stats; }

	/** The robot health set (Health, MaxHealth, IncomingDamage; OnDamageReceived and OnDeath native events). */
	URRHealthAttributeSet* GetHealthSet() const { return HealthSet; }

	/** The flow-field movement component. */
	URREnemyMovementComponent* GetEnemyMovement() const { return EnemyMovement; }

	/** True once the robot died (its ASC has State.Dead). */
	UFUNCTION(BlueprintPure, Category = "Enemy")
	bool IsDead() const;

	/** Knockback resistance from the CSV stats, clamped to [0, 1] (0 = full knockback). */
	UFUNCTION(BlueprintPure, Category = "Enemy")
	float GetKnockbackResistance() const;

	/**
	 * Pushes a living robot horizontally: launches it (ACharacter::LaunchCharacter, XY overridden, Z kept) with Velocity's XY
	 * scaled by (1 - GetKnockbackResistance()). Ignored when dead or when the scaled velocity is zero. Called by the shotgun.
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

protected:
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

	/** Advances the shrink; at its end drops the XP pickups and destroys the robot. */
	void UpdateDeath(float DeltaSeconds);

	/** Spawns Stats.XPValue pickups of XPPickupClass at DropLocation (no owner). Overridden to suppress the drop (kamikaze fuse). */
	virtual void DropXP();

	/** Health set hit callback (runs inside the GE execution): spawns the damage number when bShowDamageNumbers is on. */
	void HandleDamageReceived(float Damage, bool bIsCritical, float HealthLeft);

	/** Spawns FloatingTextClass at the capsule top + TextSpawnHeight (horizontal jitter), styled for a normal or critical hit. */
	void SpawnDamageText(float Damage, bool bIsCritical);

	/** Shows or hides every mesh component of the robot (used by the wind-up flash). */
	void SetMeshesVisible(bool bVisible);

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

	/** Duration of the death shrink to scale 0, in seconds (enemy-death M1: 0.15 s). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Death", meta = (ClampMin = 0, Units = "s"))
	float DeathShrinkDuration = 0.15f;

	/** Robots pass through each other when moving (crowd collision left to RVO avoidance); applied at BeginPlay. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Crowd")
	bool bIgnoreOtherRobots = true;

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

	/** Spawns a damage number on each hit (the killing one too). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Damage Text")
	bool bShowDamageNumbers = true;

	/** Actor spawned per hit to show the damage number. None = no number. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Damage Text")
	TSubclassOf<ARRFloatingDamageText> FloatingTextClass;

	/** Height (uu) of the damage number above the capsule top. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Damage Text", meta = (Units = "cm"))
	float TextSpawnHeight = 40.f;

	/** Random horizontal offset (uu, each axis in [-Jitter, Jitter]) so stacked numbers stay readable. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Damage Text", meta = (ClampMin = "0", Units = "cm"))
	float TextSpawnJitter = 25.f;

	/** Color of a normal hit's number (white, color-code). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Damage Text")
	FLinearColor NormalTextColor = FLinearColor::White;

	/** Color of a critical hit's number (white-gold, color-code). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Damage Text")
	FLinearColor CriticalTextColor = FLinearColor(1.f, 0.85f, 0.45f);

	/** World size (uu) of a normal hit's number. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Damage Text", meta = (ClampMin = "1"))
	float NormalTextSize = 60.f;

	/** World size (uu) of a critical hit's number (larger than normal). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Damage Text", meta = (ClampMin = "1"))
	float CriticalTextSize = 90.f;

	/** Stats read at BeginPlay. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Stats")
	FRREnemyStatsRow Stats;

	/** Cached attack target. */
	TWeakObjectPtr<ARRMiner> CachedTarget;

	/** Feet location at death: the XP pickups spawn there. */
	FVector DropLocation = FVector::ZeroVector;

	/** Actor scale at death (the shrink starts from it). */
	FVector ScaleAtDeath = FVector::OneVector;

	/** Seconds elapsed in the death shrink. */
	float DeathElapsed = 0.f;

	/** True once HandleDeath ran (never reset). */
	bool bDying = false;

	/** True once the XP was dropped and Destroy called. */
	bool bDeathFinished = false;

	/** Handle of the MoveSpeed change subscription on the ASC. */
	FDelegateHandle MoveSpeedChangedHandle;

	/** Handle of the binding on HealthSet->OnDeath. */
	FDelegateHandle DeathHandle;

	/** Handle of the binding on HealthSet->OnDamageReceived. */
	FDelegateHandle DamageReceivedHandle;
};
