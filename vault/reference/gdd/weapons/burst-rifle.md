---
id: burst-rifle
description: "Default weapon: auto-fires bursts toward the aim. Burst timing, starting values, magazine and upgradable stats."
parent: weapons-index
kind: spec
status: approved
---
# Burst rifle

Final English name: open (one name for tag, asset and this node; "burst rifle" is the working name).

- fire_mode: auto toward aim, in bursts.
- projectile: projectile; pierces only with the piercing upgrade.
- effect_on_hit: damage; critical hits possible.
- blocked_by_rock: yes
- stats:
  - fire_rate 2 bursts/s, burst_count 3, burst_duty 0.32 (starting values, untuned);
  - damage, range, spread, magazine size, reload time → open.
- burst timing: see [[sys-auto-fire]].
- upgradable_stats: fire rate, damage, critical chance, critical damage; piercing, 4-direction fire ([[upgrades]]).
- vfx_sfx: white-gold shots ([[color-code]]); muzzle, impact and sound open.
- milestone: M1

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — how it is built.
