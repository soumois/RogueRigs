---
id: task-532-02-doc
description: "Doc: Update the vault nodes that describe lint.js usage with the default output and --all."
parent: task-532-lint-quiet-output
kind: task
status: done
---
# lint.js : sortie courte par défaut: doc

- goal: Update the vault nodes that describe lint.js usage with the default output and --all.
- agent: doc
- depends_on: task-532-01-lint-default-output
- inputs: orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md), orchestrator-script-quirks (vault/reference/agents/orchestrator-script-quirks.md)
- expected_output: see done_when
- done_when: A7 holds: every vault node that describes lint.js output or flags states the default and --all; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-532
- usage: included in the executor run of task-532
- runs: 1
- milestone: M2
- started: 2026-11-13T18:20:47Z
- waiting_for: none
- result: orchestrator-runbook (Git section) documents lint.js [--write] [--all]: errors always, default warnings only for vault files differing from HEAD, summary with hidden count, --all for every warning, exit code and --write unchanged; orchestrator-script-quirks notes that committed nodes' warnings are hidden and --all shows them. _authoring (kind rule, not in inputs) left unchanged: its lint line stays true. Lint clean.

## Relations
- [[task-532-lint-quiet-output]] — parent global task.
