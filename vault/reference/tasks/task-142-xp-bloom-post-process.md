---
id: task-142-xp-bloom-post-process
description: "Global task: Add a PostProcessVolume with explicit bloom settings to the arena level so the halo effect can be tuned and tested."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Arena: post process bloom volume

- request: If a post effect is needed for the halo, activate it so the owner can test (owner, 2026-10-13); bloom is already on at the engine default (intensity 0.675, threshold -1) with no PostProcessVolume in L_ArenaM1 (owner, 2026-10-13)
- goal: Add a PostProcessVolume with explicit bloom settings to the arena level so the halo effect can be tuned and tested.
- acceptance: The volume exists with the listed overrides read back, level saved and git shows it modified, doc written, lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: task-142-01-xp-bloom-post-process
- result: open

## Subtasks
1. task-142-01-xp-bloom-post-process — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-142-01-xp-bloom-post-process]] — step 1, bp.
