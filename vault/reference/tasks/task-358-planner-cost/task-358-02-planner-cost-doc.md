---
id: task-358-02-planner-cost-doc
description: "Doc: describe path-of.js, task-confirm.js and task-create.js spec v2 in the runbook, planner, protocol, navigation and main-session instructions."
parent: task-358-planner-cost
kind: task
status: done
---
# Planner cost: doc

- goal: From the change summary of task-358-01, update the nodes and files below so the planner uses path-of.js instead of `_paths.md`, writes its plan with one task-create.js call and its defaults file at step 4, and the main session runs task-confirm.js on a pure "ok" (owner intent for the rule edits: global `## Questions` Q1, Q2).
- agent: doc
- depends_on: task-358-01-planner-cost-scripts
- inputs: orchestrator-runbook (vault/reference/agents/orchestrator-runbook.md) (Creating tasks), agent-planner (vault/reference/agents/agent-planner.md), orchestration-protocol (vault/reference/agents/orchestration-protocol.md) (step 4), _navigation (vault/_navigation.md) (Resolving a wikilink), `.claude/agents/planner.md`, `.claude/commands/task.md`, `CLAUDE.md`, _authoring (vault/_authoring.md)
- expected_output: the listed nodes and files updated, one commit.
- done_when: each listed file names the script it now uses with its usage line; `_navigation` still forbids search and states exact-id resolution only; lint clean; committed.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 14, cache write 36280, cache read 153815, output 5757
- usage: turns 7, context start 12k peak 36k, resumes 0, 1 min, units 0.09 M
- runs: 1
- milestone: M2
- started: 2026-10-29T14:16:04Z
- waiting_for: none
- result: Done: efc992a3 runbook, planner, protocol step 4, _navigation, planner.md, task.md, CLAUDE.md, quirks updated; lint clean.

## Relations
- [[task-358-planner-cost]] — parent global task.
