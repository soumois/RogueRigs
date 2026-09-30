---
id: turret
description: "Deployable turret: up to 2, placed on the aimed spot, fixed lifetime, nearest-target lock, invincible, no collision, stats frozen at placement."
parent: weapons-index
kind: spec
status: approved
---
# Turret

- fire_mode: deployable; a button places it on the aimed spot. Up to 2 active at once; cannot be picked up.
- lifetime: fixed; value open.
- targeting: locks the nearest enemy and fires until the target dies, leaves the radius or breaks line of sight, then retargets.
- invincible and untargetable; no collision with enemies or projectiles, so it never plugs a chokepoint.
- projectile: projectile; blocked_by_rock: yes.
- effect_on_hit: damage.
- stats: copied from its source weapon at placement, impact effects included (provisional). Upgrades taken later apply only to turrets placed later. Which weapon is the source: open.
- stats: damage, fire_rate, range, lifetime, placement cooldown → open.
- upgradable_stats: yes, through [[upgrades]]; list open.
- vfx_sfx: white + teal + rust body, white-gold shots ([[color-code]]); sound open.
- milestone: M2

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — attribute copy at placement.
