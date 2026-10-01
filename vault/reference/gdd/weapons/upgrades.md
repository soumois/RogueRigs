---
id: upgrades
description: "Level-up upgrade pool: targeted and global stat upgrades, player stats, weapon effect pools, fixed values, stacking arithmetic, milestone split."
parent: weapons-index
kind: spec
status: approved
---
# Upgrades

- Offered on each level-up and by every bonus chest: 3 cards, pick 1 ([[xp-level-up]], [[objective-bonus-chest]]).
- Upgrades are never removed during a run.

## Targets
- **Targeted:** one weapon. **Global:** all weapons, turret launcher included. At equal rarity, a targeted upgrade is stronger than a global one.
- **Player stats:** max health, move speed, pickup radius, XP gain, mining speed, luck ([[player-stats]]).

## Weapon stats
Damage, fire rate, reload speed, crit chance, crit damage, magazine size.

## Effects (always yellow rarity)
- Each effect belongs to specific weapons' pools: piercing (burst rifle, turret launcher), 4-direction fire (burst rifle). The shotgun has no effect.
- 4-direction fire: the main shot stays on the aim; the 3 extra shots are weaker, to protect pillar P2. Milestone M2, like piercing.
- EMP, stuns machines: provisional, M3 if time allows.
- Ice, burn, other elements: deferred ([[wishlist]]).

## Values
- Fixed: an upgrade of a given stat, target and rarity always gives the same value in every run ([[rarities]]). Values: open.
- Stacking: percentage bonuses add together (2 × +20 % damage = ×1.4). Point bonuses add (crit damage ×1.5 + 0.2 = ×1.7).
- Per-weapon-type upgrades: deferred ([[wishlist]]).

## Relations
- [[weapons-index]] — parent index.
- [[sys-upgrades]] — the value table and draw as built.
