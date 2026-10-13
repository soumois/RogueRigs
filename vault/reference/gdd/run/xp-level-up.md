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
- Pacing target (provisional, owner 2026-10-06 after the second test: the first levels were far too long; was 0:30 / 1:40 / 3:10): level 2 at about 0:15, level 5 at about 1:10, level 10 at about 3:00 of a 4-minute level, measured on a player who collects his XP (bot Collect profile). A player who runs without collecting levels up much slower on purpose: he must go back into the hordes or plan a walking loop (owner 2026-10-06; the Run profile is reported, not tuned); a fast early rhythm matters (owner).
- Collecting XP fills a level-up bar. The player's `XPGain` stat scales XP gained; `PickupRadius` sets the collection radius ([[player-stats]]).
- Bonus chests also give XP and an upgrade choice ([[objective-bonus-chest]]).
- Each level-up offers 3 upgrades; the player picks 1 ([[upgrades]]).
- Level-up keeps across the 4 levels of a run.
- XP per robot (provisional): chaser 1, kamikaze 1, shooter 2, heavy 5. XP curve (owner 2026-10-06, after the reference game curve, nearly linear rather than exponential): level 1 needs 4 XP; from level 2, XP to the next level = reference need ÷ 5, interpolated between the reference points (level 2 → 100, 5 → 180, 10 → 320, 20 → 650, 30 → 1100, 50 → 2200): 20, 25, 31, 36 (level 5) … 64 (level 10) … 130 (level 20) … 220 (level 30) … 440 (level 50); cumulative 380 XP at level 10. Extra XP carries over, several levels can pass at once. Not measured by the pacing bot. Base pickup radius: [[player-stats]].
- HUD (owner 2026-10-02): cyan XP bar across the bottom, level number on its left.
- Level-up sequence (owner 2026-10-03; values provisional): a pulsing cyan ring around the miner and "LEVEL UP"; heal 20 % of max health; sound later (open); about 0.8 s later the game pauses and shows 3 upgrade cards; picking one resumes the game. Several levels gained at once chain their choice screens.
- Level-up disc on the miner: 5-step animation, 0.5 s in total, ends hidden (confirmed by the owner after handover-2026-10-06).
- Reroll: one button per choice screen, usable once, redraws 3 other upgrades at random (owner 2026-10-03).
- Milestone: M1.

- Big XP block (owner 2026-10-12, provisional): when one drop would create more than 8 cubes, groups of 8 become one bigger block (scale 1.8, bright magenta-pink with a glow of the same hue, owner 2026-10-13) worth the sum of its cubes, so the total XP is unchanged; same magnet and pickup radius. It also lightens the pool when many robots die at once.

## Relations
- [[run-index]] — parent index.
- [[upgrades]] — what the player picks.
- [[rarities]] — rarity of the offered cards.
