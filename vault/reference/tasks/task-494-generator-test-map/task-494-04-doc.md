---
id: task-494-04-doc
description: "Doc: Document the test mode, the command and the test map."
parent: task-494-generator-test-map
kind: task
status: in-progress
---
# Enemy-free test map with a command to generate and reload a new level: doc

- goal: Document the test mode, the command and the test map.
- agent: doc
- depends_on: task-494-03-capture
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md)
- expected_output: see done_when
- done_when: A sys node (or sys-grid) describes the test mode and rr.Level.New; the map has its bp node; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T08:47:14Z
- waiting_for: none
- result: open

## Relations
- [[task-494-generator-test-map]] — parent global task.
