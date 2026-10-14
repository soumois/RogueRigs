---
id: bp-shield-chaser
description: "BP_ShieldChaser, Blueprint of ARRShieldChaser: BP_Chaser look plus a translucent pale blue-white shell that pops and hides on OnShieldBroken; open for components and spawner use."
parent: blueprints-enemies-index
kind: doc
---
# BP_ShieldChaser

- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_ShieldChaser
- parent_class: ARRShieldChaser (/Script/RogueRigs.RRShieldChaser), not a child of BP_Chaser (the spawner resolves the nearest native class, [[sys-enemies-shield-chaser]]).
- purpose: the shield variant of the chaser; visuals only, behaviour in C++.
- components: duplicated from BP_Chaser ([[bp-chaser]]) then reparented; BodyMesh and EyeMesh carry the same changes as BP_Chaser since task-152-02 (SM_Chaser at (0, 0, -62), four material overrides, EyeMesh hidden; interim static mesh, provisional, owner 2026-10-13, the baked animation comes later); ShieldShell (engine Cylinder, scale 1.0, 1.0, 1.34 = about 100 uu wide, provisional: was 0.64, 0.64, 1.34 for the 48 uu grey body, the model is 86 cm wide at the arms; attached to the capsule, NoCollision, no dynamic shadow, MI_GB_ShieldShell).
- exposed_defaults: StatsRowName = ShieldChaser (C++ default); XPPickupClass = BP_PickupXP_C.
- events: OnShieldBroken: shell scale (0.95, 0.95, 1.6) as a short burst (graph unchanged; against the new 1.0 base the XY pop now shrinks slightly instead of growing, to review), Delay 0.08 s, then Hidden. GetShieldFraction fade not used.
- logic_in_bp: visuals only (scale pop, Delay, visibility).
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_ShieldShell (instance of M_GreyBoxTranslucent, Color 0.7, 0.92, 1, Opacity 0.3).
- verified: PIE (task-028-02): spawned by the director after 1:00 (FlowShields) with the shell visible, no Blueprint runtime error. A placed spawner instance loaded before the change needed the entry set on the PIE instance (stale weights).
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-enemies-shield-chaser]] — the C++ class.
- [[bp-chaser]] — the look it copies.
- [[bp-enemy-spawner]] — the spawner entry (weight 0, director only).
- [[bp-pickup-xp]] — the pickup it drops.
