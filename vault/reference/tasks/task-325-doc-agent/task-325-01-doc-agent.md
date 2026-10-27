---
id: task-325-01-doc-agent
description: "Create the doc agent (definition file and vault node) and wire it into planner, executor, cpp, bp, protocol, runbook and agents-index."
parent: task-325-doc-agent
kind: task
status: in-progress
---
# Create and wire the doc agent

- goal: apply the decisions of [[task-325-doc-agent]] `gdd_check` (Q1 to Q4 as answered by the owner, defaults if "ok"). (1) New `.claude/agents/doc.md`: name `doc`, model sonnet, tools and maxTurns per Q4; body: read the subtask node and `agent-doc`, then `_authoring`; run `git show <hash> -- <paths>` from the brief; create or update only the listed nodes (frontmatter, `parent` and `## Relations` on both ends, `root.md` only when a branch changes, Blueprint nodes from `vault/templates/blueprint.md`, systems from `system.md`); never edit code, assets or gdd values; `node vault/scripts/lint.js --write`, fix, commit by explicit path; report a short recap (nodes created/updated, commit). (2) New vault node `agent-doc` from `vault/templates/agent.md` (status provisional, model sonnet, `token_ceiling: open` until task-326, reads `_authoring`, the templates and the nodes listed in the brief; must_not: write code or assets, change a gdd value, search the vault, document anything not in the brief). (3) `agents-index`: child line and the "Crew in use" line. (4) `agent-executor` and `.claude/agents/executor.md`: `doc` in the agent list; the doc subtask is delegated with the brief of Q2 built from the implementers' change summaries. (5) `agent-cpp`, `.claude/agents/cpp.md`, `agent-bp`, `.claude/agents/bp.md`: no longer write `sys-*` / `bp-*` nodes; return a change summary (files or assets changed with paths, commit hash, public API or properties set, decided values used, doc nodes to create or update). bp keeps saving and committing its assets. (6) `agent-planner` and [[orchestration-protocol]] step 3 (owner intent recorded): the Planner adds the doc subtask of Q2; step 5 names the brief. (7) `orchestrator-runbook` Delegation: the brief contents. (8) Cost tracking (owner addition): the doc subtask is a real subtask node from `vault/templates/task.md` (`agent: doc`, `token_ceiling`, `model`, `tokens`, `usage`, `runs`); the Executor delegates it with a description starting with the subtask id (`task-NNN-SS-...`) and closes it with `task-next.js` like a cpp or bp subtask, so `agent-usage.js` attributes it (worker by description prefix, no script change needed), `record-cost.js` writes its cost-log row and `usage` line, and it appears in `usage-snapshot.json`; written in `agent-executor`, `.claude/agents/executor.md`, `agent-planner` and protocol steps 3 and 5. Do not edit task nodes of tasks 308, 309, 314 (Q3).
- agent: cpp
- depends_on: none
- inputs: agents-index (vault/reference/agents/agents-index.md), agent-executor (vault/reference/agents/agent-executor.md), agent-planner (vault/reference/agents/agent-planner.md), agent-cpp (vault/reference/agents/agent-cpp.md), agent-bp (vault/reference/agents/agent-bp.md), orchestration-protocol (vault/reference/agents/orchestration-protocol.md), orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), _authoring (vault/_authoring.md), template agent (vault/templates/agent.md), .claude/agents/executor.md, .claude/agents/cpp.md, .claude/agents/bp.md, .claude/agents/planner.md
- expected_output: `.claude/agents/doc.md`, `vault/reference/agents/agent-doc.md`, the edited files above; no script change.
- done_when: `.claude/agents/doc.md` has `model: sonnet` and the Q4 tools; `agent-doc` exists, is a child of agents-index and links back; executor, planner, cpp and bp definitions (vault node and `.claude/agents` file of each) agree with the decisions, and no definition still tells cpp or bp to write a `sys-*` or `bp-*` node; protocol steps 3 and 5 and the runbook name the doc subtask and its brief; executor, planner and protocol state that the doc subtask has its own node, a delegation description starting with its subtask id and a `task-next.js` closing (cost-log row, `usage` line, model, alerts); tasks 308, 309, 314 untouched; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-27T14:17:20Z
- waiting_for: none
- result: open

## Relations
- [[task-325-doc-agent]] — parent global task.
