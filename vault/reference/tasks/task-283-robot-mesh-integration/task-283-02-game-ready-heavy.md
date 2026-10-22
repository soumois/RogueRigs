---
id: task-283-02-game-ready-heavy
description: "Make the heavy mesh game ready (3d)."
parent: task-283-robot-mesh-integration
kind: task
status: done
---
# Make the heavy mesh game ready

- goal: Run meshy_game_ready.py on the flattened, cleaned heavy mesh (centimetres, front +X, origin at the floor, LightMap UV1, height (provisional, owner 2026-10-22): chaser v2 123 cm, heavy 248, shooter 176, kamikaze 194) and move its red eye faces into a last slot robot_eye with eye_slot_split.py (eye zone criteria read on this mesh, not the chaser values).
- agent: 3d
- depends_on: task-282-04-texture-clean-chaser-v2
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md); files: Art/Meshes/heavy-meshy/heavy_meshy_flat.fbx, Art/Meshes/heavy-meshy/heavy_meshy_clean_basecolor.png
- expected_output: Art/Meshes/heavy-meshy/heavy_meshy_game.fbx (slots [body, robot_eye]) and Art/Meshes/heavy-meshy/heavy_meshy_game.report.json (dims cm, tris, slots, eye faces, eye texels left in body faces)
- done_when: dims match the decided height within 1 cm, origin at the floor, front +X, slots body then robot_eye, eye faces > 0 and red eye texels left in body faces 0, UV0 unchanged, LightMap UV1 without overlap; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: sonnet
- tokens: input 26, cache write 73446, cache read 586600, output 8959
- runs: 1
- milestone: M2
- result: heavy_meshy_game.fbx via new Tools/art/blender/robot_game_ready_eye.py: 137.9 x 278.1 x 248.0 cm, floor origin, front +X, slots [body, robot_eye], 105 eye faces, 0 red texels left in body faces in the eye zone (69 stray red texels elsewhere), UV0 delta 0, UV1 0 overlaps; tris 10361.

## Relations
- [[task-283-robot-mesh-integration]] — parent global task.
