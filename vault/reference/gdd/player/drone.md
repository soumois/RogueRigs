---
id: drone
description: "The small helper drone with a cutting laser, piloted by the colleague in orbit: orbit, dig and hack states, dialogue channel."
parent: player-index
kind: spec
status: approved
---
# Drone

- Small floating drone with a cutting laser (not a drill), remote-controlled by the colleague in orbit ([[lore]]).
- All dialogue comes through it ([[dialogue-tone]]).

## States
- **Orbit:** circles the miner by default.
- **Dig:** when the miner stands against rock (contact is enough, no need to push), the drone cuts an arc of blocks in front of the miner, several at once like DRGS (owner 2026-10-06). It stays right next to the miner.
- **Hack:** inside an objective zone, it links to the command console ([[objective-console]]) or a bonus chest ([[objective-bonus-chest]]). It hacks one target at a time.
- It cannot dig and hack at the same time.

## Rules and values
- It is one of the three block-destruction sources ([[destruction-rules]]).
- Dig speed scales with the miner's mining speed stat ([[player-stats]]). Base dig time per 67 uu block at mining speed 1: rock 0.5 s, ore and heal ore 1.5 s; up to 4 blocks at once within 1.5 cells of the miner, in a 120° arc toward the wall; against a flat wall this gives 3, sometimes 4, blocks, kept as is (owner, after handover-2026-10-06; values provisional, 2026-10-06, measured on the DRGS run: same cleared area per second as before, finer grain).
- Dig feedback: the block being cut shakes until it breaks; only its rendering shakes, never its collision (owner 2026-10-02).
- Dig trigger and cancel (provisional, 2026-10-06): the miner touching rock starts the dig of the blocks in the arc; a block keeps being cut while it stays in the arc and stops when the miner moves away; damage done stays on the block, so digging it again resumes. Pushing is only needed to walk to the next wall.
- Laser color: red (owner decision, 2026-10-01), against [[color-code]] (white-gold) and [[readability-rules]] (red = enemy fire or healing); check readability once the shooter's red targeting laser exists.
- Orbit (provisional): radius 150 uu, one turn per 2 s, height 150 uu above the floor; grey box: a 40 uu sphere.
- Can it be hit or destroyed: open.
- Look: small, white + teal + rust; white-gold laser ([[color-code]]).
- Milestone: M1.

## Relations
- [[player-index]] — parent index.
- [[miner]] — who it follows.
