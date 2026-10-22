---
id: task-288-02-pieces-shooter
description: "Debris pieces of the shooter (3d)."
parent: task-288-robot-death-debris
kind: task
status: done
---
# Debris pieces of the shooter

- goal: Run a generalised chaser_debris_pieces.py (per-robot config, the chaser v1 output unchanged) on the shooter parts mesh: the 13 pieces of the chaser contract (the cannon stays on the right forearm piece), rest pose, cut caps kept, pivot at the bounds centre, UV0 only, eye faces kept with the body texture (pieces have no glow).
- agent: 3d
- depends_on: task-284-02-parts-bake-shooter
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md); files: Art/Meshes/shooter-meshy/shooter_meshy_parts.fbx (task-284), Art/Meshes/shooter-meshy/shooter_meshy_clean_basecolor.png (task-282); v1 Art/Source/chaser-meshy/debris/ read only
- expected_output: Art/Source/shooter-meshy/debris/SM_ShooterDebris_*.fbx, debris_pieces.json (order = Niagara mesh index, tris, centre_offset_cm_unreal per piece)
- done_when: piece count and order as listed, offsets mirrored left and right within 0.5 cm, UV0 only, FBX_SCALE_UNITS, v1 debris files unchanged (git diff); the Orchestrator renders the pieces laid out and sends the render to the owner; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: sonnet
- tokens: input 62, cache write 92359, cache read 955573, output 15830
- runs: 1
- milestone: M2
- result: 13 shooter debris pieces Art/Source/shooter-meshy/debris/ + debris_pieces.json (cannon on Forearm_R); mirror_vs_source: all misses = source asymmetry or the cannon; v1 and heavy debris unchanged; tools take --robot; render Saved/Renders/task-288/shooter_debris_layout.png.

## Relations
- [[task-288-robot-death-debris]] — parent global task.
