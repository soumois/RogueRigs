---
id: task-332-02-space-layers-actor
description: "Build BP_SpaceBackground holding the background planes at their depths and place one instance in L_ArenaM1."
parent: task-332-space-background
kind: task
status: in-progress
---
# Space background: layer actor and placement

- goal: a data-only Blueprint actor BP_SpaceBackground with one plane per layer at the depths decided in Q2, sized so no plane edge is ever in view, and one instance placed and saved in L_ArenaM1.
- agent: bp
- depends_on: task-332-01-space-material
- inputs: the global `request` and `gdd_check` with the owner answers (vault/reference/tasks/task-332-space-background.md); the task-332-01 change summary (instance paths); bp-arena-grid (vault/reference/tech/blueprints/world/bp-arena-grid.md, arena 240 x 240 cells of 67 uu, origin, L_ArenaM1 contents); bp-miner (vault/reference/tech/blueprints/player/bp-miner.md, camera arm and camera); lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md); bp-blob-shadow-material (vault/reference/tech/blueprints/fx/bp-blob-shadow-material.md, decals to keep off the planes); bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md); unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md).
- expected_output: /Game/RogueRigs/Environment/Space/BP_SpaceBackground (Actor, no graph logic): one StaticMeshComponent per layer, /Engine/BasicShapes/Plane, relative Z per Q2, material MI_Space_Stars, MI_Space_Nebula, MI_Space_Dust; on every plane CastShadow off, bReceivesDecals off, collision NoCollision, bAffectDistanceFieldLighting off; scale per layer computed from the camera FOV, pitch, arm length, 16:9 aspect and the depth, so the plane covers the view with the miner on any cell of the arena plus 10 % margin (the arithmetic is part of the change summary). One instance placed in L_ArenaM1 centred on the arena, map saved (external actors: the empty save_assets route of [[handover-2026-10-21-lighting-floor]] quirks). Change summary for the doc subtask: component table (depth, scale, material, flags), the coverage arithmetic, placement transform, commit hash. No vault doc node.
- done_when: Blueprint compiled and saved; component values read back from the placed L_ArenaM1 instance after a map reload (not only the class); coverage arithmetic shows margin at the four arena corners for every layer; map saved; committed by explicit paths.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T21:33:12Z
- waiting_for: none
- result: open

## Relations
- [[task-332-space-background]] — parent global task.
