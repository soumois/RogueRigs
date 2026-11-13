---
id: task-533-01-auto-number-anchor
description: "Cpp: Make task-create.js compute the next task number and the index anchor when the spec omits them."
parent: task-533-task-create-auto-number
kind: task
status: done
---
# task-create.js : numéro et ancre automatiques: cpp

- goal: Make task-create.js compute the next task number and the index anchor when the spec omits them.
- agent: cpp
- depends_on: none
- inputs: task-spec (vault/reference/agents/task-spec.md), vault/scripts/task-create.js
- expected_output: see done_when
- done_when: A1 to A5 and A7 of the global hold, shown in the subtask report by a scratch run whose created files were removed afterwards.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-533
- usage: included in the executor run of task-533
- runs: 1
- milestone: M2
- started: 2026-11-13T18:21:48Z
- waiting_for: none
- result: task-create.js (6e2450ae): a global without num gets highest task-NNN in vault/reference/tasks (and explicit spec nums) + 1, in global order; without anchor the index lines go after the last lower-numbered task line, before the first task line if none is lower, at end of file when the index has no task line; explicit anchor unchanged; explicit num already taken refused, nothing written. Scratch run on a git copy of the vault (Saved scratchpad, removed): A1 two globals got 537, 538 with consecutive subtasks; A2 placed after 536, at end with empty index, before task-999 when only a higher line; A3 num 533 refused, num 900 + anchor placed after anchor; A4 created <id> lines printed; A5 task-reglage --no-commit created task-537 and task-confirm confirmed a wait-answers 537 spec; lint clean. Real vault untouched.

## Relations
- [[task-533-task-create-auto-number]] — parent global task.
