---
id: task-369-01-bp-extractor-import
description: "Bp: import the extractor body, four rotors, four textures and one material into /Game/RogueRigs/Art/Extractor/."
parent: task-369-extractor-level-exit
kind: task
status: todo
---
# Extractor in game: the level exit you walk up to: bp

- goal: The extractor body and rotor meshes, textures and material exist in Unreal, at the right scale and pivots.
- agent: bp
- depends_on: task-367-01-extractor-rotors-cut
- inputs: art-prompts-extractor (vault/reference/gdd/art-audio/art-prompts-extractor.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp (vault/reference/tech/pipelines/unreal-mcp.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), task-367-extractor-rotors (vault/reference/tasks/task-367-extractor-rotors.md), task-368-extractor-rebake (vault/reference/tasks/task-368-extractor-rebake.md), Art/Meshes/extractor/SM_Extractor_Body.fbx, Art/Meshes/extractor/SM_Extractor_Rotor_FL.fbx, _FR.fbx, _RL.fbx, _RR.fbx, Art/Meshes/extractor/T_Extractor_BaseColor|Metallic|Normal|Roughness.png, Art/Meshes/extractor/extractor_rotors.report.json (rotor pivots)
- expected_output: /Game/RogueRigs/Art/Extractor/: SM_Extractor_Body, SM_Extractor_Rotor_FL|FR|RL|RR, T_Extractor_BaseColor|Metallic|Normal|Roughness, M_Extractor (or an instance of an existing lit master) on every mesh; one commit; change summary (paths, bounds, triangle counts, pivots, material parameters).
- done_when: the 5 meshes load; union of their bounds placed at the report pivots within 1 cm of 196.6 x 201.0 x 86.1 cm (no 100x scale error); each rotor origin on its spin axis per the report; total triangles at most 50,000; normal texture green channel flipped (OpenGL source), metallic and roughness linear; material assigned to every slot; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-369-extractor-level-exit]] — parent global task.
