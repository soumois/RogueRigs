---
id: gas-engine-facts
description: "GAS 5.8 behaviors read in engine source that set Rogue Rigs rules: effect context, stacking, multiply ops, spec copy, cues, clamps, snapshots."
parent: systems-core-index
kind: doc
---
# GAS 5.8 behaviors that fix project rules

Paths relative to `Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/`. Verified 2026-09-30.

| Topic | 5.8 behavior | Project rule | Source |
|---|---|---|---|
| Effect context | `AddInstigator` caches the instigator's ASC; source captures read it | Instigator = weapon | `GameplayEffectTypes.cpp:200-210`, `GameplayEffect.cpp:1838-1848` |
| Stacking (`AggregateByTarget`) | Reapplication replaces the old spec (magnitudes, duration, instigator); only stack count and granted abilities stay. `NeverReset` keeps the period timer. `ensure` if `DynamicGrantedTags` differ | Last application wins; constant dynamic tags | `GameplayEffect.cpp:4211-4307` |
| Multiply ops | `MultiplyAdditive` sums deviations from 1; `MultiplyCompound` multiplies. `((Base + AddBase) × MultiplyAdditive / DivideAdditive × MultiplyCompound) + AddFinal` | See [[sys-status-effects]] | `GameplayEffectTypes.h:112-150`, `GameplayEffectAggregator.cpp:76-99` |
| Spec application | `ApplyGameplayEffectSpecToSelf` takes the spec by `const&` and works on its own copy | A piercing projectile reapplies the same handle, no clone | `AbilitySystemComponent.cpp:1076-1110` |
| Cue and ExecCalc | ExecCalc outputs join `ModifiedAttributes`; a cue with `MagnitudeAttribute` gets that total, before clamp | Damage number cue on the GE | `GameplayEffect.cpp:4114-4122`, `AbilitySystemComponent.cpp:1471-1473` |
| Clamps | `PreAttributeBaseChange` clamps base (also via `SetNumericAttributeBase`); `PreAttributeChange` clamps current; `PostAttributeChange` follows every change | Base clamp in `PreAttributeBaseChange`; `Health` fixed in `PostAttributeChange` | `AttributeSet.h:200-234` |
| Snapshot | Source captures freeze at spec creation, target captures at application; each shot makes a new spec | Turret stats frozen by attribute copy | `GameplayEffect.cpp:1838-1848` |
| Tag requirements | `ApplicationTagRequirements` lives on `UTargetTagRequirementsGameplayEffectComponent`; the `UGameplayEffect` field is deprecated | Invulnerability and `State.Dead` use this component | `GameplayEffect.h:2365-2366` |
| Ability instancing | Default `InstancedPerExecution`; `NonInstanced` deprecated since 5.5 | `URRGameplayAbility` uses `InstancedPerActor` | `GameplayAbility.cpp:102` |
| Modifier scope | Each modifier has `SourceTags` / `TargetTags` requirements, evaluable per query | One ASC per weapon kept, for readable debug | `AbilitySystemBlueprintLibrary.h:194` |

## Relations
- [[systems-index]] — parent index.
- [[gas-api-notes]] — APIs used by the skeletons.
