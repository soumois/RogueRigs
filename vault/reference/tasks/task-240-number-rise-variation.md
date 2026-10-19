---
id: task-240-number-rise-variation
description: "Global task: Each damage number rises with its own random speed (x0.7 to x1.3) and angle (up to 35 degrees left or right of screen up), so consecutive numbers no longer travel in parallel."
parent: tasks-to-do-index
kind: task
status: todo
---
# Damage number rise variation (C++ draw)

- request: Damage numbers video: vary the speed and the angle of the rise, numbers sometimes follow each other in parallel (point 10), defaults accepted (owner, 2026-10-19)
- goal: Each damage number rises with its own random speed (x0.7 to x1.3) and angle (up to 35 degrees left or right of screen up), so consecutive numbers no longer travel in parallel.
- acceptance: Digits of one number share one velocity; two numbers of one batch differ; the array has DigitCount entries; build passes; lint clean. The asset reads User.DigitVelocities and moves each digit with it (subtask 02); capture shows non-parallel numbers; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-19).
- confirmed_by_owner: yes (2026-10-19)
- milestone: M2
- next_step: task-240-01-number-rise-variation
- result: open

## Subtasks
1. task-240-01-number-rise-variation — cpp
2. task-240-02-number-rise-variation-asset — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-240-01-number-rise-variation]] — step 1, cpp.
- [[task-240-02-number-rise-variation-asset]] — step 2, bp.
