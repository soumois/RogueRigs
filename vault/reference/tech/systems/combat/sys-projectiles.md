---
id: sys-projectiles
description: "Projectiles without GAS: ARRProjectile carries the damage spec to impact, shotgun pellets with knockback, ARREnemyProjectile robot shots, Projectile channel, stop on rock, pierce, pooling."
parent: systems-combat-index
kind: doc
---
# Projectiles

- responsibility: carry shot data from the weapon to the impact.
- main_classes (in `Source/RogueRigs/Projectiles/`): `ARRProjectile` (abstract; no ASC, approved for now): root `USphereComponent` `CollisionSphere` (radius 14 uu by default for miner shots: rifle, pellet, turret bullet, owner fourth test task-034-01, was 12 in task-033-01 and 10 before, mesh not scaled; `ARREnemyProjectile` sets 10 uu; editable in the Blueprint, no projectile Blueprint overrides it) + `UProjectileMovementComponent` (no gravity, no bounce, `bForceSubStepping = false`, rotation follows velocity). `RRCollisionChannels::Projectile` (`RRCollisionChannels.h`) names the channel.

## Rules
- The projectile carries the spec made by the weapon's ASC at fire time and applies it only on impact ([[sys-damage]]).
- Overlap with an actor that has an ASC: `ApplyGameplayEffectSpecToSelf` once per actor; destroyed after `MaxHits` distinct actors (pierce, below). Pawns without an ASC are passed through. The owner weapon, the instigator (miner) and the owner's owner are ignored, both in the overlap handler and with `IgnoreActorWhenMoving`.
- Blocking hit: `OnProjectileStop` destroys it. If the hit actor is the `ARRArenaGrid` and the cell (`GetCellFromHit`: hit instance, else impact point) is a barrel, it first calls `ApplyShotDamage(Cell, GetCarriedDamage())` once ([[sys-grid]]). `GetCarriedDamage`: the fire-time snapshot of the weapon's `URRWeaponAttributeSet` `Damage` captured in the spec (same capture definition as `URRDamageExecution`: source, snapshot), read from `CapturedRelevantAttributes` without any ASC on the barrel; no critical roll; 0 when the spec has no such capture (enemy flat-damage shots do not hurt barrels). Every projectile stops on rock ([[destruction-rules]]); a projectile spawned inside a block stops at once.
- Collision: object channel `Projectile` = `ECC_GameTraceChannel1`, declared in `Config/DefaultEngine.ini` with default response Block (so rock HISM `BlockAll` responds Block to it). The sphere ignores every channel except Pawn (overlap), WorldStatic and WorldDynamic (block); projectiles ignore each other. A sphere/pawn pair resolves to the weaker response (Overlap), so a projectile never blocks a pawn; the miner capsule also sets Overlap on `Projectile` explicitly ([[sys-player]]). Robots take shots on their body-size `HitCapsule` only; their root capsule ignores `Projectile` (task-034-01, [[sys-enemies-base]]).
- Spawn point: the weapon's `GetShotOrigin()` ([[sys-weapons]]), `ShotHeight` (50 uu) above the floor, below the block height.
- Speed and lifetime come from the weapon: speed `ProjectileSpeed`, lifetime `Range / ProjectileSpeed`.
- Piercing (task-024-01): `MaxHits` (>= 1, default 1) set at spawn from the weapon's `GetPierceCount()` at fire time ([[sys-weapons]]); `HitActors` (weak set) keeps every actor already damaged, so a pierced robot is never hit twice by the same bullet; on the `MaxHits`-th distinct hit the projectile is destroyed. Rock still stops it at once; a bullet that pierced robots still damages a barrel it then hits. Robot shots keep `MaxHits` 1.
- Hit direction (task-166-02): before applying the spec to an `ARREnemy`, `RecordHitDirection(velocity)` (forward when the velocity is zero), the direction its death debris flies ([[sys-enemies-base]]).
- Knockback (task-024-02): `KnockbackSpeed` (uu/s, default 0) set at spawn; after the spec is applied to an `ARREnemy`, `ApplyKnockback(flight direction × KnockbackSpeed)` ([[sys-enemies]]; a robot the hit killed is not pushed). Used by `ARRShotgunPellet` (`RRShotgunPellet.h/.cpp`, Blueprintable, concrete so the native shotgun fires without a Blueprint; no mesh) with the shotgun's `KnockbackForce` ([[sys-weapons]]). `ARRTurretBullet` (`RRTurretBullet.h/.cpp`, Blueprintable, concrete, no mesh): the turret launcher's default bullet, fired by turrets ([[sys-turret]]).
- Pool projectiles before any enemy pool (provisional; M2, if stress test S2 asks for it).
- Bounce, homing, tornado, lightning, explode on impact: deferred with the module framework ([[wishlist]]).

- entry_points: static `ARRProjectile::SpawnProjectile(World, Class, Transform, Owner, Instigator, Spec, Speed, Lifetime, MaxHits = 1, KnockbackSpeed = 0)` (deferred spawn, `InitProjectile`, `FinishSpawning`; `GetMaxHits()`, `GetHitCount()`, `GetKnockbackSpeed()`; lifetime goes through `InitialLifeSpan`, speed through `InitialSpeed`), called by `ARRWeapon::FireOnce` and `ARRShotgun::FireOnce` ([[sys-weapons]]). Hit filter (task-020-02): virtual `CanHit(Actor)`, checked on overlap after the ignored actors; default = instance of `HitActorClass` (`EditDefaultsOnly`, None = any actor with an ASC); non-hittable actors are passed through. `bDamagesTerrain` (default true): barrel damage on a blocking hit. Robot shots: `ARREnemyProjectile` (`RREnemyProjectile.h/.cpp`, Blueprintable, task-021-01) sets `HitActorClass` = `ARRMiner` and `bDamagesTerrain` off: robots pass through, rock and barrels only stop it; fired by the shooter with its flat damage spec ([[sys-enemies-shooter]]).
- data: none of its own; values come from the weapon. Blueprint subclass sets the mesh (no collision) and may change the sphere radius.
- depends_on: [[sys-weapons]], [[sys-damage]].
- implements: [[burst-rifle]], [[shotgun]], [[turret]], [[enemy-shooter]].
- gotchas: see [[perf-risks]]. Fast bullets rely on the movement sweep for overlaps; no sub-stepping.

## Relations
- [[systems-index]] — parent index.
