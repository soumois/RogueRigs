---
id: ore-in-rock-proposal
description: "Design proposal (task-070-01, awaiting owner): ore nuggets scattered in rock blocks, density, ore per nugget, look, drip rate, level ore totals."
parent: production-index
kind: note
---
# Ore in the rock: proposal (task-070-01, 2026-10-10)

Every value below is `proposal`; nothing is built. The owner answers "ok" or amends by number. Code, performance, subtasks and open values: [[ore-in-rock-plan]].

## Superseded by the owner (2026-10-10)
This proposal was amended: 8 % of the rock blocks are gold, in groups of 2 × 2 never isolated, mined as one deposit (4 chunks, 1.5 s), amber-gold look, 1 per chunk, a per-ore table with only gold for now, heal veins also 2 × 2, no start-zone protection. Decided values live in [[ore-blocks]], [[heal-ore]] and [[color-code]]; items 1, 3, 5, 6 and 7 below no longer apply as written.

## Reference and current state
- DRGS: nuggets sit in the rock mass; each broken slab pops its nugget, about 3 chunks per second while digging; each arcs, lands, is pulled in 0.5–1 s later ([[drgs-video-2026-10-06]]). The video note gives no nugget size or colour beyond "gold", nor a scatter distance.
- Rogue Rigs: one chunk per broken Ore block, Ore blocks in 3 × 3 veins mined whole in 1.5 s ([[ore-blocks]]); rock 0.5 s per block; drone arc up to 8 blocks within 2.5 cells ([[sys-drone]]).
- Counted in `arena_m1_fine.json` today: 17 904 rock, 108 heal ore (12 veins of 9), 16 barrels, **0 Ore cells**. The only ore of a level is the bonus chest's 5 chunks ([[objective-bonus-chest]]). Ore value per chunk: open ([[ore-blocks]]).
- Ore pickups give no XP and the meta shop is deferred ([[sys-pickups]], [[ore-economy]]): ore is only the extraction stake.

## Proposal
1. Density: 20 % of rock blocks hold one nugget, placed by a deterministic hash of the cell and a seed at grid build (no layout file change; the M2 generator inherits it).
2. Ore per nugget: 1. Proposed common unit for every ore chunk: 1 (vein block 1, chest 5).
3. Look: a visible fleck, not hidden. A nugget block keeps the rock base and cracks and shows one small glowing violet crystal on its top face, a smaller version of the Ore block crystals ([[color-code]]); "ore glows" ([[readability-rules]]) and a visible fleck lets the player choose where to dig. No new colour.
4. Drip: the nugget pops when its block breaks, whatever broke it (same rule as [[ore-blocks]]), as the existing Ore pickup (cyan, magnetised like XP cubes), pop shortened to 30–67 uu toward the miner so it never rests over rock, attract delay 0.5 s (DRGS 0.5–1 s).
5. Rate at mining speed 1: 8 blocks per 0.5 s = 16 blocks/s × 20 % = 3.2 nuggets/s (DRGS ≈ 3/s); a 4-block arc gives 1.6/s. Mining speed upgrades scale it.
6. Veins: keep the 3 × 3 Ore vein mechanic unchanged as a rare jackpot; none in the current layout, none added now. Heal veins unchanged.
7. Level totals: today 5 chunks (chest). New: 17 904 × 0.2 ≈ 3 581 nuggets in the whole layout; one minute of full-sweep digging ≈ 960 blocks ≈ 192 nuggets. The share of rock dug per level is not in any node: open.
8. Progression: no XP or upgrade change; ore stays the stake lost on a missed shuttle.

## Relations
- [[production-index]] — parent index.
- [[ore-in-rock-plan]] — code landing, performance budget, subtasks and open values of this proposal.
- [[task-070-01-ore-in-rock]] — the subtask that produced it.
- [[drgs-video-2026-10-06]] — the DRGS ore drip it copies.
- [[ore-blocks]] — the spec it would amend once accepted.
