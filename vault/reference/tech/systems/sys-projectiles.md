---
id: sys-projectiles
description: "Projectiles without GAS: ARRProjectile carries the damage spec to impact, ARREnemyProjectile robot shots, Projectile channel, stop on rock, pierce, pooling, deferred modules."
parent: systems-index
kind: doc
---
# Projectiles

- responsibility: carry shot data from the weapon to the impact.
- main_classes (in `Source/RogueRigs/Projectiles/`): `ARRProjectile` (abstract; no ASC, approved for now): root `USphereComponent` `CollisionSphere` (radius 10 by default, editable in the Blueprint) + `UProjectileMovementComponent` (no gravity, no bounce, `bForceSubStepping = false`, rotation follows velocity). `RRCollisionChannels::Projectile` (`RRCollisionChannels.h`) names the channel.

## Rules
- The projectile carries the spec made by the weapon's ASC at fire time and applies it only on impact ([[sys-damage]]).
- Overlap with an actor that has an ASC: `ApplyGameplayEffectSpecToSelf` once, then destroyed. Pawns without an ASC are passed through. The owner weapon, the instigator (miner) and the owner's owner are ignored, both in the overlap handler and with `IgnoreActorWhenMoving`.
- Blocking hit: `OnProjectileStop` destroys it. If the hit actor is the `ARRArenaGrid` and the cell (`GetCellFromHit`: hit instance, else impact point) is a barrel, it first calls `ApplyShotDamage(Cell, GetCarriedDamage())` once ([[sys-grid]]). `GetCarriedDamage`: the fire-time snapshot of the weapon's `URRWeaponAttributeSet` `Damage` captured in the spec (same capture definition as `URRDamageExecution`: source, snapshot), read from `CapturedRelevantAttributes` without any ASC on the barrel; no critical roll; 0 when the spec has no such capture (enemy flat-damage shots do not hurt barrels). Every projectile stops on rock ([[destruction-rules]]); a projectile spawned inside a block stops at once.
- Collision: object channel `Projectile` = `ECC_GameTraceChannel1`, declared in `Config/DefaultEngine.ini` with default response Block (so rock HISM `BlockAll` responds Block to it). The sphere ignores every channel except Pawn (overlap), WorldStatic and WorldDynamic (block); projectiles ignore each other. A sphere/pawn pair resolves to the weaker response (Overlap), so a projectile never blocks a pawn; the miner capsule also sets Overlap on `Projectile` explicitly ([[sys-player]]).
- Spawn point: the weapon's `GetShotOrigin()` ([[sys-weapons]]), `ShotHeight` (50 uu) above the floor, below the block height.
- Speed and lifetime come from the weapon: speed `ProjectileSpeed`, lifetime `Range / ProjectileSpeed`.
- Piercing: a `PierceRemaining` counter copied from the weapon at fire time (provisional; M2, not implemented: one hit destroys).
- Pool projectiles before any enemy pool (provisional; M2, if stress test S2 asks for it).
- Bounce, homing, tornado, lightning, explode on impact: deferred with the module framework ([[wishlist]]).

- entry_points: static `ARRProjectile::SpawnProjectile(World, Class, Transform, Owner, Instigator, Spec, Speed, Lifetime)` (deferred spawn, `InitProjectile`, `FinishSpawning`; lifetime goes through `InitialLifeSpan`, speed through `InitialSpeed`), called by `ARRWeapon::FireOnce` ([[sys-weapons]]). Hit filter (task-020-02): virtual `CanHit(Actor)`, checked on overlap after the ignored actors; default = instance of `HitActorClass` (`EditDefaultsOnly`, None = any actor with an ASC); non-hittable actors are passed through. `bDamagesTerrain` (default true): barrel damage on a blocking hit. Robot shots: `ARREnemyProjectile` (`RREnemyProjectile.h/.cpp`, Blueprintable, task-021-01) sets `HitActorClass` = `ARRMiner` and `bDamagesTerrain` off: robots pass through, rock and barrels only stop it; fired by the shooter with its flat damage spec ([[sys-enemies-shooter]]).
- data: none of its own; values come from the weapon. Blueprint subclass sets the mesh (no collision) and may change the sphere radius.
- depends_on: [[sys-weapons]], [[sys-damage]].
- implements: [[burst-rifle]], [[turret]], [[enemy-shooter]]. The shotgun fires no projectile ([[sys-weapons]]).
- gotchas: see [[perf-risks]]. Fast bullets rely on the movement sweep for overlaps; no sub-stepping.

## Relations
- [[systems-index]] — parent index.
