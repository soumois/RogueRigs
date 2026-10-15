---
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
id: bp-shield-chaser
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
description: "BP_ShieldChaser, Blueprint of ARRShieldChaser: BP_Chaser look plus a translucent pale blue-white shell that pops and hides on OnShieldBroken; open for components and spawner use."
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
parent: blueprints-enemies-index
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
kind: doc
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
---
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
# BP_ShieldChaser
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.

- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_ShieldChaser
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- parent_class: ARRShieldChaser (/Script/RogueRigs.RRShieldChaser), not a child of BP_Chaser (the spawner resolves the nearest native class, [[sys-enemies-shield-chaser]]).
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- purpose: the shield variant of the chaser; visuals only, behaviour in C++.
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- components: duplicated from BP_Chaser ([[bp-chaser]]) then reparented; BodyMesh and EyeMesh carry the same changes as BP_Chaser (SM_Chaser_Meshy_Anim at (0, 0, -62) with the single override M_Chaser_Meshy_Anim since task-157-04, before that SM_Chaser_Meshy and M_Chaser_Meshy; EyeMesh hidden; baked run and attack animation through Custom Primitive Data 0 (clip id) and 1 (start time) written by ARRChaser, assets described in [[bp-chaser]]; provisional); ShieldShell (engine Cylinder, scale 0.9, 0.9, 1.34 = 90 uu diameter, 134 uu tall (task-156-02: the Meshy model is 35 x 74 x 123 cm, footprint corner radius about 41, so the shell encloses it with a small margin; was 1.0, 1.0, 1.34 for SM_Chaser, 0.64 for the grey body; attached to the capsule, NoCollision, MI_GB_ShieldShell). Task-160-01 (owner 2026-10-14): neither BodyMesh nor ShieldShell casts a shadow: castShadow, bCastDynamicShadow, bCastStaticShadow, bCastContactShadow, bCastInsetShadow and bAffectDistanceFieldLighting all false on both.
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- exposed_defaults: StatsRowName = ShieldChaser (C++ default); XPPickupClass = BP_PickupXP_C; DeathPuffSystem = NS_RobotDeathPuff (task-167-02; DeathPuffPoolSize 40 kept). DebrisSystem = NS_RobotDebris, DebrisPoolSize 80 (task-182-02, read back; was 40), DebrisPieces = the same 13 entries as BP_Chaser (task-166-04, read back).
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- events: OnShieldBroken: shell scale (0.95, 0.95, 1.6) as a short burst (graph unchanged; against the 0.9 base the XY pop grows slightly again), Delay 0.08 s, then Hidden. GetShieldFraction fade not used.
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- logic_in_bp: visuals only (scale pop, Delay, visibility).
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_ShieldShell (instance of M_GreyBoxTranslucent, Color 0.7, 0.92, 1, Opacity 0.3).
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- verified: PIE (task-028-02): spawned by the director after 1:00 (FlowShields) with the shell visible, no Blueprint runtime error. A placed spawner instance loaded before the change needed the entry set on the PIE instance (stale weights).
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- owner_agent: agent-bp
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.

- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
## Relations
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- [[blueprints-index]] — parent index.
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- [[sys-enemies-shield-chaser]] — the C++ class.
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- [[bp-chaser]] — the look it copies.
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- [[bp-enemy-spawner]] — the spawner entry (weight 0, director only).
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
- [[bp-pickup-xp]] — the pickup it drops.
- [[bp-robot-debris]] — the debris system set as its DebrisSystem.
