---
id: bp-chaser
description: "BP_Chaser, Blueprint of ARRChaser with the yellow grey-box body and red eye; open for its meshes, materials and XP drop class."
parent: blueprints-index
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
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-enemies]] — the C++ class.
- [[bp-pickup-xp]] — the pickup it drops.
- [[bp-enemy-spawner]] — the spawner that creates it.
