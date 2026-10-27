---
id: task-335-09-docs
description: "Doc subtask: new doc node for M_SpaceTexture, update bp-space-background-material, bp-space-background, mpp-player-light-mask and the world index."
parent: task-335-space-texture-layers
kind: task
status: done
---
# Space layers: doc nodes

- goal: the tech docs describe the textured layers as built, the procedural master as it now stands and the void far gain of the mask.
- agent: doc
- depends_on: task-335-03-space-texture-material, task-335-04-space-layers, task-335-05-void-mask-gain, task-335-07-intensity-retune
- inputs: the global `request` and `gdd_check` for decided values (vault/reference/tasks/task-335-space-texture-layers.md); the task-335-03, 04, 05 and 07 change summaries and commit hashes (the instance tables give the FINAL Intensity values of task-335-07, with the 03 values as history); the task-335-06 and 08 capture results (vault/reference/tasks/task-335-space-texture-layers/task-335-06-capture.md, task-335-08-recapture.md) for the finding; the task-333-01 result (vault/reference/tasks/task-333-nebula-visible/task-333-01-nebula-tune.md, Intensity 0.06 to 1.6, commit 975425cf); agent-doc (vault/reference/agents/agent-doc.md); nodes to update: bp-space-background-material (vault/reference/tech/blueprints/world/bp-space-background-material.md: Nebula Intensity 1.6, MI_Space_Nebula no longer on a plane, task-333 finding), bp-space-background (vault/reference/tech/blueprints/world/bp-space-background.md: four planes, DeepPlane coverage row, materials), mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md: void far gain parameter, formula, previous values to revert; the saved FarBrightness reads 0.06, not the documented 0.08 (task-335-05 and 06 results): record 0.06 as the current value and note the mismatch); blueprints-world-index (vault/reference/tech/blueprints/world/blueprints-world-index.md); node to create: bp-space-texture-material (vault/reference/tech/blueprints/world/bp-space-texture-material.md, template `vault/templates/blueprint.md`).
- expected_output: the new node and the four updated nodes, linked both ways, lint clean.
- done_when: `node vault/scripts/lint.js --write` reports no problem; every value in the nodes matches the change summaries; committed by explicit path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 28, cache write 43238, cache read 412082, output 8331
- usage: turns 14, context start 10k peak 43k, resumes 0, 3 min, units 0.14 M
- runs: 1
- milestone: M3
- started: 2026-10-27T23:14:32Z
- waiting_for: none
- result: Doc nodes done (commit edcb24c0): bp-space-texture-material created; background, background-material, mpp-player-light-mask (FarBrightness 0.06 noted), world index updated; art-prompts-space-background description trimmed to 25 words. Open: Tint/Saturation/TileSize/PanVelocity read-back values not in summaries.

## Relations
- [[task-335-space-texture-layers]] — parent global task.
