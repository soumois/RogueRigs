---
id: player-stats
description: "The miner's stat sheet: own stats (health, speed, pickup radius, XP gain, mining speed, luck) and the global weapon stats it displays."
parent: player-index
kind: spec
status: approved
---
# Player stats

Modeled on Deep Rock Galactic: Survivor.

## Stats owned by the miner
| Stat | Bonus type | Effect |
|---|---|---|
| Max health | flat points | maximum health |
| Move speed | % | movement speed |
| Pickup radius | % | collection radius of XP, ore and heal pickups |
| XP gain | % | XP gained |
| Mining speed | % | the drone's dig speed |
| Luck | points | odds of higher-rarity upgrade cards ([[rarities]]) |

## Global weapon stats shown on the sheet
Damage (%), fire rate (%), reload speed (%), critical chance (points), critical damage (points). They belong to the weapons: an "all weapons" upgrade changes every weapon, the turret launcher included ([[upgrades]]). The sheet shows the sum of global bonuses taken.

## Out of scope
- Armor: deferred.
- Life regen and dodge: not used. Healing comes only from heal ore ([[heal-ore]]).
- A future upgrade on heals received (%): candidate, not decided.

## Base values (provisional, owner to test)
| Stat | Base |
|---|---|
| Max health | 100 |
| Move speed | 600 uu/s (3 cells/s) |
| Pickup radius | 300 uu (1.5 cells) |
| XP gain | ×1.0 |
| Mining speed | ×1.0 |
| Luck | 0 |

## Relations
- [[player-index]] — parent index.
- [[sys-gas-core]] — the AttributeSets holding these stats.
