---
id: sys-damage
description: "Damage chain from shot to death: weapon as instigator, spec carried by the projectile, IncomingDamage, single death, i-frames, damage numbers."
parent: systems-core-index
kind: doc
---
# Damage pipeline

- responsibility: every damage to a character, from spec creation to death.
- main_classes: `URRGE_Damage` (player → enemy, ExecCalc with critical), `URRGE_DamageFlat` (enemy → player, SetByCaller), `URRGE_DamageExplosion` (explosions → everyone), `URRGE_IFrames`, `URRGA_OnDamaged`.
- implemented (in `Source/RogueRigs/Damage/`): `URRGE_Damage` (instant; execution `URRDamageExecution`; `UTargetTagRequirementsGameplayEffectComponent` ignoring `State.Dead`, `State.Invulnerable`). `URRGE_DamageFlat` (instant; execution `URRDamageFlatExecution`: SetByCaller `Data.Damage` × source `AttackPower`, 1 when the source ASC has no `URREnemyAttributeSet`, read at execution, no crit; same tag requirements). `URRGE_DamageExplosion` (instant; one Additive modifier on `IncomingDamage` with SetByCaller `Data.Damage` magnitude, no scaling, no crit; same tag requirements, so explosions respect i-frames; applied only by `URRExplosionSubsystem`, which caps the amount on a full-health miner — no-one-shot rule in [[sys-explosions]]). `URRGE_IFrames` (duration = SetByCaller `Data.Duration`; grants `State.Invulnerable` through `UTargetTagsGameplayEffectComponent`). Native tags in `GAS/RRGameplayTags.h` (`State_Dead`, `State_Invulnerable`, `Effect_Damage_Critical`, `Data_Damage`, `Data_Duration`). Not yet: `URRGA_OnDamaged`. The folder also holds `URRDamageNumberSubsystem`, the merged, Niagara-rendered damage numbers fed by robots ([[sys-damage-numbers]]).
- `URRDamageExecution`: captures the source weapon's `Damage`, `CritChance`, `CritDamage` (snapshot at fire time), crit if `FRand() < CritChance`, outputs `Damage` (× `CritDamage` on a crit) as Additive on `IncomingDamage`.
- Critical flag: on a crit the ExecCalc adds the dynamic asset tag `Effect.Damage.Critical` to the executed spec through `GetOwningSpecForPreExecuteMod()`; listeners read `Data.EffectSpec.GetDynamicAssetTags()` in `PostGameplayEffectExecute` ([[gas-api-notes]]). The executed spec is the target ASC's copy, so the carried handle never keeps the tag; the effect context is shared between copies, hence not used.
- Death event: on the hit taking `Health` from above 0 to 0, `URRHealthAttributeSet` adds the loose tag `State.Dead` to its ASC (only if absent) and, after `OnDamageReceived`, broadcasts the native multicast `OnDeath(AActor* Instigator)` (original instigator of the effect context). Just before `OnDeath` it records the class of the killing effect, read by `GetLethalEffectClass` (task-096-01: the kamikaze tells explosion kills from player kills). Opt-out: `SetDeathEnabled(false)` (no caller since the target dummy was removed in task-031-01). Only `IncomingDamage` hits are watched.
- Per-hit event: after consuming a positive `IncomingDamage`, `URRHealthAttributeSet` broadcasts the native multicast `OnDamageReceived(Damage, bIsCritical, HealthLeft)` — raw damage before the `Health` clamp, crit read from the spec's dynamic asset tags, `Health` after the hit. Feedback listeners bind to it instead of parsing specs; main user: robots ([[sys-enemies-base]], damage numbers). Handlers run inside the GE execution (same safety rules as post-execute).
- Global hit event (task-212-01): after its own events every health set broadcasts the static native `URRHealthAttributeSet::OnAnyDamageReceived(FRRDamageEventInfo)` (target avatar, original instigator, effect causer, source object, effect class, raw damage, crit, killed), for observers that cannot bind each actor; listeners filter by world. User: [[sys-run-log]].

## Chain
1. On fire, the weapon's ASC creates the spec. Instigator and causer = the weapon (approved). Kill credit and XP go up through `Weapon->GetOwner()`.
2. The projectile carries the spec and applies it only on impact ([[sys-projectiles]]); the shotgun's pellets carry one spec per shot ([[sys-weapons]]). The engine copies the spec on each application, so a piercing projectile reapplies the same handle.
3. Pre-execute refuses the execution if the target has `State.Invulnerable` or `State.Dead` (provisional). This also blocks DoT ticks after death. Implemented on `URRGE_Damage` as application tag requirements (the spec is refused, not executed).
4. The ExecCalc rolls the critical and writes `IncomingDamage` (provisional). No critical on periodic damage (`Effect.Damage.DoT`) (provisional; not implemented, the tag does not exist yet).
5. Post-execute converts `IncomingDamage` into `Shield` then `Health` loss (task-028-01: `Shield`, 0 for all but the shield chaser, absorbs up to its value, the rest of the same hit reaches `Health`; the hit taking it from above 0 to 0 broadcasts the native `OnShieldBroken` once, after `OnDamageReceived` and before `OnDeath`): clamp, single death, single damage log, damage number (raw hit, shield part included).
6. Death fires once, when `Health` goes from above 0 to 0 or below (approved). Damage GEs and statuses also refuse `State.Dead` targets via `UTargetTagRequirementsGameplayEffectComponent` (approved).
7. Player i-frames: `URRGE_IFrames` applied from post-execute, in the same call stack, so the next hit in the same frame is refused (provisional; 0.25 s per [[miner]]). Implemented by the opting-in pawn: `ARRMiner` binds `OnDamageReceived` and applies it on non-lethal hits ([[sys-player]]). `URRGA_OnDamaged` handles only reaction and feedback.

- entry_points: `ARRWeapon::MakeDamageSpec()` ([[sys-weapons]]); the spec is applied by `ARRProjectile` on impact ([[sys-projectiles]]) (shotgun pellets included: each hit's copy runs `URRDamageExecution`, so one critical roll per pellet hit; `RogueRigs.Shotgun.CritPerPellet`); enemy melee: `ARRChaser` applies `URRGE_DamageFlat` from its own ASC ([[sys-enemies]]); explosions: `URRExplosionSubsystem` applies `URRGE_DamageExplosion` from each target ASC with the explosion instigator in the context ([[sys-explosions]]).
- data: damage values from CSV.
- depends_on: [[sys-gas-core]], [[sys-weapons]] (source attributes).
- implements: [[damage-numbers]], [[enemy-death]], [[miner]].
- gotchas:
  - Damage numbers: a cue on the GE with `MagnitudeAttribute` receives the total before clamp; trigger manually only to pass the critical flag or the clamped value.
  - Applying a GE from `PostGameplayEffectExecute` is safe; cancelling an ability or removing an effect there is not.
  - Future resistance: an ExecCalc on `URRGE_DamageFlat`, still writing `IncomingDamage`.

## Relations
- [[systems-index]] — parent index.
- [[sys-explosions]] — explosions entering this chain.
- [[sys-damage-numbers]] — where hit values become floating numbers.
- [[sys-run-log]] — listens to the global hit event.
