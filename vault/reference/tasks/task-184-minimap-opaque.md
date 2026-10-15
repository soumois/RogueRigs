---
id: task-184-minimap-opaque
description: "Global task: Make the minimap disc fully opaque: no part of the game shows through it, unseen cells included."
parent: tasks-done-index
kind: task
status: done
---
# Minimap: fully opaque

- request: Prépare aussi une tâche pour rendre le radar totalement opaque (owner, 2026-10-15)
- goal: Make the minimap disc fully opaque: no part of the game shows through it, unseen cells included.
- acceptance: M_Minimap compiles and its opacity path reads back 1 inside the circle; UnseenColor and the MinimapImage opacity read back; the HUD Blueprints compile and are saved by explicit path; docs updated; committed by path; the editor stays open; not seen on screen unless the owner checks; committed.
- gdd_check: readability-rules Minimap line read (2026-10-14): no opacity value, no conflict; open: colour of unseen cells once opaque (default black).
- confirmed_by_owner: yes (2026-10-15); unseen cells black (0, 0, 0)
- milestone: M2
- next_step: none
- result: Done. M_Minimap opacity = circle mask only (1 inside whatever texel alpha); colour black where the sample UV leaves the texture (added so the edge texel does not smear on an opaque disc); WBP_HUD MinimapConfig.UnseenColor (0,0,0,1); MinimapImage already opaque; compiled, read back; not seen on screen; commit c7d22556; validated on screen by the owner (2026-10-15)

## Subtasks
1. task-184-01-minimap-opaque — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-184-01-minimap-opaque]] — step 1, bp.
