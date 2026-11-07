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

## Direct 240 generation
`Tools/layout_gen/arena_gen_fine.js` runs the steps above natively on 240 × 240 (67 uu) for more organic paths than the 3 × 3 upscale ([[task-205-01-direct-240-generator]]). Every value is in `Tools/layout_gen/params/fine_default.json`. CLI: `node Tools/layout_gen/arena_gen_fine.js --params <file> --seed <n> --name <name> --out <dir> [--brief <brief.json>]`; with `--brief` it tries seeds upward from `--seed` (`search.max_seeds`) until the recipe checks and the brief pass, prints the chosen seed and writes `<name>.brief.json`. Same params + seed give byte-identical output.
- Mapping from the 80 recipe: lengths × 3 (outline base 103.5 and harmonics, edge margin 6, notch run 18 / length 6–9 / depth 3, spawn disc 10.8, brush radius 2.4–4.8, walk steps 90–240, void margin 3, pocket guard 3, pocket distance 4–10, `maxd` 15, cleanup gap 9, barrel spawn exclusion 18, heal exclusion square 51 × 51); areas × 9 (pocket 162+ cells, unreachable region 135, asteroid 32400–41400). Step length is one fine cell, so heading noise is divided by √3 (`turn` 0.162) and the sharp-turn chance by 3 (2 %). Spawn zone 17 × 17 is kept open.
- Additions: value-noise jitter on the outline (±2 cells) and on the brush edge (±0.9 cell); rock islands of 9 cells or fewer become floor; the 3-cell passage repair of the fine conversion (narrow floor turns to rock), looped with region refill and filler until stable, always ending on a repair; thin outline peninsulas inside the void margin and beyond `maxd` become void.
- Changed rules: heal veins are 2 × 2 blocks (`heal.pocket_veins` 5 in pocket floor with 4+ rock neighbours and no passage break, `heal.wall_veins` 9 in walls touching the main network: 14 veins, 56 cells); barrels sit on reachable floor against a wall instead of in rock (a barrel in a straight wall breaks the 3-cell rule), `barrels.min`–`barrels.max` 17–22 drawn per seed, 3–4 pairs 2–3.2 cells apart for chains. The heal and barrel counts are 20 % above the former 12 veins (4 + 8) and 14–18 barrels (owner 2026-11-06, provisional, [[task-491-generator-heal-barrel-plus20]]); the committed layouts keep the former counts; the seed search keeps the first passing seed (no shortest-corridor ranking).
- Test seeds with the default params and a brief like `arena_m1_fine.brief.json` with `heal_ore` [56, 56] and `barrel` [17, 22] (task-491): 3 (coverage 57.6 %, reachable 77.7 %, 7 pockets, 56 heal_ore, 22 barrels) and 100 (52.9 %, 78.2 %, 9 pockets, 56 heal_ore, 22 barrels), validator OK, minimum passage 3. Before task-491 (12 veins, 14–18 barrels): 3 had 18 barrels, 100 had 14.

## For the C++ port
- Replace the LCG with `FRandomStream`; keep every parameter as data.
- Validation rules: [[level-layout-schema]].
- Done in [[sys-layout-generator]] ([[task-492-cpp-layout-generator]]): same steps and params file, `RRLayoutGen` commandlet.
- Level-Design verdict on 24 passing C++ seeds ([[task-492-03-ld-validation]]): all pass the validator with passage 3; 4 keep every 24 × 24 window in 0.05–0.95, the rest show a dead or too-open window; comparable to the Node generator (the difference is only the random stream).
- Since [[task-495-generator-window-repair]] the C++ generator also repairs and checks the [[layout-review-rules]] (windows, straight lanes, pocket distance); the Node script does not. Level-Design verdict ([[task-495-02-ld-review]]): 24 of 24 of the same starts meet every review rule and the validator, repaired areas look like the rest of the terrain.

## Relations
- [[sys-layout-generator]] — the C++ port of this recipe.
- [[layout-review-rules]] — review rules the Level-Design agent applies on top of the acceptance.
- [[pipelines-index]] — parent index.
- [[level-layout-schema]] — output format and validator.
- [[sys-grid]] — the grid that will run the C++ port.
- [[task-029-03-layout-67]] — the fine-grid conversion task.
- [[task-088-01-heal-veins-2x2]] — heal veins shrunk to 2 × 2.
- [[task-205-01-direct-240-generator]] — the direct 240 generator task.
- [[task-491-generator-heal-barrel-plus20]] — heal veins and barrels raised by 20 % in the params.
- [[task-495-02-ld-review]] — Level-Design verdict on the review repairs.
