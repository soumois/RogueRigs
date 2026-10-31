---
id: task-400-02-burst-rifle-import
description: "Bp: import SM_BurstRifle with M_BurstRifle and its four textures"
parent: task-400-burst-rifle-in-hand
kind: task
status: in-progress
---
# Burst rifle 3D mesh in the miner right hand: bp

- goal: SM_BurstRifle imported from `Art/Meshes/burst-rifle/SM_BurstRifle.fbx` into `/Game/RogueRigs/Weapons/BurstRifle/` (Q3), with T_BurstRifle_BaseColor, Normal (normal map), Metallic, Roughness (linear) and M_BurstRifle wired to them; no collision, no decimation.
- agent: bp
- depends_on: none
- inputs: burst-rifle-mesh (vault/reference/gdd/art-audio/burst-rifle-mesh.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), `Art/Meshes/burst-rifle/SM_BurstRifle.fbx`, `Art/Meshes/burst-rifle/T_BurstRifle_BaseColor.png`, `T_BurstRifle_Metallic.png`, `T_BurstRifle_Normal.png`, `T_BurstRifle_Roughness.png`
- expected_output: Assets saved under /Game/RogueRigs/Weapons/BurstRifle/, change summary (paths, triangle count, bounds read back, material wiring); commit `task-400-02-burst-rifle-import: ...`.
- done_when: SM_BurstRifle reads 9,892 triangles and bounds about 60 x 7.3 x 20.7 uu with the pivot at the grip (not 100 times too large); its slot uses M_BurstRifle with the four textures; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-31T00:00:53Z
- waiting_for: none
- result: open

## Relations
- [[task-400-burst-rifle-in-hand]] — parent global task.
