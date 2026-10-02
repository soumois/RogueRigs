// Rogue Rigs - generic enemy movement: each tick asks the active path provider for a direction and feeds it to the pawn's movement input.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlowField/RRPathProvider.h"
#include "RREnemyMovementComponent.generated.h"

/**
 * Drives a ground robot (an ACharacter or any APawn) toward the goal of a path provider. Each tick: no input within
 * ArrivalDistance of the goal; a straight line to the goal when it is within DirectChaseRange and the provider reports a
 * direct path (also used inside the goal cell, where the provider direction is zero); otherwise the provider direction.
 * The direction goes to APawn::AddMovementInput (scale 1): speed and acceleration stay with the pawn's movement component.
 * Provider: SetPathProvider, else the world subsystem or owner component of class PathProviderClass (default the flow field).
 */
UCLASS(ClassGroup = (RogueRigs), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URREnemyMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Ticks before physics so the input is consumed by the same frame's movement. */
	URREnemyMovementComponent();

	/** Asks the provider for a direction and adds it as movement input to the owning pawn. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Overrides the provider (null returns to PathProviderClass). */
	UFUNCTION(BlueprintCallable, Category = "EnemyMovement")
	void SetPathProvider(TScriptInterface<IRRPathProvider> Provider);

	/** Starts or stops adding movement input (e.g. stun, attack wind-up). */
	UFUNCTION(BlueprintCallable, Category = "EnemyMovement")
	void SetMovementEnabled(bool bEnabled) { bMovementEnabled = bEnabled; }

	/** True while the component adds movement input. */
	UFUNCTION(BlueprintPure, Category = "EnemyMovement")
	bool IsMovementEnabled() const { return bMovementEnabled; }

	/** True when the last tick found the pawn within ArrivalDistance of the goal. */
	UFUNCTION(BlueprintPure, Category = "EnemyMovement")
	bool HasArrived() const { return bArrived; }

	/** Direction added on the last tick (zero when none). */
	UFUNCTION(BlueprintPure, Category = "EnemyMovement")
	FVector GetLastMoveDirection() const { return LastMoveDirection; }

	/** Distance to the goal (2D, uu) under which the pawn stops. */
	float GetArrivalDistance() const { return ArrivalDistance; }

protected:
	/** The active provider: the override, else the PathProviderClass world subsystem or owner component; null when none. */
	IRRPathProvider* ResolveProvider();

	/** Class of the default provider: a world subsystem or an actor component implementing IRRPathProvider. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemyMovement", meta = (MustImplement = "/Script/RogueRigs.RRPathProvider"))
	TSubclassOf<UObject> PathProviderClass;

	/** 2D distance to the goal under which the pawn stops (provisional; a robot sets it from its attack range). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemyMovement", meta = (ClampMin = "0.0", Units = "cm"))
	float ArrivalDistance = 100.f;

	/** 2D distance to the goal under which the pawn steers straight at it when the provider reports a direct path (provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemyMovement", meta = (ClampMin = "0.0", Units = "cm"))
	float DirectChaseRange = 400.f;

	/** True while the component adds movement input. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemyMovement")
	bool bMovementEnabled = true;

	/** Provider set with SetPathProvider; overrides PathProviderClass. */
	UPROPERTY(Transient)
	TScriptInterface<IRRPathProvider> OverrideProvider;

	/** True when the last tick found the pawn within ArrivalDistance. */
	bool bArrived = false;

	/** Direction added on the last tick. */
	FVector LastMoveDirection = FVector::ZeroVector;
};
