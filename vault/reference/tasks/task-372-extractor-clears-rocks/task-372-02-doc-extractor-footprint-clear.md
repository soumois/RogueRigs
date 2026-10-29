---
id: task-372-02-doc-extractor-footprint-clear
description: "Doc: update sys-grid-api, sys-grid, sys-grid-deposits, run-log-schema and sys-objectives-shuttle."
parent: task-372-extractor-clears-rocks
kind: task
status: done
---
# Extractor: blocks under its footprint are destroyed, resources stay: doc

- goal: The docs describe the Extractor source, the footprint clear and its drop rule.
- agent: doc
- depends_on: task-372-01-cpp-extractor-footprint-clear
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-grid-deposits (vault/reference/tech/systems/world/sys-grid/sys-grid-deposits.md), run-log-schema (vault/reference/tech/pipelines/run-log-schema.md), sys-objectives-shuttle (vault/reference/tech/systems/world/sys-objectives/sys-objectives-shuttle.md)
- expected_output: nodes updated; one commit.
- done_when: source, entry point, drop rule and log line documented with the commit hash; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 18, cache write 47710, cache read 245051, output 6471
- usage: turns 9, context start 12k peak 48k, resumes 0, 1 min, units 0.12 M
- runs: 1
- milestone: M2
- started: 2026-10-29T21:58:23Z
- waiting_for: none
- result: Done: 10 doc nodes updated citing commits de889932 and 0e85c9a0 (doc commit 7598c940); lint clean.

## Relations
- [[task-372-extractor-clears-rocks]] — parent global task.
