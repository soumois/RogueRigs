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
- Only four things break blocks:
  1. an exploding [[barrel]] (or other industrial equipment);
  2. an exploding [[enemy-kamikaze]];
  3. the [[drone]]'s cutting laser;
  4. the extractor set on the ground (arrival and extraction landing): every block of its 16 x 12-cell footprint (4 x 3 before the x4 scale of task-375) and of a 2-cell margin around it (owner 2026-10-29, provisional, task-376: rock must not stay too close to the ship) is removed before it is seen; an ore deposit or heal vein it touches breaks whole and its chunks or heal pickups lie at rest on the floor of their cells, pickable; a barrel is removed without exploding (owner 2026-10-29, provisional, task-372).
- Breaking terrain is therefore a decision, not a side effect of shooting (pillar P3).
- Player explosive shots: out of scope. Grenades are cut; if they return, charges are limited.
- Destroyed blocks are logged by source and by type ([[run-log-schema]]).

## Relations
- [[terrain-index]] — parent index.
- [[explosions]] — terrain damage from explosions.
