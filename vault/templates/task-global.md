---
id: task-<number>-<slug>
description: "<one sentence: the owner's request as a goal>"
parent: tasks-index
kind: task
status: todo
---
# <Global task title>

- request: <the owner's words, verbatim>
- goal: <restated outcome>
- acceptance: <observable checks that prove the goal is met>
- gdd_check: <nodes read; conflicts and open values found; owner's decision>
- confirmed_by_owner: yes | no
- milestone: M1 | M2 | M3
- next_step: <id of the next subtask to run, or none>
- started: <ISO date-time UTC, written by task-status.js on every move to in-progress>
- waiting_for: none | <question or owner action awaited while status is wait-answers; written by task-status.js, reset to none on the next status>
- result: <filled when done>

## Subtasks
<ordered list, one line per subtask: `1. task-NNN-01-slug (vault/reference/tasks/task-NNN-slug/task-NNN-01-slug.md) — <agent>: <one-line goal>`; same subtasks as Relations>

## Relations
- [[tasks-index]] — parent index.
