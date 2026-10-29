---
id: task-376-02-doc-extractor-rock-margin
description: "Doc: doc: update sys-objectives-shuttle, sys-objectives-run-flow and sys-grid-api."
parent: task-376-extractor-rock-margin
kind: task
status: done
---
# Extractor: rock destroyed under it and in a margin around it: doc

- goal: The doc nodes state the clear margin and the new landing spot choice.
- agent: doc
- depends_on: task-376-01-cpp-extractor-rock-margin
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md), sys-objectives-run-flow (vault/reference/tech/systems/world/sys-objectives/sys-objectives-run-flow.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md)
- expected_output: the three nodes updated from the change summary with the commit hash; one commit.
- done_when: nodes written; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 10, cache write 33948, cache read 315891, output 3286
- usage: turns 5, context start 59k peak 78k, resumes 0, 0 min, units 0.09 M
- runs: 1
- milestone: M2
- started: 2026-10-29T23:55:31Z
- waiting_for: none
- result: ClearMarginCells, GetClearRect and in-band landing documented in 3 nodes; commit c02cf3d6; lint OK

## Relations
- [[task-376-extractor-rock-margin]] — parent global task.
