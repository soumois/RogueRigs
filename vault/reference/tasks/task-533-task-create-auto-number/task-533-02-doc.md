---
id: task-533-02-doc
description: "Doc: Update task-spec and the runbook: num and anchor optional, no listing of vault/reference/tasks."
parent: task-533-task-create-auto-number
kind: task
status: done
---
# task-create.js : numéro et ancre automatiques: doc

- goal: Update task-spec and the runbook: num and anchor optional, no listing of vault/reference/tasks.
- agent: doc
- depends_on: task-533-01-auto-number-anchor
- inputs: task-spec (vault/reference/agents/task-spec.md), orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), orchestrator-script-quirks (vault/reference/agents/orchestrator-script-quirks.md)
- expected_output: see done_when
- done_when: A6 holds; no vault node still tells the planner to list vault/reference/tasks to number a task; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-533
- usage: included in the executor run of task-533
- runs: 1
- milestone: M2
- started: 2026-11-13T18:24:21Z
- waiting_for: none
- result: c2d3ef45: task-spec shows num and anchor optional (overrides only, num refused if taken), drops the 'list vault/reference/tasks' rule and points the planner at the printed created <global-id> lines; orchestrator-script-quirks: taken-number and parallel-session lines now say omit num instead of ls. Runbook unchanged (it defers plan writing to task-spec). No node under agents or _navigation still asks to list vault/reference/tasks; lint clean.

## Relations
- [[task-533-task-create-auto-number]] — parent global task.
