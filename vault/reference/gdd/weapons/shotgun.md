---
id: shotgun
description: "Shotgun: piercing projectile pellets toward the aim with knockback; acquired as a weapon card; stats, upgrades and look."
parent: weapons-index
kind: spec
status: approved
---
# Shotgun

- fire_mode: auto toward the aim (manual aim kept, unlike DRGS); each shot fires projectile pellets (`PelletCount`) spread by `SpreadDeg`, `WeaponStats.csv` row `Shotgun` (owner 2026-10-05, replaces the cone wave).
- effect_on_hit: each pellet damages, pierces robots (`Pierce`), rolls its own crit and knocks back, reduced by the target's knockback resistance.
- blocked_by_rock: yes (pellets stop on rock)
- pellets (owner 2026-10-05, confirmed by the owner after handover-2026-10-06): even fan, speed `ProjectileSpeed`, they damage barrels like bullets.
- stats (provisional, owner 2026-10-31): `WeaponStats.csv` row `Shotgun`, columns `Damage` (per pellet), `PelletCount`, `Pierce`, `Range`, `KnockbackForce` (per pellet hit), `MagazineSize`, `FireRate`, `ReloadTime`, `CritChance`, `CritDamage`.
- Acquired as a level-up weapon card; the miner starts with the burst rifle only ([[upgrades]]).
- upgradable_stats: damage, fire rate, reload speed, crit chance, crit damage, magazine size.
- effect pool: none (4-direction fire does not apply); piercing is a stat (base `Pierce`) and can be upgraded.
- vfx_sfx: white-gold pellets and muzzle flash ([[color-code]]); sound open.
- look: no visible weapon mesh; the grey placeholder cube is removed and the pellets leave from the same point as every weapon shot (owner 2026-10-30, task-400 Q2: "Tu retires le cube, les autres tirs partiront du même endroit"); that point is the burst rifle Muzzle, with the same vertical capsule collision as the rifle bullets ([[weapons-index]] shown-weapon rule; provisional, owner 2026-10-31, task-412).
- milestone: M1

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — how it is built.
- [[wishlist]] — knockback into the void.
