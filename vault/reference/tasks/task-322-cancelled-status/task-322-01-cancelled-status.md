---
id: task-322-01-cancelled-status
description: "Allow status cancelled: _authoring, lint.js, task-status.js moves cancelled globals out of tasks-to-do-index, index texts, crew-board.js label."
parent: task-322-cancelled-status
kind: task
status: todo
---
# Allow the cancelled status

- goal: (1) `_authoring` task status list and lint.js `TASK_STATUS` plus its message gain `cancelled` (spelling `cancelled` only). (2) `task-status.js`: a global set to `cancelled` leaves tasks-to-do-index like `done` does; destination default (Q5): tasks-done-index, line ending `Status cancelled.`; leaving cancelled moves it back, as for done. Default (Q6): cancelling a global also sets each of its subtasks that is not `done` to `cancelled` and the global `next_step` to `none`; the tokens-open refusal stays for `done` only. (3) Index texts: tasks-done-index description and intro say finished or cancelled; tasks-to-do-index intro sentence and the tasks-index gloss of tasks-done-index match. (4) `crew-board.js` STATUS map gains `cancelled: 'Annulé'` and a `.p-cancelled` style, and shows `result` for cancelled steps as for done. No template change (they hold `status: todo` only). Do not touch Tools/crew-board.
- agent: cpp
- depends_on: none
- inputs: _authoring (vault/_authoring.md), lint.js (vault/scripts/lint.js), task-status.js (vault/scripts/task-status.js), task-next.js (vault/scripts/task-next.js), crew-board.js (vault/scripts/crew-board.js), tasks-index (vault/reference/tasks/tasks-index.md), tasks-to-do-index (vault/reference/tasks/tasks-to-do-index.md), tasks-done-index (vault/reference/tasks/tasks-done-index.md), orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), dashboard mapping read only (Tools/crew-board/hooks/register.tsx, `norm`, lines ~53-70)
- expected_output: updated vault/_authoring.md, vault/scripts/lint.js, task-status.js, crew-board.js, the three tasks index nodes; runbook line naming `status=cancelled`
- done_when: on a throwaway global with two subtasks (one done, one todo; never a live task, reverted after): `task-status.js <global>.md status=cancelled` moves its line to the agreed index with `Status cancelled.`, updates its parent field and parent link, sets the todo subtask cancelled and leaves the done one, sets `next_step: none`; lint accepts `cancelled` and still rejects an unknown status; `node vault/scripts/crew-board.js "test"` runs; `norm('cancelled')` in register.tsx still returns `cancel` (read, unchanged); live nodes untouched; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-322-cancelled-status]] — parent global task.
