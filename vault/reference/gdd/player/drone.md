---
id: drone
description: "The small helper drone with a cutting laser, piloted by the colleague in orbit: patrol (Orbit), dig and hack states, dialogue channel."
parent: player-index
kind: spec
status: approved
---
# Drone

- Small floating drone with a cutting laser (not a drill), remote-controlled by the colleague in orbit ([[lore]]).
- All dialogue comes through it ([[dialogue-tone]]).

## States
- **Orbit:** patrols the miner's zone by default (owner 2026-10-20, replaces the escort, which replaced the circle; the state keeps its name): it flies to a patrol point picked at random around the miner and picks another when it reaches it or after a few seconds. The zone moves with the miner; the miner's facing, aim and movement direction never place the drone. It follows with a lag: the farther from its point, the faster; the nearer, the slower. A miner walking a little in circles causes no sudden move; a miner walking away leaves the drone behind (even when its point lies ahead), then it catches up.
- **Dig:** when the miner stands against rock (contact is enough, no need to push), the drone cuts an arc of blocks in front of the miner, several at once like DRGS (owner 2026-10-06). It stays right next to the miner, on the free side of the wall (see Walls).
- **Hack:** inside an objective zone, it links to the command console ([[objective-console]]) or a bonus chest ([[objective-bonus-chest]]). It hacks one target at a time.
- It cannot dig and hack at the same time.

## Rules and values
- It is one of the three block-destruction sources ([[destruction-rules]]).
- Dig speed scales with the miner's mining speed stat ([[player-stats]]). Base dig time per 67 uu block at mining speed 1: rock 0.65 s (owner 2026-10-20, provisional, [[task-260-rock-dig-time-065]]; was 0.5 s), ore and heal ore 2 s (owner 2026-10-20, provisional, [[task-259-ore-dig-time-2s]]; was 1.5 s); up to 4 blocks at once within 1.5 cells (now 8 within 2.5 cells, see the Depth line) of the miner, in a 120° arc toward the wall; against a flat wall this gives 3, sometimes 4, blocks, kept as is (owner, after handover-2026-10-06; values provisional, 2026-10-06, measured on the DRGS run: same cleared area per second as before, finer grain).
- Dig feedback: the block being cut shakes until it breaks; only its rendering shakes, never its collision (owner 2026-10-02); smaller shake since the slab look, 8 uu horizontal and 3 uu vertical (owner 2026-10-19, to test, [[task-231-dig-shake-reduced]]). One laser beam only, aimed at the middle of the dug blocks, whatever their number (owner 2026-10-19, [[task-232-drone-single-beam]]); a particle effect at the impact: white-gold sparks, chips of the block colour, a little dust ([[task-233-dig-impact-particle]]). Each block broken by the drone also throws a one-shot burst of debris of the block colour that falls to the floor and is gone within 0.8 s, at most 12 sprites (provisional, owner 2026-11-05, [[task-438-dig-debris]]).
- Dig trigger and cancel (provisional, 2026-10-06): the miner touching rock starts the dig of the blocks in the arc; a block keeps being cut while it stays in the arc and stops when the miner moves away; damage done stays on the block, so digging it again resumes. Pushing is only needed to walk to the next wall.
- Laser color: red (owner decision, 2026-10-01; the built laser and the Look line below use white-gold: open, to confirm in play), against [[color-code]] (white-gold) and [[readability-rules]] (red = enemy fire or healing); check readability once the shooter's red targeting laser exists.
- Orbit patrol (provisional, owner 2026-10-20, [[task-251-drone-patrol-movement]]; replaces the escort of [[task-247-drone-escort-movement]]: shoulder slot, behind when moving, figure-eight at rest, whose slot depended on the heading): patrol point at a random angle and distance 60-160 uu from the miner; a new point when the drone comes within 25 uu of it, after a random 3.5-5 s, or when it becomes invalid (see Walls); follow speed from about 40 uu/s near the point to 700 uu/s far from it (smooth curve, clamped), no overshoot at the point; height: see Alive. Grey box: a 40 uu sphere.
- Walls (provisional, owner 2026-10-20, [[task-248-drone-no-flight-over-walls]]): the drone never hovers over a block. In Orbit a patrol point must be a free cell with a clear line from the miner, else it is rejected and another drawn; with no valid point the drone hovers just above the miner, a little higher. While digging it hovers on the free side of the wall, about 90 uu from the face and 130 uu above the floor; while hacking its hover point must be free too, else it comes back toward the miner.
- Alive (provisional, owner 2026-10-20, [[task-249-drone-alive-orientation-height]]: "as if someone commanded it"; visual only, no collision): in Orbit it flies 130-190 uu above the floor with a two-frequency bob, a little higher when an enemy is near the miner, lower when calm; while digging it keeps the 130 uu dig hover, the fallback above the miner keeps its extra height. It turns, with a slight delay, toward a point of interest: the dig or hack target, else the nearest enemy, else the nearest ore deposit or objective, else the miner. It tilts with the miner: nose down when it starts moving, banked into turns, at most about 15°. While the miner rests, every 4 to 8 s at random it makes a small brief gesture: a scan sweep of its heading, a nod toward the miner, or a short dip.
- Parts (provisional, owner 2026-10-20, [[task-253-drone-parts-animation]]; visual only): three animated parts. The antenna spins continuously about its vertical axis (about a quarter turn per second). The body turns toward where it mines or hacks at a limited rate (about 120° per second), never instantly. The laser arm hangs under the body and pivots in height to aim at the impact point; the beam starts at its tip. In Orbit the arm hangs straight down.
- Engines (provisional, owner 2026-10-20, [[task-254-drone-thruster-particles]]; visual only): each of the two side pods has a soft engine exhaust pointing down, driven by the drone's own speed ("depending on the speed of the robot"): a faint idle glow at rest, longer and denser streaks as it flies faster, full at its top speed (700 uu/s). Always on, in every mode.
- Can it be hit or destroyed: open.
- Look: small, white + teal + rust; white-gold laser ([[color-code]]).
- Milestone: M1.

- Depth (owner 2026-10-08, to test): the arc reaches 2.5 cells (was 1.5) and cuts up to 8 blocks at once (was 4), about two rows deep, at the same speed per block.

- Diagonal contact (owner 2026-10-09): when the miner touches rock but no block falls inside the arc (e.g. moving diagonally into a corner of two blocks), the drone still digs the nearest touching block(s), never nothing.

## Relations
- [[player-index]] — parent index.
- [[miner]] — who it follows.
