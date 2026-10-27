---
id: task-334-02-asteroid-material
description: "Build M_Asteroid and MI_Asteroid, a lit flat-coloured rock material for the background asteroids, darker than the arena rocks."
parent: task-334-near-asteroids
kind: task
status: in-progress
---
# Asteroids: material

- goal: one master material M_Asteroid with one instance MI_Asteroid that draws the asteroids per A1 (lit, flat-shaded, violet-grey, darker than the arena rocks), compiled and saved.
- agent: bp
- depends_on: none
- inputs: the global `request` and `gdd_check` with the owner answers A0 to A7 (vault/reference/tasks/task-334-near-asteroids.md); bp-rock-slab-material (vault/reference/tech/blueprints/world/bp-rock-slab-material.md, arena rock colours: SideColor (0.16, 0.15, 0.22), mean top albedo about (0.28, 0.26, 0.34)); color-code (vault/reference/gdd/art-audio/color-code.md); lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md, sun and sky); mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md); bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md).
- expected_output: in /Game/RogueRigs/Environment/Space/Asteroids/: M_Asteroid (Default Lit, Opaque, no texture, no world position offset; parameters BaseColor, Roughness, and an optional low-frequency world-noise tint strength so variants do not look identical) and MI_Asteroid with provisional values (BaseColor at most the arena SideColor, Roughness about 0.9). Change summary for the doc subtask: asset paths, parameter table, commit hash. No vault doc node.
- done_when: the instance compiles with no log error and is saved; parameters read back; no texture sampler in the graph; committed by explicit paths.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T23:30:53Z
- waiting_for: none
- result: open

## Relations
- [[task-334-near-asteroids]] — parent global task.
