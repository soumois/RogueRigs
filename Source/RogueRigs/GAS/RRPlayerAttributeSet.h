// Rogue Rigs - attributes owned by the miner only.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RRPlayerAttributeSet.generated.h"

/**
 * Stats owned by the miner (player-stats): pickup radius, XP gain, mining speed, luck.
 * PickupRadius, XPGain and MiningSpeed are clamped to >= 0 (base and current); Luck is not clamped.
 * The drone reads MiningSpeed on the player's ASC.
 */
UCLASS()
class ROGUERIGS_API URRPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	/** Starts every attribute at 0; base values come from the CSV loader. */
	URRPlayerAttributeSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	ATTRIBUTE_ACCESSORS_BASIC(URRPlayerAttributeSet, PickupRadius)
	ATTRIBUTE_ACCESSORS_BASIC(URRPlayerAttributeSet, XPGain)
	ATTRIBUTE_ACCESSORS_BASIC(URRPlayerAttributeSet, MiningSpeed)
	ATTRIBUTE_ACCESSORS_BASIC(URRPlayerAttributeSet, Luck)

	/** Clamps base PickupRadius, XPGain and MiningSpeed to >= 0. */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/** Clamps current PickupRadius, XPGain and MiningSpeed to >= 0. */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

protected:
	/** Collection radius of XP, ore and heal pickups, in uu. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Player")
	FGameplayAttributeData PickupRadius;

	/** Multiplier on XP gained (1 = no bonus). */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Player")
	FGameplayAttributeData XPGain;

	/** Multiplier on the drone's dig speed (1 = no bonus). */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Player")
	FGameplayAttributeData MiningSpeed;

	/** Points that raise the odds of higher-rarity upgrade cards. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Player")
	FGameplayAttributeData Luck;

	/** True for the attributes that must never go below 0 (PickupRadius, XPGain, MiningSpeed). */
	static bool IsNonNegativeAttribute(const FGameplayAttribute& Attribute);
};
