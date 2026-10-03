---
id: bp-kamikaze
description: "BP_Kamikaze, Blueprint of ARRKamikaze with gunmetal sphere body, orange core, red eye and XP drop class; open for its meshes and materials."
parent: blueprints-index
kind: doc
---
# BP_Kamikaze

- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_Kamikaze
- parent_class: ARRKamikaze (/Script/RogueRigs.RRKamikaze)
- purpose: the kamikaze robot; visuals only, behaviour in C++ ([[sys-enemies]]).
- components: inherited C++ components (VisualRoot, BodyMesh, CoreMesh) plus EyeMesh added under VisualRoot. BodyMesh: engine Sphere, scale 0.5 (50 uu), relative (0, 0, 25), MI_GB_Kamikaze. CoreMesh: engine Sphere, scale 0.28, relative (21, 0, 20), protrudes at the front, MI_GB_Core. EyeMesh: engine Sphere, scale 0.1, relative (17, 0, 41), MI_GB_Eye, no collision, no shadow.
- exposed_defaults: XPPickupClass = BP_PickupXP_C; all other C++ defaults kept.
- events: none
- logic_in_bp: none
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_Kamikaze (dark gunmetal, Color 0.12, 0.13, 0.15), MI_GB_Core (bright orange, Color 1, 0.5, 0), MI_GB_Eye; instances of M_GreyBox.
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-enemies]] — the C++ class.
- [[bp-pickup-xp]] — the pickup it drops.
- [[bp-enemy-spawner]] — the spawner that creates it.
