// Rogue Rigs - i-frames effect: grants State.Invulnerable for a SetByCaller duration (Data.Duration).

#include "Damage/RRGE_IFrames.h"

#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "GAS/RRGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRGE_IFrames)

URRGE_IFrames::URRGE_IFrames(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	// The duration is computed at application from the spec's SetByCaller Data.Duration.
	FSetByCallerFloat DurationByCaller;
	DurationByCaller.DataTag = RRGameplayTags::Data_Duration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(DurationByCaller);

	// GE components in a C++ constructor: CreateDefaultSubobject then GEComponents.Add (gas-api-notes).
	UTargetTagsGameplayEffectComponent* TargetTags = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTags"));
	FInheritedTagContainer GrantedTags;
	GrantedTags.Added.AddTag(RRGameplayTags::State_Invulnerable);
	TargetTags->SetAndApplyTargetTagChanges(GrantedTags);
	GEComponents.Add(TargetTags);
}
