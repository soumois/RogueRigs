---
id: task-152-chaser-static-integration
description: "Global task: Put the modelled chaser in the game as a static mesh (no animation yet) so the owner sees it in play."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Chaser: static in-game integration

- request: The owner wants to see the modelled chaser in game now (static, without the baked animation): import it, give it its four materials and replace the grey-box body of BP_Chaser (owner, 2026-10-13) (owner, 2026-10-14)
- goal: Put the modelled chaser in the game as a static mesh (no animation yet) so the owner sees it in play.
- acceptance: The mesh reads back with bounds x -12.4 to 32.5, y about plus or minus 43.3, z 0 to 123.0 and the four slots in order; the materials exist and read back; the assets saved and committed; nothing in BP_Chaser changed yet; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-152-02-chaser-blueprint-swap
- result: open

## Subtasks
1. task-152-01-chaser-static-integration — bp (import, materials)
2. task-152-02-chaser-blueprint-swap — bp (Blueprint swap)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-152-01-chaser-static-integration]] — step 1, bp.
- [[task-152-02-chaser-blueprint-swap]] — step 2, bp.
