// Rogue Rigs - test-only HUD classes: a HUD widget recording the pressure display events, a HUD panel, and a recorder of the gameplay HUD events (miner, weapon, run flow, progression).

#pragma once

#include "CoreMinimal.h"
#include "UI/RRHUDPanelWidget.h"
#include "UI/RRHUDWidget.h"
#include "RRTestHUDWidget.generated.h"

/**
 * Native URRHUDWidget subclass that records the BlueprintNativeEvents OnPlayerDamaged, OnHordeIncoming and OnAlertEventsChanged,
 * so automation tests (Source/RogueRigs/Tests) can check them without a widget Blueprint. A UCLASS, so it is compiled in every
 * configuration (UHT does not support WITH_DEV_AUTOMATION_TESTS guards); never used by gameplay code. Create with CreateWidget
 * and keep it alive with a TStrongObjectPtr.
 */
UCLASS(Transient, NotBlueprintable, NotBlueprintType, HideDropdown)
class ROGUERIGS_API URRTestHUDWidget : public URRHUDWidget
{
	GENERATED_BODY()

public:
	/** One OnPlayerDamaged call. */
	struct FDamageCall
	{
		/** Health lost. */
		float Amount = 0.f;
		/** Health left over MaxHealth. */
		float HealthFraction = 0.f;
	};

	/** Every OnPlayerDamaged call, in order. */
	TArray<FDamageCall> DamageCalls;

	/** SecondsUntil of every OnHordeIncoming call, in order. */
	TArray<float> HordeIncomingCalls;

	/** Events of every OnAlertEventsChanged call, in order. */
	TArray<TArray<FRRAlertBarEvent>> AlertEventCalls;

protected:
	/** Records the call. */
	virtual void OnPlayerDamaged_Implementation(float Amount, float HealthFraction) override { DamageCalls.Add({ Amount, HealthFraction }); }

	/** Records the call. */
	virtual void OnHordeIncoming_Implementation(float SecondsUntil) override { HordeIncomingCalls.Add(SecondsUntil); }

	/** Records the call. */
	virtual void OnAlertEventsChanged_Implementation(const TArray<FRRAlertBarEvent>& Events) override { AlertEventCalls.Add(Events); }
};

/** Native HUD panel for the tests: URRHUDPanelWidget counts its Bind hook calls (GetMinerBindCount...). Never used by gameplay code. */
UCLASS(Transient, NotBlueprintable, NotBlueprintType, HideDropdown)
class ROGUERIGS_API URRTestHUDPanelWidget : public URRHUDPanelWidget
{
	GENERATED_BODY()
};

/** Records the gameplay HUD events (ARRMiner, ARRWeapon, URRRunFlowSubsystem, URRProgressionComponent) for RogueRigs.HUD.Events* tests. */
UCLASS(Transient, NotBlueprintable, NotBlueprintType, HideDropdown)
class ROGUERIGS_API URRTestHUDEventRecorder : public UObject
{
	GENERATED_BODY()

public:
	/** (Current, Max) of every ARRMiner::OnHealthChanged. */
	TArray<FVector2D> HealthCalls;

	/** (Amount, HealthFraction) of every ARRMiner::OnDamaged. */
	TArray<FVector2D> DamageCalls;

	/** (Rounds, MagazineSize) of every ARRWeapon::OnAmmoChanged. */
	TArray<FIntPoint> AmmoCalls;

	/** Weapon of every ARRWeapon::OnAmmoChanged. */
	TArray<TWeakObjectPtr<ARRWeapon>> AmmoWeapons;

	/** Slot index of every ARRMiner::OnWeaponSlotChanged. */
	TArray<int32> SlotIndices;

	/** Slot state of every ARRMiner::OnWeaponSlotChanged. */
	TArray<FRRWeaponSlotHUD> Slots;

	/** Events of every URRRunFlowSubsystem::OnAlertBarEventsChanged. */
	TArray<TArray<FRRAlertBarEvent>> AlertBarCalls;

	/** (HordeIndex, SecondsUntil) of every URRRunFlowSubsystem::OnHordeIncoming. */
	TArray<TPair<int32, float>> HordeCalls;

	/** Target of every URRRunFlowSubsystem::OnLocatorTargetChanged. */
	TArray<ERRLocatorTarget> LocatorTargets;

	/** Location of every URRRunFlowSubsystem::OnLocatorTargetChanged. */
	TArray<FVector> LocatorLocations;

	/** Amount of every URRProgressionComponent::OnXPGained. */
	TArray<float> XPGains;

	/** Records the call. */
	UFUNCTION()
	void HandleHealthChanged(float Current, float Max) { HealthCalls.Add(FVector2D(Current, Max)); }

	/** Records the call. */
	UFUNCTION()
	void HandleDamaged(float Amount, float HealthFraction) { DamageCalls.Add(FVector2D(Amount, HealthFraction)); }

	/** Records the call. */
	UFUNCTION()
	void HandleAmmoChanged(ARRWeapon* Weapon, int32 Rounds, int32 MagazineSize) { AmmoWeapons.Add(Weapon); AmmoCalls.Add(FIntPoint(Rounds, MagazineSize)); }

	/** Records the call. */
	UFUNCTION()
	void HandleWeaponSlotChanged(int32 SlotIndex, const FRRWeaponSlotHUD& Slot) { SlotIndices.Add(SlotIndex); Slots.Add(Slot); }

	/** Records the call. */
	UFUNCTION()
	void HandleAlertBarEventsChanged(const TArray<FRRAlertBarEvent>& Events) { AlertBarCalls.Add(Events); }

	/** Records the call. */
	UFUNCTION()
	void HandleHordeIncoming(int32 HordeIndex, float SecondsUntil) { HordeCalls.Add(TPair<int32, float>(HordeIndex, SecondsUntil)); }

	/** Records the call. */
	UFUNCTION()
	void HandleLocatorTargetChanged(ERRLocatorTarget Target, FVector WorldLocation) { LocatorTargets.Add(Target); LocatorLocations.Add(WorldLocation); }

	/** Records the call. */
	UFUNCTION()
	void HandleXPGained(float Amount) { XPGains.Add(Amount); }
};
