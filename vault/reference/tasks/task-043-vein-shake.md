---
id: task-043-vein-shake
description: "Global task: All blocks of a vein being mined shake together."
parent: tasks-index
kind: task
status: todo
---
# Whole-vein shake

- request: "Le bloc de santé se détruit bien d'un coup, mais les 9 blocs ne vibrent pas en symbiose" (owner, 2026-10-07)
- goal: All blocks of a vein being mined shake together.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-043-01-vein-shake
- result: open

## Subtasks
1. task-043-01-vein-shake — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-043-01-vein-shake]] — step 1, cpp.
