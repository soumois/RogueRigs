---
id: bp-chaser
description: "BP_Chaser, Blueprint of ARRChaser with the yellow grey-box body and red eye; open for its meshes, materials and XP drop class."
parent: blueprints-enemies-index
kind: doc
---
# BP_Chaser

- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_Chaser
- parent_class: ARRChaser (/Script/RogueRigs.RRChaser)
- purpose: the chaser robot; visuals only, behaviour in C++ ([[sys-enemies]]).
- components: inherited C++ components plus BodyMesh (engine Cylinder, scale 0.48, 0.48, 1.24 = 48 x 124 uu, centred on the capsule, MI_GB_Chaser) and EyeMesh (engine Sphere scale 0.16 = 16 uu, at (24, 0, 40), MI_GB_Eye); both profile NoCollision.
- exposed_defaults: StatsRowName = Chaser (C++ default kept); XPPickupClass = BP_PickupXP_C; all other C++ defaults kept.
- events: none
- logic_in_bp: none
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_Chaser (Color 1, 0.75, 0, 1), MI_GB_Eye (Color 1, 0, 0, 1), instances of M_GreyBox.
- assets_new (task-152-01, provisional, owner 2026-10-13, static interim, animation baked later; not yet used by BP_Chaser, subtask 02): /Game/RogueRigs/Art/Chaser/ SM_Chaser (ten parts combined, cm, pivot at the floor origin, bounds x -12.4 to 32.5, y +-43.3, z 0 to 123, 2640 tris, slots yellow_body, gunmetal, eye_red, logo_plate with the four materials below as defaults); T_ChaserLogo (256 x 256, sRGB, default compression); M_Chaser_yellow (M_GreyBox instance, Color linear (0.913, 0.491, 0.0144) = #F5BA20); M_Chaser_gunmetal (M_GreyBox instance, Color (0.0423, 0.0423, 0.0423) = #3A3A3A); M_Chaser_eye (M_GreyBoxGlow instance, Color and GlowColor (1, 0.05, 0.05), GlowAmount 25); M_Chaser_logo (lit material, Base Color = T_ChaserLogo, Roughness 1, Metallic 0).
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-enemies]] — the C++ class.
- [[bp-pickup-xp]] — the pickup it drops.
- [[bp-enemy-spawner]] — the spawner that creates it.
