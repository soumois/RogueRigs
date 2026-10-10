---
id: layout-generator-recipe
description: "Algorithm that produced arena_m1 (asteroid outline, carved organic paths, hidden pockets, heal ore); the reference for the M2 C++ generator."
parent: pipelines-index
kind: doc
---
# Layout generator recipe

Reference implementation (Node, seeded LCG, deterministic): `Tools/layout_gen/arena_gen.js` (build + measure), `arena_finish.js` (heal ore, JSON output), `arena_seed_search.js` (seed pick). Produced `arena_m1` with seed 274 ([[task-003-01-layout-80-paths]]). Parameters: `base 34.5, main 0.27, turn 0.28, pockets 7, plen 14, maxd 5`.

## Steps
1. **Outline.** Polar radius around the center: `r(θ) = base + 1.6 sin(3θ+1) + 1.0 sin(5θ+2) + 0.6 sin(8θ) + 0.5 sin(13θ+0.5)`, clamped 2 cells from the grid edge. Inside = rock, outside = void. Then notch any straight outline run longer than 6 cells (remove 2–3 cells near its middle) until none remains.
2. **Spawn clearing.** Wobbly disc of radius about 3.6 at the center, plus the 5 × 5 square.
3. **Main network.** Repeat until floor reaches `main` × asteroid cells: pick a random floor cell, run a *wander walk* (30–80 steps) and carve it.
   - Wander walk: heading gets Gaussian noise × `turn` each step, plus a 6 % chance of a sharp turn (0.8–1.8 rad); brush radius drifts in [0.8, 1.6] (paths 2–3 cells wide); stops one cell before void; never carves next to a hidden pocket.
4. **Hidden pockets.** Up to `pockets` times: start a wander walk (12 + `plen` steps) inside rock; keep it only if every cell is rock, none touches the main network, it has 18+ cells, and its distance to the main network is 2 to 4 (Manhattan), so 1 to 3 rock cells separate them.
5. **Far-cell filler.** While a cell lies more than `maxd` cells (BFS) from floor, walk from the nearest reachable floor toward it and carve.
6. **Cleanup.** Unreachable regions under 15 cells, or more than 3 cells from the main network, are refilled with rock; filler again; isolated rock cells (no rock neighbor) become floor.
7. **Heal ore.** Candidates outside the 17 × 17 square around the spawn: rock cells touching the main network with 2+ rock neighbors (walls), and hidden-pocket floor cells with 3+ rock neighbors. Pick 4 in pockets and 8 in walls, each time the farthest from those already picked (best of 300 random tries).
8. **Seed search.** Keep seeds that pass: one piece, outline runs ≤ 6, max distance to floor ≤ 5, asteroid 3600–4600 cells, coverage about 0.6, reachable floor 0.65–0.86, 4+ valid pockets; then pick the one with the shortest straight corridor.

## Fine-grid conversion (240 × 240, 67 uu)
`Tools/layout_gen/arena_fine.js` turns `arena_m1` into `arena_m1_fine` ([[task-029-03-layout-67]]), deterministic (position hash, seed 6701, no RNG state).
1. Each old cell becomes 3 × 3 cells of the same type. Spawn and barrel keep only the centre cell (rest floor); ore and heal ore stay whole 3 × 3 clusters (heal veins are then shrunk to 2 × 2, step 4).
2. Roughening with 2-octave value noise (scales 6 and 2.5 cells): two erosion passes rock to floor (thresholds 0.42, 0.40), one dilation pass floor to rock (depth 1, threshold 0.60), two erosion passes rock to void on the cliff edge (0.45). An erosion needs 2 rock cells behind the face, so no wall under 3 cells is breached and hidden pockets stay closed. Ore, heal ore, barrel, spawn and the 17 × 17 spawn zone never change.
3. Passage rule (3 cells, confirmed by the owner after handover-2026-10-06): eroded cells that leave a floor cell outside every all-open 3 × 3 window are reverted; each dilation is kept only if no open cell within 2 cells ends up uncovered.
Result: 8 floor components as before, reachable floor 85.5 %, minimum passage 3.
4. **Heal vein shrink (owner decision 2026-10-10, [[task-088-01-heal-veins-2x2]]).** `arena_fine.js` runs it as its last step since [[task-088-02-heal-veins-2x2]] (`HEAL_VEIN` 2, after roughening and repair, which still read the 3 × 3 clusters; regenerated output byte-identical to the committed `arena_m1_fine.json`), deterministic (no RNG, no seed), turning each cluster into a 2 × 2 vein. Each connected heal cluster (12, 9 cells each) keeps the 2 × 2 block at its minimum x and minimum y (top-left corner of the former 3 × 3, same footprint corner for all veins); the 5 freed cells (row and column at max x or max y) become `rock` (every vein was embedded in rock, so no passage changes). Counts before/after: rock 17904 to 17964 (+60), heal_ore 108 to 48; void, barrel (16), spawn unchanged; reachable floor 85.5 %, minimum passage 3. Brief bound `heal_ore` is [48, 48]. Barrels are never touched; console marks are placed at runtime, not in the layout.

## For the C++ port
- Replace the LCG with `FRandomStream`; keep every parameter as data.
- Validation rules: [[level-layout-schema]].

## Relations
- [[pipelines-index]] — parent index.
- [[level-layout-schema]] — output format and validator.
- [[sys-grid]] — the grid that will run the C++ port.
- [[task-029-03-layout-67]] — the fine-grid conversion task.
- [[task-088-01-heal-veins-2x2]] — heal veins shrunk to 2 × 2.
