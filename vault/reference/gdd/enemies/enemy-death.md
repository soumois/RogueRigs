---
id: enemy-death
description: "How robots die: shrink hidden by a puff, then 2 to 3 random pieces thrown as pooled Niagara mesh particles, floor-only collision."
parent: enemies-index
kind: spec
status: approved
---
# Robot death

- Robots are built from separate rigid parts, each bound 100 % to one bone. That split is what makes the death effect possible.
- On death the robot shrinks for 0.15 s (as in M1) and drops its XP; a puff of sparks (about 12 sprites, 0.3 s) starts at the death and hides the shrink (provisional, owner 2026-10-14).
- When the robot disappears, 2 or 3 pieces are thrown from the direction of the last hit (radial from an explosion), as Niagara mesh particles (provisional, owner 2026-10-14):
  - the pieces are drawn at random among: torso block (head, torso and pelvis as one piece), each arm, each leg;
  - an arm is thrown whole or cut at the elbow, a leg whole or cut at the knee, one time in two; a cut limb counts as 2 pieces;
  - the count is the number of meshes thrown (2 or 3); the other parts vanish with the robot.
- The pieces have no glow: the red eye is off.
- Pieces collide with the floor only (an analytic floor plane). No interaction with the miner, robots or blocks.
- Pieces then shrink and sink into the floor: 1.2 s in all; no debris stays in the arena (provisional, owner 2026-10-14).
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
