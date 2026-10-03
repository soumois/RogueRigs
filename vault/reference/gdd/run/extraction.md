---
id: extraction
description: "End of a level: shuttle lands far from the miner, countdown to reinforcements, risk window choice, consequences of missing the shuttle."
parent: run-index
kind: spec
status: approved
---
# Extraction

- Triggered when the alert bar is full and no mandatory objective is pending ([[level-flow]]).
- The shuttle lands far from the miner. A countdown starts before company reinforcements arrive.
- **Risk window:** between landing and the end of the countdown, the player chooses to keep collecting ore and XP, or to run for the shuttle.
- Reaching the shuttle ends the level; ore carried is secured ([[ore-economy]]).
- Missing the shuttle loses the run and the ore carried.
- Values (provisional, owner 2026-10-03): lands at least 25 cells from the miner, on a reachable cell; countdown 45 s.
- M1 (single level): reaching the shuttle shows a success screen, missing it shows a run-lost screen; both offer a button that restarts the map.
- Milestone: M1.

## Relations
- [[run-index]] — parent index.
- [[run-structure]] — run win and loss.
- [[lore]] — the shuttle and the reinforcements.
