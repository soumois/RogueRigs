// Rogue Rigs - test-only HUD widget: records the pressure display events of URRHUDWidget (damage vignette, horde banner, alert-bar icons) for the automation tests.

#pragma once

#include "CoreMinimal.h"
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
