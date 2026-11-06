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
- projectile: projectile in the game rules, shown as laser shots (owner 2026-11-05, [[task-464-player-shot-colour]]: "La tourelle est laser aussi"); blocked_by_rock: yes.

## Values
- stats (provisional, owner 2026-11-01): `WeaponStats.csv` row `TurretLauncher`, columns `Damage` (per bullet), `FireRate`, `Pierce`, `CritChance`, `CritDamage`, `MagazineSize` (charges), `ReloadTime` (recharge), `TurretLifetime`, `Range` (target range). Other base ratios follow the project data.
- upgradable_stats: damage, fire rate, reload speed, crit chance, crit damage, magazine size.
- Name and texts: "Lance-tourelles" and the current short French weapon descriptions in the level-up cards are kept as working texts (confirmed by the owner after handover-2026-10-06); final wording open.
- effect pool: none; piercing is a stat (base `Pierce`) upgraded like any weapon stat ([[upgrades]]).
- vfx_sfx: white + teal + rust body, green laser shots and a green muzzle flash at the barrel, like the burst rifle (owner 2026-11-05, provisional, [[task-464-player-shot-colour]], [[color-code]]); the green impact burst of the burst rifle at each hit, sized to the turret shot (owner 2026-11-05, provisional, [[task-465-shot-impact-burst]]); sound open.
- milestone: M2, pulled into the current work (owner 2026-10-05)

- Size (owner 2026-10-06 third test): the placed turret is drawn 50 % larger; it was barely visible.

- Recharge (owner 2026-10-06 fourth test): one charge recharges in `ReloadTime` (was 6 s).

- Spacing (owner 2026-10-07, provisional): a turret placed less than 100 uu from a standing turret moves to the nearest free reachable floor cell at least 100 uu away; if none is near, it lands where aimed.

## Relations
- [[weapons-index]] — parent index.
- [[sys-weapons]] — attribute copy at placement.
