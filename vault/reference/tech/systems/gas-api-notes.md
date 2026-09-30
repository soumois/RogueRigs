---
id: gas-api-notes
description: "GAS 5.8 APIs used by the code skeletons: ability tags, effect removal, GE components, native tags, debug commands, runtime CSV, events."
parent: systems-index
kind: doc
---
# GAS 5.8 API notes

| API | Verified fact | Source |
|---|---|---|
| Ability tags | `AbilityTags` deprecated since 5.5; `SetAssetTags` only in the constructor | `Abilities/GameplayAbility.h:474-523` |
| Remove all effects | `RemoveActiveEffects` with an empty query removes all (needs `IsOwnerActorAuthoritative()`, true in solo); `GetAllActiveEffectHandles()` lists them | `GameplayEffect.cpp:5695-5712` |
| GE components | 11 components, incl. `UAssetTagsGameplayEffectComponent`, `UTargetTagsGameplayEffectComponent`, `UTargetTagRequirementsGameplayEffectComponent`, `UBlockAbilityTagsGameplayEffectComponent`, `UCancelAbilityTagsGameplayEffectComponent`. In a C++ constructor: `CreateDefaultSubobject` then `GEComponents.Add`; `AddComponent` / `FindOrAddComponent` are for runtime GEs | `Public/GameplayEffectComponents/`, `GameplayEffect.h:2497-2515` |
| Native tags | `UE_DECLARE_GAMEPLAY_TAG_EXTERN` / `UE_DEFINE_GAMEPLAY_TAG`, usable in constructors (Lyra practice, not rebuilt here) | — |
| Debug | `showdebug abilitysystem`, `AbilitySystem.Debug.NextCategory`, `AbilitySystem.Debug.SetCategory`; world overlays `AbilitySystem.DebugAttribute`, `AbilitySystem.DebugAbilityTags`, `AbilitySystem.DebugBlockedAbilityTags`; cheats `AbilityListGranted`, `EffectListActive`, `EffectApply`, `EffectRemove` | `AbilitySystemComponent.cpp:2510-2520`, `AbilitySystemDebugHUD.cpp:616-646` |
| Runtime CSV | `UDataTable::CreateTableFromCSVString`, `UCurveTable::CreateTableFromCSVString` work outside the editor; CSVs outside `Content/` must be added to non-asset directories to package | `Engine/Classes/Engine/DataTable.h:351`, `CurveTable.h:206` |
| Critical flag | `GetOwningSpecForPreExecuteMod()` gives the mutable spec; the same spec reaches `PostGameplayEffectExecute` | `GameplayEffectExecutionCalculation.h:36`, `GameplayEffect.cpp:3210-3290` |
| Events | `HandleGameplayEvent` activates the triggered ability in the same call stack. Applying a GE from `PostGameplayEffectExecute` is safe; cancelling an ability or removing an effect there is not | `AbilitySystemComponent_Abilities.cpp` |
| Wait tag | `WaitGameplayTagRemoved` fires immediately if the tag is already absent | `Abilities/Tasks/AbilityTask_WaitGameplayTag.cpp:78-95` |
| Missing SetByCaller | Returns 0 and logs a `LogGameplayEffects` error (`WarnIfNotFound = true`) | `GameplayEffect.cpp:2323-2342` |

## Relations
- [[systems-index]] — parent index.
- [[gas-engine-facts]] — behaviors that fix project rules.
- [[editor-workflow]] — Live Coding practice.
