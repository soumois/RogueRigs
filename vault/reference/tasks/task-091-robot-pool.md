---
id: task-091-robot-pool
description: "Global task: robots come from the pool, missing ones created over at most 20 frames, wave spikes measured before and after."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robots from the actor pool

- request: "pool de robots et étalement de l'initialisation (max 20 frames), pool des cubes d'XP (max 3 frames), une classe qui gère ça et généralisée, point 6 chevauchements" (owner 2026-10-10) (owner, 2026-10-11)
- goal: Robots come from the pool: reuse on spawn and death, missing ones created over at most 20 frames; wave spawn spikes measured before and after.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; before/after numbers in stress-grid-cells; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-11).
- confirmed_by_owner: yes (2026-10-11)
- milestone: M2
- next_step: task-091-01-robot-pool
- result: open

## Subtasks
1. task-091-01-robot-pool — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-091-01-robot-pool]] — step 1, cpp.
