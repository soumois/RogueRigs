---
id: task-538-01-guard
description: "Cpp: task-next.js runs every refusal check before its first write and resumes cleanly on rerun after a late failure"
parent: task-538-task-next-close-guard
kind: task
status: done
---
# task-next.js refuse avant d'écrire: cpp

- goal: task-next.js runs every refusal check before its first write and resumes cleanly on rerun after a late failure
- agent: cpp
- depends_on: none
- inputs: vault/scripts/task-next.js, orchestrator-script-quirks (vault/reference/agents/orchestrator-script-quirks.md), cost-log (vault/reference/production/cost-log.md)
- expected_output: see done_when
- done_when: A1 to A6 of the global hold, each checked on a throwaway task by comparing git status and the cost log before and after; the throwaway task is removed and only the script change is committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-538
- usage: included in the executor run of task-538
- runs: 1
- milestone: M2
- started: 2026-11-13T19:08:23Z
- waiting_for: none
- result: task-next.js (commit 43bbb300) now runs every refusal check before its first write: missing next-global/next-subtask while the global keeps an open subtask (prints the command with the next open subtask), a double-bracket link to an unknown id in the result text (named), the task-status closing gate (other subtask not done, unrecorded fields, no cost row), missing nodes, fields, index lines or an unknown or finished next subtask. Each write is skipped when already applied, so a rerun after a late failure (lint problem elsewhere, hook or commit refused) finishes the close with one cost row and unchanged started stamps; a rerun after success prints already closed. Checked on a throwaway task-999 in a sparse clone in Saved-free scratch with a stub agent-usage.js: A1, A2, A3 refusals left HEAD, status, diff and cost log identical; A4 forced pre-commit failure then rerun: one commit, one row; lint failure on another node then rerun: global closed, one row; A5 normal close of 01 then 02 closed the global and moved it to tasks-done-index; A6 the clone was deleted, no task-999 trace in the repo. Not in scope, seen: task-status.js on a CRLF working copy leaves a blank line in tasks-to-do-index when it moves a line (pre-existing).

## Relations
- [[task-538-task-next-close-guard]] — parent global task.
