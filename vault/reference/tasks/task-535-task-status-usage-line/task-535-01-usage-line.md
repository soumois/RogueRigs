---
id: task-535-01-usage-line
description: "Doc: Add the exact task-status.js usage line to task-spec."
parent: task-535-task-status-usage-line
kind: task
status: done
---
# task-spec : ligne d'usage de task-status.js: doc

- goal: Add the exact task-status.js usage line to task-spec.
- agent: doc
- depends_on: none
- inputs: task-spec (vault/reference/agents/task-spec.md), vault/scripts/task-status.js
- expected_output: see done_when
- done_when: A1 to A4 of the global hold.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-535
- usage: included in the executor run of task-535
- runs: 1
- milestone: M2
- started: 2026-11-13T18:27:41Z
- waiting_for: none
- result: task-spec Status section has one line with the exact task-status.js syntax and the wait-answers plus waiting_for example (commit 8f7787f2). Checked on a scratch copy of the script: global and subtask wait-answers write waiting_for and the index status; missing waiting_for, waiting_for without status=wait-answers, a vault/... path and an unknown field are refused; status=in-progress resets waiting_for to none. The rule Do not repeat those transitions stays. Lint clean.

## Relations
- [[task-535-task-status-usage-line]] — parent global task.
