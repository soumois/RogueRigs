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
- Each effect belongs to specific weapons' pools: 4-direction fire (burst rifle). Piercing is no longer an effect but a weapon stat upgrade: blue, purple and yellow only (`UpgradeValues.csv` rows `Pierce_Blue`, `Pierce_Purple`, `Pierce_Yellow`, column `GlobalValue`; owner 2026-10-05); never grey or green; targeted and global piercing cards give the same whole number (no 1.5 ×; confirmed by the owner after handover-2026-10-06).
- Draw weights (owner 2026-10-06 third test, provisional): draw weights per upgrade in `UpgradeValues.csv`, column `DrawWeight`, before rarity.
- New-weapon cards (owner 2026-10-06 third test, replaces the 30 % per slot rule below): only on levels 5, 10, 15…, which offer 1 weapon card, or 2 when at least 2 weapons remain to acquire; other levels offer none. Earlier rule (owner 2026-10-05): the miner starts with the burst rifle only; while fewer than 4 weapons are owned (3 exist in the prototype), each offer slot has a 30 % chance to be a "Nouvelle arme" card, at most one per offer (provisional). Taking few weapons lets the player focus upgrades.
- 4-direction fire: the main shot stays on the aim; the 3 extra shots are weaker, to protect pillar P2. Milestone M2, like piercing.
- EMP, stuns machines: provisional, M3 if time allows.
- Ice, burn, other elements: deferred ([[wishlist]]).

## Values
- Fixed: an upgrade of a given stat, target and rarity always gives the same value in every run ([[rarities]]).
- Global values per tier grey / green / blue / purple / yellow (provisional, owner 2026-10-05), about twice the previous ones, close to DRGS: one row per stat and tier in `UpgradeValues.csv` (`<Stat>_<Tier>`, column `GlobalValue`, with `Operation`: percentage stats, max health, crit chance, crit damage and luck). A targeted upgrade gives the global value times `TargetedMultiplier` of the same row.
- Only weapons the miner owns can be targeted.
- M1 scope (owner 2026-10-03): player stats and weapon stats damage, fire rate, reload speed, crit chance, crit damage; no magazine size and no effects.
- Stacking: percentage bonuses (`Operation` `MultiplyAdditive`) add together rather than multiply. Point bonuses (`Operation` `AddBase`) add to the base (crit damage base + bonus).
- Per-weapon-type upgrades: deferred ([[wishlist]]).

## Relations
- [[weapons-index]] — parent index.
- [[sys-upgrades]] — the value table and draw as built.
