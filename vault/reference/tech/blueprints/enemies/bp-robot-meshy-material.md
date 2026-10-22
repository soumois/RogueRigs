---
id: bp-robot-meshy-material
description: "M_RobotMeshy master, body and eye instances for heavy, kamikaze, shooter and chaser v2: assets, values, revert. Open before changing a robot body colour."
parent: blueprints-enemies-index
kind: doc
---
# M_RobotMeshy and the robot body and eye instances

- asset_path: /Game/RogueRigs/Materials/Robots/ (M_RobotMeshy, MI_Heavy_Body, MI_Kamikaze_Body, MI_Shooter_Body, MI_ChaserV2_Body, MI_RobotEye_Heavy, MI_RobotEye_Kamikaze, MI_RobotEye_Shooter, MI_RobotEye_ChaserV2). Created by task-283-06; MI_Heavy_Body and MI_RobotEye_Heavy are assigned to SM_Heavy_Meshy since task-283-07 (see [[bp-heavy]]); MI_Kamikaze_Body and MI_RobotEye_Kamikaze are assigned to SM_Kamikaze_Meshy and MI_Kamikaze_Body to SM_Kamikaze_Core_Meshy since task-283-08 (see [[bp-kamikaze]]); MI_Shooter_Body and MI_RobotEye_Shooter are assigned to SM_Shooter_Meshy since task-283-09 (see [[bp-shooter]]); ChaserV2 not yet.
- parent_class: none (material assets; instances are MaterialInstanceConstant of M_RobotMeshy or M_RobotEye).
- purpose: one lit body material for the `body` slot of the four new robot meshes, base colour from the Meshy texture only; the eye instances give each robot its `robot_eye` slot.
- components: M_RobotMeshy: lit, opaque. Base colour = BaseColorTex RGB (texture parameter). Roughness constant 0.6, metallic constant 0, no normal, metallic or roughness map. WPO = Animated ? MF_RobotAnimWPO(AnimTex, ClipOffset, RunRate) : (0,0,0); Animated is a static switch, default off. Parameters: BaseColorTex, AnimTex, ClipOffset, RunRate, Animated (plus ClipId and AnimStart inside the function). Compiled without error.
- exposed_defaults: M_RobotMeshy: BaseColorTex T_Chaser_Meshy_BaseColor (placeholder), AnimTex T_ChaserAnim, ClipOffset 0, RunRate 0.7, Animated false. Body instances: BaseColorTex only (Heavy = T_Heavy_Meshy_BaseColor, Kamikaze = T_Kamikaze_Meshy_BaseColor, Shooter = T_Shooter_Meshy_BaseColor, ChaserV2 = T_ChaserV2_Meshy_BaseColor), Animated false inherited, RunRate 0.7. Eye instances of M_RobotEye: BaseColorTex = the same texture as the matching body, UseEyeTexture on, Animated off, GlowAmount 23 (as the existing eye instances; the master default is 35), EyeColor (1, 0.05, 0.05). Values provisional (owner 2026-10-22).
- events: none.
- logic_in_bp: none (material graph).
- assets: textures T_Heavy_Meshy_BaseColor (/Game/RogueRigs/Art/Heavy/), T_Kamikaze_Meshy_BaseColor (/Art/Kamikaze/), T_Shooter_Meshy_BaseColor (/Art/Shooter/), T_ChaserV2_Meshy_BaseColor (/Art/ChaserV2/); sources Art/Meshes/*-meshy/*_clean_basecolor.png; sRGB on, TC_Default, World group. T_ChaserAnim for the placeholder AnimTex.
- sync_rule: a body instance and its eye instance use the same BaseColorTex, RunRate and ClipOffset; Animated is switched on together, with a baked AnimTex per robot (not built yet).
- revert: delete the nine material assets above and the four textures (and their folders if empty); nothing references them until the slot assignment subtasks.
- verified: all eight instances read back (texture, Animated, UseEyeTexture; RunRate 0.7 and ClipOffset 0 on all eight; GlowAmount 23 and EyeColor (1, 0.05, 0.05) on the four eyes); M_RobotMeshy recompiled without error; all assets saved. Not verified: any render, PIE, animation (no AnimTex baked for these robots).
- glow_283_07: MI_RobotEye_Heavy_Glow (new, task-283-07, in /Game/RogueRigs/Materials/Robots/): child of MI_RobotEye_Heavy, GlowAmount 60, EyeColor (1, 0.35, 0), RunRate 0.7 and ClipOffset 0 inherited (same as MI_Heavy_Body); BP_Heavy swaps the robot_eye slot to it on OnTelegraphStart. Provisional, owner 2026-10-22. Revert: delete the asset and restore the OnTelegraphStart pin.
- anim_284_06 (task-284-06, 2026-10-22): MI_Heavy_Body, MI_RobotEye_Heavy and MI_RobotEye_Heavy_Glow now have Animated on (the Glow instance inherits it), AnimTex T_HeavyAnim and RunRate 1.0 (was 0.7: the heavy walk is baked for RunRate 1.0), ClipOffset 0 on all three; GlowAmount (23, 60) and EyeColor unchanged. Read back. Revert: Animated off, RunRate 0.7, AnimTex back to the T_ChaserAnim default. See [[bp-heavy]].
- anim_284_07 (task-284-07, 2026-10-22): MI_Shooter_Body and MI_RobotEye_Shooter now have Animated on, AnimTex T_ShooterAnim and RunRate 1.25 (was 0.7: the shooter walk is baked for RunRate 1.25), ClipOffset 0 on both; GlowAmount and EyeColor unchanged. Read back. Revert: Animated off, RunRate 0.7, AnimTex back to the T_ChaserAnim default. See [[bp-shooter]].
- anim_284_08 (task-284-08, 2026-10-22): MI_ChaserV2_Body and MI_RobotEye_ChaserV2 now have Animated on, AnimTex T_ChaserV2Anim and RunRate 1.5 (was 0.7: the chaser v2 run is baked for RunRate 1.5), ClipOffset 0 on both; GlowAmount and EyeColor unchanged. Both assigned to SM_ChaserV2_Meshy_Anim on BP_ChaserV2. Read back. Revert: Animated off, RunRate 0.7, AnimTex back to the T_ChaserAnim default. See [[bp-chaser-v2]].
- owner_agent: agent-bp

## Relations
- [[bp-kamikaze]] — kamikaze body, eye and core use.
- [[bp-shooter]] — shooter body and eye use.
- [[bp-chaser-v2]] — chaser v2 body and eye use.
- [[bp-heavy]] — first user of the heavy instances.
- [[blueprints-enemies-index]] — parent index.
- [[bp-robot-eye-material]] — M_RobotEye and MF_RobotAnimWPO that the master and the eye instances use.
- [[bp-robot-readability]] — body glow parameters, for comparison.
