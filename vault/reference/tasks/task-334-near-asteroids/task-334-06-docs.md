---
id: task-334-06-docs
description: "Write the doc nodes of M_Asteroid and BP_AsteroidField, list them in the world index, update the bp-arena-grid map line."
parent: task-334-near-asteroids
kind: task
status: done
---
# Asteroids: docs

- goal: the asteroid material, meshes and field actor are documented so the owner can tune, move or remove them without opening the assets.
- agent: doc
- depends_on: task-334-02-asteroid-material, task-334-03-asteroid-import, task-334-04-asteroid-field, task-334-05-capture
- inputs: bp-asteroid-material (vault/reference/tech/blueprints/world/bp-asteroid-material.md, to create from `vault/templates/blueprint.md`: M_Asteroid, MI_Asteroid, parameter table, the four SM_Asteroid meshes and their import settings, revert); bp-asteroid-field (vault/reference/tech/blueprints/world/bp-asteroid-field.md, to create from `vault/templates/blueprint.md`: components, positions, scales, rotation rates, flags, placement in L_ArenaM1, removal); blueprints-world-index (vault/reference/tech/blueprints/world/blueprints-world-index.md, to update: two Children lines); bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md, to update: `placed_in` names BP_AsteroidField); bp-space-background (vault/reference/tech/blueprints/world/bp-space-background.md, to update: Relations line to bp-asteroid-field); the global `request` and `gdd_check` for decided values (vault/reference/tasks/task-334-near-asteroids.md); the change summaries and commits of task-334-02, 03 and 04 (run `git show`, the diff is not pasted); the task-334-05 result for what is verified on screen. The 3d asset node `asteroid-asset` is written by task-334-01, not here.
- expected_output: the two new doc nodes (each under ~400 words, `owner_agent: agent-bp`, `not_verified` list), the index, bp-arena-grid and bp-space-background updates, Relations on both ends. No other node.
- done_when: both nodes exist with every template field filled (undecided = `open`); index lists them; lint clean; committed by explicit paths.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 18, cache write 37583, cache read 215785, output 6745
- usage: turns 9, context start 11k peak 38k, resumes 0, 1 min, units 0.10 M
- runs: 1
- milestone: M3
- started: 2026-10-27T23:52:45Z
- waiting_for: none
- result: bp-asteroid-material and bp-asteroid-field written, index, bp-arena-grid, bp-space-background updated (e84bf2c6)

## Relations
- [[task-334-near-asteroids]] — parent global task.
