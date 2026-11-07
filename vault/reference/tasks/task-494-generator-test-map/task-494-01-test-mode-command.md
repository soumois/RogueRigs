---
id: task-494-01-test-mode-command
description: "Cpp: Enemy-free test mode (no robot spawn, objectives, shuttle or alert) and the rr.Level.New [seed] command that generates, saves and reloads a layout through the async loader."
parent: task-494-generator-test-map
kind: task
status: todo
---
# Enemy-free test map with a command to generate and reload a new level: cpp

- goal: Enemy-free test mode (no robot spawn, objectives, shuttle or alert) and the rr.Level.New [seed] command that generates, saves and reloads a layout through the async loader.
- agent: cpp
- depends_on: none
- inputs: level-flow (vault/reference/gdd/run/level-flow.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md)
- expected_output: see done_when
- done_when: A2 to A4 and A6 of the global hold in a -game run on a map using the mode (log evidence: seeds, layout counts, no robot spawned); builds; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-494-generator-test-map]] — parent global task.
