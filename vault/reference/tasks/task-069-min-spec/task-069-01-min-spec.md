---
id: task-069-01-min-spec
description: "Minimum PC specification defined and checked against the stress scenarios."
parent: task-069-min-spec
kind: task
status: todo
---
# Minimum specification

- goal: Minimum PC specification defined and checked against the stress scenarios.
- agent: cpp
- depends_on: none
- inputs: stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), sys-stress (vault/reference/tech/systems/sys-stress.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: once the owner has chosen the minimum hardware (open: CPU and GPU class, RAM, resolution, target fps), estimate or measure the game against it (scalability settings, robot caps), list what must change to meet it; results in a new performance note.
- done_when: minimum specification written with its evidence; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-069-min-spec]] — parent global task.
