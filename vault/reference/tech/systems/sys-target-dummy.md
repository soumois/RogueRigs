---
id: sys-target-dummy
description: "Debug target dummy: immobile ASC actor that never dies, hit flash, damage numbers, health refill. Open to test weapons or hit feedback."
parent: systems-index
kind: doc
---
# Target dummy (debug)

- responsibility: a punching ball to test weapons, damage and hit feedback in M1.
- main_classes (in `Source/RogueRigs/Debug/`):
  - `ARRTargetDummy` (abstract `AActor` + `IAbilitySystemInterface`): root `UStaticMeshComponent` `Mesh`, `URRAbilitySystemComponent`, `URRHealthAttributeSet`. No tick.
- Collision: `Mesh` object type Pawn, blocks every channel except `Projectile` (overlap), overlap events on. The projectile sphere overlaps Pawn, so the pair overlaps and the projectile applies its spec ([[sys-projectiles]]).
- Hit flow: `URRHealthAttributeSet::OnDamageReceived` (damage, crit, health left; see [[sys-damage]]) → log on `LogRogueRigs` (damage, crit, health left / max), flash, `URRDamageNumberSubsystem::AddHit` at the actor location + `DamageNumberOffset` ([[sys-damage-numbers]]), refill scheduling.
- Flash: dynamic instance of the mesh material slot 0 created at `BeginPlay`; vector parameter `ColorParameterName` ("Color" on `M_GreyBox`) set to `FlashColor` for `FlashDuration`, then restored to its value read at `BeginPlay`. Missing material or parameter logs a warning.
- Health: `MaxHealth` and `Health` set to `MaxHealth` at `BeginPlay`. `RefillHealth` sets `Health` to the current `MaxHealth` `RefillDelay` after the last hit (timer restarted per hit), instantly when `Health` reaches 0. Never dies, never gets `State.Dead` (`SetDeathEnabled(false)` on its health set).
- Editable (`EditAnywhere`): `MaxHealth` 1000, `RefillDelay` 3 s, `FlashDuration` 0.1 s, `FlashColor` white, `ColorParameterName` `Color`, `DamageNumberOffset` (0, 0, 150); number style and merge in [[sys-damage-numbers]] (the M1 text actor `ARRFloatingDamageText` was retired in task-025-01).
- entry_points: place a Blueprint subclass (sets `Mesh` static mesh and an `M_GreyBox`-based material); damage arrives through any GE writing `IncomingDamage`.
- data: none; all values are editable properties.
- depends_on: [[sys-gas-core]], [[sys-damage]], [[sys-projectiles]].
- implements: none (debug target; its numbers go through [[sys-damage-numbers]]).
- gotchas:
  - Flash and refill run inside the GE execution call stack (attribute set delegate); setting `Health` there is safe.

## Relations
- [[systems-index]] — parent index.
- [[sys-damage]] — the per-hit damage event the dummy listens to.
- [[sys-damage-numbers]] — where its hits become damage numbers.
- [[sys-automation-tests]] — RogueRigs.Weapons.ProjectileDamagesDummy and DummyRefill prove its damage and refill.
