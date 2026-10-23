---
id: task-291-02-bake-shield-v2-v1-clips
description: "Rebake T_ShieldChaserV2Anim: shield run and bash = shield chaser v1 clips on the v2 parts, plain clips = new chaser v2 rows (3d)."
parent: task-291-chaser-v2-v1-anims
kind: task
status: in-progress
---
# Bake the shield chaser v1 clips on shield chaser v2

- goal: With shield_chaser_v2_bake.py: clip 0 shield run and clip 1 shield bash = the shield chaser v1 clips (shield_chaser_bake.py poses, guard, wind-up and strike targets, v1 run curve with no knee cap) on the v2 parts; clips 2 and 3 = exact copy of the 291-01 T_ChaserV2Anim rows 0..30; same RunRate as 291-01; shield size and grip as task-286 unless the deeper knee makes a leg touch the shield (report the minimum distance per clip; if below 1 cm, lowest grip change that clears it, reported). No forward shield offset here (task-296 does it). Renders shield v1 top / shield v2 bottom to Saved/Renders/task-291/.
- agent: 3d
- depends_on: task-291-01-bake-chaser-v2-v1-clips
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), robot-parts-bake-pipeline (vault/reference/tech/pipelines/robot-parts-bake-pipeline.md), bp-shield-chaser-v2 (vault/reference/tech/blueprints/enemies/bp-shield-chaser-v2.md); files: Tools/art/blender/shield_chaser_v2_bake.py, shield_chaser_bake.py (read only), Art/Meshes/shield-chaser/, Art/Meshes/shield-chaser-v2/, report of 291-01
- expected_output: Art/Meshes/shield-chaser-v2/T_ShieldChaserV2Anim.exr (20 x 62), SM_ShieldChaserV2_Meshy_Anim.fbx only if the geometry changed, report json, renders, pipeline shield chaser v2 section updated
- done_when: clips 0 and 1 follow the v1 shield clips (parameters reported); rows 31..61 max diff 0.0 against 291-01 rows 0..30; leg-shield minimum distance reported; foot slide and bake error reported; renders exist; Blender stopped; shield_chaser_bake.py and v1 files unchanged
- editor_state: any (Blender headless only)
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-291-chaser-v2-v1-anims]] — parent global task.
