---
id: task-278-03-eye-slot-reimport
description: "SM_Chaser_Meshy_Anim and SM_ShieldChaser_Meshy_Anim reimported with the eye slot, slot order and PartId channel intact."
parent: task-278-chaser-eye-glow
kind: task
status: todo
---
# SM_Chaser_Meshy_Anim and SM_ShieldChaser_Meshy_Anim reimported with the eye slot, slot order and PartId channel intact

- goal: SM_Chaser_Meshy_Anim and SM_ShieldChaser_Meshy_Anim reimported with the eye slot, slot order and PartId channel intact.
- agent: cpp
- depends_on: task-278-02-eye-slot-split
- inputs: tool-unreal-reimport (vault/reference/tech/pipelines/tool-unreal-reimport.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md)
- expected_output: Editor closed. Before: check_mesh_channels.py on /Game/RogueRigs/Art/Chaser/SM_Chaser_Meshy_Anim and /Game/RogueRigs/Art/ShieldChaser/SM_ShieldChaser_Meshy_Anim (keep the lines). Reimport both with Tools/unreal/reimport_static_mesh.py (check the stored source is the FBX changed in 278-02), then static_mesh_build_settings.py, then check_uv_channel.py and check_mesh_channels.py again. Report slot names and ORDER with their materials, bounds and bounds extension (40 cm chaser, 60 cm shield), UV channel lines before vs after, UVCHECK off_bin and mixed. No other asset may change (git status; restore unchanged .uasset files).
- done_when: Slot order chaser [meshy_chaser, robot_eye], shield [meshy_chaser, M_ChaserShield, M_ChaserShield_Emblem, robot_eye] with the old slot materials kept; UVCHECK off_bin 0 and mixed 0 on both; build settings (no lightmap generation, full precision, LightMapCoordinateIndex 1) read back; bounds extension unchanged; chaser-meshy-pipeline updated (before, after, revert = git checkout 2ad7698a of the two .uasset and FBX); committed. If slot order differs or channel 2 broke, restore the .uasset files with git and stop.
- editor_state: closed
- token_ceiling: 120k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-278-chaser-eye-glow]] — parent global task.
