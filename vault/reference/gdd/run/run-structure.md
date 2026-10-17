---
id: run-structure
description: "A run is 4 levels back to back with upgrades carried over; defines run win (extract at level 4) and loss (death or missed shuttle)."
parent: run-index
kind: spec
status: approved
---
# Run structure

- A run is 4 levels. Between two levels, the [[intermission]] screen shows a recap and the [[run-shop]] (owner 2026-10-17; was "nothing in between").
- Carried into the next level (provisional, owner 2026-10-17): upgrade cards taken, weapons, miner level and XP, current health, ore. Nothing carries between runs in the core scope.
- Win: reach the extraction shuttle at the end of level 4.
- Lose: the miner dies, or misses the shuttle, on any level.
- Objective of each level: the console on all 4 levels for now (provisional, owner 2026-10-17; variants in [[objective-variants]]).
- Lose on any level: run-lost screen; restarting starts a new run at level 1 (provisional, owner 2026-10-17). Win at level 4: run-won screen.
- Difficulty ramp between levels: open (future balance task; for now every level uses the same tunables, owner 2026-10-17).
- Milestone: M2 (M1 has a single fixed arena).

## Relations
- [[run-index]] — parent index.
- [[level-flow]] — what happens inside one level.
- [[extraction]] — how a level ends.
- [[ore-economy]] — what a failed run costs.
- [[intermission]] — the screen between two levels.
- [[run-shop]] — run-only purchases between levels.
