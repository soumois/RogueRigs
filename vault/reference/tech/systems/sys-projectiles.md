---
id: sys-projectiles
description: "Projectiles without GAS: ARRProjectile carries the damage spec to impact, Projectile collision channel, stop on rock, pierce counter, pooling, deferred behavior modules."
parent: systems-index
kind: doc
---
# Projectiles

- responsibility: carry shot data from the weapon to the impact.
- main_classes (in `Source/RogueRigs/Projectiles/`): `ARRProjectile` (abstract; no ASC, approved for now): root `USphereComponent` `CollisionSphere` (radius 10 by default, editable in the Blueprint) + `UProjectileMovementComponent` (no gravity, no bounce, `bForceSubStepping = false`, rotation follows velocity). `RRCollisionChannels::Projectile` (`RRCollisionChannels.h`) names the channel.

## Rules
- The projectile carries the spec made by the weapon's ASC at fire time and applies it only on impact ([[sys-damage]]).
- Overlap with an actor that has an ASC: `ApplyGameplayEffectSpecToSelf` once, then destroyed. Pawns without an ASC are passed through. The owner weapon, the instigator (miner) and the owner's owner are ignored, both in the overlap handler and with `IgnoreActorWhenMoving`.
- Blocking hit: `OnProjectileStop` destroys it. Every projectile stops on rock ([[destruction-rules]]); a projectile spawned inside a block stops at once.
- Collision: object channel `Projectile` = `ECC_GameTraceChannel1`, declared in `Config/DefaultEngine.ini` with default response Block (so rock HISM `BlockAll` and pawns respond Block to it). The sphere ignores every channel except Pawn (overlap), WorldStatic and WorldDynamic (block); projectiles ignore each other.
- Speed and lifetime come from the weapon: speed `ProjectileSpeed`, lifetime `Range / ProjectileSpeed`.
- Piercing: a `PierceRemaining` counter copied from the weapon at fire time (provisional; M2, not implemented: one hit destroys).
- Pool projectiles before any enemy pool (provisional; M2, if stress test S2 asks for it).
- Bounce, homing, tornado, lightning, explode on impact: deferred with the module framework ([[wishlist]]).

- entry_points: spawn with `SpawnActorDeferred` (Owner = weapon, Instigator = miner), `InitProjectile(SpecHandle, Speed, Lifetime)`, then `FinishSpawning` (lifetime goes through `InitialLifeSpan`, speed through `InitialSpeed`). Called by `ARRWeapon::FireOnce` ([[sys-weapons]]).
- data: none of its own; values come from the weapon. Blueprint subclass sets the mesh (no collision) and may change the sphere radius.
- depends_on: [[sys-weapons]], [[sys-damage]].
- implements: [[burst-rifle]], [[turret]], [[enemy-shooter]]. The shotgun fires no projectile ([[sys-weapons]]).
- gotchas: see [[perf-risks]]. Fast bullets rely on the movement sweep for overlaps; no sub-stepping.

## Relations
- [[systems-index]] — parent index.
