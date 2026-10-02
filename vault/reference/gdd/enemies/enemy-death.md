---
id: enemy-death
description: "How robots die: rigid parts thrown as Niagara mesh particles from the last hit's direction, then shrink and sink; sparks, eye off."
parent: enemies-index
kind: spec
status: approved
---
# Robot death

- Robots are built from separate rigid parts, each bound 100 % to one bone. That split is what makes the death effect possible.
- On death the robot is hidden and its parts are thrown from the direction of the last hit, as Niagara mesh particles using the part meshes.
- Parts collide with the floor only. No physics interaction with the miner, robots or blocks.
- Parts then shrink and sink into the floor quickly: no debris stays in the arena.
- Juice: a burst of sparks; the red eye turns off.
- Debris systems are pooled.
- A robot dies exactly once ([[sys-damage]]).
- Chaos Destruction is not used, for robots or terrain.
- Milestone: M3 (juice pass). M1: the robot shrinks for 0.15 s, disappears and drops its XP cube (owner 2026-10-02).

Robots knocked into the void by explosions: [[wishlist]].

## Relations
- [[enemies-index]] — parent index.
- [[asset-pipeline]] — how the parts are produced.
