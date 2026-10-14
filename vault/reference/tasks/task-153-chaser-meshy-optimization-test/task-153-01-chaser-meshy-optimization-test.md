---
id: task-153-01-chaser-meshy-optimization-test
description: "Test the optimisation of the Meshy chaser mesh (792154 triangles) down to the 3000 triangle budget with a baked texture."
parent: task-153-chaser-meshy-optimization-test
kind: task
status: in-progress
---
# Chaser: Meshy optimisation test

- goal: Test the optimisation of the Meshy chaser mesh (792154 triangles) down to the 3000 triangle budget with a baked texture.
- agent: 3d
- depends_on: none
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), agent-3d (vault/reference/agents/agent-3d.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md)
- expected_output: Blender headless script Tools/art/blender/meshy_optimize.py (decimate, new UV, bake) and its results in Art/Meshes/chaser-meshy/: optimised FBX at about 3000 and about 5000 triangles with their baked base colour and normal maps (2048 px, a smaller 1024 variant of the 3000 one), a report JSON (triangles, vertices, file sizes, timings, issues found), lit renders front, side, back, three-quarter and game scale (70 px tall at 50 degrees, x4) of the original and of each optimised version side by side, and a written assessment: what is lost (thin parts, silhouette, holes, floating pieces), what the bake keeps, what the fused single mesh means for the baked texture animation (parts cannot be animated separately) and for the flat-shaded style of the project (charter A1: flat colours, no normal map)
- done_when: The script runs headless; the 3000 triangle FBX has at most 3000 triangles, one object, one material and a clean UV; the renders and the report exist; the assessment states the numbers and the problems honestly; nothing imported in Unreal; the script and the report committed by name, the FBX, textures and renders left untracked until the owner decides
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-153-chaser-meshy-optimization-test]] — parent global task.
