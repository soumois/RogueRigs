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
- HUD (owner 2026-10-02): cyan XP bar across the bottom, level number on its left.
- Level-up sequence (owner 2026-10-03; values provisional): a pulsing cyan ring around the miner and "LEVEL UP"; heal 20 % of max health; sound later (open); about 0.8 s later the game pauses and shows 3 upgrade cards; picking one resumes the game. Several levels gained at once chain their choice screens.
- Reroll: one button per choice screen, usable once, redraws 3 other upgrades at random (owner 2026-10-03).
- Milestone: M1.

## Relations
- [[run-index]] — parent index.
- [[upgrades]] — what the player picks.
- [[rarities]] — rarity of the offered cards.
