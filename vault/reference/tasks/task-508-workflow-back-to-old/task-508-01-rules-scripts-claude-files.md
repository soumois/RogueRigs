---
id: task-508-01-rules-scripts-claude-files
description: "Executor: remove the three points from rule nodes, scripts and Claude files, keep the six others, commit the style-loop fix"
parent: task-508-workflow-back-to-old
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-508
- usage: included in the executor run of task-508
- runs: 1
- milestone: M2
- started: 2026-11-12T12:23:45Z
- waiting_for: none
- result: Commit 7ad6f50e. Removed: gros-morceau split (orchestration-protocol step 3, agent-planner gros_morceau_split, request-families row now one global, task.md); 120k context handover (context-now.js deleted, task-next.js prints no context/HANDOVER line, alerts.handoverContextTokens gone, execution-protocol 5.9 and agent-executor handover removed, task.md and executor.md handover relaunch removed); active budget (budgetAlert back to the pre-502 non-blocking ALERT line, previousUsage and alerts.budgetStopFactor gone; execution-protocol 5.9, verification-levels, agent-executor, runbook, cost-log, agents-index say budget only recorded). Kept: reglage route and task-reglage.js, preview gate 5.7, task-confirm.js on pure ok, runbook/quirks read on failure only, extraProjectSlugs, agents planner/executor/style-guide. style-loop.md general-purpose text-role fix committed. Checks: node --check task-next.js and record-cost.js, cost-rates.json parses, budgetAlert gives ALERT on task-505/496/506 and null on task-507, grep finds no HANDOVER/BUDGET STOP/handoverContextTokens/budgetStopFactor in .claude, rule nodes or scripts, lint clean (pre-existing warnings only); this closing ran the edited task-next.js and record-cost.js. Tasks 500-503 and 507 untouched.

## Relations
- [[task-508-workflow-back-to-old]] — parent global task.
