---
id: task-283-03-game-ready-kamikaze
description: "Make the kamikaze mesh game ready (3d)."
parent: task-283-robot-mesh-integration
kind: task
status: todo
---
# Make the kamikaze mesh game ready

- goal: Run meshy_game_ready.py on the flattened, cleaned kamikaze mesh (centimetres, front +X, origin at the floor, LightMap UV1, height from the owner decision of the plan) and move its red eye faces into a last slot robot_eye with eye_slot_split.py (eye zone criteria read on this mesh, not the chaser values). Also split the core faces (faceted sphere with orange seams) into a separate mesh kamikaze_meshy_core.fbx with its pivot at the core centre, so the C++ CoreMesh pulse and growth keep working on it; report the core centre offset in Unreal axes.
- agent: 3d
- depends_on: task-282-04-texture-clean-chaser-v2
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md), enemy-kamikaze (vault/reference/gdd/enemies/enemy-kamikaze.md); files: Art/Meshes/kamikaze-meshy/kamikaze_meshy_flat.fbx, Art/Meshes/kamikaze-meshy/kamikaze_meshy_clean_basecolor.png
- expected_output: Art/Meshes/kamikaze-meshy/kamikaze_meshy_game.fbx (slots [body, robot_eye]) and Art/Meshes/kamikaze-meshy/kamikaze_meshy_game.report.json (dims cm, tris, slots, eye faces, eye texels left in body faces)
- done_when: dims match the decided height within 1 cm, origin at the floor, front +X, slots body then robot_eye, eye faces > 0 and red eye texels left in body faces 0, UV0 unchanged, LightMap UV1 without overlap; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-283-robot-mesh-integration]] — parent global task.
