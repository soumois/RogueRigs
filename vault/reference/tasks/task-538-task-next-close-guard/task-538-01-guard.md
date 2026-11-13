---
id: task-538-01-guard
description: "Cpp: task-next.js runs every refusal check before its first write and resumes cleanly on rerun after a late failure"
parent: task-538-task-next-close-guard
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-538-task-next-close-guard]] — parent global task.
