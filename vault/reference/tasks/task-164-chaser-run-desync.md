---
id: task-164-chaser-run-desync
description: "Global task: Make the chasers run out of step: no two chasers should show the same run pose at the same time."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Chaser: runs not synchronised

- request: In game all chasers run in step with each other (owner, 2026-10-14), although ARRChaser is meant to start each run clip at a random offset; the cause is not known (the offset may be missing on some spawn path, too small, or lost) (owner, 2026-10-14)
- goal: Make the chasers run out of step: no two chasers should show the same run pose at the same time.
- acceptance: The build passes; the cause is stated in the report with the line numbers; the game shots of simultaneous spawns show chasers in different run poses (or, if not, what remains); the doc updated; committed by name; the editor stays closed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-164-01-chaser-run-desync
- result: open

## Subtasks
1. task-164-01-chaser-run-desync — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-164-01-chaser-run-desync]] — step 1, cpp.
