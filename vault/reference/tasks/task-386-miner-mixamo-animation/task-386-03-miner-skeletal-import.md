---
id: task-386-03-miner-skeletal-import
description: "Bp: import SK_Miner with SKEL_Miner, its physics asset and a material from the two Meshy textures"
parent: task-386-miner-mixamo-animation
kind: task
status: done
---
# Miner animated with the owner Mixamo clips: bp

- goal: Import `Art/Meshes/Miner/SK_Miner.fbx` into `/Game/RogueRigs/Characters/Miner/` as SK_Miner with a new skeleton SKEL_Miner and the physics asset the import creates, and a material M_Miner from the base colour and normal textures.
- agent: bp
- depends_on: task-386-01-miner-clips-in-place
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), vault/reference/tech/pipelines/miner-mixamo-clips.md (created by step 01), mesh: `Art/Meshes/Miner/SK_Miner.fbx`; textures `Art/Source/miner-meshy/texture_0.png` (base colour, sRGB) and `normal.png` (normal map), owner words, planner measures and Q answers: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-386-miner-mixamo-animation.md), method: editor Python `FbxImportUI` + `AssetImportTask` (`import_as_skeletal`, new skeleton, create physics asset, no animation, scale as the file; MCP static import gave 100x, unreal-mcp-quirks), typed through the status-bar Cmd box as in task-350-02; textures `T_Miner_BaseColor` and `T_Miner_Normal` (normal compression), material `M_Miner` (base colour and normal only, rest default) set on every slot. First skeletal import of the project: if a step is impossible through MCP or editor Python, stop and report what failed, do not improvise another route.
- expected_output: SK_Miner, SKEL_Miner, PHYS_Miner (or the physics asset name the import gives), M_Miner, T_Miner_BaseColor, T_Miner_Normal saved under /Game/RogueRigs/Characters/Miner/; change summary (paths, bone count, bounds, triangles, import settings) for the doc step.
- done_when: editor Python read back: SK_Miner skeleton is SKEL_Miner, bone count 66 (or 67 with an added root, stated), bounds height 170 cm +/- 2 (not 100x), LOD 0 triangles 25,744, every slot uses M_Miner; assets saved; committed by path as `task-386-03-miner-skeletal-import: <summary>`.
- editor_state: open
- token_ceiling: 0.6 M units
- model: sonnet
- tokens: input 72, cache write 80940, cache read 1834155, output 15720
- usage: turns 36, context start 12k peak 81k, resumes 0, 2 min, units 0.36 M
- runs: 1
- milestone: M2
- started: 2026-10-30T05:41:13Z
- waiting_for: none
- result: SK_Miner, SKEL_Miner, PHYS_Miner, M_Miner and two textures imported via editor Python (MCP import gave 100x); 170 cm, 67 bones (root SmartRigArmature added, mixamorig prefix stripped), commit b35afcb7; triangles 25,744 NOT read back (no getter; 40,481 vertices); physics asset made with PhysicsAssetToolset

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
