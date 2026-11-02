// Rogue Rigs - the miner: player character owning the player ASC, top-down camera, world-space move and aim.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Pickups/RRPickup.h"
#include "UI/RRHUDTypes.h"
#include "Weapons/RRWeaponCatalogueEntry.h"
#include "RRMiner.generated.h"

class ARRArenaGrid;
class ARRDrone;
class ARRTurret;
class ARRTurretLauncher;
class ARRWeapon;
class UAbilitySystemComponent;
class UCameraComponent;
class UCameraShakeBase;
class URRAbilitySystemComponent;
class URRAimIndicatorComponent;
class URRBlobShadowComponent;
class URRPlayerLightMaskComponent;
class URRHealthAttributeSet;
class URRMovementAttributeSet;
class URRPlayerAttributeSet;
class URRLevelUpComponent;
class URRProgressionComponent;
class URRUpgradeComponent;
class USpringArmComponent;
class UGameplayEffect;
struct FOnAttributeChangeData;

/** Fired when the miner collects a pickup: the pickup's type. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnPickupCollectedSignature, ERRPickupType, Type);

/** Fired when the miner collects a pickup: its type and the world location of the collection (spawn the collect sparkle there). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnPickupCollectedAtSignature, ERRPickupType, Type, FVector, Location);

/** Fired once when the miner dies: the original instigator of the killing hit (may be null). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnMinerDeathSignature, AActor*, Instigator);

/** Fired after the miner acquired a weapon (starting loadout or AddWeapon): the weapon, already in GetWeapons. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnWeaponAddedSignature, ARRWeapon*, Weapon);

/** Fired when the miner's Health or MaxHealth changed (Max always > 0). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnMinerHealthChangedSignature, float, Current, float, Max);

/** Fired when the miner lost Amount health (> 0); HealthFraction = health left over MaxHealth (0..1). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnMinerDamagedSignature, float, Amount, float, HealthFraction);

/** Fired when the HUD state of weapon slot SlotIndex changed (weapon acquired, ammo, reload start or end, turret charges). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnWeaponSlotChangedSignature, int32, SlotIndex, const FRRWeaponSlotHUD&, Slot);

/**
 * Player character. Owns the player's URRAbilitySystemComponent and its three AttributeSets (health,
 * movement, player); base stats are read from PlayerBaseStats.csv at BeginPlay (row BaseStatsRowName).
 * MoveSpeed changes are pushed to CharacterMovement->MaxWalkSpeed. A fixed top-down camera (arm does not
 * rotate with the pawn) frames the arena. Input is routed here by ARRPlayerController: move is a
 * world-space 2D input, aim comes from the mouse cursor projected on the floor plane or from the right
 * stick, and the pawn turns toward the aim every tick. Spawns its helper drone (DroneClass) at BeginPlay and
 * destroys it at EndPlay. Dig trigger (arc mining by contact, drone gdd): each tick, when a block cell lies within the
 * capsule radius + DigContactMargin of the miner (2D, no push needed), the miner asks the drone to dig the arc toward
 * the wall, or along the move input when it pushes into the wall (ARRDrone::RequestArcDig); without contact, or when
 * dead, it releases the dig (ARRDrone::ReleaseDig). The arena grid is found once at
 * BeginPlay (first ARRArenaGrid in the world). Pickups (ARRPickup) read GetPickupRadius and report their collection
 * through NotifyPickupCollected, which broadcasts OnPickupCollected; no pickup effect here. XP and level live in its
 * URRProgressionComponent (GetProgression; XP pickups call AddXP on it). Upgrade offers and picked upgrades live in its
 * URRUpgradeComponent (GetUpgrades); the level-up flow (heal, effect, paused choice screens) in its URRLevelUpComponent
 * (GetLevelUp). Weapons: at BeginPlay each class of StartingWeapons, then each GuaranteedWeaponClasses entry no
 * StartingWeapons entry provides (default: none), is spawned (owner: the miner), attached (mesh socket ARRWeapon::GetHandSocketName
 * when set and present on the mesh, else WeaponAttachSocketName, or the capsule at WeaponAttachOffset when that is None too) and acquired (ARRWeapon::Acquire); they are destroyed at EndPlay.
 * Collision: the capsule overlaps the Projectile channel (never blocked by any projectile; projectiles still get
 * overlap events) and ignores its weapons when moving. No dash or item logic.
 * Damage (sys-damage): on each hit of its health set (OnDamageReceived) that leaves Health above 0, the miner applies
 * IFramesEffectClass (URRGE_IFrames, State.Invulnerable for IFramesDuration, SetByCaller Data.Duration) to itself in the
 * same call stack, and blinks its mesh at BlinkRate until the i-frames end (visibility restored). Every hit with damage
 * above 0 (lethal one included) also plays HitShakeClass through URRCameraShakeLibrary::PlayCameraShake. On the health set
 * OnDeath (single death, State.Dead added by the set) it stops moving, ignores move and aim input, stops its weapons
 * firing and broadcasts OnDeath. At BeginPlay it registers with the run game mode (ARRGameMode::RegisterMiner), which
 * turns OnDeath into the game over.
 * Blob shadow (task-264-02): BlobShadow, a flat dark disc attached under the capsule (diameter = capsule radius x
 * BlobShadowDiameterScale); the i-frames blink (mesh and its children) leaves it alone.
 * Light mask (task-272-07): LightMask (URRPlayerLightMaskComponent) writes the miner position and the mask tunables into
 * MPC_PlayerLight each tick (radial post-process brightening, neutral by default).
 * Aim indicator (task-415-02): AimIndicator (URRAimIndicatorComponent), a white-gold laser from the shown rifle Muzzle along
 * GetAimDirection at Muzzle height, stopped at the first robot or block; shown while alive, hidden in HandleDeath.
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

	/** Aim laser from the rifle Muzzle along the aim (default subobject, task-415-02). */
	UFUNCTION(BlueprintPure, Category = "Miner|Aim")
	URRAimIndicatorComponent* GetAimIndicator() const { return AimIndicator; }

	/** The helper drone spawned at BeginPlay (null if DroneClass is unset or the spawn failed). */
	UFUNCTION(BlueprintPure, Category = "Miner|Drone")
	ARRDrone* GetDrone() const { return Drone; }

	/**
	 * Extraction (task-370-01): hides the miner and his drone (hidden in game, no collision) so the end screen shows only the takeoff behind it.
	 * Reversed by ShowAfterExtraction when the next level places the miner.
	 */
	void HideForExtraction();

	/** Undoes HideForExtraction: miner and drone visible, collision back. No effect when not hidden. */
	void ShowAfterExtraction();

	/** True between HideForExtraction and ShowAfterExtraction. */
	bool IsHiddenForExtraction() const { return bHiddenForExtraction; }

	/** World direction (XY, unit length) of the screen up axis seen through the fixed camera (camera arm yaw). Used to place the arrival extractor above the miner (task-371-01). */
	FVector GetScreenUpDirection() const { return ScreenInputToWorld(FVector2D(0.f, 1.f)).GetSafeNormal2D(); }

	/** True when a block cell was within the capsule radius + DigContactMargin of the miner at its last tick (the drone digs then). */
	UFUNCTION(BlueprintPure, Category = "Miner|Dig")
	bool IsTouchingRock() const { return bTouchingRock; }

	/** Arena grid found at BeginPlay (first ARRArenaGrid in the world); null if the level has none. */
	UFUNCTION(BlueprintPure, Category = "Miner|Dig")
	ARRArenaGrid* GetArenaGrid() const { return ArenaGrid; }

	/** Weapons spawned from StartingWeapons then GuaranteedWeaponClasses, then by AddWeapon, in acquisition order (failed spawns skipped). */
	const TArray<TObjectPtr<ARRWeapon>>& GetWeapons() const { return Weapons; }

	/**
	 * Gives the miner a weapon of WeaponClass: spawned at the miner (owner and instigator: this miner), attached (mesh socket
	 * ARRWeapon::GetHandSocketName when set and present on the mesh, else WeaponAttachSocketName, or the capsule at
	 * WeaponAttachOffset when both are None), ignored by the capsule when moving, acquired
	 * (ARRWeapon::Acquire: CSV stats, upgrade replay), added to GetWeapons, then OnWeaponAdded. Null (logged) when the class is
	 * unset or the spawn fails. Used by the starting loadout; tests and weapon cards call it at runtime.
	 */
	UFUNCTION(BlueprintCallable, Category = "Miner|Weapons")
	ARRWeapon* AddWeapon(TSubclassOf<ARRWeapon> WeaponClass);

	/** Weapons of the run that new-weapon cards can offer (WeaponCatalogue). */
	UFUNCTION(BlueprintPure, Category = "Miner|Weapons")
	const TArray<FRRWeaponCatalogueEntry>& GetWeaponCatalogue() const { return WeaponCatalogue; }

	/** Most weapons the miner can own at once (MaxWeapons); new-weapon cards stop at this count. */
	UFUNCTION(BlueprintPure, Category = "Miner|Weapons")
	int32 GetMaxWeapons() const { return MaxWeapons; }

	/** True when a valid weapon of GetWeapons is WeaponClass or a subclass of it. */
	UFUNCTION(BlueprintPure, Category = "Miner|Weapons")
	bool OwnsWeaponClass(TSubclassOf<ARRWeapon> WeaponClass) const;

	/** Number of valid weapons in GetWeapons. */
	UFUNCTION(BlueprintPure, Category = "Miner|Weapons")
	int32 GetOwnedWeaponCount() const;

	/** True when WeaponClass is set, not owned (OwnsWeaponClass) and fewer than MaxWeapons weapons are owned. */
	UFUNCTION(BlueprintPure, Category = "Miner|Weapons")
	bool CanAcquireWeapon(TSubclassOf<ARRWeapon> WeaponClass) const;

	/** Catalogue entries the miner could acquire now (CanAcquireWeapon), in catalogue order, duplicates skipped; empty once MaxWeapons are owned. */
	UFUNCTION(BlueprintPure, Category = "Miner|Weapons")
	TArray<FRRWeaponCatalogueEntry> GetAcquirableWeapons() const;

	/** First turret launcher of GetWeapons, or null. */
	UFUNCTION(BlueprintPure, Category = "Miner|Weapons")
	ARRTurretLauncher* FindTurretLauncher() const;

	/**
	 * Place-turret button (input action PlaceTurret): asks the first turret launcher to place a turret
	 * (ARRTurretLauncher::TryPlaceTurret). Null when dead, without a launcher or when no turret was placed (no charge, no floor cell).
	 */
	UFUNCTION(BlueprintCallable, Category = "Miner|Input")
	ARRTurret* PlaceTurret();

	/** With mouse aim and once the cursor hit the aim plane: true and the cursor point on the plane in OutPoint. False with stick aim. */
	bool GetMouseAimPoint(FVector& OutPoint) const;

	/** Current PickupRadius attribute in uu (collection radius of XP, ore and heal pickups); 0 when the attribute set is missing. */
	UFUNCTION(BlueprintPure, Category = "Miner|Pickup")
	float GetPickupRadius() const;

	/** XP and level progression of the miner (default subobject). */
	UFUNCTION(BlueprintPure, Category = "Miner|Progression")
	URRProgressionComponent* GetProgression() const { return Progression; }

	/** Upgrade offers and picked upgrades of the miner (default subobject; sys-upgrades). */
	UFUNCTION(BlueprintPure, Category = "Miner|Upgrades")
	URRUpgradeComponent* GetUpgrades() const { return Upgrades; }

	/** Level-up flow of the miner (default subobject): heal, effect event, paused choice screens (ChooseUpgrade, RequestReroll). */
	UFUNCTION(BlueprintPure, Category = "Miner|Progression")
	URRLevelUpComponent* GetLevelUp() const { return LevelUp; }

	/** Called by a pickup when it reaches the miner: broadcasts OnPickupCollected, then OnPickupCollectedAt with Location. Applies no effect itself. */
	void NotifyPickupCollected(ERRPickupType Type, const FVector& Location);

	/** True once the miner died (its ASC has State.Dead). */
	UFUNCTION(BlueprintPure, Category = "Miner|Damage")
	bool IsDead() const;

	/** Duration in seconds of the invulnerability applied after each non-lethal hit. */
	UFUNCTION(BlueprintPure, Category = "Miner|Damage")
	float GetIFramesDuration() const { return IFramesDuration; }

	/** The miner health set (Health, MaxHealth, IncomingDamage; OnDamageReceived and OnDeath native events). */
	URRHealthAttributeSet* GetHealthSet() const { return HealthSet; }

	/** Broadcast once when the miner dies, after input, movement and firing are cut. Bind for game over. */
	UPROPERTY(BlueprintAssignable, Category = "Miner|Damage")
	FRROnMinerDeathSignature OnDeath;

	/** Broadcast each time the miner collects a pickup, with its type. */
	UPROPERTY(BlueprintAssignable, Category = "Miner|Pickup")
	FRROnPickupCollectedSignature OnPickupCollected;

	/** Broadcast each time the miner collects a pickup, with its type and the collection location (collect sparkle). */
	UPROPERTY(BlueprintAssignable, Category = "Miner|Pickup")
	FRROnPickupCollectedAtSignature OnPickupCollectedAt;

	/** Broadcast after each weapon acquisition (HUD: binds the turret charges of a launcher acquired mid-run). */
	UPROPERTY(BlueprintAssignable, Category = "Miner|Weapons")
	FRROnWeaponAddedSignature OnWeaponAdded;

	/** Current Health attribute (0 before the base stats are applied). */
	UFUNCTION(BlueprintPure, Category = "Miner|Damage")
	float GetCurrentHealth() const;

	/** Current MaxHealth attribute (0 before the base stats are applied: never divide by it then). */
	UFUNCTION(BlueprintPure, Category = "Miner|Damage")
	float GetMaxHealth() const;

	/**
	 * HUD state of weapon slot SlotIndex (0-based, valid weapons of GetWeapons in acquisition order: slot 0 = starting rifle). An
	 * index past the owned weapons, below 0 or at or above MaxWeapons gives a locked slot (bOwned false).
	 */
	UFUNCTION(BlueprintPure, Category = "Miner|Weapons")
	FRRWeaponSlotHUD GetWeaponSlotHUD(int32 SlotIndex) const;

	/**
	 * Broadcast when Health or MaxHealth changed, with both values; once per real change (a write of the same values sends
	 * nothing), never while MaxHealth is 0 (base stats not applied yet: the stat application sends the first one). Read the
	 * current values with GetCurrentHealth / GetMaxHealth when binding.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Miner|Damage")
	FRROnMinerHealthChangedSignature OnHealthChanged;

	/**
	 * Broadcast when Health went down: Amount lost (> 0), HealthFraction left (0..1, 0 = dead). Not for a heal nor for the clamp of
	 * a MaxHealth decrease. Same payload as URRHUDWidget::OnPlayerDamaged (damage vignette).
	 */
	UPROPERTY(BlueprintAssignable, Category = "Miner|Damage")
	FRROnMinerDamagedSignature OnDamaged;

	/**
	 * Broadcast when the HUD state of a weapon slot changed (GetWeaponSlotHUD): weapon acquired (slots shift if one is destroyed),
	 * ammo or magazine (ARRWeapon::OnAmmoChanged), reload start or end, turret charges or recharge start or end. Once per change;
	 * the per-frame ReloadProgress and RechargeProgress alone never fire it (read them each frame only while bReloading or
	 * bRecharging). Locked slots send nothing until a weapon fills them.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Miner|Weapons")
	FRROnWeaponSlotChangedSignature OnWeaponSlotChanged;

	/** Updates the i-frames blink; while alive, updates the mouse aim if active, turns the pawn toward the aim, then checks the dig trigger. */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Applies the camera tunables (pitch, arm length, lag, FOV) to the components, so Blueprint edits show up. */
	virtual void OnConstruction(const FTransform& Transform) override;

	/**
	 * Turns shadow casting off on every primitive component, the skeletal mesh and Blueprint ones included (task-263-01: only
	 * rocks cast), then sizes and places BlobShadow from the capsule (task-264-02).
	 */
	virtual void PostInitializeComponents() override;

	/** Initializes the ASC actor info, applies the CSV base stats and starts mirroring MoveSpeed. */
	virtual void BeginPlay() override;

	/** Refreshes the ASC actor info so it caches the new controller. */
	virtual void PossessedBy(AController* NewController) override;

	/** Stops listening to MoveSpeed changes and to the health set events, and destroys the drone and the weapons. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Pushes the current MoveSpeed attribute value to CharacterMovement->MaxWalkSpeed. */
	void ApplyMoveSpeed(float NewMoveSpeed);

	/** ASC callback: MoveSpeed changed (base or current value). */
	void HandleMoveSpeedChanged(const FOnAttributeChangeData& ChangeData);

	/** Spawns DroneClass at the miner's location (owner and instigator: this miner) and hands it this miner. */
	void SpawnDrone();

	/**
	 * Spawns each class of StartingWeapons, then each GuaranteedWeaponClasses entry not provided by StartingWeapons, at the miner (owner and instigator: this miner), attaches it (mesh socket
	 * the weapon hand socket, else WeaponAttachSocketName, or the capsule at WeaponAttachOffset when None), makes the capsule ignore it when moving and calls Acquire (AddWeapon).
	 */
	void SpawnStartingWeapons();

	/** Projects the mouse cursor on the floor plane and aims at the hit point. Keeps the last aim if it misses. */
	void UpdateMouseAim();

	/** Stores the first ARRArenaGrid of the world in ArenaGrid (logs a warning when there is none). */
	void FindArenaGrid();

	/**
	 * Dig trigger, run each tick with the move input of this frame (zero when idle): contact = a block cell whose square
	 * (2D) is within the capsule radius + DigContactMargin of the miner's location. Wall direction = opposite of the mean
	 * contact normal (from the nearest block point to the miner). Arc direction = MoveInput when its length exceeds
	 * DigMinMoveInput and it points into the wall, else the wall direction. Calls RequestArcDig (with the touching cells,
	 * for the drone's empty-arc fallback) on contact, ReleaseDig
	 * otherwise; sets bTouchingRock. Assumes an unrotated grid.
	 */
	void UpdateDigTrigger(const FVector& MoveInput);

	/** Health set hit callback: applies the i-frames when the miner survives the hit. Runs inside the GE execution. */
	void HandleDamageReceived(float Damage, bool bIsCritical, float HealthLeft);

	/** Health set death callback: cuts movement, input and firing, ends the blink, broadcasts OnDeath. Runs inside the GE execution. */
	void HandleDeath(AActor* DeathInstigator);

	/** Applies IFramesEffectClass to self with SetByCaller Data.Duration = IFramesDuration, then starts the blink. */
	void ApplyIFrames();

	/** Blink step: shows or hides the mesh by the BlinkRate phase; restores visibility once the i-frames time is over. */
	void UpdateBlink();

	/** Shows or hides the mesh (and its attached components). */
	void SetMeshVisible(bool bVisible);

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

	/** XP and level; XP pickups call AddXP on it, the HUD listens to its events. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URRProgressionComponent> Progression;

	/** Builds upgrade offers and applies the picked upgrades (player ASC and weapon ASCs); replays global ones on new weapons. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URRUpgradeComponent> Upgrades;

	/** Level-up flow: listens to Progression, heals, broadcasts the effect and offer events, pauses on the choice screens. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URRLevelUpComponent> LevelUp;

	/** Flat dark disc on the floor under the miner (task-264-02): attached to the capsule, absolute zero rotation, no collision or shadow. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URRBlobShadowComponent> BlobShadow;

	/** Writes the miner position and the light mask tunables into MPC_PlayerLight each tick (task-272-07, neutral by default). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URRPlayerLightMaskComponent> LightMask;

	/** Aim laser from the shown rifle Muzzle along the aim (task-415-02): placed each tick, hidden at death. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URRAimIndicatorComponent> AimIndicator;

	/** Blob shadow diameter = capsule radius x this (2.2, like the robots). 0 = no disc. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Blob Shadow", meta = (ClampMin = "0"))
	float BlobShadowDiameterScale = 2.2f;

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
	float CameraArmLength = 1400.f;

	/** Spring arm camera lag. Off by default so the miner stays centered on screen. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Camera")
	bool bCameraLag = false;

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

	/** True while hidden for the extraction takeoff. */
	bool bHiddenForExtraction = false;

	/** Drone class spawned at BeginPlay (a Blueprint subclass of ARRDrone). None = no drone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Drone")
	TSubclassOf<ARRDrone> DroneClass;

	/** Gap beyond the capsule radius within which a block counts as touched and gets dug (no push needed), in uu (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Dig", meta = (ClampMin = 0, Units = "cm"))
	float DigContactMargin = 10.f;

	/** Move inputs with a length at or below this value do not steer the dig arc (it points toward the wall) (provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Dig", meta = (ClampMin = 0, ClampMax = 1))
	float DigMinMoveInput = 0.1f;

	/** Invulnerability after a non-lethal hit, in seconds (miner gdd: 0.25 s, provisional). 0 = no i-frames. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Damage", meta = (ClampMin = 0, Units = "s"))
	float IFramesDuration = 0.25f;

	/** Effect applied to self for the i-frames; must read SetByCaller Data.Duration and grant State.Invulnerable. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Damage")
	TSubclassOf<UGameplayEffect> IFramesEffectClass;

	/** Mesh blink frequency during the i-frames, in full hide/show cycles per second. 0 = no blink. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Damage", meta = (ClampMin = 0, Units = "Hz"))
	float BlinkRate = 12.f;

	/**
	 * Camera shake played on each hit that takes damage (lethal hit included; a hit refused by the i-frames never
	 * reaches the handler). Default URRHitCameraShake (5 uu, 0.2 s, 25 Hz, provisional owner 2026-11-02). None = no shake.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Damage")
	TSubclassOf<UCameraShakeBase> HitShakeClass;

	/** Weapon classes spawned, attached and acquired at BeginPlay, in order (Blueprint subclasses of ARRWeapon). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Weapons")
	TArray<TSubclassOf<ARRWeapon>> StartingWeapons;

	/**
	 * Weapon classes the miner always starts with: after StartingWeapons, each class here is spawned, attached and acquired
	 * unless an entry of StartingWeapons is that class or a subclass. Default: empty (since task-024-04 the miner starts with
	 * the burst rifle only; the other weapons come from new-weapon cards).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Weapons")
	TArray<TSubclassOf<ARRWeapon>> GuaranteedWeaponClasses;

	/**
	 * Weapons of the run (burst rifle, shotgun, turret launcher), offered by new-weapon cards while not owned and fewer than
	 * MaxWeapons are owned (sys-upgrades). Code defaults: native ARRShotgun and ARRTurretLauncher (the burst rifle has no
	 * native class); BP_Miner lists the Blueprint classes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Weapons")
	TArray<FRRWeaponCatalogueEntry> WeaponCatalogue;

	/** Most weapons owned at once (upgrades gdd: 4); new-weapon cards are offered only below this count. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Weapons", meta = (ClampMin = 1))
	int32 MaxWeapons = 4;

	/**
	 * Socket of the miner mesh the weapons attach to, for weapons without their own hand socket (ARRWeapon::GetHandSocketName,
	 * which wins when set). None = attach to the capsule (root) at WeaponAttachOffset, so the weapon turns with the aim yaw.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Weapons")
	FName WeaponAttachSocketName;

	/** Location of the weapons relative to the capsule center (X forward along the aim, Y right, Z up), in uu. Used only when the weapon gets no socket (its hand socket and WeaponAttachSocketName both None or missing). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Miner|Weapons", meta = (Units = "cm"))
	FVector WeaponAttachOffset = FVector(0.f, 20.f, -40.f);

	/** Weapons spawned from StartingWeapons; destroyed at EndPlay. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Miner|Weapons")
	TArray<TObjectPtr<ARRWeapon>> Weapons;

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

	/** True when a block was touched at the last dig trigger (IsTouchingRock). */
	bool bTouchingRock = false;

	/** Last cursor point on the mouse aim plane (valid when bHasMouseAimPoint). */
	FVector MouseAimPoint = FVector::ZeroVector;

	/** True once the cursor ray hit the mouse aim plane. */
	bool bHasMouseAimPoint = false;

	/** Handle of the MoveSpeed change subscription on the ASC. */
	FDelegateHandle MoveSpeedChangedHandle;

	/** Handle of the binding on HealthSet->OnDamageReceived. */
	FDelegateHandle DamageReceivedHandle;

	/** Handle of the binding on HealthSet->OnDeath. */
	FDelegateHandle DeathHandle;

	/** ASC callback for Health and MaxHealth: OnHealthChanged when the values changed, OnDamaged when Health went down. */
	void HandleHealthAttributeChanged(const FOnAttributeChangeData& ChangeData);

	/** Binds Weapon's ammo, reload and turret charge events to RefreshWeaponSlots. */
	void BindWeaponSlotEvents(ARRWeapon* Weapon);

	/** Recomputes every slot (GetWeaponSlotHUD) and broadcasts OnWeaponSlotChanged for each one whose state changed. */
	void RefreshWeaponSlots();

	/** ARRWeapon::OnAmmoChanged callback: RefreshWeaponSlots. */
	UFUNCTION()
	void HandleWeaponAmmoChanged(ARRWeapon* Weapon, int32 Rounds, int32 MagazineSize);

	/** URRAutoFireComponent::OnReloadStarted callback: RefreshWeaponSlots. */
	UFUNCTION()
	void HandleWeaponReloadStarted(float ReloadDuration);

	/** URRAutoFireComponent::OnReloadFinished callback: RefreshWeaponSlots. */
	UFUNCTION()
	void HandleWeaponReloadFinished();

	/** ARRTurretLauncher::OnTurretChargesChanged callback: RefreshWeaponSlots. */
	UFUNCTION()
	void HandleTurretChargesChanged(int32 Charges, int32 MaxCharges, float RechargeDuration);

	/** Handle of the Health change binding on the ASC. */
	FDelegateHandle HealthChangedHandle;

	/** Handle of the MaxHealth change binding on the ASC. */
	FDelegateHandle MaxHealthChangedHandle;

	/** Health last sent by OnHealthChanged (-1 before the first). */
	float LastBroadcastHealth = -1.f;

	/** MaxHealth last sent by OnHealthChanged (-1 before the first). */
	float LastBroadcastMaxHealth = -1.f;

	/** Slot states last sent by OnWeaponSlotChanged (MaxWeapons entries, locked at BeginPlay). */
	UPROPERTY(Transient)
	TArray<FRRWeaponSlotHUD> LastWeaponSlots;

	/** World time at which the current blink started (the last i-frames application). */
	double BlinkStartTime = 0.0;

	/** World time at which the current blink ends and the mesh is shown again. */
	double BlinkEndTime = 0.0;

	/** True while the mesh blinks (between an i-frames application and BlinkEndTime). */
	bool bBlinking = false;
};
