---
id: task-323-01-wait-answers-status
description: "Allow status wait-answers with a waiting_for bullet in contract, lint, task-status.js, templates and protocol steps 4 and 5."
parent: task-323-wait-answers-status
kind: task
status: todo
---
# Allow the wait-answers status

- goal: (1) `_authoring` task status list and lint.js `TASK_STATUS` plus its message gain `wait-answers`. (2) `task-status.js`: `status=wait-answers` requires `waiting_for=<question or owner action>` (refuse without it) and writes `- waiting_for: <text>`, inserted before `- result:` when the node lacks it (same insertion as `started`, task-321); any other status on a node whose `waiting_for` is not `none` sets `waiting_for: none`. A waiting global stays in tasks-to-do-index with `Status wait-answers.`. (3) Templates `task.md`, `task-global.md` and both nodes of `task-create.js` gain `- waiting_for: none`. (4) `orchestration-protocol` (kind: rule, owner-authorised): step 4, the Planner sets each new global `wait-answers` with `waiting_for=owner confirmation of the plan` when it hands its questions back, and `todo` once confirmed; step 5, the Executor, when it stops for an owner action or answer, sets the current subtask and its global `wait-answers` with the action, and back to `in-progress` when it resumes (task-next.js and task-start.js reset the field through task-status.js). `agent-planner`, `agent-executor` and the runbook name the command. Do not edit `.claude/agents/*.md`. (5) Index texts: tasks-to-do-index description and intro list wait-answers. (6) `crew-board.js`: STATUS map gains `'wait-answers': 'En attente de réponse'`, a `.p-wait-answers` style, and `waiting_for` shown for that status.
- agent: cpp
- depends_on: task-320-01-task-start-script, task-321-01-started-field, task-322-01-cancelled-status
- inputs: _authoring (vault/_authoring.md), lint.js (vault/scripts/lint.js), task-status.js (vault/scripts/task-status.js), task-next.js (vault/scripts/task-next.js), task-start.js (vault/scripts/task-start.js, from task-320), task-create.js (vault/scripts/task-create.js), crew-board.js (vault/scripts/crew-board.js), template task (vault/templates/task.md), template task-global (vault/templates/task-global.md), orchestration-protocol (vault/reference/agents/orchestration-protocol.md), agent-planner (vault/reference/agents/agent-planner.md), agent-executor (vault/reference/agents/agent-executor.md), orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), tasks-to-do-index (vault/reference/tasks/tasks-to-do-index.md)
- expected_output: updated files above; no new script
- done_when: on throwaway copies (never a live task, reverted after): `status=wait-answers` without `waiting_for=` is refused with no file changed; with it, the node gets one `waiting_for` bullet before `result` and the global index line reads `Status wait-answers.` in tasks-to-do-index; `status=in-progress` (directly, via task-next.js and via task-start.js) and `status=todo` reset it to `none`; lint accepts `wait-answers` and still rejects an unknown status; task-create.js output lints clean with `waiting_for: none`; `node vault/scripts/crew-board.js "test"` runs; protocol steps 4 and 5 and the three agent docs name the command; live nodes untouched; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-323-wait-answers-status]] — parent global task.
