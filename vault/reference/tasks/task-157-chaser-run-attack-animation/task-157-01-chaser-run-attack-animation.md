---
id: task-157-01-chaser-run-attack-animation
description: "Animate the Meshy chaser in the game with a run loop and an attack clip, using a baked animation texture played by the material."
parent: task-157-chaser-run-attack-animation
kind: task
status: todo
---
# Chaser: cut into rigid parts

- goal: Animate the Meshy chaser in the game with a run loop and an attack clip, using a baked animation texture played by the material.
- agent: 3d
- depends_on: none
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), chaser-model-pipeline (vault/reference/tech/pipelines/chaser-model-pipeline.md)
- expected_output: Art/Meshes/chaser-meshy/chaser_meshy_parts.fbx made from chaser_meshy_game.fbx by the headless script Tools/art/blender/meshy_cut_parts.py: the mesh cut at the joint planes into 10 rigid parts (HeadTorso, Pelvis, UpperArm_L/R, ForearmHand_L/R, Thigh_L/R, ShinFoot_L/R) named as in chaser-model-pipeline, each with its pivot at its joint (shoulder, elbow, hip, knee, hip height for the pelvis, waist for the head and torso), every cut closed by a cap that takes its colour from the neighbouring texels, same UV, same texture, the combined result still showing the same silhouette as the uncut mesh; a part index per vertex stored for the material (vertex colour red channel = index / 9); a report JSON (joints in cm found on the mesh, tris and vertices per part, cap faces added, hierarchy, silhouette difference per view against the uncut mesh)
- done_when: Ten objects with the contract names and parents; pivots at the joints with the positions in the report; triangles at most 3300 in total (cap faces included; report the number); silhouette difference per view at most 1 percent of the height squared area (front, side, back); the texture unchanged; nothing imported in Unreal; the script and the report committed by name, FBX untracked
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-157-chaser-run-attack-animation]] — parent global task.
