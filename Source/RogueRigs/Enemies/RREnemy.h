// Rogue Rigs - robot base: own ASC with health, movement and enemy sets, CSV stats, flow-field movement, single death with shrink and XP drop.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Enemies/RREnemyStatsRow.h"
#include "RREnemy.generated.h"

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
 * Death (health set OnDeath, once, State.Dead on the ASC): stops, disables collision, shrinks to 0 over DeathShrinkDuration,
 * then spawns Stats.XPValue pickups of XPPickupClass at its feet and destroys itself. Subclasses implement their behavior in
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

	/** Target of the attacks: the first living ARRMiner of the world (cached); null when there is none. */
	ARRMiner* FindTarget();

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

	/** Spawns Stats.XPValue pickups of XPPickupClass at DropLocation (no owner). */
	void DropXP();

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
};
