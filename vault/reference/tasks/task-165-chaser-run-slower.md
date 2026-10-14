---
id: task-165-chaser-run-slower
description: "Global task: Slow the chaser run animation by 30 percent (playback rate 0.7), as a tunable material parameter."
parent: tasks-to-do-index
kind: task
status: todo
---
# Chaser: slower run animation

- request: The owner finds the run animation too fast and asks to try 30 percent slower (owner, 2026-10-14); the attack clip is a gameplay telegraph (0.1 s wind-up) and stays at its speed unless the owner asks otherwise (owner, 2026-10-14)
- goal: Slow the chaser run animation by 30 percent (playback rate 0.7), as a tunable material parameter.
- acceptance: The material recompiles without error; the code and the new input read back; the parameter default 0.7 read back; saved and committed by name; docs updated; the editor stays open; the owner is told how to tune it (RunRate in the material) and that the attack clip is unchanged; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-165-01-chaser-run-slower
- result: open

## Subtasks
1. task-165-01-chaser-run-slower — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-165-01-chaser-run-slower]] — step 1, bp.
