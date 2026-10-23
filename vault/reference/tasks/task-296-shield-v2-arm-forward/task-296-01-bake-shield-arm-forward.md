---
id: task-296-01-bake-shield-arm-forward
description: "Rebake the shield chaser v2 shield clips with the forearm held forward and the shield about 30 cm forward (3d)."
parent: task-296-shield-v2-arm-forward
kind: task
status: done
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
- model: sonnet
- tokens: input 66, cache write 118642, cache read 2473643, output 33196
- runs: 2
- milestone: M2
- result: Shield clips 0-1: forearm about horizontal (0 to -3 deg), arm at max reach, hand joined to shield (gap 0.23 cm, grip offset X 17 Z 14); shield centre +33.2 to +34.7 cm forward on run, +26.8 to +31.7 on bash (wind-up below 30); min shield-leg 43.6 cm run, 34.8 bash (was 2.64, 6.34); rows 31-61 diff 0.0; 7134 tris; bake error 0.068/0.061 cm (above 0.05 warning); renders Saved/Renders/task-296/; first pass floated the shield 8 cm off the hand, reworked; commit 1e8a429b.

## Relations
- [[task-296-shield-v2-arm-forward]] — parent global task.
