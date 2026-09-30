---
id: destruction-rules
description: "What breaks blocks (barrels, kamikazes, drone), and the rule that every projectile, player's or robot's, stops on rock."
parent: terrain-index
kind: spec
status: approved
---
# Destruction rules

- Shots never break blocks.
- Blocks stop all projectiles, the player's and the robots'. Nobody shoots over a block.
- Only three things break blocks:
  1. an exploding [[barrel]] (or other industrial equipment);
  2. an exploding [[enemy-kamikaze]];
  3. the [[drone]]'s cutting laser.
- Breaking terrain is therefore a decision, not a side effect of shooting (pillar P3).
- Player explosive shots: out of scope. Grenades are cut; if they return, charges are limited.
- Destroyed blocks are logged by source and by type ([[run-log-schema]]).

## Relations
- [[terrain-index]] — parent index.
- [[explosions]] — terrain damage from explosions.
