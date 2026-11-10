---
id: bp-robot-emblem-decal
description: "Kessler-Voss emblem decal on Heavy, Kamikaze, Chaser v2 and shield chaser v2: script, meshes, materials, debris, slot rules, revert. Open before changing robot emblems."
parent: blueprints-enemies-index
kind: doc
---
# Robot emblem decal (task-499, 2026-11-10)

- asset_path: /Game/RogueRigs/Art/{Heavy,Kamikaze,ChaserV2,ShieldChaserV2}/SM_*_Meshy_Anim*Emblem, /Game/RogueRigs/Materials/Robots/
- parent_class: none (static meshes and materials; the Blueprints only point to the meshes)
- purpose: the yellow Kessler-Voss emblem as a decal on the top surface seen by the game camera ([[kessler-voss-logo]], [[color-code]]), Meshy meshes and textures not regenerated.
- components: `Tools/art/blender/robot_emblem_decal.py --robot heavy|kamikaze|chaser_v2|shield_chaser_v2 [--size cm] [--centre-x cm] [--centre-y cm]` copies the faces of one VAT part hit by downward rays over a square, clips them, offsets them, gives one planar UV (emblem up = robot back) and keeps PartId, so the decal follows its part in MF_RobotAnimWPO ([[robot-parts-bake-pipeline]]). Object `Z_Emblem` sorts last, so the new slot `robot_emblem` is the last index in Unreal and existing slot indices do not move. Output `Art/Meshes/<robot>/*_emblem.fbx` + `.report.json`; the source parts FBX stay unchanged.
- exposed_defaults: provisional, owner 2026-11-10 (Q1-Q3 of [[task-499-robot-logo-decals]]): heavy HeadTorso, 30 cm at (16, 1.5) cm; kamikaze Body head top, 22 cm at (-8, -3); chaser v2 and shield chaser v2 HeadTorso hood, 14 cm at (-4, 1.5) (Blender rest cm, front +X).
- events: BP_ShieldChaserV2 OnShieldBroken also sets element 4 = MI_ShieldChaserV2_RobotEmblemPlain ([[bp-shield-chaser-v2]]).
- logic_in_bp: none.
- assets: meshes SM_Heavy_Meshy_AnimEmblem, SM_Kamikaze_Meshy_AnimEmblem, SM_ChaserV2_Meshy_AnimEmblem, SM_ShieldChaserV2_Meshy_AnimFwdEmblem (MCP import, bounds extension as the old meshes, build settings and UVCHECK clean); masters M_HeavyEmblem_Anim, M_KamikazeEmblem_Anim, M_ChaserV2Emblem_Anim = copies of the masked M_ShieldChaserV2_Anim with their TextureObject set to the robot anim texture (T_HeavyAnimRunV1, T_KamikazeAnim, T_ChaserV2AnimV1Clips: keep in sync with the body instance); instances MI_Heavy_Emblem, MI_Kamikaze_Emblem (T_ChaserShield_Emblem, yellow only), MI_ChaserV2_Emblem, MI_ShieldChaserV2_RobotEmblem / _RobotEmblemPlain (T_RobotEmblem_Dark: charcoal rock with yellow KV, /Game/RogueRigs/Art/RobotEmblem/, from `Tools/art/blender/robot_emblem_texture.py`). Debris: the torso FBX of each robot rewritten with the decal (original kept untracked as `.pre_emblem.fbx`; rerunning chaser_debris_pieces.py drops the decal), reimported, slot robot_emblem = M_ShieldDebris_Emblem (heavy, kamikaze) or M_RobotDebrisEmblemDark (chaser v2).
- gotcha: a material whose BlendMode was set to Masked through ObjectTools.set_properties rendered unmasked in -game (solid squares); copy an already masked master instead.
- verified: -game frames Saved/Renders/task-499/ (walk, heavy telegraph, heavy torso piece). Not verified in game: emblem after the shield breaks, chaser v2 and kamikaze torso pieces. Legibility at the default camera failed the style review (owner decision pending).
- revert: point the BodyMesh of BP_Heavy, BP_Kamikaze, BP_ChaserV2, BP_ShieldChaserV2 back to SM_Heavy_Meshy_Anim, SM_Kamikaze_Meshy_Anim, SM_ChaserV2_Meshy_Anim, SM_ShieldChaserV2_Meshy_AnimFwd (kept), remove the element 4 SetMaterial, restore the torso FBX from `.pre_emblem.fbx` and reimport.
- owner_agent: agent-bp

## Relations
- [[blueprints-enemies-index]] — parent index.
- [[bp-heavy]] — carries SM_Heavy_Meshy_AnimEmblem.
- [[bp-kamikaze]] — carries SM_Kamikaze_Meshy_AnimEmblem.
- [[bp-chaser-v2]] — carries SM_ChaserV2_Meshy_AnimEmblem.
- [[bp-shield-chaser-v2]] — carries SM_ShieldChaserV2_Meshy_AnimFwdEmblem.
- [[bp-robot-meshy-material]] — body instances the emblem instances mirror.
- [[robot-parts-bake-pipeline]] — PartId and anim textures the decal follows.
- [[kessler-voss-logo]] — the emblem.
- [[task-499-robot-logo-decals]] — the task and owner values.
