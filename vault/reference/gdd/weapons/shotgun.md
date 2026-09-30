---
id: shotgun
description: "Short-range auto-firing shotgun that pushes enemies back. Stats, knockback and upgradable values."
parent: weapons-index
kind: spec
status: approved
---
# Shotgun

- fire_mode: auto toward aim, one shot per cycle (burst_count 1).
- projectile: projectile pellets; pierces only with the piercing upgrade.
- effect_on_hit: damage + knockback (pushes robots back a little).
- blocked_by_rock: yes
- stats: damage, fire_rate, range, spread, pellet count, knockback force, magazine size, reload time → open.
- upgradable_stats: fire rate, damage, critical chance, critical damage; piercing, 4-direction fire ([[upgrades]]).
- vfx_sfx: white-gold pellets ([[color-code]]); muzzle, impact and sound open.
- milestone: M1

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — how it is built.
- [[wishlist]] — knockback into the void.
