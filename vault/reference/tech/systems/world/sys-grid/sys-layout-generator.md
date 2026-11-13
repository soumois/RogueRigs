---
id: sys-layout-generator
description: "C++ 240 x 240 layout generator: params, determinism, review repairs, seed acceptance, run use, JSON output, RRLayoutGen commandlet. Open to run or change it."
parent: sys-grid
kind: doc
---
# Layout generator (C++)

- responsibility: generates a level layout from a seed and data params, following [[layout-generator-recipe]] "Direct 240 generation" (port of `Tools/layout_gen/arena_gen_fine.js`, [[task-492-cpp-layout-generator]], commits 85a2ee48 and 43c51ac8); writes it in the [[level-layout-schema]] format. No runtime AI.
- main_classes: `FRRLayoutGenerator` (`Source/RogueRigs/Grid/RRLayoutGenerator.h`), plain C++, one instance per thread; `FRRLayoutGenParams`, `FRRLayoutBrief` and their readers `RRLayoutGen::LoadParams` / `LoadBrief` (`RRLayoutGenParams.h`); `URRLayoutGenCommandlet` (`RRLayoutGenCommandlet.h`).
- entry_points:
  - `Carve(Seed)`: outline and notching, spawn clearing, main network, hidden pockets, far-cell filler, cleanup, review repair passes, 3-cell passage repair. `Decorate()`: heal veins and barrels. `Measure()`, `Check(Stats, Brief)`: recipe acceptance and brief bounds.
  - `Generate(FirstSeed, Brief)`: tries seeds upward from `FirstSeed` (`search.max_seeds`) and returns the first passing one (`Fails` empty) with its stats and times. `ToJson(Name, Layout)`: compact JSON, every non-floor cell row-major.
  - Runs (task-496): `FRRGeneratorLayoutProvider` calls `Generate` on a worker for every run level, in memory, with the packaged per-level params and brief and a level seed derived from the run seed: [[sys-run-layout]]. Where the layout JSON came from before: the committed `Content/RogueRigs/Data/Levels` layouts (arena_m1_fine, arena_l2 to l4) were written by the Node scripts of `Tools/layout_gen`; they are now the run fallback.
  - `HashCells(Cells)`: FNV-1a of a cell array, logged by the run provider and the commandlet (`cells hash`) to compare a run level with a commandlet output.
  - Command line, editor closed: `UnrealEditor-Cmd.exe RogueRigs.uproject -run=RRLayoutGen -seed=<n> [-params=<file>] [-name=<snake_case>] [-out=<dir>] [-brief=<file>] -unattended -nullrhi -nosplash -nop4`. Defaults: `Tools/layout_gen/params/fine_default.json`, name `arena_gen`, `Saved/GeneratedLevels/`. Writes `<name>.json`, plus `<name>.brief.json` (copy of the brief) with `-brief`; logs one line per tried seed (with window min-max, longest lane, pocket distance, repair counts), the accepted seed, its review measures, the layout and search times; writes nothing and exits 1 when no seed passes. `-carveonly` carves twice and reports the hash, rerun identity and narrow cells, the filler time, its share of the carve and its iterations (those that carved nothing apart), then the pockets placed, removed and partly removed, the guard cells released, stale and missing, and the idle walks that lost rock to a guard.
