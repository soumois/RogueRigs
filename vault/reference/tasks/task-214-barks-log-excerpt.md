---
id: task-214-barks-log-excerpt
description: "Global task: Barks retrieval log excerpt matches T2."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Barks retrieval log excerpt matches T2

- request: "fait le 1, puis le 4, puis 7,8,9." (point 8), then "le reste est ok" (owner, 2026-10-17)
- goal: the barks retrieval log quotes the current dialogue-tone T2 wording so the text validator exits 0 on barks.json again.
- acceptance: node Tools/content/validate_text.js on barks.json with its log exits 0; no bark text changed; committed.
- gdd_check: [[dialogue-tone]] T2 now reads "Jokes target the company or the robots (owner 2026-10-13)". Owner 2026-10-17 default accepted: the two excerpts "Jokes target the company." in log 20261010-103303-barks.json become "Jokes target the company" (still a verbatim substring).
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: task-214-01-excerpt-fix
- result: open

## Subtasks
1. task-214-01-excerpt-fix — text

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-214-01-excerpt-fix]] — step 1, text.
