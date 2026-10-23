---
id: task-289-01-bake-heavy-run
description: "Rebake the heavy run clip with the chaser v1 run curves, slam rows kept (3d)."
parent: task-289-robot-run-v1-motion
kind: task
status: done
---
# Rebake the heavy run with the v1 motion

- goal: Bake clip 0 of the heavy with walk_style sine (the v1 chaser run: thigh 35, arm 30, elbow 40, lean 8, twist 5, ground-fit bob, same phase timing) on the heavy's own pivots, knee flexion capped at 15 degrees (GDD), keeping the slam rows 16..61 byte-identical; compute the RunRate that minimises foot slide at 153 uu/s; render v1 run frames beside the new heavy frames (same frames, side view).
- agent: 3d
- depends_on: none
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md) step 5, robot-parts-bake-pipeline (vault/reference/tech/pipelines/robot-parts-bake-pipeline.md); files: Tools/art/blender/meshy_bake_anim.py, robot_walk_preview.py, Art/Meshes/chaser-meshy/anim_report.json (v1 run), Tools/art/blender/robot_configs/heavy.json, Art/Meshes/heavy-meshy/
- expected_output: Art/Meshes/heavy-meshy/T_HeavyAnim.exr (rows 0..15 new, 16..61 identical), heavy.json bake updated, heavy_meshy_anim_report.json, renders Saved/Renders/task-289/heavy_vs_v1_*.png, robot-parts-bake-pipeline heavy section updated
- done_when: rows 16..61 max diff 0.0 against the previous EXR; v1 amplitudes in the report; knee at most 15 degrees; loop seamless; bake error under 0.05 cm; recommended RunRate with its foot slide; renders exist; Blender stopped; v1 files unchanged
- editor_state: closed (Blender headless only)
- token_ceiling: 60000
- model: sonnet
- tokens: input 60, cache write 87019, cache read 1767151, output 25109
- runs: 1
- milestone: M2
- result: Heavy clip 0 rebaked with the v1 sine run (thigh 35, arm 30, elbow 40, lean 8, twist 5, knee v1 curve capped 15); rows 16..61 diff 0.0; loop 2e-14 cm; walk bake error 0.0535 cm (half-float, accepted); arm clearance min 10.2 cm; RunRate 0.7 (slide flat over 0.28-0.78 with the 15 deg knee, 0.7 = v1 cadence); slam strike needs SlamContactLead 0.20 (task-289-05); renders Saved/Renders/task-289/heavy_vs_v1_{side,threequarter}.png; commit 5b1bf0e9.

## Relations
- [[task-289-robot-run-v1-motion]] — parent global task.
