---
id: level-flow
description: "How a level unfolds: alert tiers, the events they trigger (swarm, bonus chest, console), the paused bar, and when extraction starts."
parent: run-index
kind: spec
status: approved
---
# Level flow

1. **Land.** The alert bar starts filling with time and kills ([[difficulty]]).
2. **Alert tiers.** A level has 4 to 7 tiers; the exact count is a balance value. Each tier triggers one event:
   - an enemy swarm;
   - a bonus chest ([[objective-bonus-chest]]);
   - a command console ([[objective-console]]).
   Every level has at least one console.
3. **Mandatory objective.** While a console is not hacked, the alert bar is paused. Difficulty keeps rising, so the player cannot farm XP indefinitely.
4. **Extraction.** When the bar is full and no mandatory objective is pending, the shuttle lands: [[extraction]].

Throughout the level, robot waves spawn and killed robots drop XP cubes ([[xp-level-up]]).

## Open values
- Event sequence per level: whether tier events are fixed per level or drawn from a pool.
- Swarm size and composition.

## Relations
- [[run-index]] — parent index.
- [[lore]] — what the alert bar and the countdown mean in the world.
