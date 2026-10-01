---
id: sys-damage
description: "Damage chain from shot to death: weapon as instigator, spec carried by the projectile, IncomingDamage, single death, i-frames, damage numbers."
parent: systems-index
kind: doc
---
# Damage pipeline

- responsibility: every damage to a character, from spec creation to death.
- main_classes: `URRGE_Damage` (player → enemy, ExecCalc with critical), `URRGE_DamageFlat` (enemy → player, SetByCaller), `URRGE_IFrames`, `URRGA_OnDamaged`.

## Chain
1. On fire, the weapon's ASC creates the spec. Instigator and causer = the weapon (approved). Kill credit and XP go up through `Weapon->GetOwner()`.
2. The projectile carries the spec and applies it only on impact ([[sys-projectiles]]). The engine copies the spec on each application, so a piercing projectile reapplies the same handle.
3. Pre-execute refuses the execution if the target has `State.Invulnerable` or `State.Dead` (provisional). This also blocks DoT ticks after death.
4. The ExecCalc rolls the critical and writes `IncomingDamage` (provisional). No critical on periodic damage (`Effect.Damage.DoT`) (provisional).
5. Post-execute converts `IncomingDamage` into `Health` loss: clamp, single death, single damage log, damage number.
6. Death fires once, when `Health` goes from above 0 to 0 or below (approved). Damage GEs and statuses also refuse `State.Dead` targets via `UTargetTagRequirementsGameplayEffectComponent` (approved).
7. Player i-frames: `URRGE_IFrames` applied from post-execute, in the same call stack, so the next hit in the same frame is refused (provisional; duration open). `URRGA_OnDamaged` handles only reaction and feedback.

- data: damage values from CSV.
- depends_on: [[sys-gas-core]].
- implements: [[damage-numbers]], [[enemy-death]], [[miner]].
- gotchas:
  - Damage numbers: a cue on the GE with `MagnitudeAttribute` receives the total before clamp; trigger manually only to pass the critical flag or the clamped value.
  - Applying a GE from `PostGameplayEffectExecute` is safe; cancelling an ability or removing an effect there is not.
  - Future resistance: an ExecCalc on `URRGE_DamageFlat`, still writing `IncomingDamage`.

## Relations
- [[systems-index]] — parent index.
- [[sys-explosions]] — explosions entering this chain.
