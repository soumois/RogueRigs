---
id: task-065-diagonal-dig
description: "Global task: Moving diagonally into a corner of two blocks still digs the nearest touching block(s)."
parent: tasks-index
kind: task
status: todo
---
# Diagonal dig fallback

- request: "je suis en train d’avancer en diagonale, je suis bloqué par deux blocs qui ne se minent pas. Qu’il ne les mine pas en même temps car l’angle est trop grand, je comprends, mais il devrait au moins en faire." (owner, 2026-10-07)
- goal: Moving diagonally into a corner of two blocks still digs the nearest touching block(s).
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-065-01-diagonal-dig
- result: open

## Subtasks
1. task-065-01-diagonal-dig — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-065-01-diagonal-dig]] — step 1, cpp.
