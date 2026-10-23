---
id: task-289-02-bake-chaser-v2-run
description: "Rebake the chaser v2 run clip with the chaser v1 run curves, attack rows kept (3d)."
parent: task-289-robot-run-v1-motion
kind: task
status: done
---
# Rebake the chaser v2 run with the v1 motion

- goal: Same as task-289-01 for the chaser v2 (speed 180 uu/s), with the settings 289-01 settled; keep the attack rows 16..30 byte-identical; do not rerun shield_chaser_v2_bake.py.
- agent: 3d
- depends_on: task-289-01-bake-heavy-run
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md) step 5, robot-parts-bake-pipeline (vault/reference/tech/pipelines/robot-parts-bake-pipeline.md); files: Tools/art/blender/meshy_bake_anim.py, robot_walk_preview.py, Art/Meshes/chaser-meshy/anim_report.json (v1 run), Tools/art/blender/robot_configs/chaser_v2.json, Art/Meshes/chaser-v2-meshy/
- expected_output: Art/Meshes/chaser-v2-meshy/T_ChaserV2Anim.exr (rows 0..15 new, 16..30 identical), chaser_v2.json bake updated, chaser_v2_meshy_anim_report.json, renders Saved/Renders/task-289/chaserv2_vs_v1_*.png, robot-parts-bake-pipeline chaser v2 section updated
- done_when: rows 16..30 max diff 0.0; v1 amplitudes in the report; knee at most 15 degrees; loop seamless; bake error under 0.05 cm; recommended RunRate with its foot slide; renders exist; Blender stopped; shield chaser v2 files and v1 files unchanged
- editor_state: closed (Blender headless only)
- token_ceiling: 50000
- model: sonnet
- tokens: input 30, cache write 41258, cache read 450376, output 7725
- runs: 1
- milestone: M2
- result: Chaser v2 clip 0 rebaked with the v1 sine run (thigh 35, arm 30, elbow 40, lean 8, twist 5, knee capped 15); rows 16..30 diff 0.0; bake error 0.040 cm; RunRate 0.7 (slide 79 cm/cycle vs min 72 at 0.5-0.6, v1 126.6; 0.7 = v1 cadence); arm clearance not reported; shield chaser v2 untouched; renders Saved/Renders/task-289/chaserv2_vs_v1_{side,threequarter}.png; commit 1de94932.

## Relations
- [[task-289-robot-run-v1-motion]] — parent global task.