- pocket_guard ([[task-537-layout-stale-pocket-guard]], commit c324273f): a cell is guarded (never carved by a walk or a disc, never a dead-window target) only while it lies within `pockets.guard` cells (square) of a cell still hidden. When region refill or passage repair puts hidden cells back to rock, the guard is rebuilt from the cells still hidden; the guard of live pockets, and of the remaining part of a partly removed one, stays. The Node script keeps the old stale guard ([[layout-generator-recipe]]).
- review_repairs ([[task-495-generator-window-repair]], commits c34eaacd, f7cfc4b7): after the cleanup loop, up to `review.passes` passes enforce [[layout-review-rules]], each followed by a passage repair and region refill (no filler rerun). Per action the worst void-free 24 × 24 window (stride 2) outside `repair_open_min`–`repair_open_max` (0.07–0.93, tighter than the accepted 0.05–0.95 so decoration cannot push it out) is repaired: a dead one by a filler walk from the nearest main-network cell into its deepest rock (carved cells that stay unreachable or narrow go back to rock); a too-open one by a rock outcrop of 1 to `outcrop_discs` noisy discs on a random cell with room, else a bump on one of its walls (rock is kept only if reach, the 3-cell rule and maxd hold). Then the longest straight lane over `lane_max` (open span 3–10 wide, walls within ±1) is broken at its middle or a third by a wall bump or a niche. Pocket distance is only checked: region refill already removes pockets with a gap over `max_gap`. A layout with nothing to repair is unchanged.
- data: every tunable value in the params JSON (the file of the Node generator; a missing key fails the load with its name). Heal and barrel counts there are those of [[task-491-generator-heal-barrel-plus20]]; repair and review values in its `review` section (ignored by the Node script).
- determinism: one `FRandomStream` initialised with the seed; value noise from a position hash mixed with `noise_seed`. Same params and seed give byte-identical JSON (checked on seed 3, two runs). Not identical to the Node output: the Node script fed `FRandomStream`'s LCG gives the same cells.
- depends_on: [[sys-grid]] (`ERRCellType`); module Json.
- implements: [[arena-grid]] (M2 deterministic generator).
- gotchas:
  - Rock islands use a full component flood; the Node script stops its flood after `max_rock_island` cells.
  - Seed acceptance adds three review checks (fails `window`, `lane`, `pocket distance`): every void-free window in 0.05–0.95, no lane over 30, no pocket farther than 15. On the 24 starts of task-492-03 all 24 pass (was 4 of 24 by hand review). Since the guard release (task-537) still 24 of 24, all validator OK; 17 keep the same accepted seed (2 with identical cells: 1004, 2600), 7 change; windows 0.062–0.929, lanes 15–29, farthest pocket 10; the 24 searches take 10.9 s (was 16.6 s).
  - Generation 0.1 to 0.5 s per accepted seed. Idle filler walks (carving nothing) were the stale guard of removed pockets: every seed has pocket cells put back to rock (the passage repair trims pocket edges). Before task-537, default seed 17 ran 5431 filler iterations, 4971 idle, 4968 of them losing rock only to a stale guard (arena_l3 seed 422704496: 4818, 4597 idle, 4494); after, 55 iterations, 1 idle (422704496: 60, 2), carve 0.81 → 0.19 s (0.72 → 0.18 s); seeds 1–300: 48000 → 8739 idle walks. Carve times before task-536: 4.9 s and 4.3 s.
  - `RunFiller` allocates its buffers once per call, rebuilds the distance field, far list and reachable list only after a walk that carved a cell, and picks the nearest reachable cell on squared integer distances with one square root: same random draws, byte-identical cells (checked on the reference search and seeds 3, 17, 1800, 2900). The pocket guard changes only in region refill and passage repair, outside `RunFiller`, so these caches stay valid.
  - The default params path (`GetDefaultParamsPath`, commandlet, generator test map, loader test) is under `Tools/`, outside packaged content; the run reads its own copies in `Content/RogueRigs/Data/Levels` ([[sys-run-layout]]).
  - Determinism reference: level seed 422704489 (arena_l3 params and brief) gives accepted seed 422704499 (11 tried), cells hash 34b074db, identical over two commandlet runs (task-537; was 86c36f6d before the guard release, checked in a run by [[task-496-01-generator-provider]]; not rechecked in a run since).

## Relations
- [[sys-grid]] — parent; the grid that builds the generated layout.
- [[layout-generator-recipe]] — the algorithm it ports.
- [[level-layout-schema]] — its output format and validator.
- [[layout-review-rules]] — review applied to its seeds.
- [[task-492-03-ld-validation]] — Level-Design verdict on 24 seeds.
- [[task-492-cpp-layout-generator]] — the task that built it.
- [[task-495-generator-window-repair]] — the task that added the review repairs and checks.
- [[sys-run-layout]] — the run provider that generates every run level with it.
- [[task-496-generator-in-runs]] — the task that wired it into the runs.
- [[task-536-layout-filler-cheaper]] — the task that made the filler cheaper with identical output, and its timings.
- [[task-537-layout-stale-pocket-guard]] — the task that releases the guard of removed pockets, with the before/after table of the 24 starts.
