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
- Placement is always a player button, never automatic when standing still (owner 2026-10-05: auto-placement in DRGS was a frustration).

## Turret (placed)
- Fixed lifetime; cannot be picked up.
- Copies every stat of the launcher at placement. Upgrades taken later apply only to turrets placed later.
- Targeting: locks the nearest enemy and fires until the target dies, leaves the target range or breaks line of sight, then retargets.
- Invincible and untargetable; no collision with enemies or projectiles, so it never plugs a chokepoint.
- projectile: projectile; blocked_by_rock: yes.

## Values
- stats (provisional, owner 2026-10-05), ratios from DRGS LMG Gun Platform: damage 6 per bullet, 3.3 shots/s, pierce 4, crit chance 5 %, charges 2, recharge 6 s, lifetime 10 s, target range 1500 uu; crit damage ×1.5 base.
- upgradable_stats: damage, fire rate, reload speed, crit chance, crit damage, magazine size.
- Name and texts: "Lance-tourelles" and the current short French weapon descriptions in the level-up cards are kept as working texts (confirmed by the owner after handover-2026-10-06); final wording open.
- effect pool: none; piercing is a stat (base 4) upgraded like any weapon stat ([[upgrades]]).
- vfx_sfx: white + teal + rust body, white-gold shots ([[color-code]]); sound open.
- milestone: M2, pulled into the current work (owner 2026-10-05)

- Size (owner 2026-10-06 third test): the placed turret is drawn 50 % larger; it was barely visible.

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — attribute copy at placement.
