---
id: task-175-alert-objectives-node-cleanup
description: "Global task: Delete the about 45 empty, unconnected custom event nodes left in the WBP_AlertObjectives event graph by failed graph writes of task-168-07."
parent: tasks-to-do-index
kind: task
status: todo
---
# WBP_AlertObjectives: unused node cleanup

- request: Prévoit une tâche pour clean les nodes inutiles. To run after the owner adds mcp__unreal__call_tool to .claude/settings.local.json and restarts the app (delete_node was refused by the permission classifier in task-168-07) (owner, 2026-10-15)
- goal: Delete the about 45 empty, unconnected custom event nodes left in the WBP_AlertObjectives event graph by failed graph writes of task-168-07.
- acceptance: Every deleted node had no connected pin (listed in the report); compiles with warnings as errors; saved; graph read back; doc updated; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-15).
- confirmed_by_owner: yes (2026-10-15)
- milestone: M2
- next_step: task-175-01-alert-objectives-node-cleanup
- result: open

## Subtasks
1. task-175-01-alert-objectives-node-cleanup — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-175-01-alert-objectives-node-cleanup]] — step 1, bp.
