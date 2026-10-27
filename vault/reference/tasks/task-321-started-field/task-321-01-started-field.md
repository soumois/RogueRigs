---
id: task-321-01-started-field
description: "task-status.js writes or replaces a started date-time bullet on every status=in-progress; templates and task-create.js get started: open."
parent: task-321-started-field
kind: task
status: in-progress
---
# Write started on every move to in-progress

- goal: in `task-status.js`, when `status=in-progress` is passed, set `- started: <now>` on the node: replace the bullet if present, else insert it right before `- result:` (old nodes have no field; today the script throws "no field" for an unknown key, keep that for other keys). Applies to global tasks and subtasks (owner Q2) and at every move, overwriting the previous value (owner direction "at every move"). Format (owner Q3): UTC ISO 8601 to the second, `2026-10-26T23:35:54Z`. Other statuses leave `started` unchanged. Add `- started: <ISO date-time UTC, written by task-status.js on every move to in-progress>` to `vault/templates/task.md` and `task-global.md` (before `result`), and `- started: open` to both nodes written by `task-create.js`. Header comment of task-status.js mentions it. No lint rule (owner Q4). No back-fill of existing nodes.
- agent: cpp
- depends_on: none
- inputs: task-status.js (vault/scripts/task-status.js), task-next.js (vault/scripts/task-next.js), task-create.js (vault/scripts/task-create.js), template task (vault/templates/task.md), template task-global (vault/templates/task-global.md), orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), _authoring (vault/_authoring.md)
- expected_output: updated vault/scripts/task-status.js, task-create.js, vault/templates/task.md, task-global.md; runbook line under Creating tasks or Closing a subtask naming the field
- done_when: on throwaway copies (never a live task): `status=in-progress` on a node without the field inserts one `started` bullet before `result` in the agreed format; a second call replaces it (one bullet, new time); `status=done` keeps it; on a global the index line still syncs; `task-next.js` used on a throwaway pair writes `started` on the next subtask; task-create.js output lints clean and carries `started: open`; live nodes untouched; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-321-started-field]] — parent global task.
