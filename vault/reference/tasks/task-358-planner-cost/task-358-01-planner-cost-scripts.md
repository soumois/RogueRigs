---
id: task-358-01-planner-cost-scripts
description: "Cpp: vault scripts path-of.js (exact id resolver), task-confirm.js (apply an all-defaults answer) and task-create.js spec v2 (whole plan in one call)."
parent: task-358-planner-cost
kind: task
status: in-progress
---
# Planner cost: scripts

- goal: Write `vault/scripts/path-of.js`, `vault/scripts/task-confirm.js` and extend `vault/scripts/task-create.js` as decided in the global `## Questions` Q1 to Q3 (read the global `request` and answers first). Node 18+, no dependencies, same style and quiet output as task-start.js (only action lines). No doc node edits: the doc subtask writes them.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), global task-358-planner-cost (vault/reference/tasks/task-358-planner-cost.md), `vault/scripts/task-create.js`, `vault/scripts/task-status.js`, `vault/scripts/task-start.js`, `vault/scripts/lint.js`, `vault/templates/task-global.md`, `vault/templates/task.md`, orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), orchestrator-script-quirks (vault/reference/agents/orchestrator-script-quirks.md), task-355-02-shop-card-frame-details-capture (vault/reference/tasks/task-355-shop-card-frame-details/task-355-02-shop-card-frame-details-capture.md) (a hand-written subtask the spec v2 must be able to express)
- expected_output: the three scripts, committed by path; change summary: usage lines, spec v2 format with one example, defaults file format, dry-run outputs.
- done_when: path-of.js resolves `agent-cpp`, `sys-cheats`, `task-355-shop-card-frame-details` and `task-355-02-shop-card-frame-details-capture` and exits 1 on `sys-chea`; task-create.js run on a scratch copy of the vault (or with a dry-run flag) writes a global with three subtasks of different agents that passes lint, and the old array form still runs; task-confirm.js on a scratch copy marks Questions decided, appends the words, sets confirmed_by_owner and status todo, applies one gdd replacement and refuses a replacement whose old text is missing; no real task node created; committed.
- editor_state: any
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-29T14:09:28Z
- waiting_for: none
- result: open

## Relations
- [[task-358-planner-cost]] — parent global task.
