---
id: task-502-01-handover-and-budget
description: "Doc: Rebuild the context measure and the HANDOVER line, the handover section and relay relaunch, and the budget trigger."
parent: task-502-executor-handover-budget
kind: task
status: done
---
# Executor handover over a context threshold and a budget that triggers: doc

- goal: Rebuild the context measure and the HANDOVER line, the handover section and relay relaunch, and the budget trigger.
- agent: doc
- depends_on: none
- inputs: agent-executor (vault/reference/agents/agent-executor.md), execution-protocol (vault/reference/agents/execution-protocol.md), verification-levels (vault/reference/agents/verification-levels.md), orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), .claude/commands/task.md, .claude/agents/executor.md, vault/scripts/task-next.js, vault/scripts/record-cost.js, C:/Users/soumo/Desktop/Doc à envoyer/rapport-workflow/rapport.md
- expected_output: see done_when
- done_when: A1 to A5 of the global pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-502
- usage: included in the executor run of task-502
- runs: 1
- milestone: M2
- started: 2026-11-11T21:17:38Z
- waiting_for: none
- result: context-now.js prints the executor context (last turn of the newest executor transcript of the global, else the usage-snapshot trace); task-next.js prints context: <n>k and, over alerts.handoverContextTokens 120000 (cost-rates.json) with subtasks left, a HANDOVER line. BUDGET line from task-cost.js over the level budget times 1 + budget_grants (task-status.js inserts the field); execution-protocol steps 5.9 handover and 5.10 budget stop, agent-executor handover/budget/cost_alerts, verification-levels, runbook, cost-log, agents-index, task.md relay relaunch, executor.md step 8. A4 checked: task-502 live 86k, task-496 transcript 339k with HANDOVER, task-441 usage trace 318k with HANDOVER, RR_HANDOVER_TOKENS=20000 forces HANDOVER on the live run, BUDGET on task-496 (128 turns, 3.66 M) and task-499, grants x1/x2 on a fake figure. Q8 not done (default no). Commit f8c6dcf5.

## Relations
- [[task-502-executor-handover-budget]] — parent global task.
