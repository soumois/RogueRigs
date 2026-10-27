---
id: task-<number>-<step>-<slug>
description: "<one sentence: the goal of this subtask>"
parent: task-<number>-<slug>
kind: task
status: todo
---
# <Subtask title>

- goal: <one outcome, one agent>
- agent: <agent id>
- depends_on: <subtask ids, or none>
- inputs: <vault node ids and files>
- expected_output: <files, format>
- done_when: <checks the orchestrator runs before marking done>
- editor_state: closed | open | any
- token_ceiling: <number>
- model: <model of the agent that ran it (opus | sonnet | haiku), filled when done>
- tokens: <input N, cache write N, cache read N, output N — from `node vault/scripts/agent-usage.js`, all runs included; filled when done>
- usage: <turns, context start and peak, resumes with cause, minutes, units; written by record-cost.js when done>
- runs: <number of agent runs, resumes included; filled when done>
- milestone: M1 | M2 | M3
- started: <ISO date-time UTC, written by task-status.js on every move to in-progress>
- result: <filled when done: what changed, commit, tokens used>

## Relations
- [[task-<number>-<slug>]] — parent global task.
