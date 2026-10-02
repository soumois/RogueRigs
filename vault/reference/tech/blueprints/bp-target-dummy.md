---
id: bp-target-dummy
description: "BP_TargetDummy, Blueprint of ARRTargetDummy: 160 uu grey-orange sphere used to test weapons. Open to see its defaults."
parent: blueprints-index
kind: doc
---
# BP_TargetDummy

- asset_path: /Game/RogueRigs/Blueprints/Debug/BP_TargetDummy
- parent_class: ARRTargetDummy (/Script/RogueRigs.RRTargetDummy)
- purpose: debug punching ball in L_ArenaM1
- components: inherited Mesh, ASC, health set; none added
- exposed_defaults: Mesh StaticMesh = engine Sphere, scale 1.6 (160 uu, task-008-03), material slot 0 = MI_GB_Dummy (instance of M_GreyBox); collision preset kept; C++ defaults kept (MaxHealth 1000, RefillDelay 3). Placed three times in L_ArenaM1 with the sphere resting on the floor: (8900, 8300, 80), (7300, 8500, 80), (8700, 8900, 80)
- events: none
- logic_in_bp: none
- assets: engine Sphere, MI_GB_Dummy
- owner_agent: agent-bp
- verified: compiled and saved; PIE behaviour check left to task-007-05.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-target-dummy]] — the C++ class it derives from.
