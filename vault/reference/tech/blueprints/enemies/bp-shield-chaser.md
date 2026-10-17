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
- events: OnShieldBroken (ShieldChaser EventOnShieldBroken): BodyMesh SetMaterial element 0 = MI_ShieldChaser_BodyPlain, element 1 and element 2 = MI_ShieldChaser_Hidden (task-220-03, read back). A pooled reuse restores the captured materials (ARREnemy RestoreVisualParts). After the swaps it calls SpawnSystemAtLocation (Niagara|SpawnSystematLocation) with SystemTemplate NS_ShieldDebris, Location = GetActorLocation, Rotation = GetActorRotation, bAutoDestroy true (defaults for the rest), nothing set on the system (task-222-04, read back). GetShieldFraction fade not used.
- logic_in_bp: visuals only (three SetMaterial calls and one SpawnSystemAtLocation).
- assets: /Game/RogueRigs/Art/ShieldChaser/ SM_ShieldChaser_Meshy_Anim, MI_ShieldChaser_Body, MI_ShieldChaser_Shield, MI_ShieldChaser_Emblem, MI_ShieldChaser_BodyPlain, MI_ShieldChaser_Hidden (instances of M_ShieldChaser_Anim, task-220-02).
- verified: task-222-04: compile and save without error, OnShieldBroken read back with the spawn node after the three swaps; not run in PIE, so the debris spawn is not seen. Task-220-03: compile without error and save. Mesh, three materials, location and shadow flags read back. Not verified: PIE spawn with the new mesh, any render. Earlier PIE (task-028-02): spawned by the director after 1:00 with no Blueprint runtime error.
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-enemies-shield-chaser]] — the C++ class.
- [[bp-chaser]] — the look it copies.
- [[bp-enemy-spawner]] — the spawner entry (weight 0, director only).
- [[bp-pickup-xp]] — the pickup it drops.
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- [[bp-shield-debris]] — NS_ShieldDebris, spawned by OnShieldBroken.
