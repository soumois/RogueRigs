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
- **Hack:** inside an objective zone, it links to the beacon ([[objective-beacon-hack]]).
- It cannot dig and hack at the same time.

## Rules and values
- It is one of the three block-destruction sources ([[destruction-rules]]).
- Dig time per block (rock, ore): open.
- Dig speed as an upgrade target: open (appears only as an example in the GAS stacking rules).
- Can it be hit or destroyed: open.
- Look: small, white + teal + rust; white-gold laser ([[color-code]]).
- Milestone: M1.

## Relations
- [[player-index]] — parent index.
- [[miner]] — who it follows.
