---
id: task-291-01-bake-chaser-v2-v1-clips
description: "Rebake T_ChaserV2Anim rows 0..30 with the exact chaser v1 run and attack curves on the v2 parts (3d)."
parent: task-291-chaser-v2-v1-anims
kind: task
status: todo
---
# Bake the chaser v1 clips on chaser v2

- goal: Rebake clip 0 (run, rows 0..15) and clip 1 (attack, rows 16..30) of the chaser v2 with exactly the chaser v1 curves and keys (as Art/Meshes/chaser-meshy/anim_report.json: sine run thigh 35, arm 30, elbow 40, lean 8, twist 5, v1 knee curve with no cap, v1 attack keys), adapted only to the v2 pivots; RunRate 0.7 as v1 unless the foot slide table (0.4..1.0) clearly favours another value; measure knee peak, knee cut gap and part interpenetration over every frame; renders v1 top / v2 bottom, run frames 0/4/8/12 and attack 18/22, side and three-quarter, to Saved/Renders/task-291/. Do not touch the heavy, shooter, kamikaze or v1 files.
- agent: 3d
- depends_on: none
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), robot-parts-bake-pipeline (vault/reference/tech/pipelines/robot-parts-bake-pipeline.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md); files: Tools/art/blender/meshy_bake_anim.py, robot_walk_preview.py, robot_configs/chaser_v2.json, Art/Meshes/chaser-meshy/anim_report.json, Art/Meshes/chaser-v2-meshy/
- expected_output: Art/Meshes/chaser-v2-meshy/T_ChaserV2Anim.exr (20 x 31), chaser_v2.json, chaser_v2_meshy_anim_report.json, renders, robot-parts-bake-pipeline chaser v2 section updated (with revert command)
- done_when: run and attack curves equal to v1 (report the per-part parameters side by side); knee peak reported; knee gap and interpenetration measured; foot slide per cycle reported for RunRate 0.4..1.0 and a RunRate recommended; loop seamless; bake error reported; renders exist; Blender stopped; v1 and other robots files unchanged
- editor_state: any (Blender headless only)
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-291-chaser-v2-v1-anims]] — parent global task.
