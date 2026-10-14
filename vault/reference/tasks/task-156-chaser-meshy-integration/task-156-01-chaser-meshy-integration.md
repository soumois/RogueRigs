---
id: task-156-01-chaser-meshy-integration
description: "Put the corrected Meshy chaser in the game as a static mesh in place of the current chaser model."
parent: task-156-chaser-meshy-integration
kind: task
status: done
---
# Chaser Meshy: game-ready mesh

- goal: Put the corrected Meshy chaser in the game as a static mesh in place of the current chaser model.
- agent: 3d
- depends_on: none
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md)
- expected_output: Art/Meshes/chaser-meshy/chaser_meshy_game.fbx made from chaser_meshy_fixed.fbx (same geometry and UV) by the headless script Tools/art/blender/meshy_game_ready.py: centimetres, scaled uniformly to 123 cm tall, forward +X, left +Y, up +Z, origin at the floor under the centre of the feet midpoint, one object, one material slot named meshy_chaser, base colour texture chaser_meshy_fixed_basecolor.png next to it, flat shading kept as in the input, a LightMap second UV channel without overlap; a report JSON (bounds in cm, triangles, vertices, scale factor, forward axis check)
- done_when: Bounds read back from the FBX: z 0 to 123 (tolerance 0.5 cm), the robot front (the red eye) on +X, the feet midpoint at x 0, y 0; triangles equal to the input (3115); one object and one slot; the script runs headless; nothing imported in Unreal; the script and the report committed by name, FBX and textures untracked
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 24, cache write 50129, cache read 463620, output 420
- runs: 1
- milestone: M2
- result: game-ready FBX 123 cm, 3115 tris, front +X, origin at floor, LightMap UV1

## Relations
- [[task-156-chaser-meshy-integration]] — parent global task.
