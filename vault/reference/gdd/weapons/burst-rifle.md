---
id: burst-rifle
description: "Default weapon: auto-fires projectile bursts toward the aim. Burst timing, starting values, magazine, upgradable stats and effect pool."
parent: weapons-index
kind: spec
status: approved
---
# Burst rifle

Final English name: open (one name for tag, asset and this node; "burst rifle" is the working name).

- fire_mode: auto toward aim, in bursts.
- projectile: projectile; each bullet pierces 4 robots (base, like DRGS Deepcore GK2; owner 2026-10-05); piercing is an upgrade stat ([[upgrades]]).
- effect_on_hit: damage; critical hits possible.
- blocked_by_rock: yes
- stats:
  - fire_rate 1.25 bursts/s, burst_count 3, burst_duty 0.2 (owner values 2026-10-02; bullets 0.08 s apart, 0.64 s pause);
  - damage 10 per bullet, crit chance 5 %, range 2000 uu (10 cells), spread ±2°, projectile speed 3000 uu/s, magazine 30 bullets, reload 1.5 s (provisional, owner 2026-10-01);
  - crit damage: ×1.5 base.
- burst timing: see [[sys-auto-fire]].
- upgradable_stats: damage, fire rate, reload speed, crit chance, crit damage, magazine size.
- effect pool (yellow rarity): 4-direction fire.
- vfx_sfx: white-gold shots ([[color-code]]); muzzle, impact and sound open.
- milestone: M1

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — how it is built.
