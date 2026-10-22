---
id: bp-shield-chaser-v2
description: "BP_ShieldChaserV2, duplicate of BP_ShieldChaser with the animated Meshy v2 mesh and its own master and instances; open for slots, RunRate 1.5, OnShieldBroken and revert."
parent: blueprints-enemies-index
kind: doc
---
# BP_ShieldChaserV2

- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_ShieldChaserV2
- parent_class: ARRShieldChaser (/Script/RogueRigs.RRShieldChaser), same as [[bp-shield-chaser]]
- purpose: the shield chaser v2 robot (task-286-02, 2026-10-22, provisional, owner 2026-10-22); visuals only, behaviour in C++ ([[sys-enemies-shield-chaser]]). Not spawned yet: the spawner still uses BP_ShieldChaser.
- components: duplicate of BP_ShieldChaser (`AssetTools.duplicate`), so debris, puff, hidden ShieldShell and C++ defaults are identical (StatsRowName ShieldChaser, DebrisPoolSize 80). BodyMesh: StaticMesh /Game/RogueRigs/Art/ShieldChaserV2/SM_ShieldChaserV2_Meshy_Anim, relative location (0, 0, -62) (geometry z 0 to 123), no shadow, OverrideMaterials in the slot order of the v2 mesh: 0 body = MI_ShieldChaserV2_Body, 1 robot_eye = MI_RobotEye_ShieldChaserV2, 2 M_ChaserShield = MI_ShieldChaserV2_Shield, 3 M_ChaserShield_Emblem = MI_ShieldChaserV2_Emblem. The component keeps the name BodyMesh.
- slot_order_warning: the v2 mesh imports with slots body, robot_eye, M_ChaserShield, M_ChaserShield_Emblem (v1: body, shield, emblem, eye). Element indices of v2 differ from v1 (eye is 1, shield 2, emblem 3).
- exposed_defaults: as BP_ShieldChaser (C++ defaults kept).
- events: OnShieldBroken (ShieldChaser EventOnShieldBroken), read back: BodyMesh SetMaterial element 0 = MI_ShieldChaserV2_BodyPlain, element 1 = MI_RobotEye_ShieldChaserV2Plain, element 2 and element 3 = MI_ShieldChaserV2_Hidden, then SpawnSystemAtLocation NS_ShieldDebris (bAutoActivate false), SetNiagaraVariable Vector3 User.HitDirection = GetShieldDebrisDirection, Activate; unchanged from v1 apart from the four materials. Pooled reuse restores the captured materials (ARREnemy RestoreVisualParts).
- logic_in_bp: visuals only (four SetMaterial calls, spawn, SetNiagaraVariable, Activate).
- assets: /Game/RogueRigs/Art/ShieldChaserV2/ SM_ShieldChaserV2_Meshy_Anim (imported by MCP from Art/Meshes/shield-chaser-v2/SM_ShieldChaserV2_Meshy_Anim.fbx; bounds extension 60 cm, collision removed, LightMapCoordinateIndex 1, Generate Lightmap UVs off, full precision UVs on), T_ShieldChaserV2Anim (20 x 62, TC_HDR, no mips, Nearest, sRGB off, never stream, Clamp, World group), M_ShieldChaserV2_Anim (duplicate of M_ShieldChaser_Anim: the master hard-codes its anim texture in a TextureObject, so v2 needs its own master; only that TextureObject changed to T_ShieldChaserV2Anim, recompiled), MI_ShieldChaserV2_Body, _BodyPlain, _Shield, _Emblem, _Hidden; and in /Game/RogueRigs/Materials/Robots/ MI_RobotEye_ShieldChaserV2 and MI_RobotEye_ShieldChaserV2Plain (duplicates of MI_RobotEye_ChaserV2 with AnimTex T_ShieldChaserV2Anim). Values in [[bp-robot-meshy-material]].
- animation: clip 0 shield run, clip 1 bash, clip 2 plain run, clip 3 plain attack (ClipOffset 0 shield, 2 plain), baked for RunRate 1.5 ([[robot-parts-bake-pipeline]]).
- verified: UV channel 2 (PartId) on RENDER_DATA: 3 UV sets, 7134 triangles; body (id 0) 6097 triangles on bins 0.05 to 0.95, robot_eye (id 1) 58 at 0.15, shield (id 2) 977 at 0.45, emblem (id 3) 2 at 0.45; off_bin 0, mixed 0. Build settings read back. BP compiled (LogBlueprint: only the Compiling lines), BodyMesh and OnShieldBroken read back, saved. v1 assets untouched.
- not_verified: PIE, render, shield and run look, bounds sufficiency, debris direction, spawn.
- revert: delete BP_ShieldChaserV2, the Art/ShieldChaserV2 folder and the two MI_RobotEye_ShieldChaserV2* instances. No v1 asset was changed.
- owner_agent: agent-bp

## Relations
- [[blueprints-enemies-index]] — parent index.
- [[bp-shield-chaser]] — the v1 Blueprint this duplicates, kept for restore.
- [[bp-chaser-v2]] — body texture and eye instance source.
- [[bp-robot-meshy-material]] — instance values.
- [[robot-parts-bake-pipeline]] — bake and Unreal recipe of this mesh.
- [[sys-enemies-shield-chaser]] — the C++ class.
