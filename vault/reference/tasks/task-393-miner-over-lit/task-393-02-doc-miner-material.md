---
id: task-393-02-doc-miner-material
description: "Doc: refresh bp-miner (SK_Miner, ABP_Miner, M_Miner, MI_Miner, BaseColorScale, how to tune)"
parent: task-393-miner-over-lit
kind: task
status: todo
---
# Miner too bright in the arena: doc

- goal: Bring bp-miner up to date (Meshy SK_Miner and ABP_Miner instead of Quinn, M_Miner readings, MI_Miner and BaseColorScale with how to tune and revert) and link it from lights-dark-ambient.
- agent: doc
- depends_on: task-393-01-miner-material-scale
- inputs: agent-doc (vault/reference/agents/agent-doc.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), task-386-miner-mixamo-animation (vault/reference/tasks/task-386-miner-mixamo-animation.md), global request and gdd_check: vault/reference/tasks/task-393-miner-over-lit.md
- expected_output: bp-miner updated, one line in lights-dark-ambient; commit `task-393-02-doc-miner-material: ...`.
- done_when: bp-miner states the current mesh, material, MI_Miner, BaseColorScale value, tune and revert steps; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-393-miner-over-lit]] — parent global task.
