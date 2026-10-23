---
id: task-290-02-bake-chaser-v2-knee
description: "Rebake the chaser v2 run clip with the knee on the v1 curve capped at 45 degrees, attack rows kept (3d)."
parent: task-290-robot-run-knee-cap
kind: task
status: done
---
# Rebake the chaser v2 run, knee cap 45

- goal: Same as task-290-01 for the chaser v2 (180 uu/s), knee cap 45 degrees (provisional, owner 2026-10-23); attack rows 16..30 byte-identical; do not rerun shield_chaser_v2_bake.py.
- agent: 3d
- depends_on: task-290-01-robot-run-knee-cap
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), robot-parts-bake-pipeline (vault/reference/tech/pipelines/robot-parts-bake-pipeline.md); files: meshy_bake_anim.py, robot_walk_preview.py, robot_configs/chaser_v2.json, Art/Meshes/chaser-v2-meshy/, report of 290-01
- expected_output: Art/Meshes/chaser-v2-meshy/T_ChaserV2Anim.exr (rows 0..15 new), chaser_v2.json, chaser_v2_meshy_anim_report.json, Saved/Renders/task-290/chaserv2_vs_v1_*.png, pipeline chaser v2 section updated
- done_when: rows 16..30 max diff 0.0; knee peak at most 45; knee gap and interpenetration measured; foot slide reported; renders exist; Blender stopped; shield chaser v2 and v1 files unchanged
- editor_state: any (Blender headless only)
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: Cancelled before running (nothing baked or imported): the owner changed his mind on 2026-10-23, replaced by task-291-chaser-v2-v1-anims.

## Relations
- [[task-290-robot-run-knee-cap]] — parent global task.
