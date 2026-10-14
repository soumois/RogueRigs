---
id: enemy-death
description: "How robots die: shrink hidden by a puff, then 2 to 4 random pieces thrown as pooled Niagara mesh particles, sinking after 3 s."
parent: enemies-index
kind: spec
status: approved
---
# Robot death

- Robots are built from separate rigid parts, each bound 100 % to one bone. That split is what makes the death effect possible.
- On death the robot shrinks for 0.15 s (as in M1) and drops its XP; a puff of sparks (about 12 sprites, 0.3 s) starts at the death and hides the shrink (provisional, owner 2026-10-14).
- When the robot disappears, 2 to 4 pieces are thrown, as Niagara mesh particles (provisional, owner 2026-10-14; was 2 or 3):
  - the pieces are drawn at random among: torso block (head, torso and pelvis as one piece), each arm, each leg;
  - an arm is thrown whole or cut at the elbow, a leg whole or cut at the knee, one time in two; a cut limb counts as 2 pieces;
  - the count is the number of meshes thrown (2, 3 or 4, equal odds); the other parts vanish with the robot;
  - throw (provisional, owner 2026-10-14): each piece leaves along the direction of the killing hit (radial from an explosion) turned by a random angle within ±15°; speed = the first tuning (task-166) × a random factor of 1.3 to 2 drawn once per death, plus a small per-piece spread; start position = the piece place on the robot ± 10 cm on each axis.
- The pieces have no glow: the red eye is off.
- Pieces collide with the floor only (an analytic floor plane). No interaction with the miner, robots or blocks.
- Pieces stay 3 s at full size, then sink into the floor in 0.4 s without shrinking: 3.4 s in all; no debris stays in the arena (provisional, owner 2026-10-14; was 1.2 s with a shrink).
- Debris and puff systems are pooled, 40 each at most; when the debris pool is empty, only the puff plays (provisional, owner 2026-10-14).
- First robots: the chaser and the shield chaser (same mesh); other robots keep the M1 death until their parts exist.
- A robot dies exactly once ([[sys-damage]]).
- Chaos Destruction is not used, for robots or terrain.
- Milestone: M3 (juice pass), brought forward on 2026-10-14 by the owner so he can measure its cost. M1: the robot shrinks for 0.15 s, disappears and drops its XP cube (owner 2026-10-02).

Robots knocked into the void by explosions: [[wishlist]].

## Relations
- [[enemies-index]] — parent index.
- [[asset-pipeline]] — how the parts are produced.
- [[chaser-meshy-pipeline]] — the chaser parts the pieces come from.
