---
id: task-288-03-pieces-chaser-v2
description: "Debris pieces of the chaser v2 (3d)."
parent: task-288-robot-death-debris
kind: task
status: in-progress
---
# Debris pieces of the chaser v2

- goal: Run a generalised chaser_debris_pieces.py (per-robot config, the chaser v1 output unchanged) on the chaser v2 parts mesh: the 13 pieces of the chaser contract, rest pose, cut caps kept, pivot at the bounds centre, UV0 only, eye faces kept with the body texture (pieces have no glow).
- agent: 3d
- depends_on: task-284-03-parts-bake-chaser-v2
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md); files: Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_parts.fbx (task-284), Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_clean_basecolor.png (task-282); v1 Art/Source/chaser-meshy/debris/ read only
- expected_output: Art/Source/chaser-v2-meshy/debris/SM_ChaserV2Debris_*.fbx, debris_pieces.json (order = Niagara mesh index, tris, centre_offset_cm_unreal per piece)
- done_when: piece count and order as listed, offsets mirrored left and right within 0.5 cm, UV0 only, FBX_SCALE_UNITS, v1 debris files unchanged (git diff); the Orchestrator renders the pieces laid out and sends the render to the owner; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-288-robot-death-debris]] — parent global task.
