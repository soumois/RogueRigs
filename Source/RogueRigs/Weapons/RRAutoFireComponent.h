// Rogue Rigs - auto-fire accumulator: decides when a weapon, turret or enemy shooter fires (burst timing, magazine, reload).

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RRAutoFireComponent.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

/** Fired after each shot: index of the shot inside its burst (0 = first bullet). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnAutoFireShotSignature, int32, ShotIndexInBurst);

/** Fired when a reload starts: its duration in seconds. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnAutoFireReloadStartedSignature, float, ReloadDuration);

/** Fired when a reload ends and the magazine is full again. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROnAutoFireReloadFinishedSignature);

/**
 * Decides when its owner fires (sys-auto-fire); the owner decides what is fired through the shot handler
 * (ARRWeapon::FireOnce). Reads FireRate, Ammo, MagazineSize and ReloadSpeed on the owner's ASC
 * (URRWeaponAttributeSet required); BurstCount, BurstDuty and ReloadTime are CSV constants passed to Configure.
 *
 * Timing: period = 1 / FireRate; gap between bullets = BurstDuty * period / (BurstCount - 1); pause after the
 * burst = period * (1 - BurstDuty); BurstCount 1 = one shot per period. Time is accumulated and every shot
 * whose time has passed inside a frame is fired in that frame, so timing does not depend on the frame rate.
 * Intervals use the FireRate read when the previous shot is fired.
 *
 * Magazine: each shot applies ConsumeAmmoEffect. When Ammo drops below 1 the burst stops, the burst counter
 * resets and a reload of ReloadTime / ReloadSpeed seconds starts at once (it replaces the pause); at its end
 * RefillAmmoEffect fills the magazine and the next burst starts immediately.
 */
