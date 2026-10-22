---
id: bp-robot-eye-material
description: "Shared robot eye material M_RobotEye, its WPO function MF_RobotAnimWPO and four instances: assets, graph, parameter values, sync rule, revert. Open before changing eye glow."
parent: blueprints-enemies-index
kind: doc
---
# M_RobotEye and MF_RobotAnimWPO

- asset_path: /Game/RogueRigs/Materials/Robots/ (MF_RobotAnimWPO, M_RobotEye, MI_RobotEye_Chaser, MI_RobotEye_ShieldChaser, MI_RobotEye_ShieldChaserPlain, MI_RobotEye_GreyBox). Created by task-278-04; not yet assigned to any mesh slot (later subtasks).
- parent_class: none (material assets; instances are MaterialInstanceConstant of M_RobotEye).
- purpose: one lit emissive material for the `robot_eye` slot of every robot mesh, animated with the same baked bone animation as the body so the eye follows the head.
- components: MF_RobotAnimWPO holds the shield Custom node code (ChaserAnimWPO, from M_ShieldChaser_Anim). Inputs: AnimTex (texture 2D), ClipOffset, RunRate. Inside: ClipId (CPD 0), AnimStart (CPD 1) as scalar parameters, Time, UV2 red (PartId), LocalPosition, then Transform local to world. Output: WPO. With ClipOffset 0 the code gives the chaser result for clip 0 and 1 (compared, task-278-04). M_RobotEye: lit, roughness 0.6. Base colour = UseEyeTexture ? BaseColorTex RGB : EyeColor. Emissive = EyeColor x GlowAmount x (UseEyeTexture ? saturate((R - 2 max(G,B)) x 3) of BaseColorTex : 1). WPO = Animated ? MF_RobotAnimWPO(AnimTex, ClipOffset, RunRate) : 0. UseEyeTexture and Animated are static switches (UseEyeTexture appears on two nodes of the same name and value).
- exposed_defaults: M_RobotEye: EyeColor (1, 0.05, 0.05), GlowAmount 23, UseEyeTexture true, Animated true, BaseColorTex T_Chaser_Meshy_BaseColor, AnimTex T_ChaserAnim, ClipOffset 0, RunRate 0.7. Instances (all EyeColor, GlowAmount 23, RunRate 0.7): Chaser = texture on, animated, AnimTex T_ChaserAnim, ClipOffset 0; ShieldChaser = same with AnimTex T_ShieldChaserAnim; ShieldChaserPlain = same, ClipOffset 2; GreyBox = texture off, not animated (no WPO, works on engine Sphere meshes). Values provisional (owner 2026-10-21).
- events: none.
- logic_in_bp: none (material graph; WPO code is in the function, never copied).
- assets: textures T_Chaser_Meshy_BaseColor, T_ChaserAnim (/Game/RogueRigs/Art/Chaser/), T_ShieldChaserAnim (/Game/RogueRigs/Art/ShieldChaser/).
- sync_rule: an eye instance's RunRate and ClipOffset equal those of its body instance, or the eye drifts from the head.
- revert: delete the six assets above (and the folder if empty); nothing references them until the slot assignment subtasks.
- verified: all four instances read back (switches, scalars, vector, textures); M_RobotEye compile without error, saved. The MF alone logs a preview-material compile warning on save (no preview texture on its AnimTex input); the materials using it compile. Not verified: any render, PIE.
- owner_agent: agent-bp

## Relations
- [[blueprints-enemies-index]] — parent index.
- [[bp-chaser]] — body whose animation code this function shares.
- [[bp-shield-chaser]] — body whose code (ClipOffset) the function holds.
- [[bp-robot-readability]] — body glow parameters, for comparison.
- [[bp-robot-meshy-material]] — four more eye instances (Heavy, Kamikaze, Shooter, ChaserV2) and the body master.
