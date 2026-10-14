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
- components: inherited C++ components plus BodyMesh (attached to the CollisionCylinder capsule root; StaticMesh SM_Chaser_Meshy (since task-156-02), relative location (0, 0, -62) so the mesh floor origin is at the capsule bottom, scale 1, rotation 0, forward +X, OverrideMaterials [M_Chaser_Meshy] (before task-156-02: SM_Chaser with M_Chaser_yellow, M_Chaser_gunmetal, M_Chaser_eye, M_Chaser_logo), generate-overlap off, CastShadow kept) and EyeMesh (engine Sphere kept but hidden: bVisible false, bHiddenInGame true, the eye is in the model); both profile NoCollision. Interim static mesh, provisional, owner 2026-10-13; the baked animation comes later (task-152-02). Before: BodyMesh engine Cylinder scale 0.48, 0.48, 1.24, MI_GB_Chaser; EyeMesh sphere at (24, 0, 40), MI_GB_Eye.
- exposed_defaults: StatsRowName = Chaser (C++ default kept); XPPickupClass = BP_PickupXP_C; all other C++ defaults kept.
- events: none
- logic_in_bp: none
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_Chaser (Color 1, 0.75, 0, 1), MI_GB_Eye (Color 1, 0, 0, 1), instances of M_GreyBox.
- assets_new (task-152-01, provisional, owner 2026-10-13, static interim, animation baked later; used by BP_Chaser and BP_ShieldChaser since task-152-02): /Game/RogueRigs/Art/Chaser/ SM_Chaser (ten parts combined, cm, pivot at the floor origin, bounds x -12.4 to 32.5, y +-43.3, z 0 to 123, 2640 tris, slots yellow_body, gunmetal, eye_red, logo_plate with the four materials below as defaults); T_ChaserLogo (256 x 256, sRGB, default compression); M_Chaser_yellow (M_GreyBox instance, Color linear (0.913, 0.491, 0.0144) = #F5BA20); M_Chaser_gunmetal (M_GreyBox instance, Color (0.0423, 0.0423, 0.0423) = #3A3A3A); M_Chaser_eye (M_GreyBoxGlow instance, Color and GlowColor (1, 0.05, 0.05), GlowAmount 25); M_Chaser_logo (lit material, Base Color = T_ChaserLogo, Roughness 1, Metallic 0).
- assets_meshy (task-156-02): /Game/RogueRigs/Art/Chaser/ SM_Chaser_Meshy (cm, floor origin, bounds x -17.26 to 17.83, y -36.83 to 36.87, z 0 to 123, 3115 tris, one slot meshy_chaser with M_Chaser_Meshy as default), T_Chaser_Meshy_BaseColor (2048 px, sRGB, default compression), M_Chaser_Meshy (lit: Base Color = texture, Roughness 0.6, Metallic 0, Emissive = Base Color x saturate((R - max(G, B)) x 4) x GlowAmount, default 25, so only the red eye and red parts glow; no normal map). Front +X as documented from the Blender export (not checked in the editor). Old SM_Chaser and its four materials kept in the project, unused.
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-enemies]] — the C++ class.
- [[bp-pickup-xp]] — the pickup it drops.
- [[bp-enemy-spawner]] — the spawner that creates it.
