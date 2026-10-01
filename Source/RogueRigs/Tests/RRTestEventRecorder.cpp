// Rogue Rigs - test-only listener: records dynamic delegate broadcasts for the automation tests.

#include "Tests/RRTestEventRecorder.h"

#include "AbilitySystemComponent.h"
#include "Weapons/RRWeaponAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRTestEventRecorder)

void URRTestEventRecorder::HandleReloadFinished()
{
	const UAbilitySystemComponent* Source = AmmoSource.Get();
	AmmoAtReloadFinished.Add(Source ? Source->GetNumericAttribute(URRWeaponAttributeSet::GetAmmoAttribute()) : -1.f);
}
