---
id: task-077-barks
description: "Global task: A4 Rook and Vesna barks: text agent writes, validator checks, critic reviews, corrections applied (2 loops at most)."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# A4 Rook and Vesna barks

- request: "Assignment 4, dynamic content pipeline" (plan validated by the owner 2026-10-10) (owner, 2026-10-10)
- goal: A4 Rook and Vesna barks: text agent writes, validator checks, critic reviews, corrections applied (2 loops at most).
- acceptance: validate_text.js exits 0; the file is in Content/RogueRigs/Data/Text/; retrieval log in Tools/content/logs/; every item reviewed by the critic with a verdict; failures corrected within 2 loops or reported to the owner; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-10).
- confirmed_by_owner: yes (2026-10-10)
- milestone: M2
- next_step: task-077-01-barks
- result: open

## Subtasks
1. task-077-01-barks — text
2. task-077-02-barks — style-guide

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-077-01-barks]] — step 1, text.
- [[task-077-02-barks]] — step 2, style-guide.
