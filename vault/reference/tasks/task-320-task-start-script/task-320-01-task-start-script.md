---
id: task-320-01-task-start-script
description: "Write vault/scripts/task-start.js (global and first open subtask in-progress, next_step, lint, commit) and name it in protocol step 5.1, agent-executor and runbook."
parent: task-320-task-start-script
kind: task
status: in-progress
---
# Write task-start.js

- goal: `node vault/scripts/task-start.js <global-id>` (no `.md`, relative to vault/reference/tasks). Behaviour (owner answers in the global `request`): refuses (Q7) unless the global has `confirmed_by_owner: yes`; picks the global `next_step` subtask if it names one that is not done, else the first subtask of `## Subtasks` whose status is `todo`; refuses when none is left. It sets, through `task-status.js` only (so index lines stay in sync and task-321 `started` applies): global `status=in-progress`, subtask `status=in-progress`, global `next_step=<subtask>`. Then `lint.js --write` and a commit of only the touched vault files by explicit path (message `<subtask>: in-progress`, the session Co-Authored-By line), as `task-next.js` does. Idempotent: already in-progress is not an error. Header comment with usage, like the other scripts. Then docs: `orchestration-protocol` step 5.1 says the Executor starts a global with `task-start.js` and later subtasks are started by `task-next.js`; the Roles paragraph "Executor step 0" adds the command; `agent-executor` and `orchestrator-runbook` (new line under Closing a subtask or a "Starting a task" item) name it. Do not edit `.claude/agents/executor.md` (owner Q9). Edit protocol step 5.1 (kind: rule): authorised by the owner, Q1.
- agent: cpp
- depends_on: none
- inputs: task-status.js (vault/scripts/task-status.js), task-next.js (vault/scripts/task-next.js), lint.js (vault/scripts/lint.js), orchestration-protocol (vault/reference/agents/orchestration-protocol.md), agent-executor (vault/reference/agents/agent-executor.md), orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), orchestrator-script-quirks (vault/reference/agents/orchestrator-script-quirks.md), _authoring (vault/_authoring.md)
- expected_output: new vault/scripts/task-start.js; updated orchestration-protocol.md, agent-executor.md, orchestrator-runbook.md
- done_when: on a throwaway copy (copy a todo global and its subtask under new ids in a scratch git worktree or revert afterwards, never on a live task), one run sets global and subtask `in-progress`, `next_step` to the subtask, the tasks-to-do-index line to `Status in-progress.`, lint clean and one commit of only those files; a second run changes nothing and exits 0; a global with `confirmed_by_owner: no` is refused with a message and no file changed; the three docs name the script; no live task node changed; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-320-task-start-script]] — parent global task.
