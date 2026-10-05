// Rogue Rigs - test-only listener: records dynamic delegate broadcasts (pickup collected, cell destroyed, auto-fire shots and reloads, projectile stop, component hit, game over, explosion, XP and level-up) for the automation tests.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"
#include "Engine/HitResult.h"
#include "Grid/RRArenaGrid.h"
#include "Objectives/RRConsole.h"
#include "Pickups/RRPickup.h"
#include "Upgrades/RRUpgradeCard.h"
#include "RRTestEventRecorder.generated.h"

class UAbilitySystemComponent;

/**
 * Records the broadcasts of ARRMiner::OnPickupCollected, ARRArenaGrid::OnCellDestroyed, the URRAutoFireComponent
 * events (OnShotFired, OnReloadStarted, OnReloadFinished), UProjectileMovementComponent::OnProjectileStop and
 * UPrimitiveComponent::OnComponentHit, ARRGameMode::OnGameOver, URRExplosionSubsystem::OnExplosion and the
 * URRProgressionComponent events (OnXPChanged, OnLevelUp) and the URRLevelUpComponent events (OnLevelUpEffect,
 * OnUpgradeOfferShown, OnUpgradeOfferClosed) so
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

	/** Bind to UPrimitiveComponent::OnComponentHit (e.g. the miner capsule): appends the blocking hit. */
	UFUNCTION()
	void HandleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) { ComponentHits.Add(Hit); }

	/** Bind to ARRGameMode::OnGameOver: counts the broadcasts. */
	UFUNCTION()
	void HandleGameOver() { ++GameOverCount; }

	/** Bind to URRRunFlowSubsystem::OnExtractionReady: counts broadcasts. */
	UFUNCTION()
	void HandleExtractionReady() { ++ExtractionReadyCount; }

	/** Bind to URRRunFlowSubsystem::OnRunEnded: records each result. */
	UFUNCTION()
	void HandleRunEnded(bool bSuccess) { RunEndedResults.Add(bSuccess); }

	/** Bind to URRRunFlowSubsystem::OnShuttleCountdown: counts broadcasts and keeps the last value. */
	UFUNCTION()
	void HandleShuttleCountdown(float RemainingSeconds, FVector ShuttleLocation) { ++ShuttleCountdownCount; LastShuttleCountdown = RemainingSeconds; }

	/** Bind to ARRConsole::OnConsoleStateChanged: appends the new state. */
	UFUNCTION()
	void HandleConsoleStateChanged(ARRConsole* Console, ERRConsoleState NewState) { ConsoleStates.Add(NewState); }

	/** Bind to URRExplosionSubsystem::OnExplosion: appends the row name and the center. */
	UFUNCTION()
	void HandleExplosion(FVector Center, FName RowName) { ExplosionRows.Add(RowName); ExplosionCenters.Add(Center); }

	/** Bind to URRProgressionComponent::OnXPChanged: appends (Current, Needed, Level) as a vector. */
	UFUNCTION()
	void HandleXPChanged(float Current, float Needed, int32 Level) { XPChanges.Add(FVector(Current, Needed, Level)); }

	/** Bind to ARRTurretLauncher::OnTurretChargesChanged: appends (Charges, MaxCharges, RechargeDuration) as a vector. */
	UFUNCTION()
	void HandleTurretChargesChanged(int32 Charges, int32 MaxCharges, float RechargeDuration) { TurretCharges.Add(FVector(Charges, MaxCharges, RechargeDuration)); }

	/** (Charges, MaxCharges, RechargeDuration) received through HandleTurretChargesChanged, in order. */
	const TArray<FVector>& GetTurretCharges() const { return TurretCharges; }

	/** Bind to URRProgressionComponent::OnLevelUp: appends the new level. */
	UFUNCTION()
	void HandleLevelUp(int32 NewLevel) { LevelUps.Add(NewLevel); }

	/** Bind to URRLevelUpComponent::OnLevelUpEffect: appends the new level. */
	UFUNCTION()
	void HandleLevelUpEffect(int32 NewLevel) { LevelUpEffects.Add(NewLevel); }

	/** Bind to URRLevelUpComponent::OnUpgradeOfferShown: appends the card count and the reroll flag, keeps the cards. */
	UFUNCTION()
	void HandleUpgradeOfferShown(const TArray<FRRUpgradeCard>& Cards, bool bCanReroll) { OfferCardCounts.Add(Cards.Num()); OfferRerollFlags.Add(bCanReroll); LastOfferCards = Cards; }

	/** Bind to URRLevelUpComponent::OnUpgradeOfferClosed: counts the broadcasts. */
	UFUNCTION()
	void HandleUpgradeOfferClosed() { ++OfferClosedCount; }

	/** Levels received through HandleLevelUpEffect, in order. */
	const TArray<int32>& GetLevelUpEffects() const { return LevelUpEffects; }

	/** Card count of each OnUpgradeOfferShown, in order. */
	const TArray<int32>& GetOfferCardCounts() const { return OfferCardCounts; }

	/** bCanReroll of each OnUpgradeOfferShown, in order. */
	const TArray<bool>& GetOfferRerollFlags() const { return OfferRerollFlags; }

	/** Cards of the last OnUpgradeOfferShown. */
	const TArray<FRRUpgradeCard>& GetLastOfferCards() const { return LastOfferCards; }

	/** Number of OnUpgradeOfferClosed broadcasts. */
	int32 GetOfferClosedCount() const { return OfferClosedCount; }

	/** (Current, Needed, Level) received through HandleXPChanged, in order. */
	const TArray<FVector>& GetXPChanges() const { return XPChanges; }

	/** Levels received through HandleLevelUp, in order. */
	const TArray<int32>& GetLevelUps() const { return LevelUps; }

	/** Row names received through HandleExplosion, in order. */
	const TArray<FName>& GetExplosionRows() const { return ExplosionRows; }

	/** Centers received through HandleExplosion, in order. */
	const TArray<FVector>& GetExplosionCenters() const { return ExplosionCenters; }

	/** Number of OnGameOver broadcasts received through HandleGameOver. */
	int32 GetGameOverCount() const { return GameOverCount; }

	/** Number of OnExtractionReady broadcasts received through HandleExtractionReady. */
	int32 GetExtractionReadyCount() const { return ExtractionReadyCount; }

	/** Results of the OnRunEnded broadcasts received through HandleRunEnded. */
	const TArray<bool>& GetRunEndedResults() const { return RunEndedResults; }

	/** Number of OnShuttleCountdown broadcasts received through HandleShuttleCountdown. */
	int32 GetShuttleCountdownCount() const { return ShuttleCountdownCount; }

	/** Last seconds received through HandleShuttleCountdown (-1 = none). */
	float GetLastShuttleCountdown() const { return LastShuttleCountdown; }

	/** Console states received through HandleConsoleStateChanged, in order. */
	const TArray<ERRConsoleState>& GetConsoleStates() const { return ConsoleStates; }

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

	/** Blocking hits received through HandleComponentHit, in order. */
	const TArray<FHitResult>& GetComponentHits() const { return ComponentHits; }

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

	/** Blocking hits received so far. */
	TArray<FHitResult> ComponentHits;

	/** Explosion row names received so far. */
	TArray<FName> ExplosionRows;

	/** Explosion centers received so far. */
	TArray<FVector> ExplosionCenters;

	/** Turret charge changes received so far: (Charges, MaxCharges, RechargeDuration). */
	TArray<FVector> TurretCharges;

	/** XP changes received so far: (Current, Needed, Level). */
	TArray<FVector> XPChanges;

	/** Level-ups received so far. */
	TArray<int32> LevelUps;

	/** Level-up effect levels received so far. */
	TArray<int32> LevelUpEffects;

	/** Card count of each offer shown so far. */
	TArray<int32> OfferCardCounts;

	/** Reroll flag of each offer shown so far. */
	TArray<bool> OfferRerollFlags;

	/** Cards of the last offer shown. */
	TArray<FRRUpgradeCard> LastOfferCards;

	/** Offer closed broadcasts received so far. */
	int32 OfferClosedCount = 0;

	/** Game over broadcasts received so far. */
	int32 GameOverCount = 0;

	/** Extraction-ready broadcasts received so far. */
	int32 ExtractionReadyCount = 0;

	/** Run end results received so far. */
	TArray<bool> RunEndedResults;

	/** Shuttle countdown broadcasts received so far. */
	int32 ShuttleCountdownCount = 0;

	/** Last shuttle countdown received. */
	float LastShuttleCountdown = -1.f;

	/** Console states received so far. */
	TArray<ERRConsoleState> ConsoleStates;

	/** ASC sampled by HandleReloadFinished. */
	TWeakObjectPtr<UAbilitySystemComponent> AmmoSource;
};
