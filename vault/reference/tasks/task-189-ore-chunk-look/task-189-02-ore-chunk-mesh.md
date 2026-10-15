---
id: task-189-02-ore-chunk-mesh
description: "3D: Meshy mesh of the ore chunk from the owner-chosen image, cleaned in Blender, 30 cm, centred pivot, FBX and report."
parent: task-189-ore-chunk-look
kind: task
status: todo
---
# Ore chunk: Meshy mesh

- goal: a Meshy mesh of the ore chunk from the image the owner chose in task-189-01, cleaned headless in Blender: about 30 cm across like the XP nut footprint, pivot at the geometric centre (the pickup root is placed by SetActorLocation, see bp-pickup-xp), crystal faces split into material slots for a bright and a dark violet glow (plus a body slot if the image has a base), triangle count reported (provisional budget: same order as the XP chip, 844 tris; report it, the owner decides).
- agent: 3d
- depends_on: task-189-01-ore-chunk-look (owner-chosen image in vault/assets/)
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), color-code (vault/reference/gdd/art-audio/color-code.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md); the chosen image (name given at delegation)
- expected_output: Art/Meshes/OreChunk/SM_OreChunk.fbx in centimetres, SM_OreChunk.report.json, the Meshy raw files and run log under Art/Source/ore-chunk/.
- done_when: FBX and report exist; dimensions about 30 cm, pivot centred, slots listed; the Orchestrator renders a preview and sends it to the owner; the owner accepts the mesh (one return to the agent at most before asking him); committed by path.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-189-ore-chunk-look]] — parent global task.
