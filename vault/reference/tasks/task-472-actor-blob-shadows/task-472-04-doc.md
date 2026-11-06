---
id: task-472-04-doc
description: "Doc: Docs describe the shadow component, its settings and the material."
parent: task-472-actor-blob-shadows
kind: task
status: done
---
# Light-independent purple-grey shadows under actors: doc

- goal: Docs describe the shadow component, its settings and the material.
- agent: doc
- depends_on: task-472-03-capture
- inputs: bp-robot-readability (vault/reference/tech/blueprints/enemies/bp-robot-readability.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), visual-style (vault/reference/gdd/art-audio/visual-style.md)
- expected_output: see done_when
- done_when: A sys node for the component and the bp nodes of the touched actors and material describe the shadow and its settings; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-472
- usage: included in the executor run of task-472
- runs: 1
- milestone: M2
- started: 2026-11-06T18:19:45Z
- waiting_for: none
- result: Done (79316211). sys-blob-shadow rewritten (component, settings, owners, tests, gotchas); bp-blob-shadow-material rewritten (graph, parameters, Tint calibration, rebuild script); sys-grid barrel shadows and barrel no-cast; actor_shadow lines in bp-robot-readability, bp-miner, bp-turret, bp-bonus-chest; tool-unreal-reimport lists blob_shadow_material.py. Lint clean.

## Relations
- [[task-472-actor-blob-shadows]] — parent global task.
