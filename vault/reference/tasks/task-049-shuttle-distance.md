---
id: task-049-shuttle-distance
description: "Global task: Shuttle lands 37 to 50 cells from the miner (was at least 75, no maximum)."
parent: tasks-index
kind: task
status: in-progress
---
# Closer shuttle

- request: "La sortie était beaucoup trop loin, il faut à minimum réduire la distance de 50%" (owner, 2026-10-07)
- goal: Shuttle lands 37 to 50 cells from the miner (was at least 75, no maximum).
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-049-01-shuttle-distance
- result: open

## Subtasks
1. task-049-01-shuttle-distance — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-049-01-shuttle-distance]] — step 1, cpp.
