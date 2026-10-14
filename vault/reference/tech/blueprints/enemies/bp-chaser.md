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
- components: inherited C++ components plus BodyMesh (attached to the CollisionCylinder capsule root; StaticMesh SM_Chaser_Meshy_Anim (since task-157-04; SM_Chaser_Meshy from task-156-02 to task-157-03), relative location (0, 0, -62) so the mesh floor origin is at the capsule bottom, scale 1, rotation 0, forward +X, OverrideMaterials [M_Chaser_Meshy_Anim] (task-156-02: SM_Chaser_Meshy with M_Chaser_Meshy; before task-156-02: SM_Chaser with M_Chaser_yellow, M_Chaser_gunmetal, M_Chaser_eye, M_Chaser_logo), generate-overlap off, CastShadow kept) and EyeMesh (engine Sphere kept but hidden: bVisible false, bHiddenInGame true, the eye is in the model); both profile NoCollision. Interim static mesh, provisional, owner 2026-10-13; the baked animation comes later (task-152-02). Before: BodyMesh engine Cylinder scale 0.48, 0.48, 1.24, MI_GB_Chaser; EyeMesh sphere at (24, 0, 40), MI_GB_Eye.
- exposed_defaults: StatsRowName = Chaser (C++ default kept); XPPickupClass = BP_PickupXP_C; all other C++ defaults kept.
- events: none
- logic_in_bp: none
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_Chaser (Color 1, 0.75, 0, 1), MI_GB_Eye (Color 1, 0, 0, 1), instances of M_GreyBox.
- assets_new (task-152-01, provisional, owner 2026-10-13, static interim, animation baked later; used by BP_Chaser and BP_ShieldChaser since task-152-02): /Game/RogueRigs/Art/Chaser/ SM_Chaser (ten parts combined, cm, pivot at the floor origin, bounds x -12.4 to 32.5, y +-43.3, z 0 to 123, 2640 tris, slots yellow_body, gunmetal, eye_red, logo_plate with the four materials below as defaults); T_ChaserLogo (256 x 256, sRGB, default compression); M_Chaser_yellow (M_GreyBox instance, Color linear (0.913, 0.491, 0.0144) = #F5BA20); M_Chaser_gunmetal (M_GreyBox instance, Color (0.0423, 0.0423, 0.0423) = #3A3A3A); M_Chaser_eye (M_GreyBoxGlow instance, Color and GlowColor (1, 0.05, 0.05), GlowAmount 25); M_Chaser_logo (lit material, Base Color = T_ChaserLogo, Roughness 1, Metallic 0).
- assets_meshy (task-156-02): /Game/RogueRigs/Art/Chaser/ SM_Chaser_Meshy (cm, floor origin, bounds x -17.26 to 17.83, y -36.83 to 36.87, z 0 to 123, 3115 tris, one slot meshy_chaser with M_Chaser_Meshy as default), T_Chaser_Meshy_BaseColor (2048 px, sRGB, default compression), M_Chaser_Meshy (lit: Base Color = texture, Roughness 0.6, Metallic 0, Emissive = Base Color x saturate((R - 2 x max(G, B)) x 3) x GlowAmount, default 25, so only the red eye and red parts glow (task-158-01: the earlier mask (R - max(G, B)) x 4 was about 1 on the yellow body, whose linear colour is about 0.91, 0.49, 0.02, and made the whole chaser glow; the same mask is in M_Chaser_Meshy_Anim); no normal map). Front +X as documented from the Blender export (not checked in the editor). Old SM_Chaser and its four materials kept in the project, unused.
- assets_anim (task-157-04, provisional): /Game/RogueRigs/Art/Chaser/ SM_Chaser_Meshy_Anim (ten parts combined from chaser_meshy_parts.fbx, one slot meshy_chaser, 3273 tris, bounds x -17.26 to 17.83, y -36.83 to 36.87, z 0 to 123, vertex colour red = part index / 9 (import not readable back through MCP, see the task report), bounds extension 40 cm on all six sides because the moving parts leave the rest silhouette); T_ChaserAnim (20 x 31, EXR half float, TC_HDR = RGBA16F, no mips, filter Nearest, sRGB off, never stream, address Clamp, group 16BitData; texel x = 2 * part quaternion xyzw, x = 2 * part + 1 translation in cm, row = frame: run 0 to 15 loop, attack 16 to 30 clamped); M_Chaser_Meshy_Anim (copy of M_Chaser_Meshy plus WPO: Custom node ChaserAnimWPO with Texture2D.Load of the two frame rows, lerp, quaternion rotation; the Y axis is mirrored in the node because the Unreal import flips Y; parameters ClipId = Custom Primitive Data 0, AnimStart = Custom Primitive Data 1, Time node; vertex normals not animated; WPO distance limit left off; instruction count not readable through MCP; performance at 100, 200 and 400 robots not measured).
- animation_contract: ARRChaser writes Custom Primitive Data on BodyMesh (index 0 clip id: 0 run, 1 attack; index 1 start in world seconds) only on events ([[sys-enemies-chaser]]); nothing in the Blueprint graph; the component must keep the name BodyMesh.
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-enemies]] — the C++ class.
- [[bp-pickup-xp]] — the pickup it drops.
- [[bp-enemy-spawner]] — the spawner that creates it.
