---
id: task-118-telegraph-fill
description: "Global task: The ground telegraph draws the growing circle as a transparent filled disc (inside of the zone transparent) for console, chest, barrel and heavy."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Ground telegraph fill

- request: Remplissage du cercle de progression (retours 113 et 115): intérieur de la zone transparent, disque qui grandit rempli d'une couleur transparente (owner, 2026-10-13)
- goal: The ground telegraph draws the growing circle as a transparent filled disc (inside of the zone transparent) for console, chest, barrel and heavy.
- acceptance: material compiles with no error in the editor log; parameters read back (fill opacity per mode); sys-ground-telegraph and ground-telegraph docs match; lint clean; no C++ change unless a parameter is missing (report it instead); committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-118-01-telegraph-fill
- result: open

## Subtasks
1. task-118-01-telegraph-fill — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-118-01-telegraph-fill]] — step 1, bp.
