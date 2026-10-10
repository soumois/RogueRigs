---
id: task-084-card-flavor-fix
description: "Global task: The upgrade card flavor line is visible in game: the newline delimiter bug in WBP_HUD GetCardFlavor is fixed."
parent: tasks-done-index
kind: task
status: done
---
# Card flavor line fix

- request: "tu peux lancer le reste des tâches pour le devoir 4" (owner playtest 2026-10-10: shuttle line "Cease all extraction" misread, card flavor line not visible) (owner, 2026-10-10)
- goal: The upgrade card flavor line is visible in game: the newline delimiter bug in WBP_HUD GetCardFlavor is fixed.
- acceptance: Delimiter pin reads back as one real newline character; compile 0 errors; asset saved; wbp-hud and unreal-mcp-quirks updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-10).
- confirmed_by_owner: yes (2026-10-10)
- milestone: M2
- next_step: none
- result: Card flavor delimiter fixed (d24c512); owner confirms on screen.

## Subtasks
1. task-084-01-card-flavor-fix — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-084-01-card-flavor-fix]] — step 1, bp.
