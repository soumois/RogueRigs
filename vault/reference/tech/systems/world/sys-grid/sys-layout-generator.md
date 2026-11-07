---
id: sys-layout-generator
description: "C++ 240 x 240 layout generator as built: params file, FRandomStream determinism, seed acceptance, JSON output, RRLayoutGen commandlet. Open to run or change it."
parent: sys-grid
kind: doc
---
# Layout generator (C++)

- responsibility: generates a level layout from a seed and data params, following [[layout-generator-recipe]] "Direct 240 generation" (port of `Tools/layout_gen/arena_gen_fine.js`, [[task-492-cpp-layout-generator]], commits 85a2ee48 and 43c51ac8); writes it in the [[level-layout-schema]] format. No runtime AI.
- main_classes: `FRRLayoutGenerator` (`Source/RogueRigs/Grid/RRLayoutGenerator.h`), plain C++, one instance per thread; `FRRLayoutGenParams`, `FRRLayoutBrief` and their readers `RRLayoutGen::LoadParams` / `LoadBrief` (`RRLayoutGenParams.h`); `URRLayoutGenCommandlet` (`RRLayoutGenCommandlet.h`).
- entry_points:
  - `Carve(Seed)`: outline and notching, spawn clearing, main network, hidden pockets, far-cell filler, cleanup, 3-cell passage repair. `Decorate()`: heal veins and barrels. `Measure()`, `Check(Stats, Brief)`: recipe acceptance and brief bounds.
  - `Generate(FirstSeed, Brief)`: tries seeds upward from `FirstSeed` (`search.max_seeds`) and returns the first passing one (`Fails` empty) with its stats and times. `ToJson(Name, Layout)`: compact JSON, every non-floor cell row-major.
  - Command line, editor closed: `UnrealEditor-Cmd.exe RogueRigs.uproject -run=RRLayoutGen -seed=<n> [-params=<file>] [-name=<snake_case>] [-out=<dir>] [-brief=<file>] -unattended -nullrhi -nosplash -nop4`. Defaults: `Tools/layout_gen/params/fine_default.json`, name `arena_gen`, `Saved/GeneratedLevels/`. Writes `<name>.json`, plus `<name>.brief.json` (copy of the brief) with `-brief`; logs one line per tried seed, the accepted seed, the layout and search times; writes nothing and exits 1 when no seed passes. `-carveonly` carves twice and reports the hash, rerun identity and narrow cells.
- data: every tunable value in the params JSON (the file of the Node generator; a missing key fails the load with its name). Heal and barrel counts there are those of [[task-491-generator-heal-barrel-plus20]].
- determinism: one `FRandomStream` initialised with the seed; value noise from a position hash mixed with `noise_seed`. Same params and seed give byte-identical JSON (checked on seed 3, two runs). Not identical to the Node output: the Node script fed `FRandomStream`'s LCG gives the same cells.
- depends_on: [[sys-grid]] (`ERRCellType`); module Json.
- implements: [[arena-grid]] (M2 deterministic generator).
- gotchas:
  - Rock islands use a full component flood; the Node script stops its flood after `max_rock_island` cells.
  - Generation 0.1 to 0.6 s per seed, some seeds about 3 s; a search runs several seeds (about 1 in 3 passes the 240 brief).
  - The review rules of [[layout-review-rules]] are not automated: about 1 passing seed in 6 keeps every 24 × 24 window in 0.05–0.95 (verdict in [[task-492-03-ld-validation]]).
  - The default params path is under `Tools/`, outside packaged content.

## Relations
- [[sys-grid]] — parent; the grid that builds the generated layout.
- [[layout-generator-recipe]] — the algorithm it ports.
- [[level-layout-schema]] — its output format and validator.
- [[layout-review-rules]] — review applied to its seeds.
- [[task-492-03-ld-validation]] — Level-Design verdict on 24 seeds.
- [[task-492-cpp-layout-generator]] — the task that built it.
