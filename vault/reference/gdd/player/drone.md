---
id: drone
description: "The small helper drone with a cutting laser, piloted by the colleague in orbit: escort (Orbit), dig and hack states, dialogue channel."
parent: player-index
kind: spec
status: approved
---
# Drone

- Small floating drone with a cutting laser (not a drill), remote-controlled by the colleague in orbit ([[lore]]).
- All dialogue comes through it ([[dialogue-tone]]).

## States
- **Orbit:** escorts the miner by default (owner 2026-10-20, replaces the circle; the state keeps its name): it holds a slot near the miner's shoulder; when the miner moves the slot slides behind and to the side of the movement direction; at rest the drone traces a slow horizontal figure-eight around the slot. It follows with a light spring: a little lag, a small overshoot when the miner stops.
- **Dig:** when the miner stands against rock (contact is enough, no need to push), the drone cuts an arc of blocks in front of the miner, several at once like DRGS (owner 2026-10-06). It stays right next to the miner, on the free side of the wall (see Walls).
- **Hack:** inside an objective zone, it links to the command console ([[objective-console]]) or a bonus chest ([[objective-bonus-chest]]). It hacks one target at a time.
- It cannot dig and hack at the same time.

## Rules and values
- It is one of the three block-destruction sources ([[destruction-rules]]).
- Dig speed scales with the miner's mining speed stat ([[player-stats]]). Base dig time per 67 uu block at mining speed 1: rock 0.5 s, ore and heal ore 1.5 s; up to 4 blocks at once within 1.5 cells (now 8 within 2.5 cells, see the Depth line) of the miner, in a 120° arc toward the wall; against a flat wall this gives 3, sometimes 4, blocks, kept as is (owner, after handover-2026-10-06; values provisional, 2026-10-06, measured on the DRGS run: same cleared area per second as before, finer grain).
- Dig feedback: the block being cut shakes until it breaks; only its rendering shakes, never its collision (owner 2026-10-02); smaller shake since the slab look, 8 uu horizontal and 3 uu vertical (owner 2026-10-19, to test, [[task-231-dig-shake-reduced]]). One laser beam only, aimed at the middle of the dug blocks, whatever their number (owner 2026-10-19, [[task-232-drone-single-beam]]); a particle effect at the impact: white-gold sparks, chips of the block colour, a little dust ([[task-233-dig-impact-particle]]).
- Dig trigger and cancel (provisional, 2026-10-06): the miner touching rock starts the dig of the blocks in the arc; a block keeps being cut while it stays in the arc and stops when the miner moves away; damage done stays on the block, so digging it again resumes. Pushing is only needed to walk to the next wall.
- Laser color: red (owner decision, 2026-10-01; the built laser and the Look line below use white-gold: open, to confirm in play), against [[color-code]] (white-gold) and [[readability-rules]] (red = enemy fire or healing); check readability once the shooter's red targeting laser exists.
- Orbit escort (provisional, owner 2026-10-20, [[task-247-drone-escort-movement]]): slot 110-140 uu from the miner (125 uu), at the shoulder at rest, behind and to the side when moving; rest figure-eight period about 6 s; height: see Alive (was 150 uu with a light bob until task-249). Grey box: a 40 uu sphere.
- Walls (provisional, owner 2026-10-20, [[task-248-drone-no-flight-over-walls]]): the drone never hovers over a block. In Orbit it takes the nearest free slot to the ideal one (also 45° or 90° either side, or straight behind the miner) with a clear line from the miner, else it hovers just above the miner, a little higher; it changes slot at most every 0.3 s so it does not hesitate on a corner. While digging it hovers on the free side of the wall, about 90 uu from the face and 130 uu above the floor; while hacking its hover point must be free too, else it comes back toward the miner.
- Alive (provisional, owner 2026-10-20, [[task-249-drone-alive-orientation-height]]: "as if someone commanded it"; visual only, no collision): in Orbit it flies 130-190 uu above the floor with a two-frequency bob, a little higher when an enemy is near the miner, lower when calm; while digging it keeps the 130 uu dig hover, the fallback above the miner keeps its extra height. It turns, with a slight delay, toward a point of interest: the dig or hack target, else the nearest enemy, else the nearest ore deposit or objective, else the miner. It tilts with the miner: nose down when it starts moving, banked into turns, at most about 15°. While the miner rests, every 4 to 8 s at random it makes a small brief gesture: a scan sweep of its heading, a nod toward the miner, or a short dip.
- Can it be hit or destroyed: open.
- Look: small, white + teal + rust; white-gold laser ([[color-code]]).
- Milestone: M1.

- Depth (owner 2026-10-08, to test): the arc reaches 2.5 cells (was 1.5) and cuts up to 8 blocks at once (was 4), about two rows deep, at the same speed per block.

- Diagonal contact (owner 2026-10-09): when the miner touches rock but no block falls inside the arc (e.g. moving diagonally into a corner of two blocks), the drone still digs the nearest touching block(s), never nothing.

## Relations
- [[player-index]] — parent index.
- [[miner]] — who it follows.
