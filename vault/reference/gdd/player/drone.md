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
- **Dig:** when the miner stands against a block, the drone cuts that block. It stays right next to the miner, on the same block.
- **Hack:** inside an objective zone, it links to the command console ([[objective-console]]) or a bonus chest ([[objective-bonus-chest]]). It hacks one target at a time.
- It cannot dig and hack at the same time.

## Rules and values
- It is one of the three block-destruction sources ([[destruction-rules]]).
- Dig speed scales with the miner's mining speed stat ([[player-stats]]). Base dig time at mining speed 1: rock 1.5 s, ore and heal ore 4.5 s (provisional, owner 2026-10-01).
- Dig feedback: the block being cut shakes until it breaks.
- Dig trigger and cancel (provisional): the miner pushing into an adjacent block starts the dig; it goes on while the miner stays within 1.5 cells of that block and stops when the miner moves away; damage done stays on the block, so digging it again resumes.
- Laser color: red (owner decision, 2026-10-01), against [[color-code]] (white-gold) and [[readability-rules]] (red = enemy fire or healing); check readability once the shooter's red targeting laser exists.
- Orbit (provisional): radius 150 uu, one turn per 2 s, height 150 uu above the floor; grey box: a 40 uu sphere.
- Can it be hit or destroyed: open.
- Look: small, white + teal + rust; white-gold laser ([[color-code]]).
- Milestone: M1.

## Relations
- [[player-index]] — parent index.
- [[miner]] — who it follows.
