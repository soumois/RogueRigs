---
id: task-051-turret-spacing
description: "Global task: A turret placed less than 100 uu from a standing turret moves to the nearest free floor cell at least 100 uu away."
parent: tasks-index
kind: task
status: in-progress
---
# Turret spacing

- request: "Lorsqu’on pose deux tourelles au même endroit, elles se décalent pour ne pas être une sur l’autre" (owner, 2026-10-07)
- goal: A turret placed less than 100 uu from a standing turret moves to the nearest free floor cell at least 100 uu away.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-051-01-turret-spacing
- result: open

## Subtasks
1. task-051-01-turret-spacing — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-051-01-turret-spacing]] — step 1, cpp.
