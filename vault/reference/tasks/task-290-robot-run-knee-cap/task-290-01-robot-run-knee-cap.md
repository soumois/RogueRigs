---
id: task-290-01-robot-run-knee-cap
description: "Rebake the heavy run clip with the knee on the v1 curve capped at 35 degrees, slam rows kept (3d)."
parent: task-290-robot-run-knee-cap
kind: task
status: done
---
# Rebake the heavy run, knee cap 35

- goal: Rebake clip 0 of the heavy exactly as task-289-01 (sine v1 run) but with the knee following the v1 curve capped at 35 degrees (was 15; provisional, owner 2026-10-23); slam rows 16..61 byte-identical; measure the knee cut gap (pivot 13.5 cm above the cut, task-284) and part interpenetration over the 16 frames; foot slide per cycle for RunRate 0.4..1.0; side-by-side renders v1 top / heavy bottom, frames 0/4/8/12, side and three-quarter.
- agent: 3d
- depends_on: none
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), robot-parts-bake-pipeline (vault/reference/tech/pipelines/robot-parts-bake-pipeline.md); files: Tools/art/blender/meshy_bake_anim.py, robot_walk_preview.py, Tools/art/blender/robot_configs/heavy.json, Art/Meshes/chaser-meshy/anim_report.json (v1 run), Art/Meshes/heavy-meshy/, task-289-01 node
- expected_output: Art/Meshes/heavy-meshy/T_HeavyAnim.exr (rows 0..15 new, 16..61 identical), heavy.json, heavy_meshy_anim_report.json, renders Saved/Renders/task-290/heavy_vs_v1_*.png, robot-parts-bake-pipeline heavy section updated
- done_when: rows 16..61 max diff 0.0; knee peak reported and at most 35 degrees; knee cut gap and part interpenetration measured; foot slide per cycle reported; loop seamless; bake error reported; renders exist; Blender stopped; v1 files unchanged
- editor_state: any (Blender headless only)
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: Cancelled before running (nothing baked or imported): the owner changed his mind on 2026-10-23, replaced by task-291-chaser-v2-v1-anims.

## Relations
- [[task-290-robot-run-knee-cap]] — parent global task.
