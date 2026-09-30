---
id: sys-status-effects
description: "Stacking rules for stat bonuses and statuses (slow, burn), mandatory stacking settings, and the enemy stun implementation."
parent: systems-index
kind: doc
---
# Status effects and stacking

- responsibility: how bonuses and statuses combine on an ASC.

## Stacking rules (approved)
| Effect | GAS operation | Stacking | Reapplication |
|---|---|---|---|
| Percentage stat bonus (damage, fire rate) | `MultiplyAdditive` | one active GE per pick, never removed in a run | — |
| Point stat bonus (crit chance, crit damage, magazine size) | `AddBase` | one active GE per pick, never removed in a run | — |
| Slow (ice) | `MultiplyCompound`, magnitude 1 − X | one per target: `AggregateByTarget`, `StackLimitCount = 1` | duration reset (`RefreshOnSuccessfulApplication`); last application's values win |
| Burn (DoT) | loss on `IncomingDamage` each tick | same + `StackPeriodResetPolicy = NeverReset` | duration reset; last application's values win |

- Every status GE must set stacking: with `StackingType = None`, two ×0.5 slows would give ×0.
- A status GE keeps the same `DynamicGrantedTags` on every application, or GAS fires an `ensure`.
- The upgrade table stores the operation of each row; the balance agent computes with these formulas.

## Enemy stun (provisional)
- The stun GE grants `State.Stunned` and carries `UCancelAbilityTagsGameplayEffectComponent`, which cancels running `Ability.Attack` abilities; `ActivationBlockedTags` blocks new ones.
- The AI listens to the tag (`RegisterGameplayTagEvent`) to stop and resume movement ([[sys-enemy-ai]]).
- Only enemies are stunned: the player's fire does not test `State.Stunned`.

- data: upgrade CSV rows with their operation.
- depends_on: [[sys-gas-core]].
- implements: [[upgrades]] (stats now; EMP in M3; ice and burn deferred).
- gotchas: `MultiplyAdditive` sums deviations from 1 (1.2 and 1.2 → ×1.4); `MultiplyCompound` multiplies (×1.44). See [[gas-engine-facts]].

## Relations
- [[systems-index]] — parent index.
