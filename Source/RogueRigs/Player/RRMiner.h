// Rogue Rigs - the miner: player character owning the player ASC, top-down camera, world-space move and aim.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "RRMiner.generated.h"

class ARRDrone;
class UAbilitySystemComponent;
class UCameraComponent;
class URRAbilitySystemComponent;
class URRHealthAttributeSet;
class URRMovementAttributeSet;
class URRPlayerAttributeSet;
class USpringArmComponent;
struct FOnAttributeChangeData;

/**
 * Player character. Owns the player's URRAbilitySystemComponent and its three AttributeSets (health,
 * movement, player); base stats are read from PlayerBaseStats.csv at BeginPlay (row BaseStatsRowName).
 * MoveSpeed changes are pushed to CharacterMovement->MaxWalkSpeed. A fixed top-down camera (arm does not
 * rotate with the pawn) frames the arena. Input is routed here by ARRPlayerController: move is a
 * world-space 2D input, aim comes from the mouse cursor projected on the floor plane or from the right
 * stick, and the pawn turns toward the aim every tick. Spawns its helper drone (DroneClass) at BeginPlay and
 * destroys it at EndPlay. No weapon, dash, pickup or item logic here.
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

	/** Updates the mouse aim if active, then turns the pawn toward the aim direction. */
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

	/** The spawned drone; destroyed at EndPlay. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Miner|Drone")
	TObjectPtr<ARRDrone> Drone;

	/** Current world-space aim direction (unit, Z = 0). */
	FVector AimDirection = FVector::ForwardVector;

	/** True while aiming with the mouse cursor. */
	bool bUsingMouseAim = true;

	/** Handle of the MoveSpeed change subscription on the ASC. */
	FDelegateHandle MoveSpeedChangedHandle;
};
