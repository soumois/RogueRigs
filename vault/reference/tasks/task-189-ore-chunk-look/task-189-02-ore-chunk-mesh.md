---
id: task-189-02-ore-chunk-mesh
description: "3D: Meshy mesh of the ore chunk from the owner-chosen image, cleaned in Blender, 30 cm, centred pivot, FBX and report."
parent: task-189-ore-chunk-look
kind: task
status: done
---
# Ore chunk: Meshy mesh

- goal: three Meshy meshes of the ore chunk (owner 2026-10-15: candidate C plus two variations, one mesh each, single front image per mesh, no side or back views), from vault/assets/ore-chunk-c1.png, ore-chunk-c2.png and ore-chunk-c3.png (one mesh each, SM_OreChunk_01 to _03), cleaned headless in Blender: about 30 cm across like the XP nut footprint, pivot at the geometric centre (the pickup root is placed by SetActorLocation, see bp-pickup-xp), crystal faces split into material slots for a bright and a dark violet glow (plus a body slot if the image has a base), triangle budget 300 target, 500 max per variant (provisional, owner 2026-10-15, asset-pipeline). Update 2026-10-15: the owner generated the three meshes himself on the Meshy web app (no API run, no MESHY_API_KEY on this machine); raw FBX in Art/Source/ore-chunk/meshy-web/ (untracked): 1015220914 = c1 (396 tris), 1015220901 = c2 (398 tris), 1015223725 = c3 (496 tris, accepted by the owner: at most about 20 chunks on screen); no decimation needed, cleanup only.
- agent: 3d
- depends_on: task-189-01-ore-chunk-look (owner-chosen image in vault/assets/)
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), color-code (vault/reference/gdd/art-audio/color-code.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md); the chosen image (name given at delegation)
- expected_output: Art/Meshes/OreChunk/SM_OreChunk_01.fbx, _02 and _03 in centimetres with one report.json each, the Meshy raw files and run log under Art/Source/ore-chunk/.
- done_when: FBX and report exist; three FBX each within 500 tris, dimensions about 30 cm, pivot centred, slots listed; the Orchestrator renders a preview and sends it to the owner; the owner accepts the mesh (one return to the agent at most before asking him); committed by path.
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 48, cache write 71404, cache read 577082, output 1837
- runs: 2
- milestone: M1
- result: Owner made the 3 Meshy meshes on the web app (no API key here); cleaned by Tools/art/blender/ore_chunk.py, no decimation: SM_OreChunk_01 (c1) 396 tris, _02 (c2) 398, _03 (c3) 496, largest side 30 cm, centred pivot, flat shading, slots crystal_bright/crystal_dark by base-colour luminance median, LightMap UV1; c2 not watertight (34 non-manifold edges), kept as is by the owner; preview checked by the owner; commits dbb9e81b, 53539bcb

## Relations
- [[task-189-ore-chunk-look]] — parent global task.
