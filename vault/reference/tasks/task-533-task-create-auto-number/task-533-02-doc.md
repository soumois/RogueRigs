---
id: task-533-02-doc
description: "Doc: Update task-spec and the runbook: num and anchor optional, no listing of vault/reference/tasks."
parent: task-533-task-create-auto-number
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-533-task-create-auto-number]] — parent global task.
