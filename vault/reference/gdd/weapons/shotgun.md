---
id: shotgun
description: "Shotgun: 7 piercing projectile pellets toward the aim with knockback; acquired as a weapon card; stats, upgrades and look."
parent: weapons-index
kind: spec
status: approved
---
# Shotgun

- fire_mode: auto toward the aim (manual aim kept, unlike DRGS); each shot fires 7 projectile pellets spread ±15° (owner 2026-10-05, replaces the cone wave).
- effect_on_hit: each pellet damages, pierces 3 robots, rolls its own crit and knocks back, reduced by the target's knockback resistance.
- blocked_by_rock: yes (pellets stop on rock)
- stats (provisional, owner 2026-10-05): pellets in an even fan, speed 3000 uu/s, they damage barrels like bullets; 6 damage per pellet, 7 pellets, pierce 3, range 700 uu, knockback 250 uu/s per pellet hit, magazine 2, 0.5 s between shots, reload 2.5 s; crit chance 5 %; crit damage ×1.5 base.
- Acquired as a level-up weapon card; the miner starts with the burst rifle only ([[upgrades]]).
- upgradable_stats: damage, fire rate, reload speed, crit chance, crit damage, magazine size.
- effect pool: none (4-direction fire does not apply); piercing is a stat (base 3) and can be upgraded.
- vfx_sfx: white-gold pellets and muzzle flash ([[color-code]]); sound open.
- milestone: M1

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — how it is built.
- [[wishlist]] — knockback into the void.
