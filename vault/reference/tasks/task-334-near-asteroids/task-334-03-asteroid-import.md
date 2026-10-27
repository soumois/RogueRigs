---
id: task-334-03-asteroid-import
description: "Import the four validated asteroid FBX as SM_Asteroid_01 to 04 with MI_Asteroid, no collision, no shadow casting."
parent: task-334-near-asteroids
kind: task
status: todo
---
# Asteroids: import

- goal: the four validated meshes exist in Unreal with MI_Asteroid assigned and no collision.
- agent: bp
- depends_on: task-334-01-asteroid-meshes, task-334-02-asteroid-material
- inputs: the global `gdd_check` (vault/reference/tasks/task-334-near-asteroids.md, A5); the task-334-01 result and `Art/Meshes/Asteroid/SM_Asteroid_01.fbx` to `_04.fbx` with their `.report.json`; art-3d-lessons (vault/reference/agents/art-3d-lessons.md, FBX import traps); tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md); bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md).
- expected_output: /Game/RogueRigs/Environment/Space/Asteroids/SM_Asteroid_01 to _04, imported at FBX scale (about 100 uu across), flat normals kept, slot 0 = MI_Asteroid, collision removed (no simple or complex collision used), Nanite off, no lightmap UV generation. Change summary for the doc subtask: asset paths, bounds read back, triangle counts, commit hash. No vault doc node.
- done_when: the four meshes are saved; bounds read back about 100 uu (not 10000); material slot 0 reads MI_Asteroid; collision reads none; committed by explicit paths.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-334-near-asteroids]] — parent global task.
