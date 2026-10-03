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
- XP per robot (provisional): chaser 1, kamikaze 1. XP curve (provisional, owner 2026-10-02): XP to the next level = 5 × current level (5, 10, 15…); extra XP carries over, several levels can pass at once. Base pickup radius: [[player-stats]].
- M1 without upgrade cards (owner 2026-10-02): a level-up raises the level and shows "LEVEL UP" for 1 s; HUD: cyan XP bar across the bottom, level number on its left.
- Whether the game pauses on the choice screen: open.
- Milestone: M1.

## Relations
- [[run-index]] — parent index.
- [[upgrades]] — what the player picks.
- [[rarities]] — rarity of the offered cards.
