---
id: task-114-barrel-fuse
description: "Global task: Barrels have a doubled explosion radius and explode 1.5 s after reaching 0 health, with an orange danger zone and a growing circle."
parent: tasks-done-index
kind: task
status: done
---
# Barrel fuse and double radius

- request: "rapport de test du 2026-10-12" (owner); decisions confirmed 2026-10-12 (rules of the zones unchanged: only the rendering changes) (owner, 2026-10-13)
- goal: Barrels have a doubled explosion radius and explode 1.5 s after reaching 0 health, with an orange danger zone and a growing circle.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. Barrel radius 700 uu, 1.5 s fuse from 0 health (BarrelFuseSeconds), ground telegraph hosted by the arena grid (pooled), damage ignored during the fuse, chains add 1.5 s each; one 700 uu explosion costs about 8 ms (4.6-5.6 ms block destruction): proposal to spread it; 261/261 tests. Commits 459dda6, f044d39.

## Subtasks
1. task-114-01-barrel-fuse — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-114-01-barrel-fuse]] — step 1, cpp.
