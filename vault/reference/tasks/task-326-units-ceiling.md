---
id: task-326-units-ceiling
description: "Global task: token_ceiling bounds the weighted units figure of record-cost.js, written in M units, so the over-ceiling alert flags real outliers."
parent: tasks-to-do-index
kind: task
status: todo
---
# token_ceiling measured in units

- request: relayed by the main session (owner, in French, 2026-10-27): "Fix the `token_ceiling` mechanism: the field in task nodes (e.g. 80000 on task-324-02) is exceeded ~48x by the raw total (input + cache write + cache read + output) and so signals nothing. Decide, from how the executor/orchestrator currently records tokens and `usage`, which measure the ceiling should apply to (likely the weighted `units` figure, or output + cache write), and make the definitions and task template consistent with that." Scope: crew definitions only, no game content, no C++. Owner answer (2026-10-27): "ok", every default Q5 to Q8 below accepted and the plan confirmed.
- goal: one measure for `token_ceiling` everywhere (scripts, templates, agent nodes, cost-log, runbook, executor), calibrated so the ALERT fires on the dearest ~10% of subtasks.
- acceptance: see the subtask `done_when`; committed.
- gdd_check: dev tooling, no gdd node needed (Planner, 2026-10-27). Findings: `record-cost.js` compares `input + output` to the ceiling (task-324-02: 30k vs 80k, no alert; cpp p90 72k, so it almost never fires), not the raw total; `usage` already carries `units` (weights in `vault/scripts/cost-rates.json`: input 1, cache write 1.25, cache read 0.1, output 5); the Executor of task-323 quoted a third figure (Agent tool total 72842 vs 40000). Output + cache write ignores cache read, the very cost of long contexts (the docs turns of 324-02). Measured with `agent-usage.js` (units M, p50 / p90): cpp 0.66 / 1.34, bp 0.60 / 2.20, 3d 0.61 / 1.25, art 0.08 / 0.11, text+style-guide 0.22, planning passes 0.49 / 0.95, executors 0.26 / 1.21. Decided (owner "ok" to the defaults, 2026-10-27, provisional): Q5 measure = `units`, field written `token_ceiling: <x> M units` (e.g. `1.2 M units`), alert when units > ceiling; field name unchanged. Q6 per-agent defaults (provisional, owner 2026-10-27): cpp 1.2, bp 1.0, 3d 1.2, art 0.15, text 0.3, style-guide 0.3, level-design 0.5 (no data), doc 0.3, planner 1.0 per planning pass; executor stays `open`. Q7 legacy values: done task nodes keep their old integers and `record-cost.js` gives no ceiling alert on a value without `M units`; the 13 todo subtasks of tasks 308, 309 and 314 get their agent's default in units. Q8 this needs a Node script change (`record-cost.js`, comments of `task-create.js` and `cost-rates.json`): a cpp subtask, outside "definitions only" strictly; then the doc agent updates the definitions.
- confirmed_by_owner: yes (2026-10-27)
- milestone: M2
- next_step: task-326-01-units-ceiling-script
- started: open
- waiting_for: none
- result: open

Run order: after task-325 is done (326-02 is run by the doc agent).

## Subtasks
1. task-326-01-units-ceiling-script — cpp, editor any: record-cost.js compares units to a `M units` ceiling; script comments; change summary.
2. task-326-02-units-ceiling-docs — doc, editor any: templates, agent nodes, cost-log, runbook, executor, todo subtasks of 308, 309, 314.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-326-01-units-ceiling-script]] — step 1, cpp.
- [[task-326-02-units-ceiling-docs]] — step 2, doc.
- [[task-325-doc-agent]] — sibling owner point, run before; creates the doc agent.
