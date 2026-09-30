---
id: run-log-schema
description: "Content of the structured run log (one file per run, one entry per level) that feeds the balance agent."
parent: pipelines-index
kind: doc
---
# Run log schema

One structured file per run, one entry per level. File format and field names: open.

Recorded per level:
- terrain destroyed by source (barrel, kamikaze, drone) and by type (rock, ore);
- damage dealt per weapon;
- kills by robot type and by source (weapon, explosion);
- upgrades picked (order and frequency);
- time per level;
- damage taken by source;
- XP collected;
- level outcome (death, extracted, missed shuttle);
- time left on reaching the shuttle;
- ore collected during the risk window;
- events met and their outcome.

GAS already writes to the Visual Logger; no custom GAS logging in M1 ([[gas-scope]]).

## Relations
- [[pipelines-index]] — parent index.
- [[destruction-rules]] — destruction sources.
