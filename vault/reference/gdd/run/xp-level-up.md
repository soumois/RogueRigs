---
id: xp-level-up
description: "XP loop: cubes dropped by kills, level-up bar, choice of 3 upgrades on each level-up. Open for XP tuning or the level-up screen."
parent: run-index
kind: spec
status: approved
---
# XP and level-up

- Killed robots drop XP cubes (glowing cyan, see [[color-code]]). Like DRGS (provisional, owner 2026-10-05): a kill bursts several small cubes in short arcs around the body; they rest on the floor; inside the pickup radius they accelerate toward the miner and form a stream; each collect sparkles. Broken ore and heal blocks pop chunks the same way. Healing shows a green ring on the miner.
- A kill bursts its XP split over cubes whose values add up to the robot XP: chaser 3, kamikaze 4, shooter 6, heavy 12 cubes (provisional, task-027-01, owner to judge). A cube rests 1 s before the magnet can take it.
- Pacing target (provisional, owner 2026-10-05): level 2 at about 0:30, level 5 at about 1:40, level 10 at about 3:10 of a 4-minute level; a fast early rhythm matters (owner).
- Collecting XP fills a level-up bar. The player's `XPGain` stat scales XP gained; `PickupRadius` sets the collection radius ([[player-stats]]).
- Bonus chests also give XP and an upgrade choice ([[objective-bonus-chest]]).
- Each level-up offers 3 upgrades; the player picks 1 ([[upgrades]]).
- Level-up keeps across the 4 levels of a run.
- XP per robot (provisional): chaser 1, kamikaze 1, shooter 2, heavy 5. XP curve (provisional, task-030-02, tuned on a measured bot run after the director start at 1 s; was 5 × level, owner 2026-10-02): level 1 needs 12 XP, then XP to the next level = 12 × (level + 3) (60, 72, 84 … 156 at level 9); extra XP carries over, several levels can pass at once. Measured median over 3 seeds: level 2 at about 0:26, 5 at 1:34, 10 at 2:55. Base pickup radius: [[player-stats]].
- HUD (owner 2026-10-02): cyan XP bar across the bottom, level number on its left.
- Level-up sequence (owner 2026-10-03; values provisional): a pulsing cyan ring around the miner and "LEVEL UP"; heal 20 % of max health; sound later (open); about 0.8 s later the game pauses and shows 3 upgrade cards; picking one resumes the game. Several levels gained at once chain their choice screens.
- Level-up disc on the miner: 5-step animation, 0.5 s in total, ends hidden (confirmed by the owner after handover-2026-10-06).
- Reroll: one button per choice screen, usable once, redraws 3 other upgrades at random (owner 2026-10-03).
- Milestone: M1.

## Relations
- [[run-index]] — parent index.
- [[upgrades]] — what the player picks.
- [[rarities]] — rarity of the offered cards.
