---
id: bp-shield-chaser
description: "BP_ShieldChaser, Blueprint of ARRShieldChaser: combined mesh, three material slots, plain body and hidden shield on OnShieldBroken; OnShieldBroken swaps materials and spawns NS_ShieldDebris."
parent: blueprints-enemies-index
kind: doc
---
# BP_ShieldChaser

- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_ShieldChaser
- parent_class: ARRShieldChaser (/Script/RogueRigs.RRShieldChaser), not a child of BP_Chaser (the spawner resolves the nearest native class, [[sys-enemies-shield-chaser]]).
- purpose: the shield variant of the chaser; visuals only, behaviour in C++.
- components: duplicated from BP_Chaser ([[bp-chaser]]) then reparented. BodyMesh: StaticMesh SM_ShieldChaser_Meshy_Anim (task-220-03, read back), relative location (0, 0, -62), OverrideMaterials slot 0 MI_ShieldChaser_Body, slot 1 MI_ShieldChaser_Shield, slot 2 MI_ShieldChaser_Emblem (all in /Game/RogueRigs/Art/ShieldChaser/), no shadow (castShadow, dynamic, static and contact shadow all false, owner 2026-10-14). EyeMesh hidden (bVisible false, bHiddenInGame true). Baked animation through Custom Primitive Data 0 (clip id) and 1 (start time) written by ARRChaser, the component keeps the name BodyMesh. ShieldShell (engine Cylinder, MI_GB_ShieldShell) kept but hidden (owner 2026-10-17): bVisible false, bHiddenInGame true, read back; its old pop graph is removed.
- exposed_defaults: StatsRowName = ShieldChaser (C++ default); XPPickupClass = BP_PickupXP_C; DeathPuffSystem = NS_RobotDeathPuff (DeathPuffPoolSize 40 kept); DebrisSystem = NS_RobotDebris, DebrisPoolSize 80, DebrisPieces = the same 13 entries as BP_Chaser (task-166-04, task-182-02).
- events: OnShieldBroken (ShieldChaser EventOnShieldBroken): BodyMesh SetMaterial element 0 = MI_ShieldChaser_BodyPlain, element 1 and element 2 = MI_ShieldChaser_Hidden (task-220-03, read back). A pooled reuse restores the captured materials (ARREnemy RestoreVisualParts). After the swaps it calls SpawnSystemAtLocation (Niagara|SpawnSystemAtLocation) with SystemTemplate NS_ShieldDebris, Location = GetActorLocation, Rotation = GetActorRotation, bAutoDestroy true, bAutoActivate false (task-224-03); then SetNiagaraVariable (Vector3) on the returned component, InVariableName User.HitDirection, InValue = GetShieldDebrisDirection (ARRShieldChaser BlueprintPure, task-224-01); then Activate on the component, so the burst reads the direction. GetShieldFraction fade not used.
- logic_in_bp: visuals only (three SetMaterial calls, SpawnSystemAtLocation inactive, SetNiagaraVariable Vector3, Activate).
- assets: /Game/RogueRigs/Art/ShieldChaser/ SM_ShieldChaser_Meshy_Anim, MI_ShieldChaser_Body, MI_ShieldChaser_Shield, MI_ShieldChaser_Emblem, MI_ShieldChaser_BodyPlain, MI_ShieldChaser_Hidden (instances of M_ShieldChaser_Anim, task-220-02).
- verified: task-224-03: OnShieldBroken rewritten and read back (three SetMaterial, spawn with bAutoActivate false, SetNiagaraVariable User.HitDirection, Activate), compile and save without error; not run in PIE, so the debris direction is not seen. task-222-04: compile and save without error, OnShieldBroken read back with the spawn node after the three swaps; not run in PIE, so the debris spawn is not seen. Task-220-03: compile without error and save. Mesh, three materials, location and shadow flags read back. Not verified: PIE spawn with the new mesh, any render. Earlier PIE (task-028-02): spawned by the director after 1:00 with no Blueprint runtime error.
- readability: the task-266-01 Readability parameters of M_ShieldChaser_Anim and the MI_ShieldChaser_Emblem overrides were removed by task-275-02 (owner 2026-10-21, back to Monday 2026-10-19); the master (before task-278-06) had GlowAmount, ClipId, AnimStart, RunRate, ClipOffset, Visible and BaseColorTex; values and restore recipe in [[bp-robot-readability]].
- body_material_278_06 (task-278-06, 2026-10-21): M_ShieldChaser_Anim. Before = WPO from the Custom node ChaserAnimWPO (TextureObject T_ShieldChaserAnim, ClipId CPD 0, AnimStart CPD 1, Time, UV2 R via ComponentMask, LocalPosition, RunRate, ClipOffset, then Transform Local to World) and Emissive = TexRGB x saturate((R - 2 max(G,B)) x 3) x GlowAmount (default 25). After = World Position Offset wired directly to MF_RobotAnimWPO (AnimTex = TextureObject T_ShieldChaserAnim, ClipOffset = existing scalar parameter ClipOffset, RunRate = existing parameter RunRate 0.7; ClipId CPD 0 and AnimStart CPD 1 now inside the function, C++ writes unchanged), Emissive unconnected, GlowAmount, Custom node, Transform, Time, LocalPosition, UV2 mask, ClipId and AnimStart parameters removed. Masked blend, OpacityMask = texture alpha x Visible, BaseColorTex, roughness and metallic constant unchanged. Instances read back unchanged: ClipOffset 0 (Body, Shield, Emblem, Hidden) and 2 (BodyPlain), RunRate 0.7 all, Visible 1 except Hidden 0, BaseColorTex unchanged. Revert: `git checkout 2ad7698a -- Content/RogueRigs/Art/ShieldChaser/M_ShieldChaser_Anim.uasset` (instance .uasset files were not rewritten). Sync rule: the eye instances MI_RobotEye_ShieldChaser and MI_RobotEye_ShieldChaserPlain must keep the same RunRate (0.7) and ClipOffset (0 and 2) as MI_ShieldChaser_Body and MI_ShieldChaser_BodyPlain. Recompile and save: no error on this material. Not verified: render, PIE, instruction count.
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-enemies-shield-chaser]] — the C++ class.
- [[bp-chaser]] — the look it copies.
- [[bp-enemy-spawner]] — the spawner entry (weight 0, director only).
- [[bp-pickup-xp]] — the pickup it drops.
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- [[bp-shield-debris]] — NS_ShieldDebris, spawned by OnShieldBroken.
- [[bp-robot-eye-material]] — MF_RobotAnimWPO used by the body material, and the eye instances to keep in sync.
- [[bp-robot-readability]] — the readability parameters removed by task-275-02, with their values and restore recipe.
