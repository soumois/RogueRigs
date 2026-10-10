---
id: task-078-alert-texts
description: "Global task: A4 Kessler-Voss alerts: text agent writes, validator checks, critic reviews, corrections applied (2 loops at most)."
parent: tasks-to-do-index
kind: task
status: todo
---
# A4 Kessler-Voss alerts

- request: "Assignment 4, dynamic content pipeline" (plan validated by the owner 2026-10-10) (owner, 2026-10-10)
- goal: A4 Kessler-Voss alerts: text agent writes, validator checks, critic reviews, corrections applied (2 loops at most).
- acceptance: validate_text.js exits 0; the file is in Content/RogueRigs/Data/Text/; retrieval log in Tools/content/logs/; every item reviewed by the critic with a verdict; failures corrected within 2 loops or reported to the owner; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-10).
- confirmed_by_owner: yes (2026-10-10)
- milestone: M2
- next_step: task-078-01-alert-texts
- result: open

## Subtasks
1. task-078-01-alert-texts — text
2. task-078-02-alert-texts — style-guide

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-078-01-alert-texts]] — step 1, text.
- [[task-078-02-alert-texts]] — step 2, style-guide.