UCLASS(ClassGroup = (RogueRigs), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRAutoFireComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Ticks every frame; not firing until Configure and StartFiring are called. */
	URRAutoFireComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Sets the ASC holding the weapon attributes and the CSV constants. Resets the burst and reload state;
	 * the first shot is due as soon as firing starts. Returns false (logged) if the ASC lacks URRWeaponAttributeSet.
	 */
	bool Configure(UAbilitySystemComponent* InAbilitySystemComponent, int32 InBurstCount, float InBurstDuty, float InReloadTime);

	/** Sets the native callback run for each shot, before the ammo is consumed (the owner's FireOnce). */
	void SetShotHandler(FSimpleDelegate InShotHandler) { ShotHandler = MoveTemp(InShotHandler); }

	/** Starts or resumes firing; a pending cooldown or reload continues where it stopped. */
	UFUNCTION(BlueprintCallable, Category = "AutoFire")
	void StartFiring();

	/** Pauses firing: time no longer advances for shots or reloads. */
	UFUNCTION(BlueprintCallable, Category = "AutoFire")
	void StopFiring();

	/**
	 * Advances the accumulator by DeltaSeconds and fires every shot, and ends every reload, due inside that span.
	 * Called by TickComponent; tests may call it directly with a fixed step.
	 */
	void AdvanceTime(float DeltaSeconds);

	/** Fills the magazine now (RefillAmmoEffect, or a direct base write when it is unset). */
	void RefillAmmo();

	/** Turns the magazine on or off at runtime (off: infinite ammo, no reload; e.g. a placed turret, whose MagazineSize is the launcher's charges). */
	void SetUseMagazine(bool bInUseMagazine) { bUseMagazine = bInUseMagazine; }

	/** True when shots consume Ammo and an empty magazine reloads. */
	UFUNCTION(BlueprintPure, Category = "AutoFire")
	bool UsesMagazine() const { return bUseMagazine; }

	/** True between StartFiring and StopFiring. */
	UFUNCTION(BlueprintPure, Category = "AutoFire")
	bool IsFiring() const { return bFiring; }

	/** True while a reload is in progress. */
	UFUNCTION(BlueprintPure, Category = "AutoFire")
	bool IsReloading() const { return bReloading; }

	/** Bullets already fired in the current burst (0 between bursts). */
	UFUNCTION(BlueprintPure, Category = "AutoFire")
	int32 GetShotsFiredInBurst() const { return ShotsFiredInBurst; }

	/** Shots fired since Configure. */
	UFUNCTION(BlueprintPure, Category = "AutoFire")
	int32 GetTotalShotsFired() const { return TotalShotsFired; }

	/** Seconds until the next shot (meaningless while reloading). */
	UFUNCTION(BlueprintPure, Category = "AutoFire")
	float GetTimeToNextShot() const { return static_cast<float>(TimeToNextShot); }

	/** Seconds left in the current reload (0 when not reloading). */
	UFUNCTION(BlueprintPure, Category = "AutoFire")
	float GetReloadTimeRemaining() const { return static_cast<float>(ReloadTimeRemaining); }

	/** Current reload duration: ReloadTime / ReloadSpeed (0 if ReloadSpeed <= 0, meaning the reload never ends). */
	UFUNCTION(BlueprintPure, Category = "AutoFire")
	float GetReloadDuration() const;

	/** Broadcast after each shot with its index in the burst. */
	UPROPERTY(BlueprintAssignable, Category = "AutoFire")
	FRROnAutoFireShotSignature OnShotFired;

	/** Broadcast when a reload starts. */
	UPROPERTY(BlueprintAssignable, Category = "AutoFire")
	FRROnAutoFireReloadStartedSignature OnReloadStarted;

	/** Broadcast when a reload ends (magazine full). */
	UPROPERTY(BlueprintAssignable, Category = "AutoFire")
	FRROnAutoFireReloadFinishedSignature OnReloadFinished;

	/** Calls AdvanceTime with the frame delta. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	/** Fires the shot that is due now, consumes ammo and schedules the next shot or a reload. */
	void FireShot(float FireRate);

	/** Starts a reload of GetReloadDuration seconds and resets the burst counter. */
	void StartReload();

	/** Ends the reload: refills the magazine, the next shot is due immediately. */
	void FinishReload();

	/** Removes one bullet (ConsumeAmmoEffect, or a direct base write when it is unset). */
	void ConsumeAmmo();

	/** Current value of an attribute on the configured ASC (0 without ASC). */
	float GetAttributeValue(const struct FGameplayAttribute& Attribute) const;

	/** Applies EffectClass to the configured ASC; returns false if the class is unset. */
	bool ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass) const;

	/** Instant GE applied after each shot. Unset = Ammo base lowered by 1 directly. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AutoFire")
	TSubclassOf<UGameplayEffect> ConsumeAmmoEffect;

	/** Instant GE applied at the end of a reload and at acquisition. Unset = Ammo base set to MagazineSize directly. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AutoFire")
	TSubclassOf<UGameplayEffect> RefillAmmoEffect;

	/** False = infinite ammo, no reload (e.g. enemy shooters); Ammo is never read. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AutoFire")
	bool bUseMagazine = true;

	/** Safety cap on shots plus reloads handled in one AdvanceTime call; extra time is dropped (logged). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AutoFire", meta = (ClampMin = 1))
	int32 MaxEventsPerAdvance = 64;

	/** ASC holding URRWeaponAttributeSet (the owner's). */
	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	/** Shot handler of the owner (FireOnce). */
	FSimpleDelegate ShotHandler;

	/** Bullets per burst (CSV constant). */
	int32 BurstCount = 1;

	/** Share of the period used by the burst, 0 to 1 (CSV constant). */
	float BurstDuty = 0.f;

	/** Base reload duration in seconds before ReloadSpeed (CSV constant). */
	float ReloadTime = 0.f;

	/** Seconds until the next shot; 0 = due now. */
	double TimeToNextShot = 0.0;

	/** Seconds left in the current reload. */
	double ReloadTimeRemaining = 0.0;

	/** Bullets fired in the current burst. */
	int32 ShotsFiredInBurst = 0;

	/** Shots fired since Configure. */
	int32 TotalShotsFired = 0;

	/** True between StartFiring and StopFiring. */
	bool bFiring = false;

	/** True while reloading. */
	bool bReloading = false;

	/** Time tolerance: an event due within this many seconds after the frame end fires in this frame (float delta rounding). */
	static constexpr double TimeTolerance = 1.0e-6;
};
