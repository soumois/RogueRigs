---
id: sys-layout-generator
description: "C++ 240 x 240 layout generator as built: params, determinism, review repair passes, seed acceptance, JSON output, RRLayoutGen commandlet. Open to run or change it."
parent: sys-grid
kind: doc
---
# Layout generator (C++)

- responsibility: generates a level layout from a seed and data params, following [[layout-generator-recipe]] "Direct 240 generation" (port of `Tools/layout_gen/arena_gen_fine.js`, [[task-492-cpp-layout-generator]], commits 85a2ee48 and 43c51ac8); writes it in the [[level-layout-schema]] format. No runtime AI.
- main_classes: `FRRLayoutGenerator` (`Source/RogueRigs/Grid/RRLayoutGenerator.h`), plain C++, one instance per thread; `FRRLayoutGenParams`, `FRRLayoutBrief` and their readers `RRLayoutGen::LoadParams` / `LoadBrief` (`RRLayoutGenParams.h`); `URRLayoutGenCommandlet` (`RRLayoutGenCommandlet.h`).
- entry_points:
  - `Carve(Seed)`: outline and notching, spawn clearing, main network, hidden pockets, far-cell filler, cleanup, review repair passes, 3-cell passage repair. `Decorate()`: heal veins and barrels. `Measure()`, `Check(Stats, Brief)`: recipe acceptance and brief bounds.
  - `Generate(FirstSeed, Brief)`: tries seeds upward from `FirstSeed` (`search.max_seeds`) and returns the first passing one (`Fails` empty) with its stats and times. `ToJson(Name, Layout)`: compact JSON, every non-floor cell row-major.
  - Command line, editor closed: `UnrealEditor-Cmd.exe RogueRigs.uproject -run=RRLayoutGen -seed=<n> [-params=<file>] [-name=<snake_case>] [-out=<dir>] [-brief=<file>] -unattended -nullrhi -nosplash -nop4`. Defaults: `Tools/layout_gen/params/fine_default.json`, name `arena_gen`, `Saved/GeneratedLevels/`. Writes `<name>.json`, plus `<name>.brief.json` (copy of the brief) with `-brief`; logs one line per tried seed (with window min-max, longest lane, pocket distance, repair counts), the accepted seed, its review measures, the layout and search times; writes nothing and exits 1 when no seed passes. `-carveonly` carves twice and reports the hash, rerun identity and narrow cells.
- review_repairs ([[task-495-generator-window-repair]], commits c34eaacd, f7cfc4b7): after the cleanup loop, up to `review.passes` passes enforce [[layout-review-rules]], each followed by a passage repair and region refill (no filler rerun). Per action the worst void-free 24 × 24 window (stride 2) outside `repair_open_min`–`repair_open_max` (0.07–0.93, tighter than the accepted 0.05–0.95 so decoration cannot push it out) is repaired: a dead one by a filler walk from the nearest main-network cell into its deepest rock (carved cells that stay unreachable or narrow go back to rock); a too-open one by a rock outcrop of 1 to `outcrop_discs` noisy discs on a random cell with room, else a bump on one of its walls (rock is kept only if reach, the 3-cell rule and maxd hold). Then the longest straight lane over `lane_max` (open span 3–10 wide, walls within ±1) is broken at its middle or a third by a wall bump or a niche. Pocket distance is only checked: region refill already removes pockets with a gap over `max_gap`. A layout with nothing to repair is unchanged.
- data: every tunable value in the params JSON (the file of the Node generator; a missing key fails the load with its name). Heal and barrel counts there are those of [[task-491-generator-heal-barrel-plus20]]; repair and review values in its `review` section (ignored by the Node script).
- determinism: one `FRandomStream` initialised with the seed; value noise from a position hash mixed with `noise_seed`. Same params and seed give byte-identical JSON (checked on seed 3, two runs). Not identical to the Node output: the Node script fed `FRandomStream`'s LCG gives the same cells.
- depends_on: [[sys-grid]] (`ERRCellType`); module Json.
- implements: [[arena-grid]] (M2 deterministic generator).
- gotchas:
  - Rock islands use a full component flood; the Node script stops its flood after `max_rock_island` cells.
  - Seed acceptance adds three review checks (fails `window`, `lane`, `pocket distance`): every void-free window in 0.05–0.95, no lane over 30, no pocket farther than 15. On the 24 starts of task-492-03 all 24 pass (was 4 of 24 by hand review), 22 on the same first seed; carving raises the reachable share, so a seed near reach 0.86 can now fail (1800 and 2900 moved).
  - Generation 0.1 to 0.5 s per accepted seed, mean 3.5 seeds per search; seeds whose far cells the filler cannot reach take about 6 s, with or without repairs.
  - The default params path is under `Tools/`, outside packaged content.

## Relations
- [[sys-grid]] — parent; the grid that builds the generated layout.
- [[layout-generator-recipe]] — the algorithm it ports.
- [[level-layout-schema]] — its output format and validator.
- [[layout-review-rules]] — review applied to its seeds.
- [[task-492-03-ld-validation]] — Level-Design verdict on 24 seeds.
- [[task-492-cpp-layout-generator]] — the task that built it.
- [[task-495-generator-window-repair]] — the task that added the review repairs and checks.
