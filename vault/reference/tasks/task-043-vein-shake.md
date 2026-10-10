---
id: task-043-vein-shake
description: "Global task: All blocks of a vein being mined shake together."
parent: tasks-done-index
kind: task
status: done
---
# Whole-vein shake

- request: "Le bloc de santé se détruit bien d'un coup, mais les 9 blocs ne vibrent pas en symbiose" (owner, 2026-10-07)
- goal: All blocks of a vein being mined shake together.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. ARRDrone shakes every block of the dug veins (ShakeCells, RefreshDigShake, CollectDigTargets); laser and dig events still arc-only; stops on release or break. Build 0, 195/195. Commit 0d8b2d9.

## Subtasks
1. task-043-01-vein-shake — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-043-01-vein-shake]] — step 1, cpp.
