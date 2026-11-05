---
id: task-454-02-doc
description: "Doc: Docs state the new rule."
parent: task-454-swarm-icons-fixed
kind: task
status: done
---
# First swarms ignore kills until the first front is over: doc

- goal: Docs state the new rule.
- agent: doc
- depends_on: task-454-01-rule
- inputs: sys-hud-pressure (vault/reference/tech/systems/ui/sys-hud-pressure.md), sys-objectives (vault/reference/tech/systems/world/sys-objectives.md), sys-difficulty (vault/reference/tech/systems/core/sys-difficulty.md)
- expected_output: see done_when
- done_when: Nodes state the current rule; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-454
- usage: included in the executor run of task-454
- runs: 1
- milestone: M2
- started: 2026-11-05T23:42:16Z
- waiting_for: none
- result: Commit 2b43ff02. Updated sys-hud-pressure (horde icon still until the first front is over), sys-objectives-run-flow (kill rule, DoKillsAddAlert, bKillsWaitForFirstHorde), sys-objectives (entry point), sys-difficulty (alert rule), sys-tests-hud-text (new test and figures). Lint clean.

## Relations
- [[task-454-swarm-icons-fixed]] — parent global task.
