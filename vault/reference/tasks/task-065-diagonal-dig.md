---
id: task-065-diagonal-dig
description: "Global task: Moving diagonally into a corner of two blocks still digs the nearest touching block(s)."
parent: tasks-done-index
kind: task
status: done
---
# Diagonal dig fallback

- request: "je suis en train d’avancer en diagonale, je suis bloqué par deux blocs qui ne se minent pas. Qu’il ne les mine pas en même temps car l’angle est trop grand, je comprends, mais il devrait au moins en faire." (owner, 2026-10-07)
- goal: Moving diagonally into a corner of two blocks still digs the nearest touching block(s).
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. Cause: UpdateDigTrigger let any move input pointing slightly toward the wall steer the arc up to 90° off the wall, outside the ±60° cone, so a diagonal corner gave an empty arc and the dig released (reproduced in a script sweep). Fix: the miner passes its touching cells; RequestArcDig overload falls back to the touching block(s) closest to the arc direction (ties kept, capped at MaxArcBlocks) when the arc is empty. No test changed. Build 0; full run 193/195 (known two). Commit e5290cc.

## Subtasks
1. task-065-01-diagonal-dig — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-065-01-diagonal-dig]] — step 1, cpp.
