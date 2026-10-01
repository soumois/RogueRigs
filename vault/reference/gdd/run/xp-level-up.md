---
id: xp-level-up
description: "XP loop: cubes dropped by kills, level-up bar, choice of 3 upgrades on each level-up. Open for XP tuning or the level-up screen."
parent: run-index
kind: spec
status: approved
---
# XP and level-up

- Killed robots drop XP cubes (glowing cyan, see [[color-code]]).
- Collecting XP fills a level-up bar. The player's `XPGain` stat scales XP gained; `PickupRadius` sets the collection radius ([[player-stats]]).
- Bonus chests also give XP and an upgrade choice ([[objective-bonus-chest]]).
- Each level-up offers 3 upgrades; the player picks 1 ([[upgrades]]).
- Level-up keeps across the 4 levels of a run.
- XP per robot type, XP curve, base pickup radius: open.
- Whether the game pauses on the choice screen: open.
- Milestone: M1.

## Relations
- [[run-index]] — parent index.
- [[upgrades]] — what the player picks.
- [[rarities]] — rarity of the offered cards.
