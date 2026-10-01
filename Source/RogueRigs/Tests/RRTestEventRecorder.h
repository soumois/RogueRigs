// Rogue Rigs - test-only listener: records dynamic delegate broadcasts (pickup collected, cell destroyed, auto-fire shots and reloads, projectile stop) for the automation tests.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"
#include "Engine/HitResult.h"
#include "Grid/RRArenaGrid.h"
#include "Pickups/RRPickup.h"
#include "RRTestEventRecorder.generated.h"

class UAbilitySystemComponent;

/**
 * Records the broadcasts of ARRMiner::OnPickupCollected, ARRArenaGrid::OnCellDestroyed, the URRAutoFireComponent
 * events (OnShotFired, OnReloadStarted, OnReloadFinished) and UProjectileMovementComponent::OnProjectileStop so
 * automation tests (Source/RogueRigs/Tests) can check that the events fire. Dynamic delegates need a UFunction to
 * bind to, so this is a UCLASS and is compiled in every configuration (UHT does not support WITH_DEV_AUTOMATION_TESTS
 * guards); it is never used by gameplay code. Create with NewObject, keep it alive with a TStrongObjectPtr.
 */
UCLASS(Transient, NotBlueprintable, NotBlueprintType, HideDropdown)
class ROGUERIGS_API URRTestEventRecorder : public UObject
{
	GENERATED_BODY()

public:
	/** Bind to ARRMiner::OnPickupCollected: appends the collected pickup type. */
	UFUNCTION()
	void HandlePickupCollected(ERRPickupType Type) { CollectedPickups.Add(Type); }

	/** Bind to ARRArenaGrid::OnCellDestroyed: appends the destroyed cell. */
	UFUNCTION()
	void HandleCellDestroyed(FIntPoint Cell, ERRCellType Type, ERRTerrainDamageSource Source) { DestroyedCells.Add(Cell); }

	/** Bind to URRAutoFireComponent::OnShotFired: appends the shot index in its burst. */
	UFUNCTION()
	void HandleShotFired(int32 ShotIndexInBurst) { ShotIndices.Add(ShotIndexInBurst); }

	/** Bind to URRAutoFireComponent::OnReloadStarted: appends the reload duration. */
	UFUNCTION()
	void HandleReloadStarted(float ReloadDuration) { ReloadDurations.Add(ReloadDuration); }

	/** Bind to URRAutoFireComponent::OnReloadFinished: appends the Ammo of the ammo source at that moment (-1 without source). */
	UFUNCTION()
	void HandleReloadFinished();

	/** Bind to UProjectileMovementComponent::OnProjectileStop: appends the impact. */
	UFUNCTION()
	void HandleProjectileStop(const FHitResult& ImpactResult) { ProjectileStops.Add(ImpactResult); }

	/** Sets the ASC whose Ammo is sampled when a reload finishes (the weapon ASC). */
	void SetAmmoSource(UAbilitySystemComponent* InAmmoSource) { AmmoSource = InAmmoSource; }

	/** Pickup types received through HandlePickupCollected, in order. */
	const TArray<ERRPickupType>& GetCollectedPickups() const { return CollectedPickups; }

	/** Cells received through HandleCellDestroyed, in order. */
	const TArray<FIntPoint>& GetDestroyedCells() const { return DestroyedCells; }

	/** Shot indices received through HandleShotFired, in order. */
	const TArray<int32>& GetShotIndices() const { return ShotIndices; }

	/** Reload durations received through HandleReloadStarted, in order. */
	const TArray<float>& GetReloadDurations() const { return ReloadDurations; }

	/** Ammo sampled at each HandleReloadFinished, in order. */
	const TArray<float>& GetAmmoAtReloadFinished() const { return AmmoAtReloadFinished; }

	/** Impacts received through HandleProjectileStop, in order. */
	const TArray<FHitResult>& GetProjectileStops() const { return ProjectileStops; }

protected:
	/** Pickup types collected so far. */
	TArray<ERRPickupType> CollectedPickups;

	/** Cells destroyed so far. */
	TArray<FIntPoint> DestroyedCells;

	/** Shot indices fired so far. */
	TArray<int32> ShotIndices;

	/** Durations of the reloads started so far. */
	TArray<float> ReloadDurations;

	/** Ammo read on AmmoSource at each reload end. */
	TArray<float> AmmoAtReloadFinished;

	/** Projectile impacts so far. */
	TArray<FHitResult> ProjectileStops;

	/** ASC sampled by HandleReloadFinished. */
	TWeakObjectPtr<UAbilitySystemComponent> AmmoSource;
};
