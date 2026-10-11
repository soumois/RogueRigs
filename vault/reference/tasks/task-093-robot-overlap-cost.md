---
id: task-093-robot-overlap-cost
description: "Global task: Robot components audited, then unneeded attached-part transforms and overlap updates are removed, with before/after numbers."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot overlap and attached parts cost

- request: "pool de robots et étalement de l'initialisation (max 20 frames), pool des cubes d'XP (max 3 frames), une classe qui gère ça et généralisée, point 6 chevauchements" (owner 2026-10-10) (owner, 2026-10-11)
- goal: Robot components audited, then unneeded attached-part transforms and overlap updates are removed, with before/after numbers.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; audit note and before/after numbers recorded; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-11).
- confirmed_by_owner: yes (2026-10-11)
- milestone: M2
- next_step: task-093-01-robot-overlap-cost
- result: open

## Subtasks
1. task-093-01-robot-overlap-cost — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-093-01-robot-overlap-cost]] — step 1, cpp.
