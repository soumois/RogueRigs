---
id: task-332-04-docs
description: "Write the doc nodes of M_SpaceBackground and BP_SpaceBackground, list them in the world index, update the bp-arena-grid map line."
parent: task-332-space-background
kind: task
status: todo
---
# Space background: docs

- goal: the space background material and Blueprint are documented so the owner can tune or remove them without opening the assets.
- agent: doc
- depends_on: task-332-01-space-material, task-332-02-space-layers-actor, task-332-03-capture
- inputs: bp-space-background-material (vault/reference/tech/blueprints/world/bp-space-background-material.md, to create from `vault/templates/blueprint.md`: asset paths, graph, parameter table with defaults and units, blend modes, cost, revert); bp-space-background (vault/reference/tech/blueprints/world/bp-space-background.md, to create from `vault/templates/blueprint.md`: components, depths, scales and coverage arithmetic, flags, placement in L_ArenaM1, how to remove); blueprints-world-index (vault/reference/tech/blueprints/world/blueprints-world-index.md, to update: two Children lines); bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md, to update: the `placed_in` sentence "no sky atmosphere so the void is dark" now names BP_SpaceBackground); the global `request` and `gdd_check` for decided values (vault/reference/tasks/task-332-space-background.md); the change summaries and commits of task-332-01 and task-332-02 (run `git show`, the diff is not pasted); the task-332-03 result for what is verified on screen.
- expected_output: the two new doc nodes (each under ~400 words, `owner_agent: agent-bp`, `not_verified` list), the index and bp-arena-grid updates, Relations on both ends. No other node.
- done_when: both nodes exist with every template field filled (undecided = `open`); index lists them; lint clean; committed by explicit paths.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-332-space-background]] — parent global task.
