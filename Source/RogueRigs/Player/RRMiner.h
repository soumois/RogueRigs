// Rogue Rigs - the miner: player character owning the player ASC, top-down camera, world-space move and aim.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Pickups/RRPickup.h"
#include "RRMiner.generated.h"

class ARRArenaGrid;
class ARRDrone;
class UAbilitySystemComponent;
class UCameraComponent;
class URRAbilitySystemComponent;
class URRHealthAttributeSet;
class URRMovementAttributeSet;
class URRPlayerAttributeSet;
class USpringArmComponent;
struct FOnAttributeChangeData;

/** Fired when the miner collects a pickup: the pickup's type. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnPickupCollectedSignature, ERRPickupType, Type);

/**
 * Player character. Owns the player's URRAbilitySystemComponent and its three AttributeSets (health,
 * movement, player); base stats are read from PlayerBaseStats.csv at BeginPlay (row BaseStatsRowName).
 * MoveSpeed changes are pushed to CharacterMovement->MaxWalkSpeed. A fixed top-down camera (arm does not
 * rotate with the pawn) frames the arena. Input is routed here by ARRPlayerController: move is a
 * world-space 2D input, aim comes from the mouse cursor projected on the floor plane or from the right
 * stick, and the pawn turns toward the aim every tick. Spawns its helper drone (DroneClass) at BeginPlay and
 * destroys it at EndPlay. Dig trigger: each tick, when the move input points into the adjacent block cell
 * (dominant axis of the world move direction) and that block face is within the dig reach, the miner asks the
 * drone to cut it (ARRDrone::RequestDig); the drone handles the cancel rule. The arena grid is found once at
 * BeginPlay (first ARRArenaGrid in the world). Pickups (ARRPickup) read GetPickupRadius and report their collection
 * through NotifyPickupCollected, which broadcasts OnPickupCollected; no pickup effect here. No weapon, dash or item logic.
 * Subclass in Blueprint for the mesh and animation.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRMiner : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	/** Creates the ASC, the AttributeSets, the spring arm and the camera; sets the movement defaults. */
	ARRMiner(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ IAbilitySystemInterface
	/** Returns the miner's ASC (owner and avatar are the miner). */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Returns the miner's ASC with its project type. */
	URRAbilitySystemComponent* GetRRAbilitySystemComponent() const { return AbilitySystemComponent; }

	/**
	 * Moves the miner in world space. Input uses screen axes as seen through the fixed camera:
	 * X = screen right, Y = screen up (a raw stick, or WASD with the usual swizzle on W/S).
	 * Converted to world directions with the camera arm's yaw. Call every frame the input is held.
	 */
	UFUNCTION(BlueprintCallable, Category = "Miner|Input")
	void ApplyMoveInput(const FVector2D& Input);

	/**
	 * Aims with a stick. Input uses the same screen axes as ApplyMoveInput. A near-zero input keeps the
	 * last aim. Switches the miner to stick aim (the mouse cursor is no longer projected).
	 */
	UFUNCTION(BlueprintCallable, Category = "Miner|Input")
	void ApplyStickAimInput(const FVector2D& Input);

	/** Switches the miner to mouse aim: each tick the cursor is projected on the floor plane and aimed at. */
	UFUNCTION(BlueprintCallable, Category = "Miner|Input")
	void UseMouseAim();

	/** Current aim direction in world space (unit vector, Z = 0). Facing direction until the first aim input. */
	UFUNCTION(BlueprintPure, Category = "Miner|Aim")
	FVector GetAimDirection() const { return AimDirection; }

	/** True while aiming with the mouse cursor, false while aiming with a stick. */
	UFUNCTION(BlueprintPure, Category = "Miner|Aim")
	bool IsUsingMouseAim() const { return bUsingMouseAim; }

	/** The helper drone spawned at BeginPlay (null if DroneClass is unset or the spawn failed). */
	UFUNCTION(BlueprintPure, Category = "Miner|Drone")
	ARRDrone* GetDrone() const { return Drone; }

	/** Arena grid found at BeginPlay (first ARRArenaGrid in the world); null if the level has none. */
	UFUNCTION(BlueprintPure, Category = "Miner|Dig")
	ARRArenaGrid* GetArenaGrid() const { return ArenaGrid; }

	/** Current PickupRadius attribute in uu (collection radius of XP, ore and heal pickups); 0 when the attribute set is missing. */
	UFUNCTION(BlueprintPure, Category = "Miner|Pickup")
	float GetPickupRadius() const;

	/** Called by a pickup when it reaches the miner: broadcasts OnPickupCollected. Applies no effect itself. */
	void NotifyPickupCollected(ERRPickupType Type);

	/** Broadcast each time the miner collects a pickup, with its type. */
	UPROPERTY(BlueprintAssignable, Category = "Miner|Pickup")
	FRROnPickupCollectedSignature OnPickupCollected;

	/** Updates the mouse aim if active, turns the pawn toward the aim direction, then checks the dig trigger. */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Applies the camera tunables (pitch, arm length, FOV) to the components, so Blueprint edits show up. */
	virtual void OnConstruction(const FTransform& Transform) override;

	/** Initializes the ASC actor info, applies the CSV base stats and starts mirroring MoveSpeed. */
	virtual void BeginPlay() override;

	/** Refreshes the ASC actor info so it caches the new controller. */
	virtual void PossessedBy(AController* NewController) override;

	/** Stops listening to MoveSpeed changes and destroys the drone. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Pushes the current MoveSpeed attribute value to CharacterMovement->MaxWalkSpeed. */
	void ApplyMoveSpeed(float NewMoveSpeed);

	/** ASC callback: MoveSpeed changed (base or current value). */
	void HandleMoveSpeedChanged(const FOnAttributeChangeData& ChangeData);

	/** Spawns DroneClass at the miner's location (owner and instigator: this miner) and hands it this miner. */
	void SpawnDrone();

	/** Projects the mouse cursor on the floor plane and aims at the hit point. Keeps the last aim if it misses. */
	void UpdateMouseAim();

	/** Stores the first ARRArenaGrid of the world in ArenaGrid (logs a warning when there is none). */
	void FindArenaGrid();

	/**
	 * Dig trigger, run each tick with the move input of this frame: picks the cell next to the miner along the
	 * dominant grid axis of MoveDirection; if it holds a block whose facing side is within the dig reach
	 * (capsule radius + DigReachMargin), calls RequestDig on the drone.
	 */
	void UpdateDigTrigger(const FVector& MoveDirection);

	/** Converts a 2D input in screen axes to a world-space direction on the XY plane (not normalized). */
	FVector ScreenInputToWorld(const FVector2D& Input) const;

	/** Camera boom. Uses absolute rotation, so it never turns with the pawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	/** Top-down camera at the end of the arm. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	/** The player's ability system component (owner and avatar: this miner). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<URRAbilitySystemComponent> AbilitySystemComponent;

	/** Health, MaxHealth, IncomingDamage. Registered on the ASC as a default subobject. */
	UPROPERTY()
	TObjectPtr<URRHealthAttributeSet> HealthSet;

	/** MoveSpeed. Registered on the ASC as a default subobject. */
	UPROPERTY()
	TObjectPtr<URRMovementAttributeSet> MovementSet;

	/** PickupRadius, XPGain, MiningSpeed, Luck. Registered on the ASC as a default subobject. */
	UPROPERTY()
	TObjectPtr<URRPlayerAttributeSet> PlayerSet;

	/** Row of PlayerBaseStats.csv applied as base stats at BeginPlay. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Stats")
	FName BaseStatsRowName = TEXT("Miner");

	/** Camera arm pitch in degrees (negative looks down). Fixed world rotation, independent of the pawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Camera", meta = (ClampMin = -89, ClampMax = 0, Units = "deg"))
	float CameraPitch = -50.f;

	/** Camera arm yaw in degrees (world). Also defines the screen axes used by move and stick aim. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Camera", meta = (ClampMin = -180, ClampMax = 180, Units = "deg"))
	float CameraYaw = 0.f;

	/** Camera arm length in uu. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Camera", meta = (ClampMin = 0, Units = "cm"))
	float CameraArmLength = 2200.f;

	/** Camera horizontal field of view in degrees. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Camera", meta = (ClampMin = 5, ClampMax = 170, Units = "deg"))
	float CameraFieldOfView = 75.f;

	/** Speed at which the pawn yaw blends toward the aim (FMath::RInterpTo speed). 0 = turn instantly. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Aim", meta = (ClampMin = 0))
	float AimRotationInterpSpeed = 0.f;

	/** Height of the mouse aim plane above the miner's feet, in uu (0 = floor under the capsule). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Aim", meta = (Units = "cm"))
	float MouseAimPlaneHeight = 0.f;

	/** Stick inputs with a length at or below this value are ignored (keeps the last aim). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Aim", meta = (ClampMin = 0, ClampMax = 1))
	float StickAimMinInput = 0.1f;

	/** Drone class spawned at BeginPlay (a Blueprint subclass of ARRDrone). None = no drone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Drone")
	TSubclassOf<ARRDrone> DroneClass;

	/** Extra distance beyond the capsule radius within which a pushed block face triggers a dig, in uu. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Dig", meta = (ClampMin = 0, Units = "cm"))
	float DigReachMargin = 30.f;

	/** Move inputs with a length at or below this value never trigger a dig (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Dig", meta = (ClampMin = 0, ClampMax = 1))
	float DigMinMoveInput = 0.1f;

	/** Arena grid used for the dig trigger; found once at BeginPlay. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Miner|Dig")
	TObjectPtr<ARRArenaGrid> ArenaGrid;

	/** The spawned drone; destroyed at EndPlay. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Miner|Drone")
	TObjectPtr<ARRDrone> Drone;

	/** Current world-space aim direction (unit, Z = 0). */
	FVector AimDirection = FVector::ForwardVector;

	/** World-space move input of the current frame (clamped to length 1), consumed by the dig trigger in Tick. */
	FVector PendingMoveInput = FVector::ZeroVector;

	/** True while aiming with the mouse cursor. */
	bool bUsingMouseAim = true;

	/** Handle of the MoveSpeed change subscription on the ASC. */
	FDelegateHandle MoveSpeedChangedHandle;
};
