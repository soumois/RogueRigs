---
id: task-184-minimap-opaque
description: "Global task: Make the minimap disc fully opaque: no part of the game shows through it, unseen cells included."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Minimap: fully opaque

- request: Prépare aussi une tâche pour rendre le radar totalement opaque (owner, 2026-10-15)
- goal: Make the minimap disc fully opaque: no part of the game shows through it, unseen cells included.
- acceptance: M_Minimap compiles and its opacity path reads back 1 inside the circle; UnseenColor and the MinimapImage opacity read back; the HUD Blueprints compile and are saved by explicit path; docs updated; committed by path; the editor stays open; not seen on screen unless the owner checks; committed.
- gdd_check: readability-rules Minimap line read (2026-10-14): no opacity value, no conflict; open: colour of unseen cells once opaque (default black).
- confirmed_by_owner: yes (2026-10-15); unseen cells black (0, 0, 0)
- milestone: M2
- next_step: task-184-01-minimap-opaque
- result: open

## Subtasks
1. task-184-01-minimap-opaque — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-184-01-minimap-opaque]] — step 1, bp.
