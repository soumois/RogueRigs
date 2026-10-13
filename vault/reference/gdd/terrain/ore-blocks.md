---
id: ore-blocks
description: "Ore blocks: tougher than rock, always drop ore whatever broke them, explosions destroy them like any block. Look and values."
parent: terrain-index
kind: spec
status: approved
---
# Ore blocks

- Tougher than plain rock blocks: drone dig time 1.5 s per 67 uu block at mining speed 1, against 0.5 s for rock (owner, after handover-2026-10-06).
- Each broken ore block pops one ore chunk (DRGS-like arc then suction, [[xp-level-up]]); ore value per chunk: 1 (owner 2026-10-10, provisional).
- The chunk drops whatever destroyed the block, drone or explosion; no extra bundle ([[ore-economy]]; owner, after handover-2026-10-06).
- The chunk lands within about one cell of the broken block, so it never rests over unbroken rock (provisional, task-027-01, owner to judge).
- Any explosion destroys every block in its radius ([[explosions]]): luring a kamikaze onto a vein is a valid tactic.
- Look (owner 2026-10-10, validated): same base as breakable rock, with glowing violet crystals, kept as built. The ore is called Duskite (provisional name, replaces "gold"; the owner wanted a name apart from the gold colour); Duskite is the only ore for now ([[color-code]]).
- Values above are provisional, set by the owner on 2026-10-01; ore value per pickup: 1 (owner 2026-10-10).
- Milestone: M1.

- Deposits (owner 2026-10-10, replaces the 3 × 3 veins): ore comes in groups of 2 × 2 blocks, never isolated; touching any block mines the whole deposit at once, its 4 blocks break together after one block dig time (1.5 s) and pop their 4 chunks. A deposit is identified by its own id, not by neighbouring blocks: deposits may touch.
- Duskite spawn rate (owner 2026-10-10): 8 % of the rock blocks are Duskite, in 2 × 2 groups (about 358 deposits on the current layout), placed at grid build. Each ore type has its own row in a table (share of rock, group size, value, colour); only Duskite exists for now. A deposit may not overlap heal veins or barrels; since 2026-10-13 console-marked rocks include ore deposits ([[objective-console]]); the start zone needs no protection: if a block of a deposit is destroyed there (for example by the arrival capsule), its chunks are thrown to the ground like any explosion drop.

- Vein shake (owner 2026-10-07 fifth test): all blocks of a vein being mined shake together.

## Relations
- [[terrain-index]] — parent index.
- [[explosions]] — destroy every block in the radius, ore included.
