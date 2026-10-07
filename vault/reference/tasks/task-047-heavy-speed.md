---
id: task-047-heavy-speed
description: "Global task: Heavy robot speed 180 → 153 uu/s; the other robots stay at 180."
parent: tasks-index
kind: task
status: todo
---
# Slower heavy

- request: "Ralentit quand même le gros de 15%." (owner, 2026-10-07)
- goal: Heavy robot speed 180 → 153 uu/s; the other robots stay at 180.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-047-01-heavy-speed
- result: open

## Subtasks
1. task-047-01-heavy-speed — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-047-01-heavy-speed]] — step 1, cpp.
