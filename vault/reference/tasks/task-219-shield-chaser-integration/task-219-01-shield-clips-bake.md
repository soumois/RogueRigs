---
id: task-219-01-shield-clips-bake
description: "3D agent: shield-held run and shield bash poses, 4-clip bake texture, combined chaser plus shield animated mesh, posed previews."
parent: task-219-shield-chaser-integration
kind: task
status: in-progress
---
# Shield chaser: clips and combined mesh

- goal: `Art/Meshes/shield-chaser/T_ShieldChaserAnim.exr` (4 clips) and `SM_ShieldChaser_Meshy_Anim.fbx` (chaser parts plus the shield bound to ForearmHand_L).
- agent: agent-3d (Blender, no Meshy)
- depends_on: none
- inputs: `vault/reference/tech/pipelines/chaser-meshy-pipeline.md` (chain, bake format, part index in UV channel 2); `Tools/art/blender/meshy_bake_anim.py` (run_pose, attack_pose, bake, checks) and `Tools/art/blender/meshy_cut_parts.py`; `Art/Meshes/chaser-meshy/` (chaser_meshy_parts.fbx, T_ChaserAnim.exr, anim_report.json, chaser_meshy_fixed_basecolor.png); shield `Art/Meshes/ChaserShield/SM_ChaserShield.fbx` and its report (979 tris, 82 cm, front +X, bottom-centre pivot, grip_offset_cm, slots M_ChaserShield then M_ChaserShield_Emblem); `vault/reference/agents/art-3d-lessons.md`; look references `vault/assets/shield-turnaround.webp`, `vault/assets/chaser-turnaround.png`.
- method: new script `Tools/art/blender/shield_chaser_bake.py`, reusing the functions of meshy_bake_anim.py; never change T_ChaserAnim or the chaser files. Clips, same frame counts as the chaser, rows from the top:
  - clip 0, shield run, rows 0-15: legs as the chaser run (thigh 35 degrees unchanged); left arm in guard (upper arm forward and down, elbow about 90 degrees) so the shield stands in front of the torso, plate facing +X, slightly tilted back, bottom near knee height; torso and right arm as the chaser run with less swing.
  - clip 1, shield bash, rows 16-30: wind-up rows 16-18 pull the shield back to the chest, then thrust forward at arm's length with a torso lean, back to guard at row 30.
  - clips 2 and 3, rows 31-46 and 47-61: exact copies of T_ChaserAnim rows 0-15 (run) and 16-30 (attack).
  - texture 20 x 62 RGBA float16, same texel layout as T_ChaserAnim.
  - shield placement: set the shield where it must be in clip 0 frame 0 (plate centre in front of the torso, grip at the left forearm), then store its vertices in REST space through the inverse of the ForearmHand_L transform of that frame, so new = R v + t puts it back. Shield vertices get PartId (4 + 0.5) / 10 = 0.45 in UV channel 2 and the same part in the PartIndex colour.
  - combined mesh: chaser parts unchanged plus the shield; three slots in this order: chaser material slot, shield (shield base colour UV0), emblem (decal UV0); LightMap UV1 valid over the whole mesh; same units, origin and axes as SM_Chaser_Meshy_Anim (chaser_meshy_parts.fbx).
  - no shield intersection with the torso or legs in any frame of clips 0 and 1: check numerically, report the minimum distance.
- expected_output: script, EXR, FBX, report json, previews in `Art/Meshes/shield-chaser/`.
- done_when: report with tris (chaser parts + 979), clip rows, loop error of clip 0, minimum shield-to-body distance per clip, UV2 check (every shield corner 0.45, chaser parts unchanged); posed renders of clip 0 frames 0, 4, 8, 12 and clip 1 frames 16, 18, 22, 26 (three-quarter front view, the light of the shield previews) and one contact sheet in `Art/Meshes/shield-chaser/preview/`; the Orchestrator opens them.
- editor_state: any (no Unreal)
- token_ceiling: open
- model: sonnet
- tokens: input 42, cache write 112389, cache read 661533, output 1645
- runs: 1
- milestone: M1
- result: open

## Relations
- [[task-219-shield-chaser-integration]] — parent global task.
