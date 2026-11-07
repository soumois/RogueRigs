---
id: task-494-02-test-map
description: "Bp: The test map asset using the test mode, starting on a generated layout."
parent: task-494-generator-test-map
kind: task
status: in-progress
---
# Enemy-free test map with a command to generate and reload a new level: bp

- goal: The test map asset using the test mode, starting on a generated layout.
- agent: bp
- depends_on: task-494-01-test-mode-command
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md)
- expected_output: see done_when
- done_when: A1 of the global holds when the map is opened in -game; map saved; its bp doc node exists; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T08:36:35Z
- waiting_for: none
- result: open

## Relations
- [[task-494-generator-test-map]] — parent global task.
