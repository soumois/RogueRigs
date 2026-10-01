// Rogue Rigs - movement attributes shared by the player and enemies.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RRMovementAttributeSet.generated.h"

/**
 * Movement of a combat actor (player, enemies). MoveSpeed is in uu/s; the owning pawn copies it
 * to its movement component. Base and current values are clamped to >= 0.
 */
UCLASS()
class ROGUERIGS_API URRMovementAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	/** Starts MoveSpeed at 0; the base value comes from the CSV loader. */
	URRMovementAttributeSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	ATTRIBUTE_ACCESSORS_BASIC(URRMovementAttributeSet, MoveSpeed)

	/** Clamps the base MoveSpeed to >= 0. */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/** Clamps the current MoveSpeed to >= 0. */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

protected:
	/** Maximum walk speed in uu/s. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Movement")
	FGameplayAttributeData MoveSpeed;
};
