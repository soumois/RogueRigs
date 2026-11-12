---
id: task-508-01-rules-scripts-claude-files
description: "Executor: remove the three points from rule nodes, scripts and Claude files, keep the six others, commit the style-loop fix"
parent: task-508-workflow-back-to-old
kind: task
status: in-progress
---
# Workflow : retour à l'ancien, en gardant routeur réglage, porte visuelle et outils: executor

- goal: remove the three points from rule nodes, scripts and Claude files, keep the six others, commit the style-loop fix
- agent: executor
- depends_on: none
- inputs: request-families (vault/reference/agents/request-families.md), orchestration-protocol (vault/reference/agents/orchestration-protocol.md), execution-protocol (vault/reference/agents/execution-protocol.md), agent-planner (vault/reference/agents/agent-planner.md), agent-executor (vault/reference/agents/agent-executor.md), verification-levels (vault/reference/agents/verification-levels.md), task-spec (vault/reference/agents/task-spec.md), orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), task-500-request-families (vault/reference/tasks/task-500-request-families.md), task-501-visual-preview-gate (vault/reference/tasks/task-501-visual-preview-gate.md), task-502-executor-handover-budget (vault/reference/tasks/task-502-executor-handover-budget.md), task-503-executor-reading-list (vault/reference/tasks/task-503-executor-reading-list.md), task-507-budget-no-block (vault/reference/tasks/task-507-budget-no-block.md), CLAUDE.md, .claude/commands/task.md, .claude/agents/, .claude/commands/style-loop.md, vault/scripts/task-next.js
- expected_output: see done_when
- done_when: A1 to A6 of the global hold; lint clean; committed by explicit path.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-12T12:23:45Z
- waiting_for: none
- result: open

## Relations
- [[task-508-workflow-back-to-old]] — parent global task.
