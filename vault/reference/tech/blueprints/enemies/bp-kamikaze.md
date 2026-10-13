---
id: bp-kamikaze
description: "BP_Kamikaze, Blueprint of ARRKamikaze with gunmetal sphere body, orange core, red eye and XP drop class; open for its meshes and materials."
parent: blueprints-enemies-index
kind: doc
---
# BP_Kamikaze

- asset_path: /Game/RogueRigs/Blueprints/Enemies/BP_Kamikaze
- parent_class: ARRKamikaze (/Script/RogueRigs.RRKamikaze)
- purpose: the kamikaze robot; visuals only, behaviour in C++ ([[sys-enemies]]).
- components: CoreLight (PointLight on the root capsule, relative (60, 0, 60), colour 255/170/90, attenuation 400, no shadow, intensity 0 until the flare); inherited C++ components (VisualRoot, BodyMesh, CoreMesh) plus EyeMesh added under VisualRoot. Sized to about 1.1 x the miner (capsule 37.4 x 96.8, task-012-03). BodyMesh: engine Sphere, scale 1.15 (115 uu, bottom at the VisualRoot origin), relative (0, 0, 57.5), MI_GB_Kamikaze. CoreMesh: engine Sphere, scale 0.644, relative (48.3, 0, 46), protrudes at the front, MI_GB_Core. EyeMesh: engine Sphere, scale 0.23, relative (39, 0, 94.3) (front, near the top), MI_GB_Eye, no collision, no shadow. Earlier sizes (body 0.5, core 0.28, eye 0.1) scaled by 2.3.
- exposed_defaults: XPPickupClass = BP_PickupXP_C; all other C++ defaults kept.
- events: EventTick: while IsFlaring, CoreLight intensity = GetFlareProgress x 10000 (a ramp to a strong orange-white over the 0.5 s flare); OnFlareEnd sets it back to 0 (task-027-02; multiplier 60000 lowered to 10000 in task-028-02 because the flare blew out; the tick only reads a getter and sets a light).
- logic_in_bp: display only (light ramp).
- assets: /Game/RogueRigs/Materials/GreyBox/MI_GB_Kamikaze (dark gunmetal, Color 0.12, 0.13, 0.15), MI_GB_Core (bright orange, Color 1, 0.5, 0), MI_GB_Eye; instances of M_GreyBox.
- owner_agent: agent-bp

## Relations
- [[blueprints-index]] — parent index.
- [[sys-enemies]] — the C++ class.
- [[bp-pickup-xp]] — the pickup it drops.
- [[bp-enemy-spawner]] — the spawner that creates it.
