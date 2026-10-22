---
id: task-283-04-game-ready-shooter
description: "Make the shooter mesh game ready (3d)."
parent: task-283-robot-mesh-integration
kind: task
status: done
---
# Make the shooter mesh game ready

- goal: Run meshy_game_ready.py on the flattened, cleaned shooter mesh (centimetres, front +X, origin at the floor, LightMap UV1, height (provisional, owner 2026-10-22): chaser v2 123 cm, heavy 248, shooter 176, kamikaze 194) and move its red eye faces into a last slot robot_eye with eye_slot_split.py (eye zone criteria read on this mesh, not the chaser values). Report the cannon tip position in Unreal axes (cm, relative to the floor origin) for MuzzlePoint.
- agent: 3d
- depends_on: task-282-04-texture-clean-chaser-v2
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), agent-3d (vault/reference/agents/agent-3d.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), bp-robot-eye-material (vault/reference/tech/blueprints/enemies/bp-robot-eye-material.md), enemy-shooter (vault/reference/gdd/enemies/enemy-shooter.md); files: Art/Meshes/shooter-meshy/shooter_meshy_flat.fbx, Art/Meshes/shooter-meshy/shooter_meshy_clean_basecolor.png
- expected_output: Art/Meshes/shooter-meshy/shooter_meshy_game.fbx (slots [body, robot_eye]) and Art/Meshes/shooter-meshy/shooter_meshy_game.report.json (dims cm, tris, slots, eye faces, eye texels left in body faces)
- done_when: dims match the decided height within 1 cm, origin at the floor, front +X, slots body then robot_eye, eye faces > 0 and red eye texels left in body faces 0, UV0 unchanged, LightMap UV1 without overlap; Blender processes stopped
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: sonnet
- tokens: input 18, cache write 34091, cache read 248705, output 428
- runs: 1
- milestone: M2
- result: shooter_meshy_game.fbx: 82.6 x 131.5 x 176.0 cm, floor origin, front +X, slots [body, robot_eye], 83 eye faces, 0 red texels left in eye zone (cannon glow kept in body), UV0 delta 0, UV1 no overlap; cannon tip Unreal (45.7, 50.4, 70.6) cm.

## Relations
- [[task-283-robot-mesh-integration]] — parent global task.
