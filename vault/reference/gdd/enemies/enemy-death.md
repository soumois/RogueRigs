---
id: enemy-death
description: "How robots die: shrink hidden by a puff, then 2 to 4 random pieces thrown as pooled Niagara mesh particles, sinking after 4.5 s."
parent: enemies-index
kind: spec
status: approved
---
# Robot death

- Robots are built from separate rigid parts, each bound 100 % to one bone. That split is what makes the death effect possible.
- On death the robot shrinks for 0.15 s (as in M1) and drops its XP; a cartoon flash starts at the death and hides the shrink (provisional, owner 2026-11-05, [[task-436-robot-death-particle]], reference `Art/Source/robot-death-fx/20261105-owner-ref-cartoon-explosion.png`; was about 12 warm sprites in 0.3 s): small flash, white and yellow core with jagged cel-shaded edges in flat colour bands, orange and red-orange outer bands, chunky blobs and thin lines thrown outward, dark red burnt puffs at the end, at most 20 sprites, about 0.4 s; the heavy keeps the same look about 3 times larger.
- When the robot disappears, 2 to 4 pieces are thrown, as Niagara mesh particles (provisional, owner 2026-10-14; was 2 or 3):
  - the pieces are drawn at random among: torso block (head, torso and pelvis as one piece), each arm, each leg;
  - an arm is thrown whole or cut at the elbow, a leg whole or cut at the knee, one time in two; a cut limb counts as 2 pieces;
  - the count is the number of meshes thrown (2, 3 or 4, equal odds); the other parts vanish with the robot;
  - throw (provisional, owner 2026-10-14): each piece leaves along the direction of the killing hit (radial from an explosion) turned by a random angle within ±15°; final speed (provisional, owner 2026-10-15) 325-600 along the direction and 350-550 upward, i.e. a base of 250-300 and 269.2-275 × a random factor F of 1.3 to 2 drawn once per death, plus a small per-piece spread (±100 × F); start position = the piece place on the robot ± 10 cm on each axis.
- The pieces have no glow: the red eye is off.
- Pieces collide with the floor and bounce off walls (rock blocks), both through distance field collision on a GPU emitter, off from 4.5 s for the sink (owner 2026-10-15, was an analytic floor plane only). No interaction with the miner or robots.
- Pieces stay 4.5 s at full size, then sink into the floor in 0.4 s without shrinking: 4.9 s in all; no debris stays in the arena (provisional, owner 2026-10-15; was 1.2 s with a shrink, then 3.4 s).
- Each piece spins about a random axis at 0.3 to 0.8 rev/s at the throw; in the air the spin halves every 1 s, each floor bounce halves it, and it stops once the piece rests on the floor (provisional, owner 2026-10-15; was a fixed 1.5 rev/s that never slowed down).
- Debris and puff systems are pooled, debris 80 and puff 40 at most; when the debris pool is empty, only the puff plays (provisional, owner 2026-10-15, test).
- First robots: the chaser and the shield chaser (same mesh); other robots keep the M1 death until their parts exist. Planned (provisional, owner 2026-10-22, decided, not built yet: tasks 281 to 288): heavy, shooter and chaser v2 with 13 pieces each as the chaser, kamikaze with 3 pieces (body, 2 legs; 2 or 3 thrown), shield chaser v2 with the chaser v2 pieces; v1 debris unchanged.
- A robot dies exactly once ([[sys-damage]]).
- Chaos Destruction is not used, for robots or terrain.
- Milestone: M3 (juice pass), brought forward on 2026-10-14 by the owner so he can measure its cost. M1: the robot shrinks for 0.15 s, disappears and drops its XP cube (owner 2026-10-02).

Robots knocked into the void by explosions: [[wishlist]].

## Relations
- [[enemies-index]] — parent index.
- [[asset-pipeline]] — how the parts are produced.
- [[chaser-meshy-pipeline]] — the chaser parts the pieces come from.
