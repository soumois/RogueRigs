---
id: task-120-console-rules
description: "Global task: Console hack zone radius 7 cells, every rock in the zone (ore included) destroyed before hacking, hack time 12 s."
parent: tasks-to-do-index
kind: task
status: todo
---
# Console zone and hack rules

- request: Valeurs de la console: 7 pour la distance, 12 s pour pirater, tous les rochers de la zone (minerai compris) à détruire (owner, 2026-10-13)
- goal: The console hack zone radius is 7 cells, every rock in the zone (ore included) must be destroyed before hacking, and the hack lasts 12 s.
- acceptance: build exits 0 with the editor closed; RogueRigs.Console.*, RunFlow.*, Extraction.*, Integration.ConsoleExtractionWin pass headless (expectations updated only where the new values change them); sys-objectives-console-chest updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-120-01-console-rules
- result: open

## Subtasks
1. task-120-01-console-rules — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-120-01-console-rules]] — step 1, cpp.
