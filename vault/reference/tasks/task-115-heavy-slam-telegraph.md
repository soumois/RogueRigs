---
id: task-115-heavy-slam-telegraph
description: "Global task: The heavy slam shows its danger zone and a growing circle during the telegraph, and dies with the robot."
parent: tasks-done-index
kind: task
status: done
---
# Heavy slam ground signal

- request: "rapport de test du 2026-10-12" (owner); decisions confirmed 2026-10-12 (rules of the zones unchanged: only the rendering changes) (owner, 2026-10-13)
- goal: The heavy slam shows its danger zone and a growing circle on the ground during the telegraph, and the signal dies with the robot.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. ARRHeavy GroundTelegraph (Danger, slam radius from Stats.AttackRange = 289, progress = GetTelegraphProgress, hidden at slam/recover/death/pool release); 3 tests; 258/258. Commits 809054d, 37766be.

## Subtasks
1. task-115-01-heavy-slam-telegraph — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-115-01-heavy-slam-telegraph]] — step 1, cpp.
