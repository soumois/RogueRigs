---
id: task-296-01-bake-shield-arm-forward
description: "Rebake the shield chaser v2 shield clips with the forearm held forward and the shield about 30 cm forward (3d)."
parent: task-296-shield-v2-arm-forward
kind: task
status: todo
---
# Bake the shield held forward

- goal: In shield_chaser_v2_bake.py: guard pose with the left forearm held forward (roughly horizontal, pointing ahead) and the shield moved at least 30 cm forward of its task-291 place (grip offset if the arm is too short); bash clip from that guard; rows 31..61 byte-identical; renders before/after side and top to Saved/Renders/task-296/.
- agent: 3d
- depends_on: none (task-291 done first)
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), robot-parts-bake-pipeline (vault/reference/tech/pipelines/robot-parts-bake-pipeline.md), bp-shield-chaser-v2 (vault/reference/tech/blueprints/enemies/bp-shield-chaser-v2.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md); files: Tools/art/blender/shield_chaser_v2_bake.py, Art/Meshes/shield-chaser-v2/, task-291-02 report
- expected_output: SM_ShieldChaserV2_Meshy_Anim.fbx, T_ShieldChaserV2Anim.exr, report json, renders, pipeline shield chaser v2 section updated
- done_when: forward offset of the shield centre vs task-291 reported (at least 30 cm); minimum shield to foot and leg distance over clip 0 reported (above 1 cm); rows 31..61 max diff 0.0; bake error reported; renders exist; Blender stopped
- editor_state: any (Blender headless only)
- token_ceiling: 45000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-296-shield-v2-arm-forward]] — parent global task.
