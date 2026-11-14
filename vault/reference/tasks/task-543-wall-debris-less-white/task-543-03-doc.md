---
id: task-543-03-doc
description: "Doc: Update the debris effect doc node"
parent: task-543-wall-debris-less-white
kind: task
status: done
---
# Wall destruction particles: rock colours instead of near white: doc

- goal: Update the debris effect doc node
- agent: doc
- depends_on: task-543-02-capture
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: see done_when
- done_when: The effect node states the new colours; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-543
- usage: included in the executor run of task-543
- runs: 1
- milestone: M2
- started: 2026-11-14T00:15:05Z
- waiting_for: none
- result: Done. bp-dig-debris (three User colours, the Color HLSL, defaults, task-543 verification and the MCP compile pitfall) and bp-explosion-wall-debris (random rock tone per particle, capture note) state the new colours; lint clean.

## Relations
- [[task-543-wall-debris-less-white]] — parent global task.
