---
id: task-288-01-pieces-heavy
description: "Debris pieces of the heavy (3d)."
parent: task-288-robot-death-debris
kind: task
status: done
---
# Debris pieces of the heavy

- goal: Run a generalised chaser_debris_pieces.py (per-robot config, the chaser v1 output unchanged) on the heavy parts mesh: the 13 pieces of the chaser contract (Torso, UpperArm, Forearm, Arm, Thigh, LowerLeg, Leg, left and right), rest pose, cut caps kept, pivot at the bounds centre, UV0 only, eye faces kept with the body texture (pieces have no glow).
- agent: 3d
- depends_on: task-284-01-parts-bake-heavy
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md); files: Art/Meshes/heavy-meshy/heavy_meshy_parts.fbx (task-284), Art/Meshes/heavy-meshy/heavy_meshy_clean_basecolor.png (task-282); v1 Art/Source/chaser-meshy/debris/ read only
- expected_output: Art/Source/heavy-meshy/debris/SM_HeavyDebris_*.fbx, debris_pieces.json (order = Niagara mesh index, tris, centre_offset_cm_unreal per piece)
- done_when: piece count and order as listed, offsets mirrored left and right within 0.5 cm, UV0 only, FBX_SCALE_UNITS, v1 debris files unchanged (git diff); the Orchestrator renders the pieces laid out and sends the render to the owner; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: sonnet
- tokens: input 56, cache write 61190, cache read 1061705, output 19649
- runs: 2
- milestone: M2
- result: 13 heavy debris pieces Art/Source/heavy-meshy/debris/ + debris_pieces.json via chaser_debris_pieces.py --config (v1 output unchanged); UV0 only, no glow, FBX_SCALE_UNITS; mirror check misses 0.5 cm on 5 pairs (0.6 cm; forearm 6.4 cm) = the source mesh asymmetry exactly (mirror_vs_source block), no face mis-assigned; accepted (provisional, owner 2026-10-23); render Saved/Renders/task-288/heavy_debris_layout.png.

## Relations
- [[task-288-robot-death-debris]] — parent global task.
