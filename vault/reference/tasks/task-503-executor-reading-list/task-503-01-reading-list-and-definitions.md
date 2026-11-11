---
id: task-503-01-reading-list-and-definitions
description: "Doc: Shorten the executor reading list and apply the specialist definitions decision."
parent: task-503-executor-reading-list
kind: task
status: done
---
# Shorter executor reading list and specialist agent definitions: doc

- goal: Shorten the executor reading list and apply the specialist definitions decision.
- agent: doc
- depends_on: none
- inputs: agent-executor (vault/reference/agents/agent-executor.md), execution-protocol (vault/reference/agents/execution-protocol.md), orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), .claude/agents/executor.md, .claude/agents, CLAUDE.md, .gitignore, C:/Users/soumo/Desktop/Doc à envoyer/rapport-workflow/rapport.md
- expected_output: see done_when
- done_when: A1 to A4 of the global pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-503
- usage: included in the executor run of task-503
- runs: 1
- milestone: M2
- started: 2026-11-11T21:22:27Z
- waiting_for: none
- result: A1 agent-executor reads and .claude/agents/executor.md step 1: runbook and quirks only when a script or command fails, role nodes on first use. A2 Q9: .claude/agents cpp, bp, capture, doc, art, 3d, text, level-design deleted; agent-executor roles, CLAUDE.md, runbook delegation and doc-subtask lines, agents-index agent-doc gloss and definition_file of six role nodes aligned. Q10: uncommitted ruvector.db line dropped from .gitignore, ruvector.db deleted. A3 no node merged. A4 lint clean, commit 096a7033. Left as is: README.md, assignments READMEs and .claude/commands/architect.md still link the removed definitions (historical deliverables, not in the request).

## Relations
- [[task-503-executor-reading-list]] — parent global task.
