---
id: turret
description: "Turret launcher weapon and its turrets: charges and reload, placement, nearest-target lock, invincible, no collision, stats copied at placement."
parent: weapons-index
kind: spec
status: approved
---
# Turret launcher and turrets

## Turret launcher (the weapon)
- A weapon of the miner with its own stats and its own targeted upgrades; "all weapons" upgrades apply to it.
- fire_mode: deployable; a button places a turret on the aimed spot.
- Charges = magazine size. Placing a turret starts recharging that charge at the launcher's reload speed; one charge recharges at a time.
- No cap on active turrets for now.

## Turret (placed)
- Fixed lifetime; cannot be picked up.
- Copies every stat of the launcher at placement. Upgrades taken later apply only to turrets placed later.
- Targeting: locks the nearest enemy and fires until the target dies, leaves the target range or breaks line of sight, then retargets.
- Invincible and untargetable; no collision with enemies or projectiles, so it never plugs a chokepoint.
- projectile: projectile; blocked_by_rock: yes.

## Values
- stats: damage, fire rate, crit chance, magazine size (charges), reload time, lifetime, target range → open; crit damage ×1.5 base.
- upgradable_stats: damage, fire rate, reload speed, crit chance, crit damage, magazine size.
- effect pool (yellow rarity): piercing.
- vfx_sfx: white + teal + rust body, white-gold shots ([[color-code]]); sound open.
- milestone: M2

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — attribute copy at placement.
