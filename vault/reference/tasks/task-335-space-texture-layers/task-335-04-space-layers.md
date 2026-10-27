---
id: task-335-04-space-layers
description: "BP_SpaceBackground: nebula plane switched to MI_Space_Clouds, new deep plane at Z -8000 with MI_Space_Deep, scale from the coverage formula."
parent: task-335-space-texture-layers
kind: task
status: in-progress
---
# Space layers: planes in BP_SpaceBackground

- goal: the placed BP_SpaceBackground in L_ArenaM1 draws, back to front, procedural stars (Z -12000), the deep texture layer (Z -8000) and the cloud texture layer (Z -5000), then the procedural dust (Z -2000), with no plane edge in view.
- agent: bp
- depends_on: task-335-03-space-texture-material
- inputs: the global `request` and `gdd_check` with S3 and S7 (vault/reference/tasks/task-335-space-texture-layers.md); the task-335-03 change summary; bp-space-background (vault/reference/tech/blueprints/world/bp-space-background.md, components and the coverage arithmetic to apply to the new depth); bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md, map); bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md).
- expected_output: BP_SpaceBackground: NebulaPlane material MI_Space_Nebula to MI_Space_Clouds (depth and scale unchanged); new StaticMeshComponent DeepPlane, /Engine/BasicShapes/Plane, Z -8000, scale from the bp-space-background formula (state the numbers: depth h, maxR, reach, half, margin), Movable, NoCollision, CastShadow off, bReceivesDecals off, bAffectDistanceFieldLighting off, MI_Space_Deep; StarsPlane and DustPlane unchanged; no graph logic. The L_ArenaM1 instance reads back with the four planes after map reload. Change summary for the doc subtask, commit hash. No vault doc node.
- done_when: the Blueprint compiles and is saved; the placed instance reads back with the four components and their materials; only BP_SpaceBackground (and its external actor file if touched) changes in `git status`; committed by explicit path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T22:47:59Z
- waiting_for: none
- result: open

## Relations
- [[task-335-space-texture-layers]] — parent global task.
