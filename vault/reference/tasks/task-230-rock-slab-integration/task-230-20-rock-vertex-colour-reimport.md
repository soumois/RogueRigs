---
id: task-230-20-rock-vertex-colour-reimport
description: "Reimport the 15 rock slab meshes with their vertex colours (rim, top, side codes), keeping UV0-UV3 and build settings."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# Rock slab meshes reimported with vertex colours

- goal: the SM_RockSlab* and SM_RockSlabBroken* meshes carry the vertex colours of their FBX (R rim/top, G sides, B bottoms; report json files) so M_RockSlab draws the lighter rim band decided by the owner (2026-10-17, rim as vertex colour in one material); found in task-230-18: the import used VertexColorImportOption Ignore, so every vertex read white. Runs before task-230-19.
- agent: cpp
- depends_on: task-230-18-rock-material-diagnosis
- inputs: vault/reference/tech/pipelines/tool-unreal-reimport.md, vault/reference/agents/art-3d-lessons.md (lightmap UV trap), vault/reference/tech/blueprints/world/bp-rock-slab-material.md; Art/Meshes/rock-slab/SM_RockSlab.report.json, Art/Meshes/rock-slab/SM_RockSlabBroken.report.json; assets /Game/RogueRigs/Environment/Rock/SM_RockSlab_<Shape>_01 and SM_RockSlabBroken_<Shape>_{01,02}
- expected_output: the 15 meshes reimported with vertex colours (Replace), same bounds, Nanite on, slot overrides kept; static_mesh_build_settings.py rerun (no lightmap UV generation, full precision UVs); check_uv_channel.py or an equivalent read shows UV2/UV3 block data intact on the broken meshes and vertex colours present; any tool change kept in Tools/unreal/ and documented in tool-unreal-reimport.
- done_when: per-mesh REIMPORT lines ok; vertex colours read back non-white on a rim piece; UV2/UV3 check passes; editor closed; committed.
- editor_state: closed
- token_ceiling: 60000
- model: opus
- tokens: input 66, cache write 100350, cache read 1926796, output 3761
- runs: 1
- milestone: M2
- result: Commit 87c9d927. 15 meshes reimported with FBX vertex colours (Replace) via reimport_static_mesh.py --vertex-colors=replace; build settings rerun (no lightmap UVs, full precision); UV0-UV3 identical before and after (check_mesh_channels.py, new); Nanite on, slots MI_RockSlab; colours read back as in the reports (Edge_01: 12 rim, 24 side, 7 top tris). Asset bounds of 10 broken meshes grew up to about 9 uu (Nanite cluster bounds, geometry unchanged): culling only, accepted.

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
