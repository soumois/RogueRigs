---
id: task-306-01-chest-flatten
description: "Flatten the chest faces after the sheet (3d): bonus chest mesh with flat faces, step 01."
parent: task-306-chest-flat-faces
kind: task
status: in-progress
---
# Flatten the chest faces after the sheet

- goal: Blender route from the owner Meshy FBX: make every large facet planar (body panels, lid, feet, lock block, handles) after the front, side and back views of the sheet, keep the bevelled edges and the Meshy texture (re-bake if the UVs move); at most 3000 tris (default Q2); FBX_SCALE_UNITS; preview renders front, side, three-quarter and top-down with fixed camera and light, side by side with the sheet; report with tris before and after and a planarity number per large face. The Orchestrator sends the renders to the owner and waits for his go.
- agent: 3d
- depends_on: none
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), art-prompts-chest (vault/reference/gdd/art-audio/art-prompts-chest.md); files: Art/Source/chest/meshy/Meshy_AI_Black_Tactical_Storag_1023224033_texture.fbx with _texture.png, _normal.png, _metallic.png, _roughness.png, _metallic_roughness.png; vault/assets/chest-turnaround.webp
- expected_output: Art/Source/chest/clean/chest_flat.fbx, cleanup script, renders, report
- done_when: faces flat in the renders and by the planarity numbers; tris within budget; owner shown the renders and gave his go; no Blender process left running; committed
- editor_state: any
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-306-chest-flat-faces]] — parent global task.
